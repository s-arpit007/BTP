import cv2
import numpy as np

class MFTracker:

	def __init__(self):

		# Number of faces seen so far
		self.GlobalNum = 0

		# Max. no. of faces allowed
		self.MaxFaceNum = 0   # need a value

		# No. of current faces in active and passive state
		self.CurrFaceNum = 0

		# Array of Face objects for current faces
		self.m_FaceSet = []

		# No. of faces in active state
		self.m_ActiveFaceNum = 0

		# Array of FaceID's for active faces
		self.m_ActiveFaceIndxVec = np.zeros(())  # need a dimension

		# No. of faces in passive state
		self.m_PassiveFaceNum = 0

		# Array of FaceID's for passive faces
		self.m_PassiveFaceIndxVec = np.zeros(())  # need a dimension

		# No. of detected faces in current frame
		self.m_CurrDetectedFaceNum = 0

		# Max. faces allowed to be detected in the frame
		self.m_MaxDetectedFaceNum = 0   # need a value

		# Array of windows tuple for all the detected faces
		self.m_DetectedFace_WindowArray = []

		# Array of Face detection state
		self.m_DetectedFace_StateArray = np.zeros(())  # need a dimension

		# Thresholds and Properties for use in Tracking
		self.m_ColMatchThreshold = 0   # need a value
		self.m_Tracked_Detected_FaceRegionOverlap_Threshold = 0   # need a value
		self.m_Tracked_Detected_FaceRegionOverlap_Matrix = np.zeros(())

class MFTracker_Face:

	def __init__(self):
		self.GlobalID = -1                                                # Global ID
		self.FaceID = -1                                                  # FaceID in the Tracker List
		self.Face_Window =                                                # Face Window tuple
		self.m_Face =                                                     # Face image object
		self.m_HSVHist =                                                  # HSV Histogram
		self.m_ColMatchVal =                                              # Color match Val
		self.m_FaceTracked = False                                        # True if Face is Well Tracked
		self.m_FaceWithinScene = False                                    # True if Face is Within Scene
		self.m_FaceDetectionState = 0                                     # Face detection State 0/1/2/3