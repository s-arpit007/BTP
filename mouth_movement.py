import cv2, dlib, time
import numpy as np
from matplotlib import pyplot as plt
from utilities import *
from statistics import median

src = 0
cam = cv2.VideoCapture(src)
print("[INFO] Opening camera...")
time.sleep(1.0)

_, frame = cam.read()
old_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
font = cv2.FONT_HERSHEY_SIMPLEX

# model = load_gender_model()
activity = []
avg = []
med = []
X = []
prev_ratio = 0
frameCounter = 0
fps = 0
print("[INFO] Starting video...")

while cam.isOpened():
    _, frame = cam.read()
    frameCounter += 1

    if not _:
        break

    frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    flow = cv2.calcOpticalFlowFarneback(old_gray, frame_gray, None, 0.5, 3, 20, 3, 5, 1.1, 1)
    fx, fy = flow[:, :, 0], flow[:, :, 1]
    v = np.sqrt(fx * fx + fy * fy)

    old_gray = frame_gray.copy()
    faces = detect_face(frame, confidence=0.4, mode="CNN")

    for face in faces:
        (x, y, w, h) = face
        (startX, startY, endX, endY) = (x, y, x + w, y + h)

        # check detect_face for returning value

        cv2.rectangle(frame, (startX, startY), (endX, endY), (255, 255, 255), 2)
        heat_map = np.zeros_like(frame[startY:endY, startX:endX])
        heat_map[..., 1] = 0
        heat_map[..., 0] = 0
        heat_map[..., 2] = cv2.normalize(abs(fy[startY:endY, startX:endX]), None, 0, 255, cv2.NORM_MINMAX)

        m = heat_map[..., 2]
        image = cv2.resize(m, (120, 160), interpolation=cv2.INTER_CUBIC)
        (h, w) = image.shape
        y_vertical = gaussian(left_r=-60, right_r=60, sigma=30)
        y_horizontal = gaussian(left_r=-50, right_r=30, sigma=20)

        score2 = np.sum(y_vertical * image[0:int(h / 2), 0:w].T)
        score1 = np.sum(y_vertical * image[int(h / 2):h, 0:w].T)

        score2 += np.sum(y_horizontal * image[0:int(h / 2), 0:w])
        score1 += np.sum(y_horizontal * image[int(h / 2):h, 0:w])

        print(min(15, score1 / score2), score2, score1)
        cv2.imshow('image', image)
        ratio = min(15, score1 / score2)
        fps = cam.get(cv2.CAP_PROP_FPS)

        activity.append(ratio)
        if frameCounter % 10 == 0:
            X.append(frameCounter)
            avg.append(sum(activity[-int(fps + 10):]) / (fps + 10))
            med.append(median(activity[-int(fps + 10):]))

        if sum(activity[-int(fps + 10):]) > fps * 3:
            cv2.circle(frame, (x, y), 5, (0, 0, 255), -1)

    info = "No. of Frames : {}".format(frameCounter)
    cv2.putText(frame, info, (10, frame.shape[0] - 60), font, 0.5, (255, 255, 255), 1, cv2.LINE_AA)
    info = "FPS : {}".format(fps)
    cv2.putText(frame, info, (10, frame.shape[0] - 40), font, 0.5, (255, 255, 255), 1, cv2.LINE_AA)
    info = "Activity for 10 frames : {}".format(int(sum(activity[-int(fps + 10):])))
    cv2.putText(frame, info, (10, frame.shape[0] - 20), font, 0.5, (255, 255, 255), 1, cv2.LINE_AA)
    cv2.imshow("Cam", frame)

    key = cv2.waitKey(1) & 0xFF

    if key == ord('q'):
        break

print("[INFO] Closing camera...")
cam.release()
cv2.destroyAllWindows()