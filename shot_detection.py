"""
 * Make a plot between Bhattacharya Coefficient as y-axis & Time (in sec.) as x-axis
 * Compare 2 subsequent frames using Bhattacharya Coeff. by dividing each frames in 9 parts

Usage:  python shot_detection.py
"""
import cv2
import matplotlib.pyplot as plt
import numpy as np

# duration_per_frame = 164/4086

duration_per_frame = 105 / 2619
# duration_per_frame = 3250/81263
# [90:350, 20:620]]

src = 'videos/2_to_8.mp4'
cap = cv2.VideoCapture(src)
ret, frame1 = cap.read()

frame1 = cv2.cvtColor(frame1, cv2.COLOR_BGR2HSV)

# ================  Divide a frame into 9 parts =========================
hist11 = cv2.calcHist([frame1[0:150, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256])
hist12 = cv2.calcHist([frame1[0:150, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256])
hist13 = cv2.calcHist([frame1[0:150, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256])
hist14 = cv2.calcHist([frame1[151:250, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256])
hist15 = cv2.calcHist([frame1[151:250, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256])
hist16 = cv2.calcHist([frame1[151:250, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256])
hist17 = cv2.calcHist([frame1[251:, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256])
hist18 = cv2.calcHist([frame1[251:, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256])
hist19 = cv2.calcHist([frame1[251:, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256])

hist = []

for i in range(1, 4):
    for j in range(1, 4):
        if i==3:
            i = i - 1
        if j==3:
            j = j - 1
        hist.append(cv2.calcHist([frame1[0:150*i, 0:200*j]], [0, 1], None, [180, 256], [0, 180, 0, 256]))

cv2.normalize(hist11, hist11, 0, 255, cv2.NORM_MINMAX)
cv2.normalize(hist12, hist12, 0, 255, cv2.NORM_MINMAX)
cv2.normalize(hist13, hist13, 0, 255, cv2.NORM_MINMAX)
cv2.normalize(hist14, hist14, 0, 255, cv2.NORM_MINMAX)
cv2.normalize(hist15, hist15, 0, 255, cv2.NORM_MINMAX)
cv2.normalize(hist16, hist16, 0, 255, cv2.NORM_MINMAX)
cv2.normalize(hist17, hist17, 0, 255, cv2.NORM_MINMAX)
cv2.normalize(hist18, hist18, 0, 255, cv2.NORM_MINMAX)
cv2.normalize(hist19, hist19, 0, 255, cv2.NORM_MINMAX)
# ========================================================================

# B_coeff = {}
x = []
y = []
frame_count = 1

print("Opening Video...")
while 1:
    ret, frame2 = cap.read()

    if ret is not None:
        frame_count += 1
        # cv2.imshow('Frame', frame2)
        frame2 = cv2.cvtColor(frame2, cv2.COLOR_BGR2HSV)

        hist21 = cv2.calcHist([frame2[0:150, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256])
        hist22 = cv2.calcHist([frame2[0:150, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256])
        hist23 = cv2.calcHist([frame2[0:150, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256])
        hist24 = cv2.calcHist([frame2[151:250, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256])
        hist25 = cv2.calcHist([frame2[151:250, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256])
        hist26 = cv2.calcHist([frame2[151:250, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256])
        hist27 = cv2.calcHist([frame2[251:, 0:200]], [0, 1], None, [180, 256], [0, 180, 0, 256])
        hist28 = cv2.calcHist([frame2[251:, 201:400]], [0, 1], None, [180, 256], [0, 180, 0, 256])
        hist29 = cv2.calcHist([frame2[251:, 401:]], [0, 1], None, [180, 256], [0, 180, 0, 256])

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

        d = (d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8 + d9) / 9

        # B_coeff[frame_count] = 1 - d

        x.append(frame_count * duration_per_frame)
        y.append(1 - d)

        frame1 = frame2
        hist11 = hist21
        hist12 = hist22
        hist13 = hist23
        hist14 = hist24
        hist15 = hist25
        hist16 = hist26
        hist17 = hist27
        hist18 = hist28
        hist19 = hist29

        k = cv2.waitKey(10) & 0xff
        if k == 27 or k == ord('q'):
            print('Closing Video...')
            break
    else:
        break

cv2.destroyAllWindows()
cap.release()
print(frame_count)
plt.plot(x, y)
plt.xlabel("Time (in sec.)")
plt.ylabel("Coefficient")
plt.show()
