/*********************************************************************************
CODE : HEADER FILE FOR MULTIPLE FACE TRACKING
AUTHOR : PRITHWIJIT GUHA
*********************************************************************************/

#ifndef __CVTILG_MULTIPLE_FACE_TRACKER_H__
#define __CVTILG_MULTIPLE_FACE_TRACKER_H__


/*********************************************************************************
INCLUDE HEADERS : STANDARD HEADERS
*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// For disabling assertions in the code uncomment next line
//#define NDEBUG
#include <assert.h>

/*********************************************************************************
INCLUDE HEADERS : OPENCV HEADERS
*********************************************************************************/

// #include "/usr/local/include/opencv/cv.h"
// #include "/usr/local/include/opencv/highgui.h"
// #include "/usr/local/include/opencv/cvaux.h"
// #include "/usr/local/include/opencv/cxcore.h"

#include "/usr/local/include/opencv/cv.h"
#include "/usr/local/include/opencv/highgui.h"
#include "/usr/local/include/opencv/cvaux.h"
#include "/usr/local/include/opencv/cxcore.h"

/*********************************************************************************
INCLUDE HEADERS : CVTILG HEADERS
*********************************************************************************/

#include "../CVTILG_Misc/CVTILG_Misc.h"
#include "../CVTILG_MotionModel/CVTILG_MotionModel.h"
#include "../CVTILG_ImageProcessing/CVTILG_ImageProcessing.h"
#include "../CVTILG_RGBHistogram/CVTILG_RGBHistogram.h"
#include "../CVTILG_MeanShiftTracker/CVTILG_MeanShiftTracker.h"
#include "../CVTILG_MultiChannelImage/CVTILG_MultiChannelImage.h"

/*********************************************************************************
					DATA STRUCTURE FOR THE FACE
*********************************************************************************/

struct CVTILG_MFTracker_Face
{
    // Face Memory Allocation Status
	bool m_FaceMemoryAllocated;
	
	// Face Data Ready
	bool m_FaceDataReady;
	
	// The Global Face Identifier
	int m_FaceGlobalID;
	
	// The Face Identifier in the Tracker List
	int m_FaceID;
	
	// The Bounding Box of the Face
	CvRect m_FaceBoundingBox;
	
	// The RGB Color Distribution of the Face
	CVTILG_RGBHistogram m_FaceRGBHistogram;
	
	// The Face Feature Image
	CVTILG_MCImage_32F m_FaceFeatureImage;
	
	// The Face Motion Model Along X-Direction
	CVTILG_MotionModel_PVAC m_FaceMotionX;
	
	// The Face Motion Model Along Y-Direction
	CVTILG_MotionModel_PVAC m_FaceMotionY;
	
	// Motion Model Based Predicted 
	// Position of Face along X-Direction
	int m_MotionPredictedPositionX;
	
	// Motion Model Based Predicted 
	// Position of Face along Y-Direction
	int m_MotionPredictedPositionY;
	
	// Color Match Value at Tracked Position
	float m_ColMatchVal;
	
	// Face Tracking State
	// Set to TRUE, If Face is Well Tracked
	// Even if Face Detection Fails
	// Set to FALSE, if Face Disappears 
	// Due to Exit OR Full Occlusion OR Track Loss
	bool m_FaceTracked;
	
	// Face Isolation/Grouping Status
	// Set to TRUE, If Intersection with Other Face Regions is _ZERO_
	// Set to FALSE, If Face is Grouped with Other Face(s)
	bool m_FaceIsolated;
	
	// Flag to Represent if Face is Within the Scene
	// Set to TRUE : Face is Within Scene
	// Set to FALSE : Face has Exit through Scene Boundary
	bool m_FaceWithinScene;
	
	// Face Detection State
	// NOTE : faceDetectionState = 0 : Face Not Detected
	// NOTE : faceDetectionState = 1 : Face Detected as Only Frontal Face
	// NOTE : faceDetectionState = 2 : Face Detected as Only Profile Face
	// NOTE : faceDetectionState = 3 : Both Profile and Frontal Face Classifiers have Responded
	unsigned char m_FaceDetectionState;
	
	// Eye Detection State
	// Number of Eyes Detected in Face Region
	unsigned char m_EyeNumDetectedInFaceRegion;
	
	// Time Spent After Tracking Loss of Face
	int m_TimeSpan_FaceTrackLoss;
};


/*********************************************************************************
							FUNCTIONS FOR THE FACE
*********************************************************************************/

// Function for Initializing a Face Object
// Parameters :-
//				(CVTILG_MFTracker_Face*) face : Pointer to the Face
// Returns : None
// Invoked As : initialize_CVTILG_MFTracker_Face( &face );
void initialize_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face );

// Function for Allocating an Face
// NOTE : Only the Histogram is Allocated
// NOTE : The Multi-Channel Float Image is Not Allocated
// Parameters :-
//			(CVTILG_MFTracker_Face*) face : Pointer to the Face
//			(int) binsPerColorChannel : The Number of Bins Per Color Channel
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : allocate_CVTILG_MFTracker_Face( &face , binsPerColorChannel );
unsigned char allocate_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face , int binsPerColorChannel );

// Function for Deallocating the Face
// Parameters :-
//				(CVTILG_MFTracker_Face*) face : Pointer to the Face
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = deallocate_CVTILG_MFTracker_Face( &face );
unsigned char deallocate_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face );

// Function for Setting the Face Data from
// Input RGB Image and Face Bounding Box
// NOTE : The Face Region is Assumed to be Obtained by Face Detection
// NOTE : The Face Data is Set Only when the Face is Isolated from Others
// Parameters :-
//				(CVTILG_MFTracker_Face*) face : Pointer to the Face
//				(IplImage*) frameData : Pointer to the RGB Image Hosting the Face Region
//				(CvRect*) faceBoundingBox : Pointer to the Face Bounding Box
//				(bool) useFaceFeatureImage : Set to TRUE, If Face Feature Image is to be Used
//				(bool) useFaceMotionModel : Set to TRUE, If Face Motion Model is to be Used
//				(int) faceGlobalID : The Face Global Identifier
//				(int) faceID : The Face Identifier in the Tracker List
//				(unsigned char) faceDetectionState : The Face Detection State to be Set
//				NOTE : Only Frontal = 1 / Only Profile = 2 / Both Frontal and Profile = 3
//				(unsigned char) eyeNumInFaceRegion : Number of Eyes Detected in Face Region
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = set_CVTILG_MFTracker_Face( &face , frameData , &faceBoundingBox , useFaceFeatureImage , useFaceMotionModel , faceGlobalID , faceID , faceDetectionState , eyeNumInFaceRegion );
unsigned char set_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face , IplImage* frameData , CvRect* faceBoundingBox , bool useFaceFeatureImage , bool useFaceMotionModel , int faceGlobalID , int faceID , unsigned char faceDetectionState , unsigned char eyeNumInFaceRegion );

// Function for Resetting the Face Data
// Parameters :-
//				(CVTILG_MFTracker_Face*) face : Pointer to the Face
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = reset_CVTILG_MFTracker_Face( &face );
unsigned char reset_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face );

// Function for Copying the Face Feature Data from another Face
// NOTE : Both the Faces are Assumed to be Allocated with Same Sizes
// Parameters :-
//				(CVTILG_MFTracker_Face*) srcFace : Pointer to the Source Face
//				(CVTILG_MFTracker_Face*) destFace : Pointer to the Destination Face
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = copyFeatureData_CVTILG_MFTracker_Face( &srcFace , &destFace );
unsigned char copyFeatureData_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* srcFace , CVTILG_MFTracker_Face* destFace );
		
// Function for Updating the Face Data from
// Input RGB Image and Face Bounding Box
// NOTE : The Face Region is Assumed to be Obtained by Face Detection
// NOTE : The Face Data is Set Only when the Face is Isolated from Others
// Parameters :-
//				(CVTILG_MFTracker_Face*) face : Pointer to the Face
//				(IplImage*) frameData : Pointer to the RGB Image Hosting the Face Region
//				(CvRect*) faceBoundingBox : Pointer to the Face Bounding Box
//				(float) updateRate : The Rate for Updating the Face RGB Histogram
//				(bool) updateFaceFeatureImage : Set to TRUE, If Face Feature Image is to be Updated
//				(bool) updateFaceMotionModel : Set to TRUE, If Face Motion Model is to be Updated
//				(float) colMatchVal : The Color Match Value at the Current Position
//				(bool) faceIsolated : Set to TRUE, If the Face is Isolated
//				(unsigned char) faceDetectionState : The Face Detection State to be Set
//				NOTE : Only Frontal = 1 / Only Profile = 2 / Both Frontal and Profile = 3
//				(unsigned char) eyeNumInFaceRegion : Number of Eyes Detected in Face Region
//				(CVTILG_RGBHistogram*) colHist : Temporary Color Histogram used for Computations
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = update_CVTILG_MFTracker_Face( &face , frameData , &faceBoundingBox , updateRate , useFaceFeatureImage , useFaceMotionModel , colMatchVal , faceIsolated , faceDetectionState , eyeNumInFaceRegion , &colHist );
unsigned char update_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face , IplImage* frameData , CvRect* faceBoundingBox , float updateRate , bool useFaceFeatureImage , bool useFaceMotionModel , float colMatchVal , bool faceIsolated , unsigned char faceDetectionState , unsigned char eyeNumInFaceRegion , CVTILG_RGBHistogram* colHist );

// Function for Localizing an Face using Mean-Shift Tracker
// NOTE : Motion based Initialization of the Mean-Shift Tracker is Optional
// NOTE : The Face Appearance Bounding Box is Updated within this Function
// Parameters :-
//			(CVTILG_MFTracker_Face*) face : Pointer to the Face
//			(IplImage*) frameData : Pointer to the RGB Image
//			(CVTILG_MeanShiftTracker_RGB*) mst : Pointer to the Mean Shift Tracker
//			(float) colorMatchThreshold : The Color Match Threshold to Dictate Tracking
//			(bool) useMotionModel : Set to TRUE, If Motion Model based Prediction is to be Used
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = localize_CVTILG_MFTracker_Face_MeanShiftTracker_RGB( &face , &frameData , &mst , colorMatchThreshold , useMotionModel );
unsigned char localize_CVTILG_MFTracker_Face_MeanShiftTracker_RGB( CVTILG_MFTracker_Face* face , IplImage* frameData , CVTILG_MeanShiftTracker_RGB* mst , float colorMatchThreshold , bool useMotionModel );

// Function for Deciding Face Marker Color from Face ID
// Inputs :-
//			(int) faceID : The Face ID
//			(int) maxFaceID : The Maximum Allowable Face ID
//			(unsigned char*) red : Pointer to the Red Component of the Marker Color
//			(unsigned char*) green : Pointer to the Green Component of the Marker Color
//			(unsigned char*) blue : Pointer to the Blue Component of the Marker Color
// Outputs :-
//			(unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = getFaceMarkerColor( faceID , maxFaceID , &red , &green , &blue );
unsigned char getFaceMarkerColor( int faceID , int maxFaceID , unsigned char* red , unsigned char* green , unsigned char* blue );

// Function for Computing the Overlap between a Face and the Reasoning Region
// Parameters :-
//			(CVTILG_MFTracker_Face*) face : Pointer to the Face
//			(CvRect*) reasoningRegion : Pointer to the Reasoning Region
//			(int*) faceResRegOverlapVal : The Face and Reasoning Region Overlap Value
//			(float*) faceResRegFracOverlapVal : The Face and Reasoning Region 
//			Fractional Overlap Value computed as fracOverlap = overlap / faceArea
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = computeFaceAndReasoningRegionOverlapVal( &face , &reasoningRegion , &faceResRegOverlapVal , &faceResRegFracOverlapVal );
unsigned char computeFaceAndReasoningRegionOverlapVal( CVTILG_MFTracker_Face* face , CvRect* reasoningRegion , int* faceResRegOverlapVal , float* faceResRegFracOverlapVal );

// Function for Writing the Face Data to a File
// Parameters :-
//			(CVTILG_MFTracker_Face*) face : Pointer to the Face
//			(char*) faceFileName : Face File Name
//			(bool) appendFaceDataToFile : Set to TRUE, If the Face Data File is
//			Opened in APPEND Mode. Set to FALSE, If the Face Data File is
//			Opened in WRITE Mode.
//			(int) frameIndx : The Index of the Current Frame
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = write_CVTILG_MFTracker_Face( &face , faceFileName , appendFaceDataToFile , frameIndx );
unsigned char write_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face , char* faceFileName , bool appendFaceDataToFile , int frameIndx );

// Function for Writing the Detected Face Data to a File
// Parameters :-
//			(CVTILG_MFTracker_Face*) face : Pointer to the Face
//			(char*) faceFileName : Face File Name
//			(bool) appendFaceDataToFile : Set to TRUE, If the Face Data File is
//			Opened in APPEND Mode. Set to FALSE, If the Face Data File is
//			Opened in WRITE Mode.
//			(int) frameIndx : The Index of the Current Frame
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = d_write_CVTILG_MFTracker_Face( &face , faceFileName , appendFaceDataToFile , frameIndx );
unsigned char d_write_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face , char* faceFileName , bool appendFaceDataToFile , int frameIndx );


/*********************************************************************************
				DATA STRUCTURE FOR THE MULTIPLE FACE TRACKER
*********************************************************************************/

struct CVTILG_MFTracker
{
	// Memory Allocation Flag
	bool m_MFTrackerMemoryAllocated;
	
	// Parameter Ready Flag
	bool m_MFTrackerParameterReady;
	
	/*********************************
	Variables and Arrays for Face Set
	*********************************/
	
	// The Total Number of Faces Encounterd So Far
	// Initially Set to _ZERO_
	int m_GlobalFaceNumber;
	
	// Maximum Number of Allowable Faces
	int m_MaxFaceNum;
	
	// Current Number of Faces
	int m_CurrFaceNum;
	
	// Face Set
	// NOTE : Array Size = maxFaceNum
	CVTILG_MFTracker_Face* m_FaceSet;
	
	/*****************************************
	Variables for Individual Face Appearances
	*****************************************/
	
	// Bins Per Color for Face Color Distribution
	int m_BinsPerColor;
	
	// Face Feature Image Usage Option
	bool m_UseFaceFeatureImage;
	
	// Face Motion Model Usage Option
	bool m_UseFaceMotionModel;
	
	/*************************************************************
	Thresholds and Options on Face Properties for Use in Tracking
	*************************************************************/
	
	// Face Color Distribution Match Threshold
	float m_ColMatchThreshold;
	
	// Face Color Distribution Update Rate
	float m_FaceColorDistributionUpdateRate;
	
	// Maximum Allowable Time Span After Face Track Loss
	int m_MaxTimeSpan_FaceTrackLoss;
	
	/*********************************************
	Variables and Arrays for Active/Passive Faces
	*********************************************/
	
	// Number of Active Faces
	int m_ActiveFaceNum;
	
	// Active Face Index Array
	// NOTE : Array Size = maxFaceNum
	int* m_ActiveFaceIndxVec;
	
	// Number of Passive Faces
	int m_PassiveFaceNum;
	
	// Passive Face Index Array
	// NOTE : Array Size = maxFaceNum
	int* m_PassiveFaceIndxVec;
	
	/*****************************
	Variables for Reasoning Region
	*****************************/
	
	// Reasoning Region : Face Entrance/Exit is only Considered within this Region
	CvRect m_ReasoningRegion;
	
	// Entrance Threshold
	float m_EntranceThreshold;
	
	// Exit Threshold
	float m_ExitThreshold;
	
	/************************************************
	Variables, Matrices and Arrays for Detected Faces
	************************************************/
	
	// Maximum Allowable Number of Detected Faces
	int m_MaxDetectedFaceNum;
	
	// Actual Number of Blobs in the Scene
	int m_CurrDetectedFaceNum;
	
	// Detected Face Bounding Box Array
	// NOTE : Array Size = maxDetectedFaceNum
	CvRect* m_DetectedFace_BoundingBoxArray;
	
	// Face Detection State Array
	// NOTE : Array Size = maxDetectedFaceNum
	// NOTE : faceDetectionState = 1 : Frontal Face
	// NOTE : faceDetectionState = 2 : Profile Face
	// NOTE : faceDetectionState = 3 : Both Frontal and Profile
	unsigned char* m_DetectedFace_StateArray;
	
	// Array of the Number of Eyes in Detected Face Regions
	// NOTE : Array Size = maxDetectedFaceNum
	unsigned char* m_DetectedFace_EyeNumArray;
	
	/************************************************************
	Variables, Matrices and Arrays for Face Association Reasoning
	************************************************************/
	
	// The Tracked-Detected Face Region Overlap Threshold
	float m_Tracked_Detected_FaceRegionOverlap_Threshold;
	
	// Matrix for Storing the Overlaps between
	// Detected and Tracked Face Regions
	// NOTE : Matrix Size (Memory Allocation) : maxFaceNum x maxDetectedFaceNum
	// NOTE : Matrix Size (Data) = currTrackedFaceNum x currDetectedFaceNum
	float** m_Tracked_Detected_FaceRegionOverlap_Matrix;
	
	// Array Storing the Number of Detected Face Regions 
	// Overlapping with each Tracked Face Region
	// Array Size : maxFaceNum
	int* m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array;
	
	// Array Storing the Number of Tracked Face Regions 
	// Overlapping with each Detected Face Region
	// Array Size : maxDetectedFaceNum
	int* m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array;
	
	// Array Storing the Number of Detected Faces Overlapping Each Other
	// NOTE : Array Size : maxDetectedFaceNum
	int* m_Detected_Face2Face_Overlap_Array;
	
	// Array Storing the Number of Tracked Faces Overlapping Ech Other
	// NOTE : Array Size : maxFaceNum
	int* m_Tracked_Face2Face_Overlap_Array;
	
	// Status of Reasoning Matrix and Array Computation
	bool m_MFTrackerReasoningDataReady;
	
	/****************************************************************
	Temporary Variables, Matrices, Arrays and Objects for Computation
	****************************************************************/
	
	// Temporary Object Used for Computation : Meanshift Tracker
	CVTILG_MeanShiftTracker_RGB m_MeanShiftTracker;
};


/*********************************************************************************
						GENERAL FUNCTIONS OF MFTRACKER
*********************************************************************************/

// Function for Initializing the Multiple Face Tracker
// NOTE : Sets the Variable to Impossible Values and Arrays to NULL
// Inputs :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
// Returns : None
// Invoked As : initialize_CVTILG_MFTracker( &mftracker );
void initialize_CVTILG_MFTracker( CVTILG_MFTracker* mftracker );

// Function for Allocating the Multiple Face Tracker
// NOTE : Allocates Memory for the Multiple Face Tracker
// Inputs :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
//			(int) maxFaceNum : The Maximum Allowable Number of Faces in Face Set
//			(int) binsPerColor : The Number of Bins Per Color Channel
//			(int) maxDetectedFaceNum : The Maximum Allowable Number of Detected Faces
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = allocate_CVTILG_MFTracker( &mftracker , maxFaceNum , binsPerColor , maxDetectedFaceNum );
unsigned char allocate_CVTILG_MFTracker( CVTILG_MFTracker* mftracker , int maxFaceNum , int binsPerColor , int maxDetectedFaceNum );

// Function for Deallocating the Multiple Face Tracker
// NOTE : Deallocates Memory, Sets Variables to Impossible Values and Arrays to NULL
// Inputs :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = deallocate_CVTILG_MFTracker( &mftracker );
unsigned char deallocate_CVTILG_MFTracker( CVTILG_MFTracker* mftracker );

// Function for Setting the Parameters of the Multiple Face Tracker
// NOTE : MFTracker Memory should be Allocated
// NOTE : Global Face Count is Initialized to _ZERO_ here
// Inputs :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
//			(bool) useFaceFeatureImage : Set to TRUE, If Face Feature Image is to be used
//			(bool) useFaceMotionModel : Set to TRUE, If Face Motion Model is to be used
//			(float) colMatchThreshold : The Color Match Threshold - A Match Value
//										Lower than this Threshold Implies
//										The Invisibility of the Face
//			(float) colDistUpdateRate : Face Color Distibution Update Rate
//			(int) maxTrackLossTimeSpan : Maximum Allowable Time after Track Loss
//			(CvRect*) reasoningRegion : Pointer to the Reasoning Region Rectangle
//			(float) entranceThreshold : The Entrance Threshold
//			(float) exitThreshold : The Exit Threshold
//			(float) overlapThreshold : The Tracked-Detected Face Region Overlap Threshold
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = set_CVTILG_MFTracker_Parameters( &mftracker , useFaceFeatureImage , useFaceMotionModel , colMatchThreshold , colDistUpdateRate , maxTrackLossTimeSpan , &reasoningRegion , entranceThreshold , exitThreshold , overlapThreshold );
unsigned char set_CVTILG_MFTracker_Parameters( CVTILG_MFTracker* mftracker , bool useFaceFeatureImage , bool useFaceMotionModel , float colMatchThreshold , float colDistUpdateRate , int maxTrackLossTimeSpan , CvRect* reasoningRegion , float entranceThreshold , float exitThreshold , float overlapThreshold );

// Function for Obtaining the Available Face Index
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
//			(int*) availableFaceIndx : Pointer to the Available Face Index
// 								 NOTE : Set to _MINUS_ONE_ If None Available
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = getAvailableFaceIndx( &mftracker , &availableFaceIndx );
unsigned char getAvailableFaceIndx( CVTILG_MFTracker* mftracker , int* availableFaceIndx );

// Function for Obtaining the Indices and Numbers of Active/Passive Faces in Face Set
// Parameters :-
//				CVTILG_MFTracker* mftracker : Pointer to the Multiple Face Tracker
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = getActivePassiveFaceData( &mftracker );
unsigned char getActivePassiveFaceData( CVTILG_MFTracker* mftracker );

// Function for Marking the Active Faces in an Image
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker Object
//			(IplImage*) frameData : Pointer to the RGB Image
//			(int) lineWidth : The Line Width of Marker Rectangle
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = markActiveFaces( &mftracker , &frameData , lineWidth );
unsigned char markActiveFaces( CVTILG_MFTracker* mftracker , IplImage* frameData , int lineWidth );

// Function for Writing the Multi-Face Tracker Data
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
//			(char*) mftrackerFileName : The Multiple Face Tracker File Name
//			(int) frameIndx : The Index of the Current Image
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = write_CVTILG_MFTracker( &mftracker , mftrackerFileName , frameIndx );
unsigned char write_CVTILG_MFTracker( CVTILG_MFTracker* mftracker , char* mftrackerFileName , int frameIndx );


/*********************************************************************************
					REASONING FUNCTIONS OF MFTRACKER
*********************************************************************************/

// Function for Resetting the Reasoning Arrays and Matrices to _ZERO_s
// Inputs :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = reset_CVTILG_MFTracker_Reasoning_Data( &mftracker );
unsigned char reset_CVTILG_MFTracker_Reasoning_Data( CVTILG_MFTracker* mftracker );

// Function for Computing the Reasoning Matrices and Arrays
// NOTE : Assumes that the Reasoning Data are Reset
// NOTE : Assumes the Number of Active and Passive Faces to be Ready
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = set_CVTILG_MFTracker_Reasoning_Data( &mftracker );
unsigned char set_CVTILG_MFTracker_Reasoning_Data( CVTILG_MFTracker* mftracker );


/*********************************************************************************
						CORE FUNCTIONS OF MFTRACKER
*********************************************************************************/


// Function for Processing New Faces
// NOTE : Assumes that the Active/Passive Face Index Arrays are Ready
// NOTE : Assumes that the Reasoning Matrices are Ready
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
//			(IplImage*) frameData : Pointer to the RGB IplImage
//			(bool) verbose : Set to TRUE for Printing Notifications
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = processNewFaces( &mftracker , &frameData , verbose );
unsigned char processNewFaces( CVTILG_MFTracker* mftracker , IplImage* frameData , bool verbose );

// Function for Processing the Active Face
// NOTE : Assumptions :-
// NOTE : 	The Active/Passive Face Index Array is Available
// NOTE : 	The Face Detection Informations are Available
// NOTE :	The Face Predicted Positions (Obtained by Tracking) are Ready
// NOTE : 	The Reasoning Matrices are Computed
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the MFTracker
//			(IplImage*) frameData : Pointer to Current RGB Image
//			(int) faceIndxInActiveSet : Index of the Face in Active Set
//			(bool) verbose : Set to TRUE for Printing Notifications
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = processActiveFace( &mftracker , frameData , verbose );
unsigned char processActiveFace( CVTILG_MFTracker* mftracker , IplImage* frameData , int faceIndxInActiveSet , bool verbose );

// Function for Tracking Multiple Faces
// NOTE : Assumes the Detected Face Bounding Box, 
// NOTE : Frontal/Profile/Both State and Eye Number Array
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multi-Face Tracker 
//			(IplImage*) frameData : Pointer to the Current RGB Image
//			(bool) verbose : Set to TRUE to Turn On Notifications
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = multiFaceTracking( &mftracker , frameData , verbose );
unsigned char multiFaceTracking( CVTILG_MFTracker* mftracker , IplImage* frameData , bool verbose );

// Function for Merging Equivalent Active Trackers
// Parameters :-
//				(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
//				(float) equivOverlapThreshold : The Equivalence Tracker Overlap Threshold
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = mergeEquivalentActiveTrackers( &mftracker , equivOverlapThreshold );
unsigned char mergeEquivalentActiveTrackers( CVTILG_MFTracker* mftracker , float equivOverlapThreshold );





#endif


