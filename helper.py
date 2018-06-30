import cv2
import numpy as np

# Structure for individual Face

class MFTracker_Face:

	def __init__(self, (x, y, w, h)=(-1, -1, -1, -1)):
		self.m_FaceGlobalID = -1                                               # Global ID
		self.m_FaceID = -1                                                     # FaceID in the Tracker List
		self.m_Face_Window = (x, y, w, h)                                      # Face Window tuple
		self.m_Face = None                                                     # Face image object
		self.m_FaceHSV = None
		self.m_HSVHist = None
		self.m_ColMatchVal = 1.0                                               # Color match Val at Tracked position
		self.m_FaceTracked = False                                             # True if Face is Well Tracked
		self.m_FaceWithinScene = False                                         # True if Face is Within Scene
		self.m_FaceDetectionState = 0                                          # Face detection State 0/1/2/3
		self.m_FaceDataReady = False
		self.m_TimeSpan_FaceTrackLoss = -1
		self.m_FaceIsolated = False


	def reset_mftrackerFace_Data(self):
		self.m_FaceGlobalID = -1
		self.m_FaceID = -1
		self.m_Face_Window = (-1, -1, -1, -1)
		self.m_FaceHSV = None
		self.m_Face = None
		self.m_HSVHist = None
		self.m_ColMatchVal = -1
		self.m_FaceTracked = False
		self.m_FaceIsolated = False
		self.m_FaceWithinScene = False
		self.m_FaceDetectionState = 0
		self.m_TimeSpan_FaceTrackLoss = -1
		self.m_FaceDataReady = False


	def set_mftrackerFace_Data(self, faceGlobalID, faceID, window, faceDetectionState):
		self.m_FaceDataReady = True
		self.m_FaceGlobalID = faceGlobalID
		self.m_FaceID = faceID
		self.m_Face_Window = window
		self.m_Face = frame[window[1]:window[1]+window[3], window[0]:window[0]+window[2]]
		self.m_FaceHSV = cv2.cvtColor(self.m_Face, cv2.COLOR_BGR2HSV)
		self.m_HSVHist = cv2.calcHist([self.m_FaceHSV], [0], None, [180], [0, 180])
		cv2.normalize(self.HSVHist, self.HSVHist, 0, 255, cv2.NORM_MINMAX)     # HSV Histogram
		self.m_FaceTracked = True
		self.m_FaceIsolated = True
		self.m_FaceWithinScene = True
		self.m_FaceDetectionState = faceDetectionState
		self.m_TimeSpan_FaceTrackLoss = -1


	def update_mftrackerFace_Data(self, frame, faceIsolated, colMatchVal, faceDetectionState, window):
		self.m_FaceDataReady = True
		self.m_Face_Window = window
		self.m_Face = frame[window[1]:window[1]+window[3], window[0]:window[0]+window[2]]
		self.m_FaceHSV = cv2.cvtColor(self.m_Face, cv2.COLOR_BGR2HSV)
		self.m_HSVHist = cv2.calcHist([self.m_FaceHSV], [0], None, [180], [0, 180])
		cv2.normalize(self.HSVHist, self.HSVHist, 0, 255, cv2.NORM_MINMAX)     # HSV Histogram
		self.m_FaceTracked = True
		self.m_ColMatchVal = colMatchVal
		self.m_FaceIsolated = faceIsolated
		self.m_FaceWithinScene = True
		self.m_FaceDetectionState = faceDetectionState
		self.m_TimeSpan_FaceTrackLoss = -1
		

# Structure for MultifaceTracker

class MFTracker:

	def __init__(self, frame=None, maxfacenum=0, colmatchthreshold=0, faceregionthreshold=0):

		# Current Frame
		self.CurrFrame = frame

		# Current frame's HSV transform
		if frame:
			self.CurrHSVFrame = cv2.cvtColor(self.CurrFrame, cv2.COLOR_BGR2HSV)
		else:
			self.CurrHSVFrame = None
		
		# True when Detection/Reasoning data is ready
		self.ParameterDataReady = False

		# Number of faces seen so far
		self.GlobalNum = 0

		# Max. no. of faces allowed
		self.MaxFaceNum = maxfacenum

		# No. of current faces in active and passive state
		self.m_CurrFaceNum = 0

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

		# Array of window tuple for all the detected faces
		self.m_DetectedFace_WindowArray = []

		# Array of Face detection state
		self.m_DetectedFace_StateArray = np.zeros((self.MaxFaceNum))

		# Thresholds and Properties for use in Tracking
		self.m_ColMatchThreshold = colmatchthreshold

		# Detected and Tracked region overlap Threshold
		self.m_Tracked_Detected_FaceRegionOverlap_Threshold = faceregionthreshold

		# Matrix of Detected and Tracked Face region overlap value
		self.m_Tracked_Detected_FaceRegionOverlap_Matrix = np.zeros((self.MaxFaceNum, self.MaxFaceNum), dtype=float)

		# Maximum allowed time span for track loss
		self.m_MaxTimeSpan_FaceTrackLoss = 0

		# Array Storing the Number of Detected Face Regions Array Storing the Number of Detected Face Regions
		self.m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array = np.zeros((self.MaxFaceNum))

		# Array Storing the Number of Tracked Face Regions Overlapping with each Detected Face Region
		self.m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array = np.zeros((self.MaxFaceNum))

		# Array Storing the Number of Detected Faces Overlapping Each Other
		self.m_Detected_Face2Face_Overlap_Array = np.zeros((self.MaxFaceNum))

		# Array Storing the Number of Tracked Faces Overlapping Each Other
		self.m_Tracked_Face2Face_Overlap_Array = np.zeros((self.MaxFaceNum))

		# True if Reasoning Data is ready
		self.m_MFTrackerReasoningDataReady = False


	def __str__(self):
		return "MultifaceTracker with "+str(self.m_PassiveFaceNum)+" passive detected faces and "+str(self.m_ActiveFaceNum)+" active detected faces."


	def getActivePassiveFaceData(self):
		self.m_ActiveFaceNum = 0
		self.m_PassiveFaceNum = 0
		i = 0

		for face in self.m_FaceSet:
			i += 1
			if face.m_FaceDataReady:
				if face.m_FaceTracked:
					self.m_ActiveFaceIndxVec[self.m_ActiveFaceNum] = i
					self.m_ActiveFaceNum += 1
				else:
					self.m_PassiveFaceIndxVec[self.m_PassiveFaceNum] = i
					self.m_PassiveFaceNum += 1
		self.m_CurrFaceNum = self.m_ActiveFaceNum + self.m_PassiveFaceNum


	def reset_mftracker_Reasoning_Data(self):
		self.m_Tracked_Face2Face_Overlap_Array = np.zeros((self.MaxFaceNum))
		self.m_Detected_Face2Face_Overlap_Array = np.zeros((self.MaxFaceNum))
		self.m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array = np.zeros((self.MaxFaceNum))
		self.m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array = np.zeros((self.MaxFaceNum))
		self.m_Tracked_Detected_FaceRegionOverlap_Matrix = np.zeros((self.MaxFaceNum, self.MaxFaceNum), dtype=float)
		self.m_MFTrackerReasoningDataReady = False


	def set_mftracker_Reasoning_Data(self):
		if self.m_CurrDetectedFaceNum > 0:

			if self.m_ActiveFaceNum > 0:

				for trackedFaceCount, faceIndx in enumerate( self.m_ActiveFaceIndxVec ):
					self.m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array[trackedFaceCount] = 0
					invFaceArea = 1.0 / ( self.m_FaceSet[ faceIndx ].m_Face_Window[2] * self.m_FaceSet[ faceIndx ].m_Face_Window[3] )
					for detectedFaceCount, window in enumerate( self.m_DetectedFace_WindowArray ):
						overlapVal = computeIntervalOverlap_2D( self.m_FaceSet[ faceIndx ].m_Face_Window, window )
						self.m_Tracked_Detected_FaceRegionOverlap_Matrix[ trackedFaceCount ][ detectedFaceCount ] = overlapVal
						fracOverlapVal1 = invFaceArea * overlapVal
						fracOverlapVal2 = overlapVal / ( window[2] * window[3] )
						if fracOverlapVal1 > self.m_Tracked_Detected_FaceRegionOverlap_Threshold or fracOverlapVal2 > self.m_Tracked_Detected_FaceRegionOverlap_Threshold:
							self.m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array[ trackedFaceCount ] += 1
				
				for detectedFaceCount, window in enumerate( self.m_DetectedFace_WindowArray ):
					self.m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array[ detectedFaceCount ] = 0
					invFaceArea = 1.0 / ( window[2] * window[3] )
					for trackedFaceCount, faceIndx in enumerate( self.m_ActiveFaceIndxVec ):
						fracOverlapVal1 = invFaceArea * self.m_Tracked_Detected_FaceRegionOverlap_Matrix[ trackedFaceCount ][ detectedFaceCount ]
						fracOverlapVal2 = self.m_Tracked_Detected_FaceRegionOverlap_Matrix[ trackedFaceCount ][ detectedFaceCount ] / ( self.m_FaceSet[ faceIndx ].m_Face_Window[2] * self.m_FaceSet[ faceIndx ].m_Face_Window[3] )

						if fracOverlapVal1 > self.m_Tracked_Detected_FaceRegionOverlap_Threshold or fracOverlapVal2 > self.m_Tracked_Detected_FaceRegionOverlap_Threshold:
							self.m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array[ trackedFaceCount ] += 1

				for detectedFaceCount, window in enumerate( self.m_DetectedFace_WindowArray ):

					invFaceArea = 1.0 / ( window[2] * window[3] )
					self.m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] = 0
					for counter, detected_window in enumerate( self.m_DetectedFace_WindowArray ):
						if counter != detectedFaceCount:
							overlapVal = computeIntervalOverlap_2D( window, detected_window )
							fracOverlapVal1 = overlapVal * invFaceArea
							fracOverlapVal2 = overlapVal / ( detected_window[2] * detected_window[3] )

							if fracOverlapVal1 > m_Tracked_Detected_FaceRegionOverlap_Threshold or fracOverlapVal2 > self.m_Tracked_Detected_FaceRegionOverlap_Threshold:
								self.m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] += 1

				for trackedFaceCount, faceIndx1 in enumerate( self.m_ActiveFaceIndxVec ):
					invFaceArea = 1.0 / ( self.m_FaceSet[ faceIndx1 ][2] * self.m_FaceSet[ faceIndx1 ][3] )
					self.m_Tracked_Face2Face_Overlap_Array[ trackedFaceCount ] = 0

					for counter, faceIndx2 in enumerate( self.m_ActiveFaceIndxVec ):
						overlapVal = computeIntervalOverlap_2D( self.m_FaceSet[ faceIndx1 ].m_Face_Window, self.m_FaceSet[ faceIndx2 ].m_Face_Window )
						fracOverlapVal1 = overlapVal * invFaceArea
						fracOverlapVal2 = overlapVal / ( self.m_FaceSet[ faceIndx2 ].m_Face_Window[2] * self.m_FaceSet[ faceIndx2 ].m_Face_Window[3] )
						if fracOverlapVal1 > self.m_Tracked_Detected_FaceRegionOverlap_Threshold or fracOverlapVal2 > self.m_Tracked_Detected_FaceRegionOverlap_Threshold:
							self.m_Tracked_Face2Face_Overlap_Array[ trackedFaceCount ] += 1

			else:
				for detectedFaceCount, window in enumerate( self.m_DetectedFace_WindowArray ):
					invFaceArea = 1.0 / ( window[2] * window[3] )
					self.m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] = 0

					for counter, detected_window in enumerate( self.m_DetectedFace_WindowArray ):
						if counter != detectedFaceCount:
							overlapVal = computeIntervalOverlap_2D( detected_window, window )
							fracOverlapVal1 = overlapVal * invFaceArea
							fracOverlapVal2 = overlapVal / ( detected_window[2] * detected_window[3] )

							if fracOverlapVal1 > self.m_Tracked_Detected_FaceRegionOverlap_Threshold or fracOverlapVal2 > self.m_Tracked_Detected_FaceRegionOverlap_Threshold:
								self.m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] += 1

		else:
			if self.m_ActiveFaceNum > 0:
				for trackedFaceCount, faceIndx1 in enumerate( self.m_ActiveFaceIndxVec ):
					invFaceArea = 1.0 / ( self.m_FaceSet[ faceIndx1 ].m_Face_Window[2] * self.m_FaceSet[ faceIndx1 ].m_Face_Window[3] )
					self.m_Tracked_Face2Face_Overlap_Array[ trackedFaceCount ] = 0

					for counter, faceIndx2 in enumerate( self.m_ActiveFaceIndxVec ):
						if counter != detectedFaceCount:
							overlapVal = computeIntervalOverlap_2D( self.m_FaceSet[ faceIndx1 ].m_Face_Window, self.m_FaceSet[ faceIndx2 ].m_Face_Window )
							fracOverlapVal1 = overlapVal * invFaceArea
							fracOverlapVal2 = overlapVal / ( self.m_FaceSet[ faceIndx2 ].m_Face_Window[2] * self.m_FaceSet[ faceIndx2 ].m_Face_Window[3] )
							if fracOverlapVal1 > self.m_Tracked_Detected_FaceRegionOverlap_Threshold or fracOverlapVal2 > self.m_Tracked_Detected_FaceRegionOverlap_Threshold:
								self.m_Tracked_Face2Face_Overlap_Array[ trackedFaceCount ] += 1

		self.m_MFTrackerReasoningDataReady = True


#	def getAvailableIndex(self):

def procesActiveFace(mftracker, faceIndx, faceCount):


def processNewFaces(mftracker):

	for detectedFaceCount, detected_window in enumerate( mftracker.m_DetectedFace_WindowArray ):
		invFaceArea = 1.0 / ( detected_window[2] * detected_window[3] )
		
		if mftracker.m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array[ detectedFaceCount ] = 0:
			bestPassiveFaceIndx = -1
			bestOverlapVal = -1
			for faceCount, tracked_window in enumerate( mftracker.m_FaceSet ):
				if mftracker.m_FaceSet[ faceCount ].m_FaceDataReady and (not mftracker.m_FaceSet[ faceCount ].m_FaceTracked ) :
					overlapVal = computeIntervalOverlap_2D(tracked_window, detected_window)
					if overlapVal > bestOverlapVal:
						bestPassiveFaceIndx = faceCount
						bestOverlapVal = overlapVal
			passiveFaceFound = False
			if bestPassiveFaceIndx >= 0:
				fracOverlapVal1 = bestOverlapVal *invFaceArea
				fracOverlapVal2 = bestOverlapVal / ( tracked_window[2] * tracked_window[3] )
				if fracOverlapVal1 > mftracker.m_Tracked_Detected_FaceRegionOverlap_Threshold or fracOverlapVal2 > mftracker.m_Tracked_Detected_FaceRegionOverlap_Threshold:
					passiveFaceFound = True

					if mftracker.m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] == 0:
						faceIsolated = True
						# need to be reviewed
						update_mftrackerFace_Data( mftracker.CurrFrame, faceIsolated,  )

					else:
						mftracker.m_FaceSet[ bestPassiveFaceIndx ].m_Face_Window = mftracker.m_DetectedFace_WindowArray[ detectedFaceCount ]
						mftracker.m_FaceSet[ bestPassiveFaceIndx ].m_FaceTracked = True
						mftracker.m_FaceSet[ bestPassiveFaceIndx ].m_FaceIsolated = False
						mftracker.m_FaceSet[ bestPassiveFaceIndx ].m_FaceDetectionState = mftracker.m_DetectedFace_StateArray[ detectedFaceCount ]
						mftracker.m_FaceSet[ bestPassiveFaceIndx ].m_TimeSpan_FaceTrackLoss = -1
			
			if not passiveFaceFound:
				faceID = -1
				getAvailableFaceIndx()
				if (faceID >= 0) and ( faceID < mftracker.MaxFaceNum ):
					mftracker.m_GlobalNum += 1
					
					# need to be reviewed
					mftracker.m_FaceSet[ faceID ].set_mftrackerFace_Data()
				else:
					print(" Face ARRAY OVERFLOW ")
			
def multifacetracker(mftracker):
	term_crit = ( cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 1 )


	mftracker.getActivePassiveFaceData()

	if mftracker.m_ActiveFaceNum > 0:
		for faceCount, faceIndx in enumerate( mftracker.m_ActiveFaceIndxVec ):
			dst = cv2.calcBackProject([ mftracker.CurrHSVFrame ], [0], mftracker.m_FaceSet[ faceIndx ].HSVHist , [0,180], 1,)
			ret, mftracker.m_FaceSet[ faceIndx ].m_Face_Window = cv2.CamShift(dst, mftracker.m_FaceSet[ faceIndx ].m_Face_Window, term_crit)
		
		mftracker.reset_mftracker_Reasoning_Data()
		mftracekr.set_mftracekr_Reasoning_Data()

		for faceCount, faceIndx in enumerate( mftracker.m_ActiveFaceIndxVec ):
			processActiveFace( mftracker, faceIndx, faceCount )
		
		if mftracker.m_CurrDetectedFaceNum > 0:
			processNewFaces( mftracker )

			for faceCount in range( mftracker.MaxFaceNum ):
				if mftracker.m_FaceSet[ faceCount ].m_FaceDataReady:
					if (not mftracker.m_FaceSet[ faceCount ].m_FaceTracked ) or ( not mftracker.m_FaceSet[ faceCount ].m_FaceWithinScene ) :
						mftracker.m_FaceSet[ faceCount ].m_TimeSpan_FaceTrackLoss += 1
					
					if mftracker.m_FaceSet[ faceCount ].m_TimeSpan_FaceTrackLoss > mftracker.m_MaxTimeSpan_FaceTrackLoss:
						mftracker.m_FaceSet[ faceCount ].reset_mftrackerFace_Data()

		else:
			for faceCount in range( mftracker.MaxFaceNum ):
				if mftracker.m_FaceSet[ faceCount ].m_FaceDataReady:
					if ( not mftracker.m_FaceSet[ faceCount ].m_FaceTracked ) or ( mftracker.m_FaceSet[ faceCount ].m_FaceWithinScene ):
						mftracker.m_FaceSet[ faceCount ].m_TimeSpan_FaceTrackLoss += 1
					if mftracker.m_FaceSet[ faceCount ].m_TimeSpan_FaceTrackLoss > mftracker.m_MaxTimeSpan_FaceTrackLoss:
						mftracker.m_FaceSet[ faceCount ].reset_mftrackerFace_Data()
	else:
		mftracker.reset_mftracker_Reasoning_Data()
		mftracker.set_mftracekr_Reasoning_Data()

		if mftracker.m_CurrDetectedFaceNum > 0:
			processNewFaces(mftracker)
		else:
			print(" ActiveFaceNum = 0 AND DetectedFaceNum = 0")


'''
functions need to be implemented
1. getAvailableFaceIndx()
2. computeIntervalOverlap_2D()
3. processActiveFace()

'''