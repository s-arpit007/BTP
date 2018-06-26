import cv2
import numpy as np

class MFTracker:

	def __init__(self, maxfacenum=0, colmatchthreshold=0, faceregionthreshold=0):

		# Number of faces seen so far
		self.GlobalNum = 0

		# Max. no. of faces allowed
		self.MaxFaceNum = maxfacenum

		# No. of current faces in active and passive state
		self.CurrFaceNum = 0

		# Array of MFTracker_Face objects for current faces
		self.m_FaceSet = []	

		# No. of faces in active state
		self.m_ActiveFaceNum = 0

		# Array of FaceID's for active faces
		self.m_ActiveFaceIndxVec = np.zeros((self.MaxFaceNum))

		# No. of faces in passive state
		self.m_PassiveFaceNum = 0

		# Array of FaceID's for passive faces
		self.m_PassiveFaceIndxVec = np.zeros((self.MaxFaceNum))

		# No. of detected faces in current frame
		self.m_CurrDetectedFaceNum = 0

		# Array of windows tuple for all the detected faces
		self.m_DetectedFace_WindowArray = []

		# Array of Face detection state
		self.m_DetectedFace_StateArray = np.zeros((self.MaxFaceNum))

		# Thresholds and Properties for use in Tracking
		self.m_ColMatchThreshold = colmatchthreshold
		self.m_Tracked_Detected_FaceRegionOverlap_Threshold = faceregionthreshold
		self.m_Tracked_Detected_FaceRegionOverlap_Matrix = np.zeros((self.MaxFaceNum, self.MaxFaceNum), dtype=float)

	def __str__(self):
		return "MultifaceTracker with "+str(self.m_PassiveFaceNum)+" passive detected faces and "+str(self.m_ActiveFaceNum)+" active detected faces."

class MFTracker_Face:

	def __init__(self, frame=None, (x, y, w, h)=()):
		self.m_FaceGlobalID = -1                                               # Global ID
		self.m_FaceID = -1                                                     # FaceID in the Tracker List
		self.m_Face_Window = (x, y, w, h)                                      # Face Window tuple
		self.m_Face = frame                                      # Face image object
		self.m_FaceHSV = cv2.cvtColor(self.m_Face, cv2.COLOR_BGR2HSV)
		self.m_HSVHist = cv2.calcHist([self.m_FaceHSV], [0], None, [180], [0, 180])
		cv2.normalize(self.HSVHist, self.HSVHist, 0, 255, cv2.NORM_MINMAX)     # HSV Histogram
		self.m_ColMatchVal = 1.0                                               # Color match Val
		self.m_FaceTracked = False                                             # True if Face is Well Tracked
		self.m_FaceWithinScene = False                                         # True if Face is Within Scene
		self.m_FaceDetectionState = 0                                          # Face detection State 0/1/2/3