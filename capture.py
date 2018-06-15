import numpy as np
import cv2


def detect_faces(i, f_cascade, colored_img, scaleFactor = 1.1):
	img_copy = colored_img.copy()          

	gray = img_copy
	faces = f_cascade.detectMultiScale(gray, scaleFactor=scaleFactor, minNeighbors=5);          

	for (x, y, w, h) in faces:
		i+=1
		face = img_copy[y:y+h, x:x+w]
		cv2.imwrite('arpit/arpit%3d.jpg'%i, face)
		cv2.rectangle(img_copy, (x, y), (x+w, y+h), (0, 255, 0), 2)              
		#print(x, y, w, h)
	return img_copy, i
cap = cv2.VideoCapture('videos/arnab_om.mp4')

i=0
# Define the codec and create VideoWriter object
#fourcc = cv2.VideoWriter_fourcc('X','V','I','D')
#out = cv2.VideoWriter('output.avi',fourcc, 20.0, (640,480))

haar_face_cascade = cv2.CascadeClassifier('opencv-3.4.1/data/haarcascades/haarcascade_frontalface_alt.xml')

while(cap.isOpened()):
	ret, frame = cap.read()
	if ret==True:
		
		# write the frame
		faces, i = detect_faces(i, haar_face_cascade, frame)
		#out.write(frame)
		cv2.imshow('frame',faces)
		k = cv2.waitKey(1) & 0xff
		if k == ord('q'):
			break
	else:
		break

# Release everything if job is finished
cap.release()
#out.release()
cv2.destroyAllWindows()
