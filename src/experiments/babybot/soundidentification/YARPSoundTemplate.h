// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2004 
//
//                    #Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  YARPSoundTemplate.h
// 
//     Description:  This class implements a sound template.  
// 
//         Version:  $Id: YARPSoundTemplate.h,v 1.2 2004-08-24 13:32:43 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================
#ifndef __YARPSoundTemplateh__
#define __YARPSoundTemplateh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPMatrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#define ARRAY_MAX 110 // Aproximatelly 5 seconds sound (22 frames/second)

#ifndef	HUGE
	#define	HUGE	1e30
#endif

// =====================================================================================
//        Class:  YARPSoundTemplate
// 
//  Description:  YARPSoundTemplate has an array of YVector * pointing to a serie of YVectors
//				  representing a sound template; originally it has been thought to work with
//				  sound parametrization known as MFCC (Mel-Frequency Cepstral Coefficients) but
//				  it can support any other kind of sound parametrization. 
// 
//       Author:  Carlos Beltran
//      Created:  03/08/2004 16:08:13 W. Europe Daylight Time
//     Revision:  none
// =====================================================================================
class YARPSoundTemplate
{
private:
    int m_currentsize;    // This stores the real size of the used part of the m_parray
    int m_totalsize;      // This is the real total size of the m_parray
    int m_vectors_length; // This is the size of the internal vectors. This length is defined by
                          // the first vector introduced in the template
    YVector ** m_parray;  // This is the actual pointer to the array of vectors

public:
	YARPSoundTemplate()
	{
		YARPSoundTemplate(ARRAY_MAX);
	}
	YARPSoundTemplate(int size)
	{
		m_currentsize    = 0;
		m_vectors_length = 0;
		m_totalsize      = size;
		m_parray         = new YVector *[size];
		memset(m_parray, 0 , sizeof(YVector *) * size);
	}
	~YARPSoundTemplate()
	{
		if (m_parray != NULL)
		{
			for( int i = 0; i < m_currentsize; i++)	
				if ( m_parray[i] != NULL ) delete m_parray[i];
			delete[] m_parray;
		}
	}

	// Returns a pointer to the array of vectors
	inline YVector ** data(void) { return m_parray; }

	//--------------------------------------------------------------------------------------
	//       Class:  YARPSoundTemplate
	//      Method:  Resize
	// Description:  Resize the array with the new size. It copies all the pointers from the 
	//				 old array to the new one. This should be efficient. If the new size is
    // 				 smaller that the previous one some data is being lost.	
	//--------------------------------------------------------------------------------------
	inline void
	Resize()
	{
		Resize(m_totalsize + ARRAY_MAX);  // Just add some more time storing capacity to the template
	}

	inline void 
	Resize( int new_size)
	{
		if ( new_size == m_totalsize) //The user is asking for the same size!
			return;

		YVector ** temp_parray = new YVector *[new_size];
		memset(temp_parray, 0 , sizeof(YVector *) * new_size);

		if( m_parray != NULL)
		{
			for( int i = 0; i < m_currentsize; i++)
				if ( i < new_size)
					temp_parray[i] = m_parray[i]; 
				else // Atention some data is beeing lost because there is no space in the new array 
					delete m_parray[i];

			if ( new_size < m_currentsize)
			{
				ACE_OS::fprintf(stdout, "YARPSoundTemplate: Atention! You are problably losing data in a resize\n");
				m_currentsize = new_size;
			}

			m_totalsize = new_size;

			delete[] m_parray;      // Delete the old pointers array
			m_parray = temp_parray; // Get the address of the new pointers array

		}else //The template has not any previous reserved memory
		{
			m_parray = temp_parray;
			m_currentsize = 0;
			m_totalsize = new_size;
		}
	}

	//--------------------------------------------------------------------------------------
	//       Class:  YARPSoundTemplate
	//      Method:  Length
	// Description:  Returns the "current" size of the template
	//--------------------------------------------------------------------------------------
	inline int
	Length(){ return m_currentsize; }

	//--------------------------------------------------------------------------------------
	//       Class:  YARPSoundTemplate
	//      Method:  Size
	// Description:  Returns the total available size of the template
	//--------------------------------------------------------------------------------------
	inline int
	Size(){ return m_totalsize; }
	
	//--------------------------------------------------------------------------------------
	//       Class:  YARPSoundTemplate
	//      Method:  VectorLength
	// Description:  Returns the length of the vectors inside the template
	//--------------------------------------------------------------------------------------
	inline int
	VectorLength(){ return m_vectors_length; }

	//--------------------------------------------------------------------------------------
	//       Class:  YARPSoundTemplate
	//      Method:  Add
	// Description:  Just add a new vector at the end of the array
	// Parameters: 
	//		in : the YVector to be added to the template, the vector is copied to a local copy
	//		out: 0 if the Template is already full ()
	//			 -1 the added vector has a different length of those already inside the template
	//			 1 if the new vector was added successfully
	//			 -2 The template has not reserved memory, a resize is needed for the new data
	//			    this could happen in the case a Destroy is called before an Add
	//--------------------------------------------------------------------------------------
	inline int
	Add(YVector &in)
	{
		int vectorsize        = 0;
		YVector * new_pvector = NULL;

		if ( m_parray == NULL)
		{
			ACE_OS::fprintf(stdout,"YARPSoundTemplate: Sorry, the sound template has not memory available\n");
			return (-2);
		}

        if (m_currentsize == m_totalsize)       // The Template is full
            return (0);

        vectorsize = in.Length();

        if (m_currentsize == 0)                 // This is the first vector in the template
            m_vectors_length = vectorsize;      // Define the template vector length
        else                                    // There are some other vectors to compare with
            if (vectorsize != m_vectors_length) // The 'in' vector has a different size of that
                return (-1);                    // of the other vectors in the template

		new_pvector = new YVector();
		new_pvector->Resize(m_vectors_length);

		*(new_pvector) = in;

		m_parray[m_currentsize] = new_pvector;
		m_currentsize++;
		return 1;
	}

	//--------------------------------------------------------------------------------------
	//       Class:  YARPSoundTemplate
	//      Method:  Save
	// Description:  Save the template to a file
	//--------------------------------------------------------------------------------------
	void 
	Save(YARPString name)
	{
		FILE * pfile;
		int i = 0;
		int j = 0;
		YVector * pvector = NULL;
		double  * pdata   = NULL;

		pfile = fopen(name.c_str(),"w");
		ACE_ASSERT(pfile != NULL);

		for ( i = 0; i < m_currentsize; i++)
		{
			pvector = m_parray[i];
			pdata = pvector->data();
			for ( j = 0; j < pvector->Length(); j++)
				fprintf(pfile,"%f ",pdata[j]);
			fprintf(pfile,"\n");
		}

		fclose(pfile);
	}

	//--------------------------------------------------------------------------------------
	//       Class:  YARPSoundTemplate
	//      Method:  destroy
	// Description:  Delete all the content of the buffer and deallocated the memory
	//--------------------------------------------------------------------------------------
	void 
	Destroy()
	{
		int i = 0;
		YVector * pvector = NULL;

		// Delete the individual vectors
		for( i = 0; i < m_currentsize; i++ )
		{
			pvector = m_parray[i];
			delete pvector;
		}

		// Delete the vector pointers array
		delete[] m_parray;
		m_parray         = NULL;
		m_currentsize    = 0;
		m_vectors_length = 0;
		m_totalsize      = 0;
	}

	//--------------------------------------------------------------------------------------
	//       Class: YARPSoundTemplate
	//      Method: Dtw 
	// Description: Dynamic Time Warping algorithm. For a detailed description have a look
    // at the next reference:
	// 		"Fundamentals of Speech Recognition"
    //		Lawrence Rabiner, Biing-Hwang Juang
	//		Prentice Hall Signal Precessing Series	
	// It performs the comparison between this template and another template beeing passed as
	// a parameter. It return an optimal distance measurement
	//--------------------------------------------------------------------------------------
	inline double 
	Dtw(YARPSoundTemplate &unknow_template, int * result)
	{
		int i = 0;
		int j = 0;
		int k = 0;
		int tempSize = 0;
		int nunkTemp = 0;
		int nrefTemp = 0;
		double bestDist = 0.0;
		double diff     = 0.0;
		double sum      = 0.0;
		double   *p_unkTemp       = NULL;
		double   *p_refTemp       = NULL;
		YVector **punkvectorarray = NULL;
		YMatrix   localDist;
		YMatrix   globalDist;

		nunkTemp = unknow_template.Length();
		nrefTemp = Length();
        tempSize = m_vectors_length; // This assigns the length of the template vectors. This length has to be the same in both templates

		//----------------------------------------------------------------------
		//  Check if both template vectors length are equal
		//----------------------------------------------------------------------
		if ( m_vectors_length != unknow_template.VectorLength())
		{
			ACE_OS::fprintf(stdout,"YARPSoundTemplate: Error, trying to compare two Templates with different vector lengths");
			*result = YARP_FAIL;
			return -1.0;
		}

		localDist.Resize  ( nunkTemp,nrefTemp);
		globalDist.Resize ( nunkTemp,nrefTemp);
		
		//----------------------------------------------------------------------
		// Get the pointer to the array of vector pointers of the unknown template
		//----------------------------------------------------------------------
        punkvectorarray = unknow_template.data(); 

		//----------------------------------------------------------------------
		// Check if the templates have the data
		//----------------------------------------------------------------------
		if ((m_parray           ==  NULL ) || 
			(m_parray[0]        ==  NULL ) || 
			(punkvectorarray    ==  NULL ) || 
			(punkvectorarray[0] ==  NULL ))
		{
			ACE_OS::fprintf(stdout,"YARPSoundTemplate: Error, trying to compare against an empty template");
			*result = YARP_FAIL;
			return -1.0;
		}

		//----------------------------------------------------------------------
		//  Compute all the local distances
		//----------------------------------------------------------------------
		for(i = 1; i <= nunkTemp; i++)
			for(j = 1; j <= nrefTemp; j++)
			{
				sum = 0.0;
				p_unkTemp = punkvectorarray[i-1]->data();
				p_refTemp = m_parray[j-1]->data();

				//Just calculate the distance between the vectors
				for ( k = 0; k < tempSize; k++)
				{
					diff = p_unkTemp[k] - p_refTemp[k];
					sum += (diff*diff);
				}
				localDist(i,j) = sum;
			}

		//----------------------------------------------------------------------
		// For the first frame the only possible mathc is at (1,1) 
		//----------------------------------------------------------------------
		globalDist(1,1) = localDist(1,1);
		for(j = 2; j <= nrefTemp; j++)
			globalDist(1,j) = HUGE;

		//----------------------------------------------------------------------
		//  In the second frame the only valid state is (2,2)
		//----------------------------------------------------------------------
		globalDist(2,1) = HUGE;
		globalDist(2,2) = globalDist(1,1) + localDist(2,2);
		for(j = 3; j <= nrefTemp; j++)
			globalDist(2,j) = HUGE;

		//----------------------------------------------------------------------
		// Do the general case for the rest of the frames 
		//----------------------------------------------------------------------
		for(i = 3; i <= nunkTemp; i++) 
		{
			globalDist(i,1) = HUGE;
			globalDist(i,2) = globalDist(i - 1,1) + localDist(i,2);

			for(j = 3; j <= nrefTemp; j++) 
			{
				double topPath = 0.0;
				double midPath = 0.0;
				double botPath = 0.0;

				if( globalDist(i-2,j-1) < 0.0)
					printf("DTWMessage: globalDist(i-2,j-1) < 0.0\n: %d\t%d\t%f\n", i, j, globalDist(i-2,j-1));

				topPath = globalDist(i-2,j-1) + localDist(i-1,j) + localDist(i,j);
				midPath = globalDist(i-1,j-1) + localDist(i,j);
				botPath = globalDist(i-1,j-2) + localDist(i,j-1) + localDist(i,j);

				//----------------------------------------------------------------------
				// Find and store the smallest global distance 
				//----------------------------------------------------------------------
				if(topPath < midPath && topPath < botPath)
					globalDist(i,j) = topPath;
				else if (midPath < botPath)
					globalDist(i,j) = midPath;
				else
					globalDist(i,j) = botPath;
			}    
		}

		bestDist = globalDist(nunkTemp,nrefTemp);

		*result = YARP_OK;
		return(bestDist);
	}
};
#endif
