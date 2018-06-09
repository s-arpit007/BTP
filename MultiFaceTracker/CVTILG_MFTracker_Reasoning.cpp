/*********************************************************************************
CODE : CPP FILE FOR THE FUNCTIONS RELATED TO MFTRACKER REASONING
AUTHOR : PRITHWIJIT GUHA
*********************************************************************************/

/*********************************************************************************
INCLUDE HEADERS
*********************************************************************************/


#include "CVTILG_MFTracker.h"


/*********************************************************************************
					FUNCTIONS FOR MFTRACKER REASONING
*********************************************************************************/

// Function for Resetting the Reasoning Arrays and Matrices to _ZERO_s
// Inputs :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = reset_CVTILG_MFTracker_Reasoning_Data( &mftracker );
unsigned char reset_CVTILG_MFTracker_Reasoning_Data( CVTILG_MFTracker* mftracker )
{
	// Check Inputs
	if( !( mftracker->m_MFTrackerMemoryAllocated ) )
	{
		// Error Code 1 : Invalid Inputs
		printf("\n error in reset_CVTILG_MFTracker_Reasoning_Data error code 1 \n") ;
		return( 1 );
	}

	// Reset the Overlap Matrix Entries to _ZERO_
	for( int trackedFaceCount = 0 ; trackedFaceCount < mftracker->m_MaxFaceNum ; ++trackedFaceCount )
	{
		// Reset DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array to _ZERO_
		mftracker->m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array[ trackedFaceCount ] = 0;
		
		// Reset Tracked_Face2Face_Overlap_Array to _ZERO_
		mftracker->m_Tracked_Face2Face_Overlap_Array[ trackedFaceCount ] = 0;
		
		// Looping over the Columns
		for( int detectedFaceCount = 0 ; detectedFaceCount < mftracker->m_MaxDetectedFaceNum ; ++detectedFaceCount )
		{
			// Reset the Matrix Elements to _ZERO_
			mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix[ trackedFaceCount ][ detectedFaceCount ] = 0;
		}
	}
	
	// Reset to _ZERO_
	for( int detectedFaceCount = 0 ; detectedFaceCount < mftracker->m_MaxDetectedFaceNum ; ++detectedFaceCount )
	{
		// Reset DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array to _ZERO_
		mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array[ detectedFaceCount ] = 0;
		
		// Reset Detected_Face2Face_Overlap_Array to _ZERO_
		mftracker->m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] = 0;
	}
	
	// Reset the Reasoning Data Ready Flag to FALSE
	mftracker->m_MFTrackerReasoningDataReady = false;
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

// Function for Computing the Reasoning Matrices and Arrays
// NOTE : Assumes that the Reasoning Data are Reset
// NOTE : Assumes the Number of Active and Passive Faces to be Ready
// Parameters :-
//			(CVTILG_MFTracker*) mftracker : Pointer to the Multiple Face Tracker
// Returns : (unsigned char) errCode : The Error Code of Execution
// Invoked As : errCode = set_CVTILG_MFTracker_Reasoning_Data( &mftracker );
unsigned char set_CVTILG_MFTracker_Reasoning_Data( CVTILG_MFTracker* mftracker )
{
	// Check Inputs
	if( ( !( mftracker->m_MFTrackerMemoryAllocated ) ) || ( mftracker->m_MFTrackerReasoningDataReady ) )
	{
		// Error Code 1 : Invalid Inputs
		return( 1 );
	}
	
	// IF Faces are Detected in the Image
	if( mftracker->m_CurrDetectedFaceNum > 0 )
	{
		// Case-I : [ detectedFaceNum > 0 ] AND [ activeFaceNum > 0 ]
		if( mftracker->m_ActiveFaceNum > 0 )
		{
			// Looping Over the Detected and Tracked 
			// Faces to Compute the Overlap Matrix
			for( int trackedFaceCount = 0 ; trackedFaceCount < mftracker->m_ActiveFaceNum ; ++trackedFaceCount )
			{
				// Read the Face Index in Face Set
				int faceIndx = mftracker->m_ActiveFaceIndxVec[ trackedFaceCount ];
				
				// Initialize the Number of Detected Faces Overlapped with the Tracked Face to _ZERO_
				mftracker->m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array[ trackedFaceCount ] = 0;
				
				// Compute the Inverse Face Area
				float invFaceArea = 1.0 / ( (float) ( mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.width ) * ( mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.height ) );
				
				// Looping over the Detected Faces
				for( int detectedFaceCount = 0 ; detectedFaceCount < mftracker->m_CurrDetectedFaceNum ; ++detectedFaceCount )
				{
					// Computing the Overlap Between the Detected-Tracked Face Bounding Boxes
					float overlapVal = 0;
					computeIntervalOverlap_2D( &mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox , &mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ] , &overlapVal );
					
					// Set the Overlap Value
					mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix[ trackedFaceCount ][ detectedFaceCount ] = overlapVal;
					
					// Compute the Fractional Overlap Value
					float fracOverlapVal1 = invFaceArea * overlapVal;
					float fracOverlapVal2 = overlapVal / ( (float) ( mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].width ) * ( mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].height ) );
					
					// Check the Fractional Overlap against the Threshold
					if( ( fracOverlapVal1 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) || ( fracOverlapVal2 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) )
					{
						// Increment the Number of Detected Faces Overlapped with the Tracked Face
						mftracker->m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array[ trackedFaceCount ] = ( mftracker->m_DetectedFaceNum_OverlappedWith_TrackedFaceRegion_Array[ trackedFaceCount ] ) + 1;
					}
				}
			}
			
			// Looping over the Detected Faces
			for( int detectedFaceCount = 0 ; detectedFaceCount < mftracker->m_CurrDetectedFaceNum ; ++detectedFaceCount )
			{
				// Initialize the Number of Tracked Faces Overlapped with the Detected Face to _ZERO_
				mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array[ detectedFaceCount ] = 0;
				
				// Compute the Inverse of the Detected Face Area
				float invFaceArea = 1.0 / ( (float) ( mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].width ) * ( mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].height ) );
			
				// Looping over the Tracked Faces
				for( int trackedFaceCount = 0 ; trackedFaceCount < mftracker->m_ActiveFaceNum ; ++trackedFaceCount )
				{
					// Read the Face Index
					int faceIndx = mftracker->m_ActiveFaceIndxVec[ trackedFaceCount ];
					
					// Compute the Fractional Overlap with the Detected Face
					float fracOverlapVal1 = invFaceArea * ( mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix[ trackedFaceCount ][ detectedFaceCount ] );
					float fracOverlapVal2 = ( mftracker->m_Tracked_Detected_FaceRegionOverlap_Matrix[ trackedFaceCount ][ detectedFaceCount ] ) / ( (float) ( mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.width ) * ( mftracker->m_FaceSet[ faceIndx ].m_FaceBoundingBox.height ) );
					
					// Check against the Overlap Threshold
					if( ( fracOverlapVal1 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) || ( fracOverlapVal2 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) )
					{
						// Increment the Count
						mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array[ trackedFaceCount ] = ( mftracker->m_TrackedFaceNum_OverlappedWith_DetectedFaceRegion_Array[ trackedFaceCount ] ) + 1;
					}
				}
			}
			
			// Looping Over the Detected Faces to Compute the Number of Faces Associated with Each Other
			for( int detectedFaceCount = 0 ; detectedFaceCount < mftracker->m_CurrDetectedFaceNum ; ++detectedFaceCount )
			{
				// Compute the Inverse of the Current Face Area
				float invFaceArea = 1.0 / ( ( float ) ( mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].width ) * ( mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].height ) );
				
				// Initialize the Number of Overlapped Faces to _ZERO_
				mftracker->m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] = 0;
				
				// Looping Over the Other Faces to Compute the Overlaps between Faces
				for( int counter = 0 ; counter < mftracker->m_CurrDetectedFaceNum ; ++counter )
				{
					// Proceed for Different Face Regions
					if( counter != detectedFaceCount )
					{
						// Compute the Overlap between the Faces
						float overlapVal = 0;
						computeIntervalOverlap_2D( &mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ] , &mftracker->m_DetectedFace_BoundingBoxArray[ counter ] , &overlapVal );
						
						// Check for Significant Overlap
						float fracOverlapVal1 = overlapVal * invFaceArea;
						float fracOverlapVal2 = overlapVal / ( ( float ) ( mftracker->m_DetectedFace_BoundingBoxArray[ counter ].width ) * ( mftracker->m_DetectedFace_BoundingBoxArray[ counter ].height ) );
						if( ( fracOverlapVal1 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) || ( fracOverlapVal2 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) )
						{
							// Increment the Overlap Count
							mftracker->m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] = ( mftracker->m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] ) + 1;
						}
					}
				}
			}
				
			// Looping Over the Tracked Faces to Compute the Number of Faces Associated with Each Other
			for( int trackedFaceCount = 0 ; trackedFaceCount < mftracker->m_ActiveFaceNum ; ++trackedFaceCount )
			{
				// Read the Face Index
				int faceIndx1 = mftracker->m_ActiveFaceIndxVec[ trackedFaceCount ];
				
				// Compute the Inverse of the Current Face Area
				float invFaceArea = 1.0 / ( ( float ) ( mftracker->m_FaceSet[ faceIndx1 ].m_FaceBoundingBox.width ) * ( mftracker->m_FaceSet[ faceIndx1 ].m_FaceBoundingBox.height ) );
				
				// Initialize the Number of Overlapped Faces to _ZERO_
				mftracker->m_Tracked_Face2Face_Overlap_Array[ trackedFaceCount ] = 0;
				
				// Looping Over the Other Faces to Compute the Overlaps between Faces
				for( int counter = 0 ; counter < mftracker->m_ActiveFaceNum ; ++counter )
				{
					// Proceed for Different Face Regions
					if( counter != trackedFaceCount )
					{
						// Read the Face Index
						int faceIndx2 = mftracker->m_ActiveFaceIndxVec[ counter ];
						
						// Compute the Overlap between the Faces
						float overlapVal = 0;
						computeIntervalOverlap_2D( &mftracker->m_FaceSet[ faceIndx1 ].m_FaceBoundingBox , &mftracker->m_FaceSet[ faceIndx2 ].m_FaceBoundingBox , &overlapVal );
						
						// Check for Significant Overlap
						float fracOverlapVal1 = overlapVal * invFaceArea;
						float fracOverlapVal2 = overlapVal / ( ( float ) ( mftracker->m_FaceSet[ faceIndx2 ].m_FaceBoundingBox.width ) * ( mftracker->m_FaceSet[ faceIndx2 ].m_FaceBoundingBox.height ) );
						if( ( fracOverlapVal1 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) || ( fracOverlapVal2 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) )
						{
							// Increment the Overlap Count
							mftracker->m_Tracked_Face2Face_Overlap_Array[ trackedFaceCount ] = ( mftracker->m_Tracked_Face2Face_Overlap_Array[ trackedFaceCount ] ) + 1;
						}
					}
				}
			}
		}
		// Otherwise...
		// Case-II : [ detectedFaceNum > 0 ] AND [ activeFaceNum = 0 ]
		else
		{
			// Looping Over the Detected Faces to Compute the Number of Faces Associated with Each Other
			for( int detectedFaceCount = 0 ; detectedFaceCount < mftracker->m_CurrDetectedFaceNum ; ++detectedFaceCount )
			{
				// Compute the Inverse of the Current Face Area
				float invFaceArea = 1.0 / ( ( float ) ( mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].width ) * ( mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ].height ) );
				
				// Initialize the Number of Overlapped Faces to _ZERO_
				mftracker->m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] = 0;
				
				// Looping Over the Other Faces to Compute the Overlaps between Faces
				for( int counter = 0 ; counter < mftracker->m_CurrDetectedFaceNum ; ++counter )
				{
					// Proceed for Different Face Regions
					if( counter != detectedFaceCount )
					{
						// Compute the Overlap between the Faces
						float overlapVal = 0;
						computeIntervalOverlap_2D( &mftracker->m_DetectedFace_BoundingBoxArray[ detectedFaceCount ] , &mftracker->m_DetectedFace_BoundingBoxArray[ counter ] , &overlapVal );
						
						// Check for Significant Overlap
						float fracOverlapVal1 = overlapVal * invFaceArea;
						float fracOverlapVal2 = overlapVal / ( ( float ) ( mftracker->m_DetectedFace_BoundingBoxArray[ counter ].width ) * ( mftracker->m_DetectedFace_BoundingBoxArray[ counter ].height ) );
						if( ( fracOverlapVal1 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) || ( fracOverlapVal2 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) )
						{
							// Increment the Overlap Count
							mftracker->m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] = ( mftracker->m_Detected_Face2Face_Overlap_Array[ detectedFaceCount ] ) + 1;
						}
					}
				}
			}
		}
	}
	// Otherwise...
	else
	{
		// Case-III : [ detectedFaceNum = 0 ] AND [ activeFaceNum > 0 ]
		if( mftracker->m_ActiveFaceNum > 0 )
		{
			// Looping Over the Tracked Faces to Compute the Number of Faces Associated with Each Other
			for( int trackedFaceCount = 0 ; trackedFaceCount < mftracker->m_ActiveFaceNum ; ++trackedFaceCount )
			{
				// Read the Face Index
				int faceIndx1 = mftracker->m_ActiveFaceIndxVec[ trackedFaceCount ];
				
				// Compute the Inverse of the Current Face Area
				float invFaceArea = 1.0 / ( ( float ) ( mftracker->m_FaceSet[ faceIndx1 ].m_FaceBoundingBox.width ) * ( mftracker->m_FaceSet[ faceIndx1 ].m_FaceBoundingBox.height ) );
				
				// Initialize the Number of Overlapped Faces to _ZERO_
				mftracker->m_Tracked_Face2Face_Overlap_Array[ trackedFaceCount ] = 0;
				
				// Looping Over the Other Faces to Compute the Overlaps between Faces
				for( int counter = 0 ; counter < mftracker->m_ActiveFaceNum ; ++counter )
				{
					// Proceed for Different Face Regions
					if( counter != trackedFaceCount )
					{
						// Read the Face Index
						int faceIndx2 = mftracker->m_ActiveFaceIndxVec[ counter ];
						
						// Compute the Overlap between the Faces
						float overlapVal = 0;
						computeIntervalOverlap_2D( &mftracker->m_FaceSet[ faceIndx1 ].m_FaceBoundingBox , &mftracker->m_FaceSet[ faceIndx2 ].m_FaceBoundingBox , &overlapVal );
						
						// Check for Significant Overlap
						float fracOverlapVal1 = overlapVal * invFaceArea;
						float fracOverlapVal2 = overlapVal / ( ( float ) ( mftracker->m_FaceSet[ faceIndx2 ].m_FaceBoundingBox.width ) * ( mftracker->m_FaceSet[ faceIndx2 ].m_FaceBoundingBox.height ) );
						if( ( fracOverlapVal1 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) || ( fracOverlapVal2 > mftracker->m_Tracked_Detected_FaceRegionOverlap_Threshold ) )
						{
							// Increment the Overlap Count
							mftracker->m_Tracked_Face2Face_Overlap_Array[ trackedFaceCount ] = ( mftracker->m_Tracked_Face2Face_Overlap_Array[ trackedFaceCount ] ) + 1;
						}
					}
				}
			}
		}
		
		// Case-IV : [ detectedFaceNum = 0 ] AND [ activeFaceNum = 0 ]
		// Do Nothing
	}
	
	// Set the Reasoning Data Ready Flag to TRUE
	mftracker->m_MFTrackerReasoningDataReady = true;
	
	// Error Code 0 : All Well, That Ends Well
	return( 0 );
}

