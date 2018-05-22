import numpy as np
import cv2

cap = cv2.VideoCapture('videos/arnab_om.mp4')

# ====================OM Puri======================

r,h,c,w = 250,90,400,125
track_window = (c,r,w,h)

frame = cv2.imread('faces/127.jpg', 1)

roi = frame
hsv_roi =  cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
mask = cv2.inRange(hsv_roi, np.array((0., 60.,32.)), np.array((180.,255.,255.)))
roi_hist = cv2.calcHist([hsv_roi],[0],mask,[180],[0,180])
cv2.normalize(roi_hist,roi_hist,0,255,cv2.NORM_MINMAX)

# ======================Arnab======================

r,h,c,w = 250,90,400,125
track_window_arnab = (c,r,w,h)

frame_arnab = cv2.imread('faces/123.jpg')
roi_arnab = frame_arnab
hsv_roi_arnab =  cv2.cvtColor(frame_arnab, cv2.COLOR_BGR2HSV)
mask_arnab = cv2.inRange(hsv_roi_arnab, np.array((0., 60.,32.)), np.array((180.,255.,255.)))
roi_hist_arnab = cv2.calcHist([hsv_roi_arnab],[0],mask_arnab,[180],[0,180])
cv2.normalize(roi_hist_arnab,roi_hist_arnab,0,255,cv2.NORM_MINMAX)
cv2.imshow('hist', roi_hist_arnab)
# ================================================

# Setup the termination criteria, either 10 iteration or move by atleast 1 pt
term_crit = ( cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1 )

while(1):
	ret ,frame = cap.read()

	if ret == True:
		hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
		#dst = cv2.calcBackProject([hsv],[0],roi_hist,[0,180],1)
		dst_arnab = cv2.calcBackProject([hsv],[0],roi_hist_arnab,[0,180],1)

		# apply meanshift to get the new location
		#ret, track_window = cv2.CamShift(dst, track_window, term_crit)

		ret_arnab, track_window_arnab = cv2.CamShift(dst_arnab, track_window_arnab, term_crit)
		
		# Draw it on image
		#pts = cv2.boxPoints(ret)
		#pts = np.int0(pts)
		#img2 = cv2.polylines(hsv,[pts],True, (255, 0, 0),2)
		
		pts_a = cv2.boxPoints(ret_arnab)
		pts_a = np.int0(pts_a)
		img2 = cv2.polylines(hsv,[pts_a],True, (0, 255, 0),2)

		cv2.imshow('img2',img2)

		k = cv2.waitKey(10) & 0xff
		if k == 27:
			break
		else:
			cv2.imwrite(chr(k)+".jpg",img2)

	else:
		break

cv2.destroyAllWindows()
cap.release()

'''
from face_tracking import Vision
path_i = 'iniesta.png'
path_v = 'videos/arnab_om.mp4'
obj = Vision(path_i, path_v)
obj.track()

'''