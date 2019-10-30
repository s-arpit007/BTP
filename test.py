import cv2
import helper as hp
import numpy as np

src = 'videos/TIMES_NOW_4_10_2016.mp4'
cap = cv2.VideoCapture(src)
f_cascade = cv2.CascadeClassifier('opencv-3.4.1/data/haarcascades/haarcascade_frontalface_alt.xml')

if cv2.__version__.startswith('4.'):
	print("The version of the opencv is 4.*.*")
	use_spatial_propagation = False
	inst = cv2.DISOpticalFlow.create(cv2.DISOPTICAL_FLOW_PRESET_MEDIUM)
	inst.setUseSpatialPropagation(use_spatial_propagation)
	inst.setFinestScale(2)
	inst.setVariationalRefinementIterations(8)


#profile_cascade = cv2.CascadeClassifier('opencv-3.4.1/data/haarcascades/haarcascade_profileface.xml')


mftracker = hp.MFTracker(maxfacenum=20, colmatchthreshold=0.4, faceregionthreshold=0.4)
term_crit = ( cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1 )

print("Opening Camera...")
while 1:
	_ ,frame = cap.read()
	mftracker.m_DetectedFace_WindowArray.clear()
	faces = f_cascade.detectMultiScale(frame, scaleFactor=1.2, minNeighbors=5)
	for face in faces:
		mftracker.m_DetectedFace_WindowArray.append(( face[0], face[1], face[2], face[3] ))
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
