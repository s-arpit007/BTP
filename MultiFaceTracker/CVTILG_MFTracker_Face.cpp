/*********************************************************************************
CODE : CPP FILE FOR THE FUNCTIONS RELATED TO FACE
AUTHOR : PRITHWIJIT GUHA
*********************************************************************************/

/*********************************************************************************
								INCLUDE HEADERS
*********************************************************************************/

#include "CVTILG_MFTracker.h"

/*********************************************************************************
								DEFINE PARAMETERS
*********************************************************************************/

#define EPSILON 0.00001

/*********************************************************************************
						FUNCTIONS FOR THE FACE OBJECT
*********************************************************************************/

// Function for Initializing a Face Object
// Parameters :-
//				(CVTILG_MFTracker_Face*) face : Pointer to the Face
// Returns : None
// Invoked As : initialize_CVTILG_MFTracker_Face( &face );
void initialize_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face )
{
	// Face Memory Allocation Status
	face->m_FaceMemoryAllocated = false;
	
	// Face Data Ready
	face->m_FaceDataReady = false;
	
	// The Global Face Identifier
	face->m_FaceGlobalID = -1;
	
	// The Face Identifier in the Tracker List
	face->m_FaceID = -1;
	
	// The Bounding Box of the Face
	face->m_FaceBoundingBox.x = -1;
	face->m_FaceBoundingBox.y = -1;
	face->m_FaceBoundingBox.width = -1;
	face->m_FaceBoundingBox.height = -1;
	
	// The RGB Color Distribution of the Face
	initialize_CVTILG_RGBHistogram( &face->m_FaceRGBHistogram );
	
	// The Face Feature Image
	initialize_CVTILG_MCImage_32F( &face->m_FaceFeatureImage );
	
	// The Face Motion Model Along X-Direction
	initialize_CVTILG_MotionModel_PVAC( &face->m_FaceMotionX );
	
	// The Face Motion Model Along Y-Direction
	initialize_CVTILG_MotionModel_PVAC( &face->m_FaceMotionY );
	
	// Motion Model Based Predicted 
	// Position of Face along X-Direction
	face->m_MotionPredictedPositionX = -1;
	
	// Motion Model Based Predicted 
	// Position of Face along Y-Direction
	face->m_MotionPredictedPositionY = -1;
	
	// Color Match Value at Tracked Position
	face->m_ColMatchVal = -1.0;
	
	// Face Tracking State
	// Set to TRUE, If Face is Well Tracked
	// Even if Face Detection Fails
	// Set to FALSE, if Face Disappears 
	// Due to Exit OR Full Occlusion OR Track Loss
	face->m_FaceTracked = false;
	
	// Face Isolation/Grouping Status
	// Set to TRUE, If Intersection with Other Face Regions is _ZERO_
	// Set to FALSE, If Face is Grouped with Other Face(s)
	face->m_FaceIsolated = false;
	
	// Flag to Represent if Face is Within the Scene
	// Set to TRUE : Face is Within Scene
	// Set to FALSE : Face has Exit through Scene Boundary
	face->m_FaceWithinScene = false;
	
	// Face Detection State
	// NOTE : faceDetectionState = 0 : Face Not Detected
	// NOTE : faceDetectionState = 1 : Face Detected as Frontal Face
	// NOTE : faceDetectionState = 2 : Face Detected as Profile Face
	face->m_FaceDetectionState = 0;
	
	// Eye Detection State
	// Number of Eyes Detected in Face Region
	face->m_EyeNumDetectedInFaceRegion = 0;
	
	// Time Spent After Tracking Loss of Face
	face->m_TimeSpan_FaceTrackLoss = -1;
}

// Function for Allocating an Face
// NOTE : Only the Histogram is Allocated
// NOTE : The Multi-Channel Float Image is Not Allocated
// Parameters :-
//			(CVTILG_MFTracker_Face*) face : Pointer to the Face
//			(int) binsPerColorChannel : The Number of Bins Per Color Channel
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : allocate_CVTILG_MFTracker_Face( &face , binsPerColorChannel );
unsigned char allocate_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face , int binsPerColorChannel )
{
    // Check Face Status
	if( ( face->m_FaceMemoryAllocated ) || ( face->m_FaceDataReady ) )
	{
		// Error Code 1 : Invalid Input
		return( 1 );
	}
	
	// Face Memory Allocation Status
	face->m_FaceMemoryAllocated = true;
	
	// Face Data Ready
	face->m_FaceDataReady = false;
	
	// The Global Face Identifier
	face->m_FaceGlobalID = -1;
	
	// The Face Identifier in the Tracker List
	face->m_FaceID = -1;
	
	// The Bounding Box of the Face
	face->m_FaceBoundingBox.x = -1;
	face->m_FaceBoundingBox.y = -1;
	face->m_FaceBoundingBox.width = -1;
	face->m_FaceBoundingBox.height = -1;
	
	// Allocating the Face RGB Histogram
	unsigned char erc = allocate_CVTILG_RGBHistogram( &face->m_FaceRGBHistogram , binsPerColorChannel );
	
	// The Face Feature Image
	initialize_CVTILG_MCImage_32F( &face->m_FaceFeatureImage );
	
	// The Face Motion Model Along X-Direction
	initialize_CVTILG_MotionModel_PVAC( &face->m_FaceMotionX );
	
	// The Face Motion Model Along Y-Direction
	initialize_CVTILG_MotionModel_PVAC( &face->m_FaceMotionY );
	
	// Motion Model Based Predicted 
	// Position of Face along X-Direction
	face->m_MotionPredictedPositionX = -1;
	
	// Motion Model Based Predicted 
	// Position of Face along Y-Direction
	face->m_MotionPredictedPositionY = -1;
	
	// Color Match Value at Tracked Position
	face->m_ColMatchVal = -1.0;
	
	// Face Tracking State
	// Set to TRUE, If Face is Well Tracked
	// Even if Face Detection Fails
	// Set to FALSE, if Face Disappears 
	// Due to Exit OR Full Occlusion OR Track Loss
	face->m_FaceTracked = false;
	
	// Face Isolation/Grouping Status
	// Set to TRUE, If Intersection with Other Face Regions is _ZERO_
	// Set to FALSE, If Face is Grouped with Other Face(s)
	face->m_FaceIsolated = false;
	
	// Flag to Represent if Face is Within the Scene
	// Set to TRUE : Face is Within Scene
	// Set to FALSE : Face has Exit through Scene Boundary
	face->m_FaceWithinScene = false;
	
	// Face Detection State
	// NOTE : faceDetectionState = 0 : Face Not Detected
	// NOTE : faceDetectionState = 1 : Face Detected as Frontal Face
	// NOTE : faceDetectionState = 2 : Face Detected as Profile Face
	face->m_FaceDetectionState = 0;
	
	// Eye Detection State
	// Number of Eyes Detected in Face Region
	face->m_EyeNumDetectedInFaceRegion = 0;
	
	// Time Spent After Tracking Loss of Face
	face->m_TimeSpan_FaceTrackLoss = -1;
	
	// Return the Final Error Code
	return( erc );
}

// Function for Deallocating the Face
// Parameters :-
//				(CVTILG_MFTracker_Face*) face : Pointer to the Face
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = deallocate_CVTILG_MFTracker_Face( &face );
unsigned char deallocate_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face )
{
    // Check Memory Allocation Status
	if( !( face->m_FaceMemoryAllocated ) )
	{
		// Error Code 1 : Invalid Input
		return( 1 );
	}
	
	// The Global Face Identifier
	face->m_FaceGlobalID = -1;
	
	// The Face Identifier in the Tracker List
	face->m_FaceID = -1;
	
	// The Bounding Box of the Face
	face->m_FaceBoundingBox.x = -1;
	face->m_FaceBoundingBox.y = -1;
	face->m_FaceBoundingBox.width = -1;
	face->m_FaceBoundingBox.height = -1;
	
	// The RGB Color Distribution of the Face
	deallocate_CVTILG_RGBHistogram( &face->m_FaceRGBHistogram );
	
	// If the Face Feature Image is Already Allocated
	if( face->m_FaceFeatureImage.m_ImgSize > 0 )
	{
		// Deallocate Face Feature Image
		deallocate_CVTILG_MCImage_32F( &face->m_FaceFeatureImage );
	}
	
	// The Face Motion Model Along X-Direction
	initialize_CVTILG_MotionModel_PVAC( &face->m_FaceMotionX );
	
	// The Face Motion Model Along Y-Direction
	initialize_CVTILG_MotionModel_PVAC( &face->m_FaceMotionY );
	
	// Motion Model Based Predicted 
	// Position of Face along X-Direction
	face->m_MotionPredictedPositionX = -1;
	
	// Motion Model Based Predicted 
	// Position of Face along Y-Direction
	face->m_MotionPredictedPositionY = -1;
	
	// Color Match Value at Tracked Position
	face->m_ColMatchVal = -1.0;
	
	// Face Tracking State
	// Set to TRUE, If Face is Well Tracked
	// Even if Face Detection Fails
	// Set to FALSE, if Face Disappears 
	// Due to Exit OR Full Occlusion OR Track Loss
	face->m_FaceTracked = false;
	
	// Face Isolation/Grouping Status
	// Set to TRUE, If Intersection with Other Face Regions is _ZERO_
	// Set to FALSE, If Face is Grouped with Other Face(s)
	face->m_FaceIsolated = false;
	
	// Flag to Represent if Face is Within the Scene
	// Set to TRUE : Face is Within Scene
	// Set to FALSE : Face has Exit through Scene Boundary
	face->m_FaceWithinScene = false;
	
	// Face Detection State
	// NOTE : faceDetectionState = 0 : Face Not Detected
	// NOTE : faceDetectionState = 1 : Face Detected as Frontal Face
	// NOTE : faceDetectionState = 2 : Face Detected as Profile Face
	face->m_FaceDetectionState = 0;
	
	// Eye Detection State
	// Number of Eyes Detected in Face Region
	face->m_EyeNumDetectedInFaceRegion = 0;
	
	// Time Spent After Tracking Loss of Face
	face->m_TimeSpan_FaceTrackLoss = -1;
	
	// Face Memory Allocation Status
	face->m_FaceMemoryAllocated = false;
	
	// Face Data Ready
	face->m_FaceDataReady = false;
	
    // Error Code 0 : All Well, That Ends Well
    return( 0 );
}

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
unsigned char set_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face , IplImage* frameData , CvRect* faceBoundingBox , bool useFaceFeatureImage , bool useFaceMotionModel , int faceGlobalID , int faceID , unsigned char faceDetectionState , unsigned char eyeNumInFaceRegion )
{
    // Check Inputs
    //assert( )
    if( ( !( face->m_FaceMemoryAllocated ) ) || ( face->m_FaceDataReady ) || ( frameData->imageSize <= 0 ) || ( faceBoundingBox->width <= 0 ) || ( !( isInside( frameData , faceBoundingBox ) ) ) || ( faceBoundingBox->height <= 0 ) || ( faceGlobalID <= 0 ) || ( faceID < 0 ) || ( faceDetectionState > 3 ) )
    {
        // Error Code 1 : Invalid Inputs
        return( 1 );
    }

	// Face Memory Allocation Status
	face->m_FaceMemoryAllocated = true;

	// Face Data Ready
	face->m_FaceDataReady = true;

	// The Global Face Identifier
	face->m_FaceGlobalID = faceGlobalID;

	// The Face Identifier in the Tracker List
	face->m_FaceID = faceID;

	// The Bounding Box of the Face
	face->m_FaceBoundingBox.x = faceBoundingBox->x;
	face->m_FaceBoundingBox.y = faceBoundingBox->y;
	face->m_FaceBoundingBox.width = faceBoundingBox->width;
	face->m_FaceBoundingBox.height = faceBoundingBox->height;

	// Computing the Position Weighted RGB Histogram from the Face Region
	unsigned char erc = computePositionWeightedRGBHistogram( &face->m_FaceRGBHistogram , frameData , faceBoundingBox );

	// If the Face Feature Image is Desired
	if( useFaceFeatureImage )
	{
		// If the Face Feature Image is Allocated
		if( face->m_FaceFeatureImage.m_ImgSize > 0 )
		{
			// Deallocate the the Face Feature Image
			deallocate_CVTILG_MCImage_32F( &face->m_FaceFeatureImage );
		}
		
		// Initialize the Face Feature Image
		initialize_CVTILG_MCImage_32F( &face->m_FaceFeatureImage );
		
		// Allocate the Face Feature Image
		allocate_CVTILG_MCImage_32F( &face->m_FaceFeatureImage , faceBoundingBox->x , faceBoundingBox->y , 3 );
		
		// Acquiring the Pixel Color Information in the Feature Image
		crop_IplImage_To_CVTILG_MCImage_32F( frameData , faceBoundingBox , &face->m_FaceFeatureImage );
	}
	
	// If the Face Motion Model is to be Used
	if( useFaceMotionModel )
	{
		// Set the Face Motion Model Along X-Direction
		set_CVTILG_MotionModel_PVAC( &face->m_FaceMotionX , 1.0 , 0.01 , faceBoundingBox->x );
		
		// Set the Face Motion Model Along Y-Direction
		set_CVTILG_MotionModel_PVAC( &face->m_FaceMotionY , 1.0 , 0.01 , faceBoundingBox->y );
		
		// Motion Model Based Predicted 
		// Position of Face along X-Direction
		face->m_MotionPredictedPositionX = faceBoundingBox->x;
		
		// Motion Model Based Predicted 
		// Position of Face along Y-Direction
		face->m_MotionPredictedPositionY = faceBoundingBox->y;
	}
	
	// Color Match Value at Tracked Position
	face->m_ColMatchVal = 1.0;

	// Face Tracking State
	// Set to TRUE, If Face is Well Tracked
	// Even if Face Detection Fails
	// Set to FALSE, if Face Disappears 
	// Due to Exit OR Full Occlusion OR Track Loss
	face->m_FaceTracked = true;

	// Face Isolation/Grouping Status
	// Set to TRUE, If Intersection with Other Face Regions is _ZERO_
	// Set to FALSE, If Face is Grouped with Other Face(s)
	face->m_FaceIsolated = true;

	// Flag to Represent if Face is Within the Scene
	// Set to TRUE : Face is Within Scene
	// Set to FALSE : Face has Exit through Scene Boundary
	face->m_FaceWithinScene = true;
	
	// Face Detection State
	// NOTE : faceDetectionState = 0 : Face Not Detected
	// NOTE : faceDetectionState = 1 : Face Detected as Frontal Face
	// NOTE : faceDetectionState = 2 : Face Detected as Profile Face
	face->m_FaceDetectionState = faceDetectionState;

	// Eye Detection State
	// Number of Eyes Detected in Face Region
	face->m_EyeNumDetectedInFaceRegion = eyeNumInFaceRegion;

	// Time Spent After Tracking Loss of Face
	face->m_TimeSpan_FaceTrackLoss = -1;
	
	// Error Code 0 : All Well, That Ends Well
    return( 0 );
}

// Function for Resetting the Face Data
// Parameters :-
//				(CVTILG_MFTracker_Face*) face : Pointer to the Face
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = reset_CVTILG_MFTracker_Face( &face );
unsigned char reset_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face )
{
    // Check Inputs
	if( ( !( face->m_FaceMemoryAllocated ) ) || ( !( face->m_FaceDataReady ) ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Face Memory Allocation Status
	face->m_FaceMemoryAllocated = true;
	
	// The Global Face Identifier
	face->m_FaceGlobalID = -1;
	
	// The Face Identifier in the Tracker List
	face->m_FaceID = -1;
	
	// The Bounding Box of the Face
	face->m_FaceBoundingBox.x = -1;
	face->m_FaceBoundingBox.y = -1;
	face->m_FaceBoundingBox.width = -1;
	face->m_FaceBoundingBox.height = -1;
	
	// The RGB Color Distribution of the Face
	for( int counter = 0 ; counter < ( face->m_FaceRGBHistogram.m_RGBHistogramSize ) ; ++counter )
	{
		// Set the Histogram Entries to _ZERO_s
		face->m_FaceRGBHistogram.m_RGBHistogram[ counter ] = 0.0;
	}
	
	// If the Face Feature Image is Already Allocated
	if( face->m_FaceFeatureImage.m_ImgSize > 0 )
	{
		// Deallocate Face Feature Image
		deallocate_CVTILG_MCImage_32F( &face->m_FaceFeatureImage );
	}
	
	// The Face Motion Model Along X-Direction
	initialize_CVTILG_MotionModel_PVAC( &face->m_FaceMotionX );
	
	// The Face Motion Model Along Y-Direction
	initialize_CVTILG_MotionModel_PVAC( &face->m_FaceMotionY );
	
	// Motion Model Based Predicted 
	// Position of Face along X-Direction
	face->m_MotionPredictedPositionX = -1;
	
	// Motion Model Based Predicted 
	// Position of Face along Y-Direction
	face->m_MotionPredictedPositionY = -1;
	
	// Color Match Value at Tracked Position
	face->m_ColMatchVal = -1.0;
	
	// Face Tracking State
	// Set to TRUE, If Face is Well Tracked
	// Even if Face Detection Fails
	// Set to FALSE, if Face Disappears 
	// Due to Exit OR Full Occlusion OR Track Loss
	face->m_FaceTracked = false;
	
	// Face Isolation/Grouping Status
	// Set to TRUE, If Intersection with Other Face Regions is _ZERO_
	// Set to FALSE, If Face is Grouped with Other Face(s)
	face->m_FaceIsolated = false;
	
	// Flag to Represent if Face is Within the Scene
	// Set to TRUE : Face is Within Scene
	// Set to FALSE : Face has Exit through Scene Boundary
	face->m_FaceWithinScene = false;
	
	// Face Detection State
	// NOTE : faceDetectionState = 0 : Face Not Detected
	// NOTE : faceDetectionState = 1 : Face Detected as Frontal Face
	// NOTE : faceDetectionState = 2 : Face Detected as Profile Face
	face->m_FaceDetectionState = 0;
	
	// Eye Detection State
	// Number of Eyes Detected in Face Region
	face->m_EyeNumDetectedInFaceRegion = 0;
	
	// Time Spent After Tracking Loss of Face
	face->m_TimeSpan_FaceTrackLoss = -1;
	
	// Face Data Ready
	face->m_FaceDataReady = false;
	
	// Error Code 0 : All Well, That Ends Well
    return( 0 );
}

// Function for Copying the Face Feature Data from another Face
// NOTE : Both the Faces are Assumed to be Allocated with Same Sizes
// Parameters :-
//				(CVTILG_MFTracker_Face*) srcFace : Pointer to the Source Face
//				(CVTILG_MFTracker_Face*) destFace : Pointer to the Destination Face
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = copyFeatureData_CVTILG_MFTracker_Face( &srcFace , &destFace );
unsigned char copyFeatureData_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* srcFace , CVTILG_MFTracker_Face* destFace )
{
	// Check Inputs
	if( ( !( srcFace->m_FaceMemoryAllocated ) ) || ( !( srcFace->m_FaceDataReady ) ) || ( !( destFace->m_FaceMemoryAllocated ) ) || ( srcFace->m_FaceRGBHistogram.m_BinsPerColorChannel != destFace->m_FaceRGBHistogram.m_BinsPerColorChannel ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Set the Face Data Ready to TRUE
	destFace->m_FaceDataReady = true;
	
	// NOTE : Do Not Copy the Face Identifier data
	// The Global Face Identifier
	// The Face Identifier in the Tracker List
	
	// The Bounding Box of the Face
	destFace->m_FaceBoundingBox.x = srcFace->m_FaceBoundingBox.x;
	destFace->m_FaceBoundingBox.y = srcFace->m_FaceBoundingBox.y;
	destFace->m_FaceBoundingBox.width = srcFace->m_FaceBoundingBox.width;
	destFace->m_FaceBoundingBox.height = srcFace->m_FaceBoundingBox.height;
	
	// The RGB Color Distribution of the Face
	copy_CVTILG_RGBHistogram( &srcFace->m_FaceRGBHistogram , &destFace->m_FaceRGBHistogram );
	
	// The Face Feature Image
	if( srcFace->m_FaceFeatureImage.m_ImgSize > 0 )
	{
		// If the Destination Face has a Feature Image
		if( destFace->m_FaceFeatureImage.m_ImgSize > 0  )
		{
			// Deallocate the Destination Face Feature Image
			deallocate_CVTILG_MCImage_32F( &destFace->m_FaceFeatureImage );
		}
		
		// Initialize the Face Feature Image of the Destination Face
		initialize_CVTILG_MCImage_32F( &destFace->m_FaceFeatureImage );
		
		// Copy the Source Feature Image to the Destination Feature Image
		bool destImgAlreadyAllocated = false;
		copy_CVTILG_MCImage_32F( &srcFace->m_FaceFeatureImage , &destFace->m_FaceFeatureImage , destImgAlreadyAllocated );
	}
	
	// The Face Motion Model Along X-Direction
	destFace->m_FaceMotionX.m_MinConfVar = srcFace->m_FaceMotionX.m_MinConfVar;
	destFace->m_FaceMotionX.m_X = srcFace->m_FaceMotionX.m_X;
	destFace->m_FaceMotionX.m_V = srcFace->m_FaceMotionX.m_V;
	destFace->m_FaceMotionX.m_A = srcFace->m_FaceMotionX.m_A;
	destFace->m_FaceMotionX.m_ConfVarUpdateRate = srcFace->m_FaceMotionX.m_ConfVarUpdateRate;
	destFace->m_FaceMotionX.m_ConfVar = srcFace->m_FaceMotionX.m_ConfVar;
	
	// The Face Motion Model Along Y-Direction
	destFace->m_FaceMotionY.m_MinConfVar = srcFace->m_FaceMotionY.m_MinConfVar;
	destFace->m_FaceMotionY.m_X = srcFace->m_FaceMotionY.m_X;
	destFace->m_FaceMotionY.m_V = srcFace->m_FaceMotionY.m_V;
	destFace->m_FaceMotionY.m_A = srcFace->m_FaceMotionY.m_A;
	destFace->m_FaceMotionY.m_ConfVarUpdateRate = srcFace->m_FaceMotionY.m_ConfVarUpdateRate;
	destFace->m_FaceMotionY.m_ConfVar = srcFace->m_FaceMotionY.m_ConfVar;
	
	// Motion Model Based Predicted 
	// Position of Face along X-Direction
	destFace->m_MotionPredictedPositionX = srcFace->m_MotionPredictedPositionX;
	
	// Motion Model Based Predicted 
	// Position of Face along Y-Direction
	destFace->m_MotionPredictedPositionY = srcFace->m_MotionPredictedPositionY;
	
	// Color Match Value at Tracked Position
	destFace->m_ColMatchVal = srcFace->m_ColMatchVal;
	
	// Face Tracking State
	// Set to TRUE, If Face is Well Tracked
	// Even if Face Detection Fails
	// Set to FALSE, if Face Disappears 
	// Due to Exit OR Full Occlusion OR Track Loss
	destFace->m_FaceTracked = srcFace->m_FaceTracked;
	
	// Face Isolation/Grouping Status
	// Set to TRUE, If Intersection with Other Face Regions is _ZERO_
	// Set to FALSE, If Face is Grouped with Other Face(s)
	destFace->m_FaceIsolated = srcFace->m_FaceIsolated;
	
	// Flag to Represent if Face is Within the Scene
	// Set to TRUE : Face is Within Scene
	// Set to FALSE : Face has Exit through Scene Boundary
	destFace->m_FaceWithinScene = srcFace->m_FaceWithinScene;
	
	// Face Detection State
	// NOTE : faceDetectionState = 0 : Face Not Detected
	// NOTE : faceDetectionState = 1 : Face Detected as Only Frontal Face
	// NOTE : faceDetectionState = 2 : Face Detected as Only Profile Face
	// NOTE : faceDetectionState = 3 : Both Profile and Frontal Face Classifiers have Responded
	destFace->m_FaceDetectionState = srcFace->m_FaceDetectionState;
	
	// Eye Detection State
	// Number of Eyes Detected in Face Region
	destFace->m_EyeNumDetectedInFaceRegion = srcFace->m_EyeNumDetectedInFaceRegion;
	
	// Time Spent After Tracking Loss of Face
	destFace->m_TimeSpan_FaceTrackLoss = srcFace->m_TimeSpan_FaceTrackLoss;
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

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
unsigned char update_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face , IplImage* frameData , CvRect* faceBoundingBox , float updateRate , bool useFaceFeatureImage , bool useFaceMotionModel , float colMatchVal , bool faceIsolated , unsigned char faceDetectionState , unsigned char eyeNumInFaceRegion , CVTILG_RGBHistogram* colHist )
{
	// Check Inputs
	//assert( )
	if( ( !( face->m_FaceMemoryAllocated ) ) || ( !( face->m_FaceDataReady ) ) || ( frameData->imageSize <= 0 ) || ( faceBoundingBox->width <= 0 ) || ( !( isInside( frameData , faceBoundingBox ) ) ) || ( faceBoundingBox->height <= 0 ) || ( updateRate < 0 ) || ( updateRate > 1 ) || ( colMatchVal < 0 ) || ( colMatchVal > 1 ) || ( faceDetectionState > 3 ) || ( colHist->m_RGBHistogramSize != face->m_FaceRGBHistogram.m_RGBHistogramSize ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Face Memory Allocation Status
	face->m_FaceMemoryAllocated = true;
	
	// Face Data Ready
	face->m_FaceDataReady = true;
	
	// The Bounding Box of the Face
	face->m_FaceBoundingBox.x = faceBoundingBox->x;
	face->m_FaceBoundingBox.y = faceBoundingBox->y;
	face->m_FaceBoundingBox.width = faceBoundingBox->width;
	face->m_FaceBoundingBox.height = faceBoundingBox->height;
	
	// Computing the Position Weighted RGB Histogram from the Face Region
	// And Storing the Result in the Temporary Color Histogram
	unsigned char erc = computePositionWeightedRGBHistogram( colHist , frameData , faceBoundingBox );
	
	// Update the Face RGB Histogram
	erc = update_CVTILG_RGBHistogram( &face->m_FaceRGBHistogram , colHist , updateRate );
	
	// If the Face Feature Image is to be Updated
	if( useFaceFeatureImage )
	{
		// If the Face Feature Image is Allocated
		if( face->m_FaceFeatureImage.m_ImgSize > 0 )
		{
			// Deallocate the the Face Feature Image
			deallocate_CVTILG_MCImage_32F( &face->m_FaceFeatureImage );
		}
		
		// Initialize the Face Feature Image
		initialize_CVTILG_MCImage_32F( &face->m_FaceFeatureImage );
		
		// Allocate the Face Feature Image
		allocate_CVTILG_MCImage_32F( &face->m_FaceFeatureImage , faceBoundingBox->x , faceBoundingBox->y , 3 );
		
		// Acquiring the Pixel Color Information in the Feature Image
		crop_IplImage_To_CVTILG_MCImage_32F( frameData , faceBoundingBox , &face->m_FaceFeatureImage );
	}
	
	// If the Face Motion Model is to be Updated
	if( useFaceMotionModel )
	{
		// Update the Face Motion Model Along X-Direction
		update_CVTILG_MotionModel_PVAC( &face->m_FaceMotionX , faceBoundingBox->x );
		
		// Set the Face Motion Model Along Y-Direction
		update_CVTILG_MotionModel_PVAC( &face->m_FaceMotionY , faceBoundingBox->y );
		
		// Motion Model Based Predicted 
		// Position of Face along X-Direction
		face->m_MotionPredictedPositionX = faceBoundingBox->x;
		
		// Motion Model Based Predicted 
		// Position of Face along Y-Direction
		face->m_MotionPredictedPositionY = faceBoundingBox->y;
	}
	
	// Color Match Value at Tracked Position
	face->m_ColMatchVal = colMatchVal;
	
	// Face Tracking State
	// Set to TRUE, If Face is Well Tracked
	// Even if Face Detection Fails
	// Set to FALSE, if Face Disappears 
	// Due to Exit OR Full Occlusion OR Track Loss
	face->m_FaceTracked = true;
	
	// Face Isolation/Grouping Status
	// Set to TRUE, If Intersection with Other Face Regions is _ZERO_
	// Set to FALSE, If Face is Grouped with Other Face(s)
	face->m_FaceIsolated = faceIsolated;
	
	// Flag to Represent if Face is Within the Scene
	// Set to TRUE : Face is Within Scene
	// Set to FALSE : Face has Exit through Scene Boundary
	face->m_FaceWithinScene = true;
	
	// Face Detection State
	// NOTE : faceDetectionState = 0 : Face Not Detected
	// NOTE : faceDetectionState = 1 : Face Detected as Frontal Face
	// NOTE : faceDetectionState = 2 : Face Detected as Profile Face
	face->m_FaceDetectionState = faceDetectionState;
	
	// Eye Detection State
	// Number of Eyes Detected in Face Region
	face->m_EyeNumDetectedInFaceRegion = eyeNumInFaceRegion;
	
	// Time Spent After Tracking Loss of Face
	face->m_TimeSpan_FaceTrackLoss = -1;
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

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
unsigned char localize_CVTILG_MFTracker_Face_MeanShiftTracker_RGB( CVTILG_MFTracker_Face* face , IplImage* frameData , CVTILG_MeanShiftTracker_RGB* mst , float colorMatchThreshold , bool useMotionModel )
{
    // Check Inputs
	if( ( !( face->m_FaceMemoryAllocated ) ) || ( !( face->m_FaceDataReady ) ) || ( frameData->imageSize <= 0 ) || ( !( mst->m_MeanShiftTrackerMemoryAllocated ) ) || ( mst->m_TargetHist.m_RGBHistogramSize != face->m_FaceRGBHistogram.m_RGBHistogramSize ) || ( colorMatchThreshold < 0 ) || ( colorMatchThreshold > 1 ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Initializing Variables and Objects
	unsigned char erc;
	float predPosColMatchVal = -1.0;
	CvRect targetBoundingBox = cvRect( face->m_FaceBoundingBox.x , face->m_FaceBoundingBox.y , face->m_FaceBoundingBox.width , face->m_FaceBoundingBox.height );
	
	// If the Motion Model Based Prediction is to be Used
	if( useMotionModel )
	{
		// Predict the Co-ordinates of the Face Bounding Box
		float predData , predConfVar;
		erc = predict_CVTILG_MotionModel_PVAC( &face->m_FaceMotionX , &predData , &predConfVar );
		face->m_MotionPredictedPositionX = (int) cvRound( predData );
		erc = predict_CVTILG_MotionModel_PVAC( &face->m_FaceMotionY , &predData , &predConfVar) ;
		face->m_MotionPredictedPositionY = (int) cvRound( predData );
		
		// Create a Temporary Rectangle for the Face Region
		targetBoundingBox = cvRect( face->m_MotionPredictedPositionX , face->m_MotionPredictedPositionY , face->m_FaceBoundingBox.width , face->m_FaceBoundingBox.height );
		
		// Compute the Position Weighted RGB Histogram from the Motion Predcited Face Bounding Box
		// NOTE : Use the Target Histogram of the Mean-Shift Tracker
		erc = computePositionWeightedRGBHistogram( &mst->m_TargetHist , frameData , &targetBoundingBox );
        
		// Compute the Color Match Value at the Motion Predicted Position
		// And, Store the Result in the Face Color Match Value
		erc = computeBhattacharyaCoefficient( &mst->m_TargetHist , &face->m_FaceRGBHistogram , &predPosColMatchVal );
    }

    // Set the Mean-Shift Tracker's Default Parameters
	setDefaultParameters_CVTILG_MeanShiftTracker_RGB( mst );
	
	// Set the Target to the Mean Shift Tracker
	erc = setTarget_CVTILG_MeanShiftTracker_RGB( mst , &face->m_FaceRGBHistogram , &targetBoundingBox );
	
	// Localize the Object using the Mean-Shift Tracker
	erc = meanShiftTracking_RGB( mst , frameData , colorMatchThreshold );
	
	// Update the Face Bounding Box from 
	// The Currently Tracked Region
	face->m_FaceBoundingBox.x = mst->m_CurrRegion.x;
	face->m_FaceBoundingBox.y = mst->m_CurrRegion.y;

	// Update the Current Color Match Value
	face->m_ColMatchVal = mst->m_CurrBhatCoeff;
	
	// If Motion Model was Used
	// AND
	// The Motion Predicted Region is Better than Tracked region
	if( ( useMotionModel ) && ( predPosColMatchVal > mst->m_CurrBhatCoeff ) )
	{
		// Update the Face Bounding Box from 
		// The Motion Predcited Region
		face->m_FaceBoundingBox.x = face->m_MotionPredictedPositionX;
		face->m_FaceBoundingBox.y = face->m_MotionPredictedPositionY;
		
		// Update the Color Match Value
		face->m_ColMatchVal = predPosColMatchVal;
	}

	// Error Code 0 : All Well, That Ends Well
    return( 0 );
}

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
unsigned char getFaceMarkerColor( int faceID , int maxFaceID , unsigned char* red , unsigned char* green , unsigned char* blue )
{
    // Check Inputs
	if( ( faceID < 0 ) || ( faceID >= maxFaceID ) || ( maxFaceID <= 1 ) )
	{
		// Error Code 1 : Invalid Input
		return( 1 );
	}

	// Decide Face Marker Colors
	switch( faceID )
	{
		case 0 : {
				// Assign Colors
				(*red) = 255;
				(*green) = 0;
				(*blue) = 0;

				// Exit Color Assignment Structure
				break;
			}

		case 1 : {
				// Assign Colors
				(*red) = 0;
				(*green) = 255;
				(*blue) = 0;

				// Exit Color Assignment Structure
				break;
			}
		case 2 : {
				// Assign Colors
				(*red) = 0;
				(*green) = 0;
				(*blue) = 255;

				// Exit Color Assignment Structure
				break;
			}
		case 3 : {
				// Assign Colors
				(*red) = 255;
				(*green) = 255;
				(*blue) = 0;

				// Exit Color Assignment Structure
				break;
			}
		case 4 : {
				// Assign Colors
				(*red) = 0;
				(*green) = 255;
				(*blue) = 255;

				// Exit Color Assignment Structure
				break;
			}
		case 5 : {
				// Assign Colors
				(*red) = 255;
				(*green) = 0;
				(*blue) = 255;

				// Exit Color Assignment Structure
				break;
			}
		case 6 : {
				// Assign Colors
				(*red) = 128;
				(*green) = 0;
				(*blue) = 0;

				// Exit Color Assignment Structure
				break;
			}
		case 7 : {
				// Assign Colors
				(*red) = 0;
				(*green) = 128;
				(*blue) = 0;

				// Exit Color Assignment Structure
				break;
			}
		case 8 : {
				// Assign Colors
				(*red) = 0;
				(*green) = 0;
				(*blue) = 128;

				// Exit Color Assignment Structure
				break;
			}
		case 9 : {
				// Assign Colors
				(*red) = 128;
				(*green) = 128;
				(*blue) = 0;

				// Exit Color Assignment Structure
				break;
			}
		case 10 : {
				// Assign Colors
				(*red) = 0;
				(*green) = 128;
				(*blue) = 128;

				// Exit Color Assignment Structure
				break;
			}
		case 11 : {
				// Assign Colors
				(*red) = 128;
				(*green) = 0;
				(*blue) = 128;

				// Exit Color Assignment Structure
				break;
			}
		case 12 : {
				// Assign Colors
				(*red) = 128;
				(*green) = 128;
				(*blue) = 128;

				// Exit Color Assignment Structure
				break;
			}
		case 13 : {
				// Assign Colors
				(*red) = 255;
				(*green) = 128;
				(*blue) = 0;

				// Exit Color Assignment Structure
				break;
			}
		case 14 : {
				// Assign Colors
				(*red) = 128;
				(*green) = 255;
				(*blue) = 0;

				// Exit Color Assignment Structure
				break;
			}
		case 15 : {
				// Assign Colors
				(*red) = 255;
				(*green) = 0;
				(*blue) = 128;

				// Exit Color Assignment Structure
				break;
			}
		case 16 : {
				// Assign Colors
				(*red) = 128;
				(*green) = 0;
				(*blue) = 255;

				// Exit Color Assignment Structure
				break;
			}
		case 17 : {
				// Assign Colors
				(*red) = 0;
				(*green) = 255;
				(*blue) = 128;

				// Exit Color Assignment Structure
				break;
			}
		case 18 : {
				// Assign Colors
				(*red) = 0;
				(*green) = 128;
				(*blue) = 255;

				// Exit Color Assignment Structure
				break;
			}
		default : {
			// Computing the Common Difference
			float comDiff = ( ( 256.0 * 256.0 * 256.0 ) - 1.0 ) / ( ( (float) maxFaceID ) + 1.0 );

			// Computing the Color Index
			int colorIndx = cvRound( comDiff * faceID );

			// Computing the Colors
			(*blue) = colorIndx % 256;
			colorIndx = ( colorIndx - (*blue) ) / 256;
			(*green) = colorIndx % 256;
			(*red) = ( colorIndx - (*green) ) / 256;

			// Exit Color Assignment Structure
			break;
		}
	}

	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

// Function for Computing the Overlap between a Face and the Reasoning Region
// Parameters :-
//			(CVTILG_MFTracker_Face*) face : Pointer to the Face
//			(CvRect*) reasoningRegion : Pointer to the Reasoning Region
//			(int*) faceResRegOverlapVal : The Face and Reasoning Region Overlap Value
//			(float*) faceResRegFracOverlapVal : The Face and Reasoning Region 
//			Fractional Overlap Value computed as fracOverlap = overlap / faceArea
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = computeFaceAndReasoningRegionOverlapVal( &face , &reasoningRegion , &faceResRegOverlapVal , &faceResRegFracOverlapVal );
unsigned char computeFaceAndReasoningRegionOverlapVal( CVTILG_MFTracker_Face* face , CvRect* reasoningRegion , int* faceResRegOverlapVal , float* faceResRegFracOverlapVal )
{
    // Initially, Set the Overlap Values to _MINUS_ONE_
    (*faceResRegOverlapVal) = -1;
    (*faceResRegFracOverlapVal) = -1;

    // Check Inputs
	if( ( !( face->m_FaceMemoryAllocated ) ) || ( !( face->m_FaceDataReady ) ) || ( face->m_FaceBoundingBox.width <= 0 ) || ( face->m_FaceBoundingBox.height <= 0 ) || ( reasoningRegion->width <= 0 ) || ( reasoningRegion->height <= 0 ) )
    {
        // Error Code 1 : Invalid Inputs
        return( 1 );
    }

	// Compute the 2D Intersection of the Face Bounding Box
	// And the Rectangular Reasoning Region
	float overlapVal = -1.0;
	unsigned char erc = computeIntervalOverlap_2D( &face->m_FaceBoundingBox , reasoningRegion , &overlapVal );
	
	// Assign the Overlap Value
	(*faceResRegOverlapVal) = cvRound( overlapVal );

	// Assign the Fractional Overlap Value
	float faceArea = ( face->m_FaceBoundingBox.width ) * ( face->m_FaceBoundingBox.height );
	(*faceResRegFracOverlapVal) = overlapVal / faceArea;

    // Error Code 0 : All Well, That Ends Well
    return( 0 );
}

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
unsigned char write_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face , char* faceFileName , bool appendFaceDataToFile , int frameIndx )
{
    // Check Inputs
	if( ( !( face->m_FaceMemoryAllocated ) ) || ( !( face->m_FaceDataReady ) ) || ( frameIndx < 0 ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}

    // Open the File According to Append/Write Mode
    FILE* fp = NULL;
    if( appendFaceDataToFile )
    {
        // Open the File in Append Mode
        fp = fopen( faceFileName , "a" );
    }
	// Otherwise...
    else
    {
        // Open the File in Write Mode
        fp = fopen( faceFileName , "w" );
    }

    // Check the File Opening Status
    if( ! fp )
    {
        // Error Code 5 : File Handling Error
        printf( "\n\nError in Module write_CVTILG_MFTracker_Face() : Could Not Open File %s\n\n" , faceFileName );
        return( 5 );
    }

    // Write the Frame Index
    fprintf( fp , "frameIndx = %d\n" , frameIndx );

    // Write the Face Global Index
	fprintf( fp , "faceGlobalID = %d\n" , face->m_FaceGlobalID );
	
	// Write the Face ID
	fprintf( fp , "faceID = %d\n" , face->m_FaceID );
	
	// Write the Face Bounding Box
	fprintf( fp , "FaceBoundingBox = %d %d %d %d\n" , face->m_FaceBoundingBox.x , face->m_FaceBoundingBox.y , face->m_FaceBoundingBox.width , face->m_FaceBoundingBox.height );
	
	// Find out the Number of Non-Zero Indices in Face RGB Histogram
	int nzIndxNum = 0;
	for( int counter = 0 ; counter < face->m_FaceRGBHistogram.m_RGBHistogramSize ; ++counter )
	{
		// Check for Non-Zero Entry
		if( face->m_FaceRGBHistogram.m_RGBHistogram[ counter ] > 0 )
		{
			// Update the Non-Zero Index Counter
			nzIndxNum = nzIndxNum + 1;
		}
	}
	
	// Write the Non-Zero Entries of the Face RGB Histogram
	fprintf( fp , "nonZeroHistogramEntryNum = %d\n" , nzIndxNum );
	
	// The Format for Histogram Entry
	fprintf( fp , "INDEX_VALUE\n" );
	
	// Looping to Write the Non-Zero Indices
	for( int counter = 0 ; counter < face->m_FaceRGBHistogram.m_RGBHistogramSize ; ++counter )
	{
		// Check for Non-Zero Entry
		if( face->m_FaceRGBHistogram.m_RGBHistogram[ counter ] > 0 )
		{
			// Write the Index and the Value
			fprintf( fp , "%d %f\n" , counter , face->m_FaceRGBHistogram.m_RGBHistogram[ counter ] );
		}
	}
	
	// If the Face Feature Image is Used
	if( face->m_FaceFeatureImage.m_ImgSize > 0 )
	{
		// Write the Usage of Face Feature Image
		fprintf( fp , "useFaceFeatureImage = 1\n" );
		
		// Write the Face Feature Image Entries in a Sequential Manner
		for( int counter = 0 ; counter < face->m_FaceFeatureImage.m_ImgSize ; ++counter )
		{
			// Write the Image Entries
			fprintf( fp , "%f\n" , face->m_FaceFeatureImage.m_Img[ counter ] );
		}
	}
	// Otherwise...
	else
	{
		// Write the Non-Usage of Face Feature Image
		fprintf( fp , "useFaceFeatureImage = 0\n" );
	}
		
	// Write the Velocity and Acceleration along X-Direction
	fprintf( fp , "faceMotionX_Vel_Acc = %f %f\n" , face->m_FaceMotionX.m_V , face->m_FaceMotionX.m_A );
	
	// Write the Velocity and Acceleration along Y-Direction
	fprintf( fp , "faceMotionY_Vel_Acc = %f %f\n" , face->m_FaceMotionY.m_V , face->m_FaceMotionY.m_A );
	
	// Write the Current Color Match Value
	fprintf( fp , "faceColorMatchValue = %f\n" , face->m_ColMatchVal );
	
	// Write the Face Tracking State
	if( face->m_FaceTracked )
	{
		// Declare Face Tracked
		fprintf( fp , "faceTracked = 1\n" );
	}
	// Otherwise...
	else
	{
		// Declare face Not Tracked
		fprintf( fp , "faceTracked = 0\n" );
	}
	
	// Write the Face Isolation/Grouping Status
	if( face->m_FaceIsolated )
	{
		// Declare Face Tracked
		fprintf( fp , "faceIsolated = 1\n" );
	}
	// Otherwise...
	else
	{
		// Declare face Not Tracked
		fprintf( fp , "faceIsolated = 0\n" );
	}
	
	// Write the Face Withion-Scene/Exit Status
	if( face->m_FaceWithinScene )
	{
		// Declare Face Within Scene
		fprintf( fp , "faceWithinScene = 1\n" );
	}
	// Otherwise...
	else
	{
		// Declare Face Exit Status
		fprintf( fp , "faceWithinScene = 0\n" );
	}
	
	// Write the Face Detection State
	fprintf( fp , "faceDetectionState = %d\n" , face->m_FaceDetectionState );
	
	// Write the Eye Detection State
	fprintf( fp , "eyeNumInFaceRegion = %d\n" , face->m_EyeNumDetectedInFaceRegion );
	
	// Time Spent After Tracking Loss of Face
	fprintf( fp , "timeSpentAfterFaceTrackLoss = %d\n" , face->m_TimeSpan_FaceTrackLoss );
	
	// Write a 50 "DASH" Marker to Indicate the End of Face Data
    fprintf( fp , "--------------------------------------------------\n" );

    // Close the Face Data File
    fclose( fp );

    // Error Code 0 : All Well, That Ends Well
    return( 0 );
}


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
unsigned char d_write_CVTILG_MFTracker_Face( CVTILG_MFTracker_Face* face , char* faceFileName , bool appendFaceDataToFile , int frameIndx )
{
    // Check Inputs
	if( ( !( face->m_FaceMemoryAllocated ) ) || ( !( face->m_FaceDataReady ) ) || ( frameIndx < 0 ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}

    // Open the File According to Append/Write Mode
    FILE* fp = NULL;
    if( appendFaceDataToFile )
    {
        // Open the File in Append Mode
        fp = fopen( faceFileName , "a" );
    }
	// Otherwise...
    else
    {
        // Open the File in Write Mode
        fp = fopen( faceFileName , "w" );
    }

    // Check the File Opening Status
    if( ! fp )
    {
        // Error Code 5 : File Handling Error
        printf( "\n\nError in Module write_CVTILG_MFTracker_Face() : Could Not Open File %s\n\n" , faceFileName );
        return( 5 );
    }

    // Write the Frame Index
    fprintf( fp , "frameIndx = %d\n" , frameIndx );

    // Write the Face Global Index
	fprintf( fp , "faceGlobalID = %d\n" , face->m_FaceGlobalID );
	
	// Write the Face ID
	fprintf( fp , "faceID = %d\n" , face->m_FaceID );
	
	// Write the Face Bounding Box
	fprintf( fp , "FaceBoundingBox = %d %d %d %d\n" , face->m_FaceBoundingBox.x , face->m_FaceBoundingBox.y , face->m_FaceBoundingBox.width , face->m_FaceBoundingBox.height );
	
		
	// Write the Face Detection State
	fprintf( fp , "faceDetectionState = %d\n" , face->m_FaceDetectionState );
	
	// Write the Eye Detection State
	fprintf( fp , "eyeNumInFaceRegion = %d\n" , face->m_EyeNumDetectedInFaceRegion );
	
	// Write a 50 "DASH" Marker to Indicate the End of Face Data
    fprintf( fp , "--------------------------------------------------\n" );

    // Close the Face Data File
    fclose( fp );

    // Error Code 0 : All Well, That Ends Well
    return( 0 );
}






