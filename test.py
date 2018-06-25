import cv2


src = 'videos/TIMES_NOW_4_10_2016.mp4'
cap = cv2.VideoCapture(src)
f_cascade = cv2.CascadeClassifier('opencv-3.4.1/data/haarcascades/haarcascade_frontalface_alt.xml')
_, frame1 = cap.read()

print("Opening Camera...")
while 1:
	ret ,frame = cap.read()

	# detect faces only if shot has changed
	#if shot_detection(frame, frame1) > 0.4:
	#faces = f_cascade.detectMultiScale(frame, scaleFactor=1.2, minNeighbors=5)

	if ret:
		#for (x, y, w, h) in faces:
		#	cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
		cv2.imshow('Play', frame)
		
		k = cv2.waitKey(1) & 0xff
		if k == 27 or k == ord('q'):
			break
	else:
		break
print("Closing Camera...")
cv2.destroyAllWindows()
cap.release()