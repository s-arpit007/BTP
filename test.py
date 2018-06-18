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

# checks for the shot change in the video
def shot_change(frame1, frame2):
	frame1 = cv2.cvtColor(frame1, cv2.COLOR_BGR2HSV)
	frame2 = cv2.cvtColor(frame2, cv2.COLOR_BGR2HSV)

	hist11 = cv2.calcHist( [frame1[0:150, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist12 = cv2.calcHist( [frame1[0:150, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist13 = cv2.calcHist( [frame1[0:150, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist14 = cv2.calcHist( [frame1[151:250, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist15 = cv2.calcHist( [frame1[151:250, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist16 = cv2.calcHist( [frame1[151:250, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist17 = cv2.calcHist( [frame1[251:, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist18 = cv2.calcHist( [frame1[251:, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist19 = cv2.calcHist( [frame1[251:, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256] )

	cv2.normalize(hist11, hist11, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist12, hist12, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist13, hist13, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist14, hist14, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist15, hist15, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist16, hist16, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist17, hist17, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist18, hist18, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist19, hist19, 0, 255, cv2.NORM_MINMAX)

	hist21 = cv2.calcHist( [frame2[0:150, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist22 = cv2.calcHist( [frame2[0:150, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist23 = cv2.calcHist( [frame2[0:150, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist24 = cv2.calcHist( [frame2[151:250, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist25 = cv2.calcHist( [frame2[151:250, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist26 = cv2.calcHist( [frame2[151:250, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist27 = cv2.calcHist( [frame2[251:, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist28 = cv2.calcHist( [frame2[251:, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
	hist29 = cv2.calcHist( [frame2[251:, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256] )

	cv2.normalize(hist21, hist21, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist22, hist22, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist23, hist23, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist24, hist24, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist25, hist25, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist26, hist26, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist27, hist27, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist28, hist28, 0, 255, cv2.NORM_MINMAX)
	cv2.normalize(hist29, hist29, 0, 255, cv2.NORM_MINMAX)


	d1 = cv2.compareHist(hist11, hist21, cv2.HISTCMP_BHATTACHARYYA)
	d2 = cv2.compareHist(hist12, hist22, cv2.HISTCMP_BHATTACHARYYA)
	d3 = cv2.compareHist(hist13, hist23, cv2.HISTCMP_BHATTACHARYYA)
	d4 = cv2.compareHist(hist14, hist24, cv2.HISTCMP_BHATTACHARYYA)
	d5 = cv2.compareHist(hist15, hist25, cv2.HISTCMP_BHATTACHARYYA)
	d6 = cv2.compareHist(hist16, hist26, cv2.HISTCMP_BHATTACHARYYA)
	d7 = cv2.compareHist(hist17, hist27, cv2.HISTCMP_BHATTACHARYYA)
	d8 = cv2.compareHist(hist18, hist28, cv2.HISTCMP_BHATTACHARYYA)
	d9 = cv2.compareHist(hist19, hist29, cv2.HISTCMP_BHATTACHARYYA)

	d = (d1+d2+d3+d4+d5+d6+d7+d8+d9)/9
	return 1-d
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
			#  new
			#  Making a list of faces in a frame with their hsv, histogram and image
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
		k = cv2.waitKey(10) & 0xff
		if k == 27 or k == ord('q'):
			break
	else:
		break
print("Closing Camera...")
cv2.destroyAllWindows()
cap.release()