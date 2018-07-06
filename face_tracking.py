import cv2
import numpy as np
import matplotlib.pyplot as plt


class Vision:

	def __init__(self, path_image=None, path_video=None):
		self.__image = path_image
		self.__video = path_video
		self.__haar_face = cv2.CascadeClassifier('opencv-3.4.1/data/haarcascades/haarcascade_frontalface_alt.xml')
		self.__track = path_image
		self.__roi = cv2.imread(self.__track)
		self.__hsv = cv2.cvtColor(self.__roi, cv2.COLOR_BGR2HSV)
		self.__mask = cv2.inRange(self.__hsv, np.array((0., 60.,32.)), np.array((180.,255.,255.)))
		self.__hist = cv2.calcHist([self.__hsv],[0], None,[180],[0,180])
		cv2.normalize(self.__hist,self.__hist,0,255,cv2.NORM_MINMAX)


	def open(self, media=None, save=False):
		if self.__image and media=='i':
			img = cv2.imread(self.__image)
			cv2.imshow('frame', img)
			k = cv2.waitKey(0)
			if k == 27 or k == ord('q'):
				cv2.destroyAllWindows()
			elif k == ord('s'):
				cv2.imwrite('image.jpg', img)
				cv2.destroyAllWindows()

		elif self.__video and media=='v':

			cap = cv2.VideoCapture(self.__video)
			fourcc = cv2.VideoWriter_fourcc('X','V','I','D')
			out = cv2.VideoWriter('output.avi',fourcc, 20.0, (640,480))

			while (cap.isOpened()):
				ret, frame = cap.read()

				if ret:
					cv2.imshow('Video', frame)
					if save:
						out.write(frame)
					k = cv2.waitKey(10) & 0xff
					if k == 27 or k == ord('q'):
						break
				else:
					break

			cap.release()
		else:
			print('Nothing to show.')
		cv2.destroyAllWindows()
		return True

	def detect(self, media=None, scaleFactor = 1.1, save=False):
		i=0
		if self.__image and media=='i':
			img = cv2.imread(self.__image)
			faces = self.__haar_face.detectMultiScale(img, scaleFactor=scaleFactor, minNeighbors=5)
			for (x, y, w, h) in faces:
				if save:
					i+=1
					face = img[y:y+h, x:x+w]
					cv2.imwrite('test%d.jpg'%i, face)
				cv2.rectangle(img, (x, y), (x+w, y+h), (0, 255, 0), 2)
			cv2.imshow('Photo', img)
			k = cv2.waitKey(0)
			if k == 27 or k == ord('q'):
				cv2.destroyAllWindows()
		elif self.__video and media=='v':
			i=0
			cap = cv2.VideoCapture(self.__video)
			fourcc = cv2.VideoWriter_fourcc('X','V','I','D')
			out = cv2.VideoWriter('output.avi',fourcc, 20.0, (640,480))

			while (cap.isOpened()):
				ret, frame = cap.read()

				if ret:
					faces = self.__haar_face.detectMultiScale(frame, scaleFactor=scaleFactor, minNeighbors=5)
					print(faces)
					for (x, y, w, h) in faces:
						if save:
							i+=1
							face = frame[y:y+h, x:x+w]
							cv2.imwrite('faces/arnab%d.jpg'%i, face)
						cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
					cv2.imshow('Video', frame)
					#out.write(frame)
					k = cv2.waitKey(10) & 0xff
					if k == 27 or k == ord('q'):
						break
				else:
					break
			cap.release()
		else:
			print('Nothing to show.')
		cv2.destroyAllWindows()
		return True

	'''
	Tracking is done using camshift method of OpenCV module.
	This method takes nothing as a input, all inputs are 
	already there in the object defination.
	After calling this method to an object, it plays the video 
	in self.__video (video_path) with tracking the object in the self.__image (image_path).
	'''

	def track(self):
		cap = cv2.VideoCapture(0)
		#r,h,c,w = 150,90,200,125
		while 1:
			ret ,frame = cap.read()
		#track_window = (c,r,w,h)
			face = self.__haar_face.detectMultiScale(frame, scaleFactor=1.2, minNeighbors=5)
			if len(face)>0:
				break
		for (x, y, w, h) in face:
			track_windo = (x, y, w, h)
			
		roi = frame[y:y+h, x:x+w]
		f_hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
		hist = cv2.calcHist([f_hsv],[0], None,[180],[0,180])
		cv2.normalize(hist, hist,0,255,cv2.NORM_MINMAX)
		term_crit = ( cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1 )

		while (1):
			ret ,frame = cap.read()
			if ret:
				hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
				#dst = cv2.calcBackProject([hsv], [0], self.__hist, [0,180], 1)
				dst = cv2.calcBackProject([hsv], [0], hist, [0,180], 1)

				ret, track_window = cv2.CamShift(dst, track_windo, term_crit)

				cv2.rectangle(frame, (track_window[0], track_window[1]), (track_window[0]+track_window[2], track_window[3]+track_window[1]), (0, 255, 0), 1)
				cv2.imshow('Play', frame)
				
				k = cv2.waitKey(10) & 0xff
				if k == 27 or k == ord('q'):
					break
			else:
				break

		cv2.destroyAllWindows()
		cap.release()
		return True
