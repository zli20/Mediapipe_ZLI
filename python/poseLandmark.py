import mediapipe as mp
import cv2
import numpy as np

mp_drawing = mp.solutions.drawing_utils  # 画图
mp_drawing_styles = mp.solutions.drawing_styles
mp_holistic = mp.solutions.holistic  # 全身识别跟踪

mp_pose = mp.solutions.pose  # 姿态估计

path = r"D:\svn_file\HSCore\MotionCapture\data\taiji.mp4"
cap = cv2.VideoCapture(path)

holistic = mp_holistic.Holistic(static_image_mode=False,
                                smooth_landmarks=True,
                                min_detection_confidence=0.5,
                                min_tracking_confidence=0.5)

pose = mp_pose.Pose(static_image_mode=False,
                    smooth_landmarks=True,
                    min_detection_confidence=0.5,
                    min_tracking_confidence=0.5)

while cap.isOpened():
    success, image = cap.read()
    if not success:
        print("Ignoring empty camera frame.")
        break
    image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # results = holistic.process(image_rgb)
    results = pose.process(image_rgb)
    mp_drawing.draw_landmarks(image, results.pose_landmarks, mp_holistic.POSE_CONNECTIONS)
    if results.pose_landmarks:
        for index, landmarks in enumerate(results.pose_landmarks.landmark):
            print(index,landmarks)

    cv2.imshow("frame", image)
    cv2.waitKey(1)
