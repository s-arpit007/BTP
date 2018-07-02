import cv2
import helper as hp
import numpy as np

src = 'videos/TIMES_NOW_4_10_2016.mp4'
cap = cv2.VideoCapture(src)
f_cascade = cv2.CascadeClassifier('opencv-3.4.1/data/haarcascades/haarcascade_frontalface_alt.xml')
#profile_cascade = cv2.CascadeClassifier('opencv-3.4.1/data/haarcascades/haarcascade_profileface.xml')


mftracker = hp.MFTracker(maxfacenum=5, colmatchthreshold=0.4, faceregionthreshold=0.4)
term_crit = ( cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1 )

print("Opening Camera...")
while 1:
	_ ,frame = cap.read()
	mftracker.m_DetectedFace_WindowArray = f_cascade.detectMultiScale(frame, scaleFactor=1.2, minNeighbors=5)

	if _:

		mftracker.CurrFrame = frame
		mftracker.CurrHSVFrame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
		mftracker.m_CurrDetectedFaceNum = len(mftracker.m_DetectedFace_WindowArray)

		hp.multifacetracker(mftracker)

		cv2.imshow('Play', mftracker.CurrFrame)

		k = cv2.waitKey(1) & 0xff
		if k == 27 or k == ord('q'):
			break
	else:
		break
print("Closing Camera...")
cv2.destroyAllWindows()
cap.release()