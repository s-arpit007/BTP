import cv2
import numpy as np

class Speaker:

	def __init__(self, image=None, track_window=(200, 150, 125, 90), roll=0):
		self.roll = roll
		self.image = image
		self.window = track_window
		self.hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
		self.hist = cv2.calcHist([self.hsv], [0], None, [180], [0, 180])
		cv2.normalize(self.hist, self.hist, 0, 255, cv2.NORM_MINMAX)

	def __str__(self):
		return str(self.roll)
	
src = 'videos/2_to_8.mp4'
speakers = []
cap = cv2.VideoCapture(src)
haar_face = cv2.CascadeClassifier('opencv-3.4.1/data/haarcascades/haarcascade_frontalface_alt.xml')

# ==============  initializing faces ===================

while 1:
	ret, frame = cap.read()
	faces = haar_face.detectMultiScale(frame, scaleFactor=1.1, minNeighbors=5)
	if len(faces) > 1:
		break
roll = 0

for (x, y, w, h) in faces:
	track_window = (x, y, w, h)
	roi = frame[y:y+h, x:x+w]
	roll += 1
	speakers.append( Speaker( roi, track_window, roll ) )

# ====================================================

term_crit = ( cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1 )
font = cv2.FONT_HERSHEY_SIMPLEX

while 1:
	ret, frame = cap.read()
	if ret:
		hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
		for speaker in speakers:
			dst = cv2.calcBackProject([hsv], [0], speaker.hist, [0,180], 1,)
			ret, track_window = cv2.CamShift(dst, speaker.window, term_crit)

			pts = cv2.boxPoints(ret)
			pts = np.int0(pts)
			frame = cv2.polylines(frame,[pts],True, (255, 255, 255),2)
			cv2.putText(frame, str(speaker.roll), (int(ret[0][0]), int(ret[0][1])), font, 0.5, (255, 255, 255), 1)
		cv2.imshow('Frame', frame)
		k = cv2.waitKey(10) & 0xff
		if k == 27 or k == ord('q'):
			break
	else:
		break

cap.release()
cv2.destroyAllWindows()