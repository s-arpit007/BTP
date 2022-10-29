import cv2
import numpy as np

class Person:
    
	def __init__(self, image=None, track_window=(200, 150, 125, 90), roll=0):
		self.roll = roll
		self.image = image
		self.hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
		self.hist = cv2.calcHist([self.hsv], [0], None, [180], [0, 180])
		cv2.normalize(self.hist, self.hist, 0, 255, cv2.NORM_MINMAX)
		self.window = track_window
		self.active = False

	def __str__(self):
		return str(self.roll)
# =============== new ==========
class Face:

	def __init__(self, hist=None, hsv=None, window=(200, 150, 125, 90), image=None):
		self.f_image = image
		self.f_hsv = hsv
		self.f_hist = hist
		self.f_window = window
		self.identity = -1
		
	def delete_face(self):
		self.roll = 0
		self.image = None
		self.hsv = None
		self.hist = None
		self.window = None
		self.active = False

# ==============================
src = 'videos/TIMES_NOW_4_10_2016.mp4'
#src = 'videos/2_to_8.mp4'
cap = cv2.VideoCapture(src)
f_cascade = cv2.CascadeClassifier('opencv-3.4.1/data/haarcascades/haarcascade_frontalface_alt.xml')
faces = None
i = []
j = 0
allFaces = []
speakers = []
roll = 0
frame1 = cap.read()

term_crit = ( cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1 )
font = cv2.FONT_HERSHEY_SIMPLEX

print("Opening Camera...")
while 1:
	ret ,frame = cap.read()
	if ret:
		
		faces = f_cascade.detectMultiScale(frame, scaleFactor=1.1, minNeighbors=5)               # new
		if len(faces) > 0:
			for (x, y, w, h) in faces:
				f_hsv = cv2.cvtColor(frame[y-10:y+h, x:x+w], cv2.COLOR_BGR2HSV)
				f_hist = cv2.calcHist([f_hsv], [0], None, [180], [0,180])
				cv2.normalize( f_hist, f_hist, 0, 255, cv2.NORM_MINMAX )

				allFaces.append( Face( f_hist, f_hsv, (x, y-10, w, h+10), frame[y-10:y+h, x:x+w] ) )
			# ========================================================================
			if len(speakers) == 0:
				for face in allFaces:
					roll += 1
					face.identity = roll
					speakers.append( Person( face.f_image, face.f_window, roll ) )
			else:
				for face in allFaces:
					d = 0
					coeff = 1
					for speaker in speakers:
						d = cv2.compareHist(face.f_hist, speaker.hist, cv2.HISTCMP_BHATTACHARYYA)
						if coeff > d and d < 0.4 :
							coeff = d
							face.identity = speaker.roll
							i.append(speaker.roll)

					for speaker in speakers:
						if speaker.roll == face.identity:
							speaker.image = face.f_image
							speaker.hist = face.f_hist
							speaker.hsv = face.f_hsv
							speaker.active = True

					# if still no face has matched
					if face.identity == -1:
						roll += 1
						speakers.append( Person( face.f_image, face.f_window, roll ) )
			# ========================================================================

#			hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
			for face in allFaces:
				j+=1
#				dst = cv2.calcBackProject([hsv], [0], speaker.hist, [0,180], 1)
#				ret, track_window = cv2.CamShift(dst, speaker.window, term_crit)
#				for face in allFaces:
#					if face.identity == speaker.roll:
#						break

#				(x1, x2, x3, x4) = (int(track_window[0]), int(track_window[1]), int(track_window[2]), int(track_window[3]))

#				h1 = cv2.calcHist([ cv2.cvtColor(frame[x2:x2+x4, x1:x1+x3], cv2.COLOR_BGR2HSV) ], [0], None, [180], [0, 180])
#				cv2.normalize(h1, h1, 0, 255, cv2.NORM_MINMAX)
#				h2 = cv2.calcHist([face.f_hsv], [0], None, [180], [0, 180] )
#				cv2.normalize(h2, h2, 0, 255, cv2.NORM_MINMAX)

				(x1, x2, x3, x4) = face.f_window
				#cv2.imwrite('faces/%d'%j+'test%d.jpg'%face.identity, frame[x2:x2+x4, x1:x1+x3])
#				if cv2.compareHist(h1, h2, cv2.HISTCMP_BHATTACHARYYA) < 0.5:
				cv2.rectangle(frame, (x1, x2), (x1+x3, x2+x4), (0, 255, 0), 2)
				cv2.putText(frame, str(face.identity), (x1, x2) ,font, 1, (255, 255, 255), 2)
		
		cv2.imshow('Play', frame)
		allFaces.clear()
		i.clear()
		k = cv2.waitKey(1) & 0xff
		if k == 27 or k == ord('q'):
			break
	else:
		break
print("Closing Camera...")
cv2.destroyAllWindows()
cap.release()
