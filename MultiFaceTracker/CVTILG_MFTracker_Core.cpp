/*********************************************************************************
CODE : CPP FILE FOR THE FUNCTIONS RELATED TO CORE MFTRACKER
AUTHOR : PRITHWIJIT GUHA
*********************************************************************************/

/*********************************************************************************
								INCLUDE HEADERS
*********************************************************************************/


#include "CVTILG_MFTracker.h"

#define EPSILON 0.0001


/*********************************************************************************
					FUNCTIONS FOR CORE MULTIPLE FACE TRACKER
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
unsigned char processNewFaces( CVTILG_MFTracker* mftracker , IplImage* frameData , bool verbose )
{
	// Check Inputs
	if( ( !( mftracker->m_MFTrackerMemoryAllocated ) ) || ( !( mftracker->m_MFTrackerParameterReady ) ) || ( !( mftracker->m_MFTrackerReasoningDataReady ) ) || ( mftracker->m_CurrDetectedFaceNum <= 0 ) || ( frameData->imageSize <= 0 ) || ( frameData->nChannels != 3 ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Looping over the Detected Faces
	for( int detectedFaceCount = 0 ; detectedFaceCount < mftracker->m_CurrDetectedFaceNum ; ++detectedFaceCount )
	{
		// Compute the Inverse of the Detected Face Area
		float invFaceArea = 1.0 / ( (float) ( mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].width ) * ( mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].height ) );
		
		// Compute the Fractional Overlap of the Detected Face Region with the Reasoning Region
		float fracOverlapVal = 0.0;
		computeIntervalOverlap_2D( &mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ] , &mftracker->m_ReasoningRegion , &fracOverlapVal );
		fracOverlapVal = fracOverlapVal * invFaceArea;
		
		// Proceed, If the Detected Face is within the Reasoning Region
		if( fracOverlapVal > mftracker->m_EntranceThreshold )
		{
			// If the Detected Face is Not Associated with Any Tracked Face Region
			if( mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array[ detectedFaceCount ] == 0 )
			{
				// Search for the Passive Face having the Most Significant Overlap with this Detected Face Region
				int bestPassiveFaceIndx = -1;
				float bestOverlapVal = -1;
				for( int faceCount = 0 ; faceCount < mftracker->m_MaxFaceNum ; ++faceCount )
				{
					// If [ Current Face Data is Ready ] AND [ Current Face is Passive ]
					if( ( mftracker->m_FaceSet[ faceCount ].m_FaceDataReady ) && ( !( mftracker->m_FaceSet[ faceCount ].m_FaceTracked ) ) )
					{
						// Compute the Overlap with the New Face Region
						float overlapVal = 0.0;
						computeIntervalOverlap_2D( &mftracker->m_FaceSet[ faceCount ].m_FaceBoundingBox , &mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ] , &overlapVal );
				
						// Update the Best Overlap Information
						if( overlapVal > bestOverlapVal )
						{
							// Update the Best Overlapping Passive Face Index
							bestPassiveFaceIndx = faceCount;
							
							// Update the Best Overlap Value
							bestOverlapVal = overlapVal;
						}
					}
				}
				
				// If an Overlapping Passive Face is Found
				bool passiveFaceFound = false;
				if( bestPassiveFaceIndx >= 0 )
				{
					// Compute the Fractional Overlaps
					float fracOverlapVal1 = bestOverlapVal * invFaceArea;
					float fracOverlapVal2 = bestOverlapVal / ( (float) ( mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceBoundingBox.width ) * ( mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceBoundingBox.height ) );
					
					// If this is Significant Fractional Overlap
					if( ( fracOverlapVal1 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) || ( fracOverlapVal2 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) )
					{
						// Set the Passive Face Found Flag to TRUE
						passiveFaceFound = true;
						
						// If Notifications are Turned On
						if( verbose )
						{
							// Display Face Track Restore
							printf( "\n\nTrack of ExistingFace[ %d ] Restored from DetectedFace[ %d ]\n\n" , bestPassiveFaceIndx , detectedFaceCount );
						} 
						
						// If the Associated Detected Face is Isolated
						// Detected Face Does Not have Significant Overlap with Any Other Detected Face
						// NOTE : Target Histogram of the Mean Shift Tracker is Used as the Temporary Histogram
						if( mftracker->m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] == 0 )
						{
							// Update the Face Properties
							bool faceIsolated = true;
							update_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ bestPassiveFaceIndx ] , frameData , &mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ] , mftracker->m_FaceColorDistributionUpdateRate , mftracker->m_UseFaceFeatureImage , mftracker->m_UseFaceMotionModel , 1.0 , faceIsolated , mftracker->m_DetectedFace_StateArray[ detectedFaceCount ] , mftracker->m_DetectedFace_EyeNumArray[ detectedFaceCount  ] , &mftracker->m_MeanShiftTracker.m_TargetHist );
						}
						// Otherwise...
						// If the Associated Detected Face is Grouped with Other Detected/Tracked Faces
						// NOTE : Only Update Face Bounding Box
						else
						{
							// Update the Face Bounding Box
							mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceBoundingBox.x = mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].x;
							mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceBoundingBox.y = mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].y;
							mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceBoundingBox.width = mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].width;
							mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceBoundingBox.height = mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].height;
				
							// If Motion Model is used
							if( mftracker->m_UseFaceMotionModel )
							{
								// Update the Face Motion Model Along X-Direction
								update_CVTILG_MotionModel_PVAC( &mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceMotionX , mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceBoundingBox.x );
					
								// Set the Face Motion Model Along Y-Direction
								update_CVTILG_MotionModel_PVAC( &mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceMotionY , mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceBoundingBox.y );
					
								// Motion Model Based Predicted 
								// Position of Face along X-Direction
								mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_MotionPredictedPositionX = mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceBoundingBox.x;
					
								// Motion Model Based Predicted 
								// Position of Face along Y-Direction
								mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_MotionPredictedPositionY = mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceBoundingBox.y;
							}
				
							// Update the Face Tracking State
							mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceTracked = true;
				
							// Update the Face Isolation/Grouping Status
							mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceIsolated = false;
				
							// Update the Face Detection State
							mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_FaceDetectionState = mftracker->m_DetectedFace_StateArray[ detectedFaceCount ];
				
							// Update the Number of Eyes Detected in Face Region
							mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_EyeNumDetectedInFaceRegion = mftracker->m_DetectedFace_EyeNumArray[ detectedFaceCount ];
				
							// Time Spent After Face Track Loss
							mftracker->m_FaceSet[ bestPassiveFaceIndx ].m_TimeSpan_FaceTrackLoss = -1;
						}
					}
				}
				
				// If a Matching Passive Face is Not Found
				if( !( passiveFaceFound ) )
				{
					// Get the Available Face ID
					int faceID = -1;
					getAvailableFaceIndx( mftracker , &faceID );
				
					// Proceed, If Face Slots are Available
					if( ( faceID >= 0 ) && ( faceID < mftracker->m_MaxFaceNum ) )
					{
						// Increment the Global Face Number
						mftracker->m_GlobalFaceNumber = ( mftracker->m_GlobalFaceNumber ) + 1;
					
						// Set the Face Data
						set_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ faceID ] , frameData , &mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ] , mftracker->m_UseFaceFeatureImage , mftracker->m_UseFaceMotionModel , mftracker->m_GlobalFaceNumber , faceID , mftracker->m_DetectedFace_StateArray[ detectedFaceCount ] , mftracker->m_DetectedFace_EyeNumArray[ detectedFaceCount ] );
						
						// If Notifications are Turned On
						if( verbose )
						{
							// Display Face Track Restore
							printf( "\n\nDetectedFace[ %d ] Added as NewFace[ %d ]\n\n" , detectedFaceCount , faceID );
						}
					}
					// Otherwise...
					else
					{
						// Alert the Face Array Overflow
						printf( "\n\n FACE ARRAY OVERFLOW...CAN NOT ADD NEW FACE\n\n" );
					
						// Exit Loop
						break;
					}
				}
			}
		}
	}
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

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
unsigned char processActiveFace( CVTILG_MFTracker* mftracker , IplImage* frameData , int faceIndxInActiveSet , bool verbose )
{
	// Check Inputs
	if( ( !( mftracker->m_MFTrackerMemoryAllocated ) ) || ( !( mftracker->m_MFTrackerParameterReady ) ) || ( !( mftracker->m_MFTrackerReasoningDataReady ) ) || ( faceIndxInActiveSet < 0 ) || ( faceIndxInActiveSet >= mftracker->m_ActiveFaceNum ) || ( frameData->imageSize <= 0 ) || ( frameData->nChannels != 3 ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Read the Face Index in Face Set
	int faceIndx = mftracker->m_ActiveFaceIndxVec[ faceIndxInActiveSet ];
	
	// Compute the Inverse of the Face Area
	float invFaceArea = 1.0 / ( (float) ( mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.width ) * ( mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.height ) );
	
	// If the Face is Well Tracked
	if( mftracker->m_FaceSet[ faceIndx ].m_ColMatchVal > mftracker->m_ColMatchThreshold )
	{
		// Case-1 : [ Face is Well Tracked ] AND [ Face Overlaps with Detected Face(s) ]
		if( mftracker->m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array[ faceIndxInActiveSet ] > 0 )
		{
			// Search for the Detected Face with Largest Overlap
			int ascDetectedFaceIndx = -1;
			float maxOverlapVal = -1.0;
			for( int detectedFaceCount = 0 ; detectedFaceCount < mftracker->m_CurrDetectedFaceNum ; ++detectedFaceCount )
			{
				// Read the Overlap Between the Current and the Detected Face
				float overlapVal = mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix[ faceIndxInActiveSet ][ detectedFaceCount ];
				
				// Update the Best Overlap Information
				if( overlapVal > maxOverlapVal )
				{
					// Update the Maximum Overlap
					maxOverlapVal = overlapVal;
					
					// Update the Best Face Index
					ascDetectedFaceIndx = detectedFaceCount;
				}
			}
			
			// Case-1.1 : The Associated Detected Face is Isolated
			// Detected Face Does Not have Significant Overlap with Any Other Detected Face
			// AND
			// Detected Face Does Not have Significant Overlap with Any Other Tracked Face
			// NOTE : Use the Target Histogram of the Mean Shift Tracker as 
			// NOTE : The Temporary Histogram
			if( ( mftracker->m_Detected_Face2Face_Overlap_Array[ ascDetectedFaceIndx ] == 0 ) && ( mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array[ ascDetectedFaceIndx ] == 1 ) )
			{
				// Update the Face Properties
				bool faceIsolated = true;
				update_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ faceIndx ] , frameData , &mftracker->m_DetectedFace_BoundingBoxArray[ ascDetectedFaceIndx ] , mftracker->m_FaceColorDistributionUpdateRate , mftracker->m_UseFaceFeatureImage , mftracker->m_UseFaceMotionModel , 1.0 , faceIsolated , mftracker->m_DetectedFace_StateArray[ ascDetectedFaceIndx ] , mftracker->m_DetectedFace_EyeNumArray[ ascDetectedFaceIndx ] , &mftracker->m_MeanShiftTracker.m_TargetHist );
			}
			// Otherwise...
			// Case-1.2 : The Associated Detected Face is Grouped with Other Detected/Tracked Faces
			// NOTE : Only Update Face Bounding Box
			else
			{
				// Update the Face Bounding Box
				mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.x = mftracker->m_DetectedFace_BoundingBoxArray[ ascDetectedFaceIndx ].x;
				mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.y = mftracker->m_DetectedFace_BoundingBoxArray[ ascDetectedFaceIndx ].y;
				mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.width = mftracker->m_DetectedFace_BoundingBoxArray[ ascDetectedFaceIndx ].width;
				mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.height = mftracker->m_DetectedFace_BoundingBoxArray[ ascDetectedFaceIndx ].height;
				
				// If Motion Model is used
				if( mftracker->m_UseFaceMotionModel )
				{
					// Update the Face Motion Model Along X-Direction
					update_CVTILG_MotionModel_PVAC( &mftracker->m_FaceSet[ faceIndx ].m_FaceMotionX , mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.x );
					
					// Set the Face Motion Model Along Y-Direction
					update_CVTILG_MotionModel_PVAC( &mftracker->m_FaceSet[ faceIndx ].m_FaceMotionY , mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.y );
					
					// Motion Model Based Predicted 
					// Position of Face along X-Direction
					mftracker->m_FaceSet[ faceIndx ].m_MotionPredictedPositionX = mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.x;
					
					// Motion Model Based Predicted 
					// Position of Face along Y-Direction
					mftracker->m_FaceSet[ faceIndx ].m_MotionPredictedPositionY = mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.y;
				}
				
				// Update the Face Tracking State
				mftracker->m_FaceSet[ faceIndx ].m_FaceTracked = true;
				
				// Update the Face Isolation/Grouping Status
				mftracker->m_FaceSet[ faceIndx ].m_FaceIsolated = false;
				
				// Update the Face Detection State
				mftracker->m_FaceSet[ faceIndx ].m_FaceDetectionState = mftracker->m_DetectedFace_StateArray[ ascDetectedFaceIndx ];
				
				// Update the Number of Eyes Detected in Face Region
				mftracker->m_FaceSet[ faceIndx ].m_EyeNumDetectedInFaceRegion = mftracker->m_DetectedFace_EyeNumArray[ ascDetectedFaceIndx ];
				
				// Time Spent After Face Track Loss
				mftracker->m_FaceSet[ faceIndx ].m_TimeSpan_FaceTrackLoss = -1;
			}
		}
		// Otherwise...
		// Case-2 : [ Well Tracked Face ] AND [ No Overlap Between Tracked and Detected Face ]
		else
		{
			// Set the Face Detection State to _ZERO_
			mftracker->m_FaceSet[ faceIndx ].m_FaceDetectionState = 0;
		
			// Set the Number of Eyes in Detected Face Region to _ZERO_
			mftracker->m_FaceSet[ faceIndx ].m_EyeNumDetectedInFaceRegion = 0;
		
			// Set the Face Tracked Flag to TURE
			mftracker->m_FaceSet[ faceIndx ].m_FaceTracked = true;
			
			// Set the Isolation Flag by Checking Tracked Region Overlaps
			mftracker->m_FaceSet[ faceIndx ].m_FaceIsolated = ( mftracker->m_Tracked_Face2Face_Overlap_Array[ faceIndxInActiveSet ] > 0 ) ? false : true;
		}
	}
	// Otherwise...
	// Face Tracking Fails
	else
	{
		// Case-3 : [ Face Not Tracked ] AND [ Face Overlaps with Detected Face(s) ]
		if( mftracker->m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array[ faceIndxInActiveSet ] > 0 )
		{
			// Search for the Detected Face with Largest Overlap
			int ascDetectedFaceIndx = -1;
			float maxOverlapVal = -1.0;
			for( int detectedFaceCount = 0 ; detectedFaceCount < mftracker->m_CurrDetectedFaceNum ; ++detectedFaceCount )
			{
				// Read the Overlap Between the Current and the Detected Face
				float overlapVal = mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix[ faceIndxInActiveSet ][ detectedFaceCount ];
				
				// Update the Best Overlap Information
				if( overlapVal > maxOverlapVal )
				{
					// Update the Maximum Overlap
					maxOverlapVal = overlapVal;
					
					// Update the Best Face Index
					ascDetectedFaceIndx = detectedFaceCount;
				}
			}
			
			// Case-3.1 : The Associated Detected Face is Isolated
			// Detected Face Does Not have Significant Overlap with Any Other Detected Face
			// AND
			// Detected Face Does Not have Significant Overlap with Any Other Tracked Face
			// NOTE : Use the Target Histogram of the Mean Shift Tracker as 
			// NOTE : The Temporary Histogram
			if( ( mftracker->m_Detected_Face2Face_Overlap_Array[ ascDetectedFaceIndx ] == 0 ) && ( mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array[ ascDetectedFaceIndx ] == 1 ) )
			{
				// Update the Face Properties
				bool faceIsolated = true;
				update_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ faceIndx ] , frameData , &mftracker->m_DetectedFace_BoundingBoxArray[ ascDetectedFaceIndx ] , mftracker->m_FaceColorDistributionUpdateRate , mftracker->m_UseFaceFeatureImage , mftracker->m_UseFaceMotionModel , 1.0 , faceIsolated , mftracker->m_DetectedFace_StateArray[ ascDetectedFaceIndx ] , mftracker->m_DetectedFace_EyeNumArray[ ascDetectedFaceIndx ] , &mftracker->m_MeanShiftTracker.m_TargetHist );
			}
			// Otherwise...
			// Case-3.2 : The Associated Detected Face is Grouped with Other Detected/Tracked Faces
			// NOTE : Only Update Face Bounding Box
			else
			{
				// Update the Face Bounding Box
				mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.x = mftracker->m_DetectedFace_BoundingBoxArray[ ascDetectedFaceIndx ].x;
				mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.y = mftracker->m_DetectedFace_BoundingBoxArray[ ascDetectedFaceIndx ].y;
				mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.width = mftracker->m_DetectedFace_BoundingBoxArray[ ascDetectedFaceIndx ].width;
				mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.height = mftracker->m_DetectedFace_BoundingBoxArray[ ascDetectedFaceIndx ].height;
				
				// If Motion Model is used
				if( mftracker->m_UseFaceMotionModel )
				{
					// Update the Face Motion Model Along X-Direction
					update_CVTILG_MotionModel_PVAC( &mftracker->m_FaceSet[ faceIndx ].m_FaceMotionX , mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.x );
					
					// Set the Face Motion Model Along Y-Direction
					update_CVTILG_MotionModel_PVAC( &mftracker->m_FaceSet[ faceIndx ].m_FaceMotionY , mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.y );
					
					// Motion Model Based Predicted 
					// Position of Face along X-Direction
					mftracker->m_FaceSet[ faceIndx ].m_MotionPredictedPositionX = mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.x;
					
					// Motion Model Based Predicted 
					// Position of Face along Y-Direction
					mftracker->m_FaceSet[ faceIndx ].m_MotionPredictedPositionY = mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.y;
				}
				
				// Update the Face Tracking State
				mftracker->m_FaceSet[ faceIndx ].m_FaceTracked = true;
				
				// Update the Face Isolation/Grouping Status
				mftracker->m_FaceSet[ faceIndx ].m_FaceIsolated = false;
				
				// Update the Face Detection State
				mftracker->m_FaceSet[ faceIndx ].m_FaceDetectionState = mftracker->m_DetectedFace_StateArray[ ascDetectedFaceIndx ];
				
				// Update the Number of Eyes Detected in Face Region
				mftracker->m_FaceSet[ faceIndx ].m_EyeNumDetectedInFaceRegion = mftracker->m_DetectedFace_EyeNumArray[ ascDetectedFaceIndx ];
				
				// Time Spent After Face Track Loss
				mftracker->m_FaceSet[ faceIndx ].m_TimeSpan_FaceTrackLoss = -1;
			}
		}
		// Otherwise...
		// Case-4 : [ Face Not Tracked ] AND [ No Overlap Between Tracked and Detected Face ]
		else
		{
			// Set the Face Tracked Flag to FALSE
			mftracker->m_FaceSet[ faceIndx ].m_FaceTracked = false;
		
			// Set Time Spent After Face Track Loss to _ZERO_
			mftracker->m_FaceSet[ faceIndx ].m_TimeSpan_FaceTrackLoss = 0;
		}
	}
	
	// Compute the Fractional Overlap of the Face Region with the Reasoning Region
	float fracOverlapVal = 0.0;
	computeIntervalOverlap_2D( &mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox , &mftracker->m_ReasoningRegion , &fracOverlapVal );
	fracOverlapVal = fracOverlapVal * invFaceArea;
	
	// If the Face Region is Within the Scene
	if( fracOverlapVal > mftracker->m_ExitThreshold )
	{
		// Set the WITHIN_SCENE Flag to TRUE
		mftracker->m_FaceSet[ faceIndx ].m_FaceWithinScene = true;
	}
	// Otherwise...
	else
	{
		// Set the FACE_TRACK Flag to FALSE
		mftracker->m_FaceSet[ faceIndx ].m_FaceTracked = false;
		
		// Set the WITHIN_SCENE Flag to FALSE
		mftracker->m_FaceSet[ faceIndx ].m_FaceWithinScene = false;
		
		// Set the the Time Spent after Track Loss to _ZERO_
		mftracker->m_FaceSet[ faceIndx ].m_TimeSpan_FaceTrackLoss = 0;
	}
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

// Function for Tracking Multiple Faces
// NOTE : Assumes the Detected Face Bounding Box, 
// NOTE : Frontal/Profile/Both State and Eye Number Array
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multi-Face Tracker 
//			(IplImage*) frameData : Pointer to the Current RGB Image
//			(bool) verbose : Set to TRUE to Turn On Notifications
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = multiFaceTracking( &mftracker , frameData , verbose );
unsigned char multiFaceTracking( CVTILG_MFTracker* mftracker , IplImage* frameData , bool verbose )
{
	// Check Inputs
	if( ( !( mftracker->m_MFTrackerMemoryAllocated ) ) || ( !( mftracker->m_MFTrackerParameterReady ) ) || ( frameData->imageSize <= 0 ) || ( frameData->nChannels != 3 ) )
	{
		// Error Code 1 : Invalid Input
		return( 1 );
	}
	
	// Compute the Active/Passive Face Data
	// Set Error Code to 0;
	unsigned char errCode = 0;
	errCode = getActivePassiveFaceData( mftracker );

	// If Notifications are Turned On
	if( verbose )
	{
		// Print the Number of Detected Faces
		printf( "\n\nDetectedFaceNum = %d\n\n" , mftracker->m_CurrDetectedFaceNum );
		
		// Print the Number of Active/Passive Faces Before Tracking
		printf( "\n\nBefore Tracking : Active Face Num = %d AND Passive Face Num = %d\n\n" , mftracker->m_ActiveFaceNum , mftracker->m_PassiveFaceNum );
	}

	// If Existing Faces are Tracked in the Image
	if( mftracker->m_ActiveFaceNum > 0 )
	{
		// Looping to Localize the Actively Tracked Faces in the Current Frame
		for( int faceCount = 0 ; faceCount < mftracker->m_ActiveFaceNum ; ++faceCount )
		{
			// Read the Face Index
			int faceIndx = mftracker->m_ActiveFaceIndxVec[ faceCount ];
			
			// Localize the Face
			localize_CVTILG_MFTracker_Face_MeanShiftTracker_RGB( &mftracker->m_FaceSet[ faceIndx ] , frameData , &mftracker->m_MeanShiftTracker , mftracker->m_ColMatchThreshold , mftracker->m_UseFaceMotionModel );
		}
		
		// Reset the Reasoning Parameters
		reset_CVTILG_MFTracker_Reasoning_Data( mftracker );
		
		// Set the Reasoning Parameters
		set_CVTILG_MFTracker_Reasoning_Data( mftracker );
		
		// Process the Existing Faces Tracked in the Image
		for( int faceCount = 0 ; faceCount < mftracker->m_ActiveFaceNum ; ++faceCount )
		{
			// Process the Active Face
			processActiveFace( mftracker , frameData , faceCount , verbose );
		}
		
		// Case-I : [ trackedFaceNum > 0 ] AND [ detectedFaceNum > 0 ]
		if( mftracker->m_CurrDetectedFaceNum > 0 )
		{
			// If Notifications are Turned On
			if( verbose )
			{
				// Display the Case
				printf( "\n\nCASE 1 : [ ActiveFaceNum > 0 ] AND [ DetectedFaceNum > 0 ]\n\n" );
			}
			
			// Process New Faces
			processNewFaces( mftracker , frameData , verbose );
			
			// Process the Passive Faces
			// NOTE : Passive Face : One which has Lost Track or Outside the Scene
			for( int faceCount = 0 ; faceCount < mftracker->m_MaxFaceNum ; ++faceCount )
			{
				// Only Check the Faces with Data Ready Flag Set to TRUE
				if( mftracker->m_FaceSet[ faceCount ].m_FaceDataReady )
				{
					// If the Face has Lost Track
					// OR
					// If the Face is Not Within Scene
					if( ( !( mftracker->m_FaceSet[ faceCount ].m_FaceTracked ) ) || ( !( mftracker->m_FaceSet[ faceCount ].m_FaceWithinScene ) ) )
					{
						// Increment the Time Span After Track Loss
						mftracker->m_FaceSet[ faceCount ].m_TimeSpan_FaceTrackLoss = ( mftracker->m_FaceSet[ faceCount ].m_TimeSpan_FaceTrackLoss ) + 1;
					}
					
					// If the Time Span Exceeds the Allowable Limit
					if( mftracker->m_FaceSet[ faceCount ].m_TimeSpan_FaceTrackLoss > mftracker->m_MaxTimeSpan_FaceTrackLoss )
					{
						// Reset the Face Data
						reset_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ faceCount ] );
					}
				}
			}
		}
		// Otherwise...
		// Case-II : [ trackedFaceNum > 0 ] AND [ detectedFaceNum = 0 ]
		else
		{
			// If Notifications are Turned On
			if( verbose )
			{
				// Display the Case
				printf( "\n\nCASE 2 : [ ActiveFaceNum > 0 ] AND [ DetectedFaceNum = 0 ]\n\n" );
			}
			
			// Process the Passive Faces
			// NOTE : Passive Face : One which has Lost Track or Outside the Scene
			for( int faceCount = 0 ; faceCount < mftracker->m_MaxFaceNum ; ++faceCount )
			{
				// Only Check the Faces with Data Ready Flag Set to TRUE
				if( mftracker->m_FaceSet[ faceCount ].m_FaceDataReady )
				{
					// If the Face has Lost Track
					// OR
					// If the Face is Not Within Scene
					if( ( !( mftracker->m_FaceSet[ faceCount ].m_FaceTracked ) ) || ( !( mftracker->m_FaceSet[ faceCount ].m_FaceWithinScene ) ) )
					{
						// Increment the Time Span After Track Loss
						mftracker->m_FaceSet[ faceCount ].m_TimeSpan_FaceTrackLoss = ( mftracker->m_FaceSet[ faceCount ].m_TimeSpan_FaceTrackLoss ) + 1;
					}
					
					// If the Time Span Exceeds the Allowable Limit
					if( mftracker->m_FaceSet[ faceCount ].m_TimeSpan_FaceTrackLoss > mftracker->m_MaxTimeSpan_FaceTrackLoss )
					{
						// Reset the Face Data
						reset_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ faceCount ] );
					}
				}
			}
		}
	}
	// Otherwise...
	// If there are No Actively Tracked Faces
	else
	{
		// Reset the Reasoning Parameters
		reset_CVTILG_MFTracker_Reasoning_Data( mftracker );
		
		// Set the Reasoning Parameters
		set_CVTILG_MFTracker_Reasoning_Data( mftracker );
		
		// Case-III : [ trackedFaceNum = 0 ] AND [ detectedFaceNum > 0 ]
		if( mftracker->m_CurrDetectedFaceNum > 0 )
		{
			// If Notifications are Turned On
			if( verbose )
			{
				// Display the Case
				printf( "\n\nCASE 3 : [ ActiveFaceNum = 0 ] AND [ DetectedFaceNum > 0 ]\n\n" );
			}
			
			// Process New Faces
			processNewFaces( mftracker , frameData , verbose );
		}
		// Case-IV : [ trackedFaceNum = 0 ] AND [ detectedFaceNum = 0 ]
		// NOTE : Nothing is to be Done in this Case
		else
		{
			// If Notifications are Turned On
			if( verbose )
			{
				// Display the Case
				printf( "\n\nCASE 4 : [ ActiveFaceNum = 0 ] AND [ DetectedFaceNum = 0 ]\n\n" );
			}
		}
	}
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

// Function for Merging Equivalent Active Trackers
// Parameters :-
//				(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
//				(float) equivOverlapThreshold : The Equivalence Tracker Overlap Threshold
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = mergeEquivalentActiveTrackers( &mftracker , equivOverlapThreshold );
unsigned char mergeEquivalentActiveTrackers( CVTILG_MFTracker* mftracker , float equivOverlapThreshold )
{
	// Check Inputs
	if( ( !( mftracker->m_MFTrackerMemoryAllocated ) ) || ( !( mftracker->m_MFTrackerParameterReady ) ) || ( equivOverlapThreshold < EPSILON ) || ( equivOverlapThreshold > 1.0 ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// Looping over the Faces
	for( int faceIndx1 = 0 ; faceIndx1 < mftracker->m_MaxFaceNum ; ++faceIndx1 )
	{
		// Proceed, If the Pivot Face is Actively Tracked
		if( mftracker->m_FaceSet[ faceIndx1 ].m_FaceTracked )
		{
			// Compute the Inverse of the Pivot Face Area
			float invFaceArea = 1.0 / ( (float) ( mftracker->m_FaceSet[ faceIndx1 ].m_FaceBoundingBox.width ) * ( mftracker->m_FaceSet[ faceIndx1 ].m_FaceBoundingBox.height ) );
			
			// Looping over the Unprocessed Faces
			for( int faceIndx2 = ( faceIndx1 + 1 ) ; faceIndx2 < mftracker->m_MaxFaceNum ; ++faceIndx2 )
			{
				// Proceed, If the Current Face is Actively Tracked
				if( mftracker->m_FaceSet[ faceIndx2 ].m_FaceTracked )
				{
					// Compute the Fractional Overlaps Between Each Other
					float overlapVal = -1.0;
					computeIntervalOverlap_2D( &mftracker->m_FaceSet[ faceIndx1 ].m_FaceBoundingBox , &mftracker->m_FaceSet[ faceIndx2 ].m_FaceBoundingBox , &overlapVal );
					float fracOverlapVal1 = overlapVal * invFaceArea;
					float fracOverlapVal2 = overlapVal / ( (float) ( mftracker->m_FaceSet[ faceIndx2 ].m_FaceBoundingBox.width ) * ( mftracker->m_FaceSet[ faceIndx2 ].m_FaceBoundingBox.height ) );
					
					// If Either of the Fractional Overlaps is Above Threshold
					if( ( fracOverlapVal1 > equivOverlapThreshold ) || ( fracOverlapVal2 > equivOverlapThreshold ) )
					{
						// If the Pivot Face is Older than the Current Face
						// NOTE : An Older Face will have a Lower Global ID
						if( mftracker->m_FaceSet[ faceIndx1 ].m_FaceGlobalID < mftracker->m_FaceSet[ faceIndx2 ].m_FaceGlobalID )
						{
							// Copy the Feature Data of the Newer Face to the Older Face
							copyFeatureData_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ faceIndx2 ] , &mftracker->m_FaceSet[ faceIndx1 ] );
							
							// Reset the Newer Face Data
							reset_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ faceIndx2 ] );
						}
						// Otherwise...
						// If the Current Face is Older than the Pivot Face
						else
						{
							// Copy the Feature Data of the Newer Face to the Older Face
							copyFeatureData_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ faceIndx1 ] , &mftracker->m_FaceSet[ faceIndx2 ] );
							
							// Reset the Newer Face Data
							reset_CVTILG_MFTracker_Face( &mftracker->m_FaceSet[ faceIndx1 ] );
						}
						
						// Decrement the Global Number of Faces Encountered
						mftracker->m_GlobalFaceNumber = ( mftracker->m_GlobalFaceNumber ) - 1;
					}
				}
			}
		}
	}
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}











