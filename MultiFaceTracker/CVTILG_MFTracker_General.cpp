/*********************************************************************************
CODE : CPP FILE FOR THE GENERAL FUNCTIONS RELATED TO MFTRACKER OBJECT
AUTHOR : PRITHWIJIT GUHA
*********************************************************************************/

/*********************************************************************************
								INCLUDE HEADERS
*********************************************************************************/

#include "CVTILG_MFTracker.h"

/*********************************************************************************
							DEFINE PARAMETERS
*********************************************************************************/

#define EPSILON 0.000001
#define ONE_MINUS_EPSILON 0.999999

/*********************************************************************************
					GENERAL FUNCTIONS FOR MATRACKER
*********************************************************************************/

// Function for Initializing the Multiple Face Tracker
// NOTE : Sets the Variable to Impossible Values and Arrays to NULL
// Inputs :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
// Returns : None
// Invoked As : initialize_CVTILG_MFTracker( &mftracker );
void initialize_CVTILG_MFTracker( CVTILG_MFTracker* mftracker )
{
	// Memory Allocation Flag
	mftracker->m_MFTrackerMemoryAllocated = false;
	
	// Parameter Ready Flag
	mftracker->m_MFTrackerParameterReady = false;
	
	/*********************************
	Variables and Arrays for Face Set
	*********************************/
	
	// The Total Number of Faces Encounterd So Far
	// Initially Set to _ZERO_
	mftracker->m_GlobalFaceNumber = -1;
	
	// Maximum Number of Allowable Faces
	mftracker->m_MaxFaceNum = -1;
	
	// Current Number of Faces
	mftracker->m_CurrFaceNum = -1;
	
	// Face Set
	mftracker->m_FaceSet = NULL;
	
	/*****************************************
	Variables for Individual Face Appearances
	*****************************************/
	
	// Bins Per Color for Face Color Distribution
	mftracker->m_BinsPerColor = -1;
	
	// Face Feature Image Usage Option
	mftracker->m_UseFaceFeatureImage = false;
	
	// Face Motion Model Usage Option
	mftracker->m_UseFaceMotionModel = false;
	
	/*************************************************************
	Thresholds and Options on Face Properties for Use in Tracking
	*************************************************************/
	
	// Face Color Distribution Match Threshold
	mftracker->m_ColMatchThreshold = -1.0;
	
	// Face Color Distribution Update Rate
	mftracker->m_FaceColorDistributionUpdateRate = -1.0;
	
	// Maximum Allowable Time Span After Face Track Loss
	mftracker->m_MaxTimeSpan_FaceTrackLoss = -1;
	
	/*********************************************
	Variables and Arrays for Active/Passive Faces
	*********************************************/
	
	// Number of Active Faces
	mftracker->m_ActiveFaceNum = -1;
	
	// Active Face Index Array
	// NOTE : Array Size = maxFaceNum
	mftracker->m_ActiveFaceIndxVec = NULL;
	
	// Number of Passive Faces
	mftracker->m_PassiveFaceNum = -1;
	
	// Passive Face Index Array
	// NOTE : Array Size = maxFaceNum
	mftracker->m_PassiveFaceIndxVec = NULL;
	
	/*****************************
	Variables for Reasoning Region
	*****************************/
	
	// Reasoning Region : Face Entrance/Exit is only Considered within this Region
	mftracker->m_ReasoningRegion.x = -1;
	mftracker->m_ReasoningRegion.y = -1;
	mftracker->m_ReasoningRegion.width = -1;
	mftracker->m_ReasoningRegion.height = -1;
	
	// Entrance Threshold
	mftracker->m_EntranceThreshold = -1.0;
	
	// Exit Threshold
	mftracker->m_ExitThreshold = -1.0;
	
	/************************************************
	Variables, Matrices and Arrays for Detected Faces
	************************************************/
	
	// Maximum Allowable Number of Detected Faces
	mftracker->m_MaxDetectedFaceNum = -1;
	
	// Actual Number of Blobs in the Scene
	mftracker->m_CurrDetectedFaceNum = -1;
	
	// Detected Face Bounding Box Array
	// NOTE : Array Size = maxDetectedFaceNum
	mftracker->m_DetectedFace_BoundingBoxArray = NULL;
	
	// Face Detection State Array
	// NOTE : Array Size = maxDetectedFaceNum
	// NOTE : faceDetectionState = 1 : Frontal Face
	// NOTE : faceDetectionState = 2 : Profile Face
	// NOTE : faceDetectionState = 3 : Both Frontal and Profile
	mftracker->m_DetectedFace_StateArray = NULL;
	
	// Array of the Number of Eyes in Detected Face Regions
	// NOTE : Array Size = maxDetectedFaceNum
	mftracker->m_DetectedFace_EyeNumArray = NULL;
	
	/************************************************************
	Variables, Matrices and Arrays for Face Association Reasoning
	************************************************************/
	
	// The Tracked-Detected Face Region Overlap Threshold
	mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold = -1.0;
	
	// Matrix for Storing the Overlaps between
	// Detected and Tracked Face Regions
	// NOTE : Matrix Size (Memory Allocation) : maxFaceNum x maxDetectedFaceNum
	// NOTE : Matrix Size (Data) = currTrackedFaceNum x currDetectedFaceNum
	mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix = NULL;
	
	// Array Storing the Number of Detected Face Regions 
	// Overlapping with each Tracked Face Region
	// Array Size : maxFaceNum
	mftracker->m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array = NULL;
	
	// Array Storing the Number of Tracked Face Regions 
	// Overlapping with each Detected Face Region
	// Array Size : maxDetectedFaceNum
	mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array = NULL;
	
	// Array Storing the Number of Detected Faces Overlapping Each Other
	// NOTE : Array Size : maxDetectedFaceNum
	mftracker->m_Detected_Face2Face_Overlap_Array = NULL;
	
	// Array Storing the Number of Tracked Faces Overlapping Ech Other
	// NOTE : Array Size : maxFaceNum
	mftracker->m_Tracked_Face2Face_Overlap_Array = NULL;
	
	// Status of Reasoning Matrix and Array Computation
	mftracker->m_MFTrackerReasoningDataReady = false;
	
	/****************************************************************
	Temporary Variables, Matrices, Arrays and Objects for Computation
	****************************************************************/
	
	// Temporary Object Used for Computation : Meanshift Tracker
	initialize_CVTILG_MeanShiftTracker_RGB( &mftracker->m_MeanShiftTracker );
}

// Function for Allocating the Multiple Face Tracker
// NOTE : Allocates Memory for the Multiple Face Tracker
// Inputs :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
//			(int) maxFaceNum : The Maximum Allowable Number of Faces in Face Set
//			(int) binsPerColor : The Number of Bins Per Color Channel
//			(int) maxDetectedFaceNum : The Maximum Allowable Number of Detected Faces
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = allocate_CVTILG_MFTracker( &mftracker , maxFaceNum , binsPerColor , maxDetectedFaceNum );
unsigned char allocate_CVTILG_MFTracker( CVTILG_MFTracker* mftracker , int maxFaceNum , int binsPerColor , int maxDetectedFaceNum )
{
	// Check Inputs
	if( ( mftracker->m_MFTrackerMemoryAllocated ) || ( maxFaceNum < 1 ) || ( binsPerColor < 1 ) || ( binsPerColor > 256 ) || ( maxDetectedFaceNum < 1 ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Memory Allocation Flag
	mftracker->m_MFTrackerMemoryAllocated = true;
	
	// Parameter Ready Flag
	mftracker->m_MFTrackerParameterReady = false;
	
	/*********************************
	Variables and Arrays for Face Set
	*********************************/
	
	// The Total Number of Faces Encounterd So Far
	// Initially Set to _ZERO_
	mftracker->m_GlobalFaceNumber = -1;
	
	// Maximum Number of Allowable Faces
	mftracker->m_MaxFaceNum = maxFaceNum;
	
	// Current Number of Faces
	mftracker->m_CurrFaceNum = -1;
	
	// Face Set
	mftracker->m_FaceSet = NULL;
	mftracker->m_FaceSet = (CVTILG_MFTracker_Face*) malloc( maxFaceNum * sizeof( CVTILG_MFTracker_Face ) );
	for( int counter = 0 ; counter < mftracker->m_MaxFaceNum ; ++counter )
	{
		// Initialize and Allocate Each Face
		initialize_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ counter ] );
		allocate_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ counter ] , binsPerColor );
	}
	
	/*****************************************
	Variables for Individual Face Appearances
	*****************************************/
	
	// Bins Per Color for Face Color Distribution
	mftracker->m_BinsPerColor = binsPerColor;
	
	// Face Feature Image Usage Option
	mftracker->m_UseFaceFeatureImage = false;
	
	// Face Motion Model Usage Option
	mftracker->m_UseFaceMotionModel = false;
	
	/*************************************************************
	Thresholds and Options on Face Properties for Use in Tracking
	*************************************************************/
	
	// Face Color Distribution Match Threshold
	mftracker->m_ColMatchThreshold = -1.0;
	
	// Face Color Distribution Update Rate
	mftracker->m_FaceColorDistributionUpdateRate = -1.0;
	
	// Maximum Allowable Time Span After Face Track Loss
	mftracker->m_MaxTimeSpan_FaceTrackLoss = -1;
	
	/*********************************************
	Variables and Arrays for Active/Passive Faces
	*********************************************/
	
	// Number of Active Faces
	mftracker->m_ActiveFaceNum = -1;
	
	// Active Face Index Array
	// NOTE : Array Size = maxFaceNum
	mftracker->m_ActiveFaceIndxVec = NULL;
	mftracker->m_ActiveFaceIndxVec = (int*) malloc( maxFaceNum * sizeof( int ) );
	
	// Number of Passive Faces
	mftracker->m_PassiveFaceNum = -1;
	
	// Passive Face Index Array
	// NOTE : Array Size = maxFaceNum
	mftracker->m_PassiveFaceIndxVec = NULL;
	mftracker->m_PassiveFaceIndxVec = (int*) malloc( maxFaceNum * sizeof( int ) );
	
	/*****************************
	Variables for Reasoning Region
	*****************************/
	
	// Reasoning Region : Face Entrance/Exit is only Considered within this Region
	mftracker->m_ReasoningRegion.x = -1;
	mftracker->m_ReasoningRegion.y = -1;
	mftracker->m_ReasoningRegion.width = -1;
	mftracker->m_ReasoningRegion.height = -1;
	
	// Entrance Threshold
	mftracker->m_EntranceThreshold = -1.0;
	
	// Exit Threshold
	mftracker->m_ExitThreshold = -1.0;
	
	/************************************************
	Variables, Matrices and Arrays for Detected Faces
	************************************************/
	
	// Maximum Allowable Number of Detected Faces
	mftracker->m_MaxDetectedFaceNum = maxDetectedFaceNum;
	
	// Actual Number of Blobs in the Scene
	mftracker->m_CurrDetectedFaceNum = -1;
	
	// Detected Face Bounding Box Array
	// NOTE : Array Size = maxDetectedFaceNum
	mftracker->m_DetectedFace_BoundingBoxArray = NULL;
	mftracker->m_DetectedFace_BoundingBoxArray = (CvRect*) malloc( maxFaceNum * sizeof( CvRect ) );
	
	// Face Detection State Array
	// NOTE : Array Size = maxDetectedFaceNum
	// NOTE : faceDetectionState = 1 : Frontal Face
	// NOTE : faceDetectionState = 2 : Profile Face
	// NOTE : faceDetectionState = 3 : Both Frontal and Profile
	mftracker->m_DetectedFace_StateArray = NULL;
	mftracker->m_DetectedFace_StateArray = (unsigned char*) malloc( maxFaceNum * sizeof( unsigned char ) );
	
	// Array of the Number of Eyes in Detected Face Regions
	// NOTE : Array Size = maxDetectedFaceNum
	mftracker->m_DetectedFace_EyeNumArray = NULL;
	mftracker->m_DetectedFace_EyeNumArray = (unsigned char*) malloc( maxFaceNum * sizeof( unsigned char ) );
	
	/************************************************************
	Variables, Matrices and Arrays for Face Association Reasoning
	************************************************************/
	
	// The Tracked-Detected Face Region Overlap Threshold
	mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold = -1.0;
	
	// Matrix for Storing the Overlaps between
	// Detected and Tracked Face Regions
	// NOTE : Matrix Size (Memory Allocation) : maxFaceNum x maxDetectedFaceNum
	// NOTE : Matrix Size (Data) = currTrackedFaceNum x currDetectedFaceNum
	mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix = NULL;
	mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix = (float**) malloc( maxFaceNum * sizeof( float* ) );
	for( int counter = 0 ; counter < maxFaceNum ; ++counter )
	{
		// Allocate the Rows
		mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix[ counter ] = (float*) malloc( maxFaceNum * sizeof( float ) );
	}
	
	// Array Storing the Number of Detected Face Regions 
	// Overlapping with each Tracked Face Region
	// Array Size : maxFaceNum
	mftracker->m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array = NULL;
	mftracker->m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array = (int*) malloc( maxFaceNum * sizeof( int ) );
	
	// Array Storing the Number of Tracked Face Regions 
	// Overlapping with each Detected Face Region
	// Array Size : maxDetectedFaceNum
	mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array = NULL;
	mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array = (int*) malloc( maxDetectedFaceNum * sizeof( int ) );
	
	// Array Storing the Number of Detected Faces Overlapping Each Other
	// NOTE : Array Size : maxDetectedFaceNum
	mftracker->m_Detected_Face2Face_Overlap_Array = NULL;
	mftracker->m_Detected_Face2Face_Overlap_Array = (int*) malloc( maxDetectedFaceNum * sizeof( int ) );
	
	// Array Storing the Number of Tracked Faces Overlapping Ech Other
	// NOTE : Array Size : maxFaceNum
	mftracker->m_Tracked_Face2Face_Overlap_Array = NULL;
	mftracker->m_Tracked_Face2Face_Overlap_Array =  (int*) malloc( maxFaceNum * sizeof( int ) );
	
	// Status of Reasoning Matrix and Array Computation
	mftracker->m_MFTrackerReasoningDataReady = false;
	
	/****************************************************************
	Temporary Variables, Matrices, Arrays and Objects for Computation
	****************************************************************/
	
	// Temporary Object Used for Computation : Meanshift Tracker
	allocate_CVTILG_MeanShiftTracker_RGB( &mftracker->m_MeanShiftTracker , binsPerColor );
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

// Function for Deallocating the Multiple Face Tracker
// NOTE : Deallocates Memory, Sets Variables to Impossible Values and Arrays to NULL
// Inputs :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = deallocate_CVTILG_MFTracker( &mftracker );
unsigned char deallocate_CVTILG_MFTracker( CVTILG_MFTracker* mftracker )
{
	// Check Inputs
	if( !( mftracker->m_MFTrackerMemoryAllocated ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Memory Allocation Flag
	mftracker->m_MFTrackerMemoryAllocated = false;
	
	// Parameter Ready Flag
	mftracker->m_MFTrackerParameterReady = false;
	
	/*********************************
	Variables and Arrays for Face Set
	*********************************/
	
	// The Total Number of Faces Encounterd So Far
	// Initially Set to _ZERO_
	mftracker->m_GlobalFaceNumber = -1;
	
	// Current Number of Faces
	mftracker->m_CurrFaceNum = -1;
	
	// Deallocating Face Set
	for( int counter = 0 ; counter < mftracker->m_MaxFaceNum ; ++counter )
	{
		// Deallocate Each Face
		deallocate_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ counter ] );
	}
	free( mftracker->m_FaceSet );
	mftracker->m_FaceSet = NULL;
	
	/*****************************************
	Variables for Individual Face Appearances
	*****************************************/
	
	// Bins Per Color for Face Color Distribution
	mftracker->m_BinsPerColor = -1;
	
	// Face Feature Image Usage Option
	mftracker->m_UseFaceFeatureImage = false;
	
	// Face Motion Model Usage Option
	mftracker->m_UseFaceMotionModel = false;
	
	/*************************************************************
	Thresholds and Options on Face Properties for Use in Tracking
	*************************************************************/
	
	// Face Color Distribution Match Threshold
	mftracker->m_ColMatchThreshold = -1.0;
	
	// Face Color Distribution Update Rate
	mftracker->m_FaceColorDistributionUpdateRate = -1.0;
	
	// Maximum Allowable Time Span After Face Track Loss
	mftracker->m_MaxTimeSpan_FaceTrackLoss = -1;
	
	/*********************************************
	Variables and Arrays for Active/Passive Faces
	*********************************************/
	
	// Number of Active Faces
	mftracker->m_ActiveFaceNum = -1;
	
	// Active Face Index Array
	// NOTE : Array Size = maxFaceNum
	free( mftracker->m_ActiveFaceIndxVec );
	mftracker->m_ActiveFaceIndxVec = NULL;
	
	// Number of Passive Faces
	mftracker->m_PassiveFaceNum = -1;
	
	// Passive Face Index Array
	// NOTE : Array Size = maxFaceNum
	free( mftracker->m_PassiveFaceIndxVec );
	mftracker->m_PassiveFaceIndxVec = NULL;
		
	/*****************************
	Variables for Reasoning Region
	*****************************/
	
	// Reasoning Region : Face Entrance/Exit is only Considered within this Region
	mftracker->m_ReasoningRegion.x = -1;
	mftracker->m_ReasoningRegion.y = -1;
	mftracker->m_ReasoningRegion.width = -1;
	mftracker->m_ReasoningRegion.height = -1;
	
	// Entrance Threshold
	mftracker->m_EntranceThreshold = -1.0;
	
	// Exit Threshold
	mftracker->m_ExitThreshold = -1.0;
	
	/************************************************
	Variables, Matrices and Arrays for Detected Faces
	************************************************/
	
	// Actual Number of Blobs in the Scene
	mftracker->m_CurrDetectedFaceNum = -1;
	
	// Detected Face Bounding Box Array
	// NOTE : Array Size = maxDetectedFaceNum
	free( mftracker->m_DetectedFace_BoundingBoxArray );
	mftracker->m_DetectedFace_BoundingBoxArray = NULL;
	
	// Face Detection State Array
	// NOTE : Array Size = maxDetectedFaceNum
	// NOTE : faceDetectionState = 1 : Frontal Face
	// NOTE : faceDetectionState = 2 : Profile Face
	// NOTE : faceDetectionState = 3 : Both Frontal and Profile
	free( mftracker->m_DetectedFace_StateArray );
	mftracker->m_DetectedFace_StateArray = NULL;
	
	// Array of the Number of Eyes in Detected Face Regions
	// NOTE : Array Size = maxDetectedFaceNum
	free( mftracker->m_DetectedFace_EyeNumArray );
	mftracker->m_DetectedFace_EyeNumArray = NULL;
	
	/************************************************************
	Variables, Matrices and Arrays for Face Association Reasoning
	************************************************************/
	
	// The Tracked-Detected Face Region Overlap Threshold
	mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold = -1.0;
	
	// Matrix for Storing the Overlaps between
	// Detected and Tracked Face Regions
	// NOTE : Matrix Size (Memory Allocation) : maxFaceNum x maxDetectedFaceNum
	// NOTE : Matrix Size (Data) = currTrackedFaceNum x currDetectedFaceNum
	for( int counter = 0 ; counter < mftracker->m_MaxFaceNum ; ++counter )
	{
		// Deallocate the Rows
		free( mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix[ counter ] );
	}
	free( mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix );
	mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix = NULL;
	
	// Array Storing the Number of Detected Face Regions 
	// Overlapping with each Tracked Face Region
	// Array Size : maxFaceNum
	free( mftracker->m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array );
	mftracker->m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array = NULL;
	
	// Array Storing the Number of Tracked Face Regions 
	// Overlapping with each Detected Face Region
	// Array Size : maxDetectedFaceNum
	free( mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array );
	mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array = NULL;
	
	// Array Storing the Number of Detected Faces Overlapping Each Other
	// NOTE : Array Size : maxDetectedFaceNum
	free( mftracker->m_Detected_Face2Face_Overlap_Array );
	mftracker->m_Detected_Face2Face_Overlap_Array = NULL;
	
	// Array Storing the Number of Tracked Faces Overlapping Ech Other
	// NOTE : Array Size : maxFaceNum
	free( mftracker->m_Tracked_Face2Face_Overlap_Array );
	mftracker->m_Tracked_Face2Face_Overlap_Array = NULL;
	
	// Status of Reasoning Matrix and Array Computation
	mftracker->m_MFTrackerReasoningDataReady = false;
	
	// Maximum Allowable Number of Faces
	mftracker->m_MaxFaceNum = -1;
	
	// Maximum Allowable Number of Detected Faces
	mftracker->m_MaxDetectedFaceNum = -1;
	
	/****************************************************************
	Temporary Variables, Matrices, Arrays and Objects for Computation
	****************************************************************/
	
	// Temporary Object Used for Computation : Meanshift Tracker
	deallocate_CVTILG_MeanShiftTracker_RGB( &mftracker->m_MeanShiftTracker );
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

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
unsigned char set_CVTILG_MFTracker_Parameters( CVTILG_MFTracker* mftracker , bool useFaceFeatureImage , bool useFaceMotionModel , float colMatchThreshold , float colDistUpdateRate , int maxTrackLossTimeSpan , CvRect* reasoningRegion , float entranceThreshold , float exitThreshold , float overlapThreshold )
{
	// Check Inputs
	if( ( !( mftracker->m_MFTrackerMemoryAllocated ) ) || ( colMatchThreshold < EPSILON ) || ( colMatchThreshold > ONE_MINUS_EPSILON ) || ( colDistUpdateRate < 0 ) || ( colDistUpdateRate > 1 ) || ( maxTrackLossTimeSpan < 1 ) || ( reasoningRegion->width < 1 ) || ( reasoningRegion->height < 1 ) || ( entranceThreshold < EPSILON ) || ( entranceThreshold > ONE_MINUS_EPSILON ) || ( exitThreshold < EPSILON ) || ( exitThreshold > ONE_MINUS_EPSILON ) || ( overlapThreshold < EPSILON ) || ( overlapThreshold > ONE_MINUS_EPSILON ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Memory Allocation Flag
	mftracker->m_MFTrackerMemoryAllocated = true;
	
	// Parameter Ready Flag
	mftracker->m_MFTrackerParameterReady = true;
	
	/*********************************
	Variables and Arrays for Face Set
	*********************************/
	
	// The Total Number of Faces Encounterd So Far
	// Initially Set to _ZERO_
	mftracker->m_GlobalFaceNumber = 0;
	
	// Current Number of Faces
	mftracker->m_CurrFaceNum = 0;
	
	/*****************************************
	Variables for Individual Face Appearances
	*****************************************/
	
	// Face Feature Image Usage Option
	mftracker->m_UseFaceFeatureImage = useFaceFeatureImage;
	
	// Face Motion Model Usage Option
	mftracker->m_UseFaceMotionModel = useFaceMotionModel;
	
	/*************************************************************
	Thresholds and Options on Face Properties for Use in Tracking
	*************************************************************/
	
	// Face Color Distribution Match Threshold
	mftracker->m_ColMatchThreshold = colMatchThreshold;
	
	// Face Color Distribution Update Rate
	mftracker->m_FaceColorDistributionUpdateRate = colDistUpdateRate;
	
	// Maximum Allowable Time Span After Face Track Loss
	mftracker->m_MaxTimeSpan_FaceTrackLoss = maxTrackLossTimeSpan;
	
	/*********************************************
	Variables and Arrays for Active/Passive Faces
	*********************************************/
	
	// Number of Active Faces
	mftracker->m_ActiveFaceNum = 0;
	
	// Number of Passive Faces
	mftracker->m_PassiveFaceNum = 0;
	
	/*****************************
	Variables for Reasoning Region
	*****************************/
	
	// Reasoning Region : Face Entrance/Exit is only Considered within this Region
	mftracker->m_ReasoningRegion.x = reasoningRegion->x;
	mftracker->m_ReasoningRegion.y = reasoningRegion->y;
	mftracker->m_ReasoningRegion.width = reasoningRegion->width;
	mftracker->m_ReasoningRegion.height = reasoningRegion->height;
	
	// Entrance Threshold
	mftracker->m_EntranceThreshold = entranceThreshold;
	
	// Exit Threshold
	mftracker->m_ExitThreshold = exitThreshold;
	
	/************************************************
	Variables, Matrices and Arrays for Detected Faces
	************************************************/
	
	// Actual Number of Blobs in the Scene
	mftracker->m_CurrDetectedFaceNum = 0;
	
	/************************************************************
	Variables, Matrices and Arrays for Face Association Reasoning
	************************************************************/
	
	// The Tracked-Detected Face Region Overlap Threshold
	mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold = overlapThreshold;
	
	// Status of Reasoning Matrix and Array Computation
	mftracker->m_MFTrackerReasoningDataReady = false;
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

// Function for Obtaining the Available Face Index
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
//			(int*) availableFaceIndx : Pointer to the Available Face Index
// 								 NOTE : Set to _MINUS_ONE_ If None Available
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = getAvailableFaceIndx( &mftracker , &availableFaceIndx );
unsigned char getAvailableFaceIndx( CVTILG_MFTracker* mftracker , int* availableFaceIndx )
{
	// Initially, Set the Available Face Index to _MINUS_ONE_
	(*availableFaceIndx) = -1;
	
	// Check Inputs
	if( !( mftracker->m_MFTrackerMemoryAllocated ) )
	{
		// Error Code 1 : Invalid Input
		return( 1 );
	}
	
	// Search for the Available Face Index
	for( int counter = 0 ; counter < mftracker->m_MaxFaceNum ; ++counter )
	{
		// Check for Uninitialized Face Slots
		if( !( mftracker->m_FaceSet[ counter ].m_FaceDataReady ) )
		{
			// Set the Available Face Index
			(*availableFaceIndx) = counter;
			
			// Exit the Loop
			break;
		}
	}
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

// Function for Obtaining the Indices and Numbers of Active/Passive Faces in Face Set
// Parameters :-
//				CVTILG_MFTracker* mftracker : Pointer to the Multiple Face Tracker
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = getActivePassiveFaceData( &mftracker );
unsigned char getActivePassiveFaceData( CVTILG_MFTracker* mftracker )
{
	// Reset the Number of Active and Passive Faces to _ZERO_
	mftracker->m_ActiveFaceNum = 0;
	mftracker->m_PassiveFaceNum = 0;
	
	// Check Inputs
	if( !( mftracker->m_MFTrackerMemoryAllocated )   )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Looping Over the Face Set to Count the Number of Active/Passive Faces
	for( int faceCount = 0 ; faceCount < mftracker->m_MaxFaceNum ; ++faceCount )
	{
		// If the Face Data is Set
		if( mftracker->m_FaceSet[ faceCount ].m_FaceDataReady )
		{
			// If the Face is Well Tracked
			if( mftracker->m_FaceSet[ faceCount ].m_FaceTracked )
			{
				// Log the Index of the Active Face
				mftracker->m_ActiveFaceIndxVec[ ( mftracker->m_ActiveFaceNum ) ] = faceCount;

				// Increment the Number of Active Faces
				mftracker->m_ActiveFaceNum = ( mftracker->m_ActiveFaceNum ) + 1;
			}
			// Otherwise...
			else
			{
				// Log the Index of the Active Face
				mftracker->m_PassiveFaceIndxVec[ ( mftracker->m_PassiveFaceNum ) ] = faceCount;

				// Increment the Number of Passive Faces
				mftracker->m_PassiveFaceNum = ( mftracker->m_PassiveFaceNum ) + 1;
			}
		}
	}
	
	// Update the Number of Faces
	mftracker->m_CurrFaceNum = ( mftracker->m_ActiveFaceNum ) + ( mftracker->m_PassiveFaceNum );
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

// Function for Marking the Active Faces in an Image
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker Object
//			(IplImage*) frameData : Pointer to the RGB Image
//			(int) lineWidth : The Line Width of Marker Rectangle
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = markActiveFaces( &mftracker , &frameData , lineWidth );
unsigned char markActiveFaces( CVTILG_MFTracker* mftracker , IplImage* frameData , int lineWidth )
{
	// Check Inputs
	if( ( !( mftracker->m_MFTrackerMemoryAllocated ) ) || ( frameData->imageSize <= 0 ) || ( frameData->nChannels != 3 ) || ( lineWidth < 1 ) )
	{
		// Error Code 1 : Invalid Input
		return( 1 );
	}
	
	// Compute the Number of Active/Passive Faces
	getActivePassiveFaceData( mftracker );
	
	// Proceed If there are Active Faces
	if( mftracker->m_ActiveFaceNum > 0 )
	{
		// Looping over the Faces
		for( int faceCount = 0 ; faceCount < mftracker->m_ActiveFaceNum ; ++faceCount )
		{
			// Read the Face Index
			int faceIndx = mftracker->m_ActiveFaceIndxVec[ faceCount ];
			
			// Acquiring the Marker Color
			unsigned char red , green , blue;
			getFaceMarkerColor( faceIndx , mftracker->m_MaxFaceNum , &red , &green , &blue );
			
			// Set the Drawing Parameters
			int leftTopX = mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.x;
			int leftTopY = mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.y;
			int rightBotX = leftTopX + mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.width;
			int rightBotY = leftTopY + mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.height;
			int lineType = 8;
			int shift = 0;
			
			// Marking the Face Region
			cvRectangle( frameData , cvPoint( leftTopX , leftTopY ) , cvPoint( rightBotX , rightBotY ) , CV_RGB(  red , green , blue ) , lineWidth , lineType , shift );
		}
	}
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

// Function for Writing the Multi-Face Tracker Data
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
//			(char*) mftrackerFileName : The Multiple Face Tracker File Name
//			(int) frameIndx : The Index of the Current Image
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = write_CVTILG_MFTracker( &mftracker , mftrackerFileName , frameIndx );
unsigned char write_CVTILG_MFTracker( CVTILG_MFTracker* mftracker , char* mftrackerFileName , int frameIndx )
{
	// Check Inputs
	if( ( !( mftracker->m_MFTrackerMemoryAllocated ) ) || ( frameIndx < 0 ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Open the File for Writing the Multi-Face Tracker Data
	FILE* fp = NULL;
	if( !( fp = fopen( mftrackerFileName , "w" ) ) )
	{
		// Error Code 5 : File Handling Error
		printf( "\n\nError in Module write_CVTILG_MFTracker() : Could Not Open File %s\n\n" , mftrackerFileName );
		return( 5 );
	}
	
	// Write the Frame Index
	fprintf( fp , "frameIndx = %d\n" , frameIndx );
	
	// Write the Number of Detected Faces
	fprintf( fp , "detectedFaceNum = %d\n" , mftracker->m_CurrDetectedFaceNum );
	
	// If Faces are Detected
	if( mftracker->m_CurrDetectedFaceNum > 0 )
	{
		// Write the Detected Face Array Format
		fprintf( fp , "DETECTED_FACE_X_Y_W_H_STATE_EYENUM\n" );
	
		// Writing the Detected Faces
		for( int counter = 0 ; counter < mftracker->m_CurrDetectedFaceNum ; ++counter )
		{
			fprintf( fp , "%d %d %d %d %d %d\n" , mftracker->m_DetectedFace_BoundingBoxArray[ counter ].x , mftracker->m_DetectedFace_BoundingBoxArray[ counter ].y , mftracker->m_DetectedFace_BoundingBoxArray[ counter ].width , mftracker->m_DetectedFace_BoundingBoxArray[ counter ].height , mftracker->m_DetectedFace_StateArray[ counter ] , mftracker->m_DetectedFace_EyeNumArray[ counter ] );
		}
	}
	
	// Write the Global face Number
	fprintf( fp , "globalFaceNum = %d\n" , mftracker->m_GlobalFaceNumber );
	
	// Write the Number of Active Faces
	fprintf( fp , "activeFaceNum = %d\n" , mftracker->m_ActiveFaceNum );
	
	// If there are Active Faces
	if( mftracker->m_ActiveFaceNum > 0 )
	{
		// Write the Active Face Index Array
		for( int counter = 0 ; counter < mftracker->m_ActiveFaceNum ; ++counter )
		{
			// Write the Active Face Index
			fprintf( fp , "%d\n" , mftracker->m_ActiveFaceIndxVec[ counter ] );
		}
	}
	
	// Write the Number of Passive Faces
	fprintf( fp , "passiveFaceNum = %d\n" , mftracker->m_PassiveFaceNum );
	
	// If there are Passive Faces
	if( mftracker->m_PassiveFaceNum > 0 )
	{
		// Write the Passive Face Index Array
		for( int counter = 0 ; counter < mftracker->m_PassiveFaceNum ; ++counter )
		{
			// Write the Passive Face Index
			fprintf( fp , "%d\n" , mftracker->m_PassiveFaceIndxVec[ counter ] );
		}
	}
	
	// Write the Number of Faces
	fprintf( fp , "currFaceNum = %d\n" , mftracker->m_CurrFaceNum );
	
	// If there are Faces in the Face Set
	if( mftracker->m_CurrFaceNum > 0 )
	{
		// Write a 50 "DASH" Marker to Indicate the End of Multi-Face Tracker Base Data
		fprintf( fp , "--------------------------------------------------\n" );
		
		// Close the File
		fclose( fp );
		
		// Looping over the Faces
		for( int counter = 0 ; counter < mftracker->m_MaxFaceNum ; ++counter )
		{
			// If the Face Data is Ready
			if( mftracker->m_FaceSet[ counter ].m_FaceDataReady )
			{
				// Write the Face Data
				bool appendFaceDataToFile = true;
				write_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ counter ] , mftrackerFileName , appendFaceDataToFile , frameIndx );
			}
		}
	}
	// Otherwise...
	else
	{
		// Close the File
		fclose( fp );
	}
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

