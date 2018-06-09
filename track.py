import scipy
import cv2
import numpy as np

class Person:
	id = 0
	def __init__(self, image=None, track_window=(200, 150, 125, 90)):
		type(self).id +=1
		self.image = image
		self.hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
		self.mask = cv2.inRange(self.hsv, np.array((0., 60.,32.)), np.array((180.,255.,255.)))
		self.hist = cv2.calcHist([self.hsv],[0],None,[180],[0,180])
		self.window = track_window
		cv2.normalize(self.hist,self.hist,0,255,cv2.NORM_MINMAX)
	
	def __str__(self):
		return str(self.id)


cap = cv2.VideoCapture('videos/2_to_8.mp4')
f_cascade = cv2.CascadeClassifier('opencv-3.4.1/data/haarcascades/haarcascade_frontalface_alt.xml')
faces = None

while 1:
	ret, frame = cap.read()
	faces = f_cascade.detectMultiScale(frame, scaleFactor=1.1, minNeighbors=5)
	if len(faces) > 1:
		print("it's empty")
		break

cv2.imshow('First', frame)

speakers = [Person(frame[y:y+h, x:x+w], (x, y, w, h)) for (x, y, w, h) in faces]

'''
for (x, y, w, h) in faces:
	track_window = (x,y,w,h)
	roi = frame[y:y+h, x:x+w]
	speakers.append(Person(roi, track_window))
	print("Done.")
'''
#print(speakers[0], speakers[1])


'''
else:
	r,h,c,w = 150,90,200,125
	track_window = (c,r,w,h)
	roi = cv2.imread('arpit/arpit183.jpg')
'''

term_crit = ( cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1 )

print("Opening Camera...")
while (1):
	ret ,frame = cap.read()
	if ret:
		hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
		
		for speaker in speakers:
			dst = cv2.calcBackProject([hsv], [0], speaker.hist, [0,180], 1)
			ret, track_window = cv2.CamShift(dst, speaker.window, term_crit)
			
			pts = cv2.boxPoints(ret)
			pts = np.int0(pts)
			frame = cv2.polylines(frame,[pts],True, (255, 0, 0),2)
			font = cv2.FONT_HERSHEY_SIMPLEX
			cv2.putText(frame, str(speaker.id), (int(ret[0][0]), int(ret[0][1])) ,font, 0.3, (255, 255, 255), 1)
		cv2.imshow('Play', frame)
		cv2.imshow('Crop', frame[90:350, 20:620])

		k = cv2.waitKey(10) & 0xff
		if k == 27 or k == ord('q'):
			print("Closing Camera...")
			break
	else:
		break

cv2.destroyAllWindows()
cap.release()
