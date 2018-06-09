import cv2
import matplotlib.pyplot as plt
import numpy as np

src = 'videos/2_to_8.mp4'
duration = 105
# 2619  1749
total_frames = 2619
per_frame = duration/total_frames

cap = cv2.VideoCapture(src)
ret, frame1 = cap.read()
frame1 = cv2.cvtColor(frame1, cv2.COLOR_BGR2HSV)

hist1 = cv2.calcHist( [frame1[90:350, 20:620]], [0, 1], None, [180, 256], [0, 180, 0, 256] )

#hist1 = cv2.calcHist([frame1[90:350, 20:620]], [0, 1, 2], None, [8, 8, 8], [0, 256, 0, 256, 0, 256])
cv2.normalize(hist1, hist1, 0, 255, cv2.NORM_MINMAX)
B_coeff = {}
frame_count = 1
x = []
y = []

print('Opening Video...')
while 1:
    ret, frame2 = cap.read()
    if ret:
        frame_count += 1
        frame2 = cv2.cvtColor(frame2, cv2.COLOR_BGR2HSV)
        hist2 = cv2.calcHist( [frame2[90:350, 20:620]], [0, 1], None, [180, 256], [0, 180, 0, 256] )
        cv2.normalize(hist2, hist2, 0, 255, cv2.NORM_MINMAX)

        d = cv2.compareHist(hist1, hist2, cv2.HISTCMP_BHATTACHARYYA)
        B_coeff[frame_count*per_frame] = 1 - d

        frame1 = frame2
        hist1 = hist2
        
        x.append(frame_count*per_frame)
        y.append(1 - d)

        k = cv2.waitKey(10) & 0xff
        if k == 27 or k == ord('q'):
            print('Closing Video...')
            break
    else:
        break

cv2.destroyAllWindows()
cap.release()
print('Video Closed.')
print('Plotting.....')
print(frame_count)
plt.plot(x, y)
plt.show()
