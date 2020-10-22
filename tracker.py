import cv2, os
import numpy as np
#import imutils

face_classifier = 'opencv-3.4.1/data/haarcascades/haarcascade_frontalface_alt.xml'
f_cascade = cv2.CascadeClassifier(face_classifier)
profile_classifier = 'opencv-3.4.1/data/haarcascades/haarcascade_profileface_alt.xml'
p_cascade = cv2.CascadeClassifier(profile_classifier)

class Face:

	def __init__(self, image=None, box=None, faceID=-1):
		self.image = image
		self.newboundingbox = box
		self.identity = faceID
		self.no_faces = 1
		self.tracker = cv2.TrackerKCF_create()

	def save(self, face):
		path = r'facesKCF/speaker%i/'%self.identity
		if not os.path.exists(path):
			try:
				original_umask = os.umask(0)
				os.mkdir(path, mode=0o777)
			finally:
				os.umask(original_umask)

		try:
			cv2.imwrite(path+r'face%d.jpg'%self.no_faces, face)
			self.no_faces += 1
		except Exception as err:
			print(err)
			return False
		return True

def is_inside(box1, box2):
	(x1, y1, w1, h1) = [int(v) for v in box1]
	(x2, y2, w2, h2) = [int(v) for v in box2]
	midX = (x1+h1/2)
	midY = (y1+w1/2)

	if x2 < midX < x2+h2 and y2 < midY < y2 + w2:
		return True	

	return False

def confidence(ids, frame):
	image_hsv = cv2.cvtColor(multiFaceTracker[ids].image, cv2.COLOR_BGR2HSV)
	try:
		hist = cv2.calcHist( [image_hsv], [0, 1], None, [180, 256], [0, 180, 0, 256] )
		cv2.normalize(hist, hist, 0, 255, cv2.NORM_MINMAX)
	except Exception as err:
		print(err)
		pass

	(x, y, w, h) = [int(v) for v in multiFaceTracker[ids].newboundingbox]
	face_hsv = cv2.cvtColor(frame[y:y+w, x:x+h], cv2.COLOR_BGR2HSV)
	try:
		hist1 = cv2.calcHist( [face_hsv], [0, 1], None, [180, 256], [0, 180, 0, 256] )
		cv2.normalize(hist1, hist1, 0, 255, cv2.NORM_MINMAX)
	except Exception as err:
		print(err)
		pass

	score = 1.0

	try:
		score = cv2.compareHist(hist, hist1, cv2.HISTCMP_BHATTACHARYYA)
	except Exception as err:
		print(err)
		pass
	if score > 0.72:
		print(str(multiFaceTracker[ids].identity) + " lost its tracker with score : " +str(score))
		return True
	return False

src = 0
cap = cv2.VideoCapture(src)

# Tracker
multiFaceTracker = {}

# face ids counter
faceID = 0

# frame counter
frameCounter = 0

keys = ''

faces_to_delete = []

# all the ids that are currently present in the frame
ids_to_check = []

font = cv2.FONT_HERSHEY_SIMPLEX

while cap.isOpened():

	ret, frame = cap.read()
	frameCounter +=1
#	frame = imutils.resize(frame, width=500)
	ids_to_check.clear()
	faces_to_delete.clear()
	
	if not ret:            # If no frame is left
		break

	for key in multiFaceTracker:
		ids_to_check.append(key)

	if not multiFaceTracker:
		faces = f_cascade.detectMultiScale(frame, scaleFactor=1.2, minNeighbors=5)
		profile = p_cascade.detectMultiScale(frame, scaleFactor=1.2, minNeighbors=5)
		
		for face in faces:
			(x, y, w, h) = [int(v) for v in face]

			faceID+=1
			multiFaceTracker[faceID] = Face(frame[y:y+w, x:x+h], (x, y, w, h), faceID)
			multiFaceTracker[faceID].tracker.init(frame, (x, y, w, h))
			
	else:
		for key in multiFaceTracker:
			(success, box) = multiFaceTracker[key].tracker.update(frame)
			
			if success:
				(x, y, w, h) = [int(v) for v in box]
				multiFaceTracker[key].newboundingbox = (x, y, w, h)

			
		if frameCounter%5 == 0:
			faces = f_cascade.detectMultiScale(frame, scaleFactor=1.2, minNeighbors=5)
			profile = p_cascade.detectMultiScale(frame, scaleFactor=1.2, minNeighbors=5)
			
			for ( x, y, w, h ) in profile:
				for ( x1, y1, w1, h1 ) in faces:
					if is_inside( (x, y, w, h), (x1, y1, w1, h1) ):
						# SOMETHING TO BE DONE
			
			for i, face in enumerate(faces):				
				for key in ids_to_check:
					(x, y, w, h) = [int(v) for v in face]
					if is_inside(face, multiFaceTracker[key].newboundingbox):
						multiFaceTracker[key].tracker.clear()
						multiFaceTracker[key].tracker = cv2.TrackerKCF_create()
						multiFaceTracker[key].tracker.init(frame, (x, y, w, h))
						faces_to_delete.append(i)
						ids_to_check.remove(key)
						break

			# initialise new faces with tracker
			for i, face in enumerate(faces):
				if i not in faces_to_delete:
					(x, y, w, h) = [int(v) for v in face]
					faceID+=1
					multiFaceTracker[faceID] = Face(frame[y:y+w, x:x+h], (x, y, w, h), faceID)
					multiFaceTracker[faceID].tracker.init(frame, (x, y, w, h))

			for ids in ids_to_check:
				if confidence(ids, frame):
					multiFaceTracker[ids].tracker.clear()
					del multiFaceTracker[ids]

			for key in multiFaceTracker:
				(x, y, w, h) = [int(v) for v in multiFaceTracker[key].newboundingbox]
				multiFaceTracker[key].image = frame[y:y+w, x:x+h]

		
		keys = 'Tracker ID : '
		for key in multiFaceTracker:
			(x, y, w, h) = [int(v) for v in multiFaceTracker[key].newboundingbox]
			face = frame[y:y+w, x:x+h]
#			multiFaceTracker[key].save(face)
			keys += str(key) + ' '
			cv2.rectangle(frame, (x, y), (x + w, y + h), (255, 255, 255), 2)
			cv2.putText(frame, str(multiFaceTracker[key].identity), (x, y), font, 0.5, (255, 255, 255), 1, cv2.LINE_AA)


	info = "No. of Tracker : {}".format(len(multiFaceTracker))
	cv2.putText(frame, info, (10, 420), font, 0.5, (255, 255, 255), 1, cv2.LINE_AA)
	info = "No. of Frames : {}".format(frameCounter)
	cv2.putText(frame, info, (10, 440), font, 0.5, (255, 255, 255), 1, cv2.LINE_AA)

	cv2.putText(frame, keys, (10, 460), font, 0.5, (255, 255, 255), 1, cv2.LINE_AA)
	cv2.imshow('Frame', frame)

	key = cv2.waitKey(1) & 0xFF
	
	if key == ord('q'):
		break

cap.release()
cv2.destroyAllWindows()
