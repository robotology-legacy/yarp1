/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Carlos Beltran Gonzalez#                      ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: YARPSoundTemplate.h,v 1.8 2004-09-13 17:54:25 beltran Exp $
///

/** 
 * @file YARPSoundTemplate.h Implements a sound template
 * 
 * @author Carlos Beltran
 * @date 2004-08-24
 */

#ifndef __YARPSoundTemplateh__
#define __YARPSoundTemplateh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPMatrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include "YARPExMatrix.h"

#define ARRAY_MAX 110 // Aproximatelly 5 seconds sound (22 frames/second)

#ifndef	HUGE
	#define	HUGE	1e30
#endif

/** 
 * A class that implements sound template.
 * This class has an array of YVector * pointing to a serie of YVectors representing a sound 
 * template; originally it has been thougth to work with sound parametrization known as MFCC
 * (Mel-Frequency Cepstral Coefficients) but it can support any other kind of sound parametrization.
 */
class YARPSoundTemplate
{
private:

    int m_currentsize;     /** This stores the real size of the used part of the m_parray.          */
    int m_totalsize;       /** This is the real total size of the m_parray.                         */
    int m_vectors_length;  /** This is the size of the internal vectors; This length is defined by. */
                           /** the first vector introduced in the template.                         */
    YVector ** m_parray;   /** This is the actual pointer to the array of vectors.                  */

public:
	/** 
	 * Constructor.
	 */
	YARPSoundTemplate()
	{
		YARPSoundTemplate(ARRAY_MAX);
	}
	/** 
	 * Overloaded constructor.
	 * 
	 * @param size the size of the internal vectors array
	 */
	YARPSoundTemplate(int size)
	{
		m_currentsize    = 0;
		m_vectors_length = 0;
		m_totalsize      = size;
		m_parray         = new YVector *[size];
		memset(m_parray, 0 , sizeof(YVector *) * size);
	}
	/** 
	 * Destructor.
	 */
	~YARPSoundTemplate()
	{
		if (m_parray != NULL)
		{
			for( int i = 0; i < m_currentsize; i++)	
				if ( m_parray[i] != NULL ) delete m_parray[i];
			delete[] m_parray;
		}
	}

	/** 
	 * Returns a pointer to the array of vectors.
	 * 
	 * @return 
	 */
	inline YVector ** data(void) { return m_parray; }

	/** 
	 * Resize the array with a standart size.
	 * It copies all the pointers form the old array to the new one. This should be efficient.
	 * It resizes with the standart size (ARRAY_MAX) 
	 */
	inline void
	Resize()
	{
		Resize(m_totalsize + ARRAY_MAX);  // Just add some more time storing capacity to the template
	}

	/** 
	 * Resizes with a new size.
	 * If the new size is smaller that the previous one some data is being lost.
	 * 
	 *
	 * @param new_size The new size of the array of sound vectors
	 */
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

	/** 
	 * Returns the "current" size of the template.
	 * 
	 * @return  the current size of the template
	 */
	inline int
	Length(){ return m_currentsize; }

	/** 
	 * Returns the total available size of the template.
	 * 
	 * @return the total available size of the template
	 */
	inline int
	Size(){ return m_totalsize; }
	
	/** 
	 * Returns the length of the vectors inside the template.
	 * 
	 * @return the length of the vector inside the template
	 */
	inline int
	VectorLength(){ return m_vectors_length; }

	/** 
	 * Just add a new vector at the end of the array.
	 * 
	 * @param in the YVector to be added to the template, the vector is copied to a local copy
	 * @param flag this flag controls what to do when the Template is full.
	 * 	-# 0 The new vector is NOT added. It returns waiting from software on the top to resize the Template
	 * 	and repeat the adding operation.
	 * 	-# 1 Resizes the template in just one new space and adds the new vector in this new created space.
	 * 	-# 2 Removes the oldest vector creating the space for the new one.
	 * 
	 * @return The result of the operation
	 * 	-# 0  The template is already full
	 * 	-# -1 The added vector has a differnt length of those already inside the template
	 * 	-# 1  The new vector was added succesfully
	 * 	-# -2 The template has not reserved memory, a resize is needed for the new data
	 * 		  this could happen in the case a Destroy is called before an Add
	 */
	inline int
	Add(YVector &in, int flag = 0)
	{
        int vectorsize        = 0;    /** The size of the vector we want to add.             */
        YVector * new_pvector = NULL; /** A pointer to create a new vector for the template. */

		if ( m_parray == NULL)
		{
			ACE_OS::fprintf(stdout,"YARPSoundTemplate: Sorry, the sound template has not memory available\n");
			return (-2);
		}
        
		vectorsize = in.Length();
        if (m_currentsize == 0)                 // This is the first vector in the template
            m_vectors_length = vectorsize;      // Define the template vector length
        else                                    // There are some other vectors to compare with
            if (vectorsize != m_vectors_length) // The 'in' vector has a different size of that
                return (-1);                    // of the other vectors in the template

        if (m_currentsize == m_totalsize)       // The Template is full
			switch(flag)
			{
                case 0: return(0);break;                // Nothing to do just returning with error
                case 1: Resize(m_totalsize+1);break;    // Making just one space for one new vector
                case 2: Bufferize(in); return(1);break; // Force the introduction of the new vector
			}

		new_pvector = new YVector();
		new_pvector->Resize(m_vectors_length);

		*(new_pvector) = in;

		m_parray[m_currentsize] = new_pvector;
		m_currentsize++;
		return 1;
	}

	/** 
	  * Adds a vector in the position of the last element; it moves left-size all the vectors to make space 
	  * to the new element. In other words, the oldest vector is eliminated to make space to the new arrival
	  * that is stored at the end of the template.
	  * 
	  * @param vector A reference to the new vector.
	  * 
	  * @return YARP_OK
	  */
	inline 
	int Bufferize(YVector &vector)
	{
		int i = 0;
        YVector * new_pvector = NULL; /** A pointer to create a new vector for the template. */

		delete m_parray[0]; // Just eliminate the first element

		for( i = 0; i < m_currentsize-1; i++)
			m_parray[i] = m_parray[i+1];

		new_pvector = new YVector();
		new_pvector->Resize(m_vectors_length);
		*(new_pvector) = vector;

		i++;
		m_parray[i] = new_pvector;

		return YARP_OK;
		
	}

	/** 
	  * Deletes a vector in the template; "element" determinates the position of the vector
	  * inside the template.
	  * 
	  * @param index An integer that indicates the position of the vector to be eliminated (starts at 0).
	  * 
	  * @return 
	  * 	-# YARP_OK if the deleting operation was done correctly
	  * 	-# YARP_FAIL if there was an error
	  * 		-# the index parameter was bigger than the current size of the template
	  * 		-# the index parameter was negative
	  */
	inline
	int Delete(int index)
	{
		int i = 0;
		int j = 0;

		if (index >= m_currentsize || index < 0)
		{
			ACE_OS::fprintf(stdout, "YARPSoundTemplate: Sorry! Element %d can not be removed \n",index);
			return YARP_FAIL;
		}

		ACE_ASSERT( m_parray != NULL);

		delete m_parray[index];

		for( i = index; i < m_currentsize-1; i++)
			m_parray[i] = m_parray[i+1];

		m_currentsize -= 1;

		return YARP_OK;
	}
	
	/** 
	  * Save the template to a file.
	  * 
	  * @param name The name of the file to save the template
	  */
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

	/** 
	  * Delete all the content of the buffer and deallocated the memory.
	  */
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

	/** 
	  * Calculates the covariance matrix of the sound template.
	  * 
	  * @param mCov A reference to an external matrix to put the covariance data.
	  * @param flag Determinates if the data in normalized with N or N-1
	  * 	-# 0 N-1 is used
	  * 	-# 1 N is used
	  * 
	  * @return 
	  * 	-# YARP_OK
	  * 	-# YARP_FAIL
	  */
	int
	CovarianceMatrix(YARPCovMatrix &mCov, int flag = 0)
	{
		int i;
		int j;
		double sum = 0.0;
        YVector _means;           /** Local temporal vector to store the mean values.   */
        YMatrix _xvars;           /** Temporal matrix to store the local variances.     */
        YVector * pvector = NULL; /** Temporal pointer to YVector.                      */
        double  * pdata   = NULL; /** Temporal pointer to access YVector internal data. */

		_means.Resize(m_vectors_length);
        _xvars.Resize(m_currentsize,m_vectors_length);
		
		//----------------------------------------------------------------------
		// Calculate means. 
		// We navigate in the template structure to calculate the means of the
		// coefficients in the "time" dimension. This is, we calculate the mean
		// of each coefficient of the YVectors using the values of that coeficient 
		// in the different YVectors (samples) of the template.
		//----------------------------------------------------------------------
		for ( i = 0; i < m_vectors_length; i++)
		{
			sum = 0.0;
			for( j = 0; j < m_currentsize; j++)
			{
                pvector = m_parray[j];     // Get the pointer to the vector data
                pdata = pvector->data();
                sum += pdata[i];           // Add the i value of the vector to the sum
            }
            _means[i] = sum/m_currentsize; // We store the mean in the means vector
		}

		//----------------------------------------------------------------------
		//  Calculate the local variances matrix.
		//----------------------------------------------------------------------
		for ( i = 1; i <= m_currentsize; i++)
		{
			// Get the pointer to the vector data
			pvector = m_parray[i-1];
			pdata   = pvector->data();

			for( j = 1; j <= m_vectors_length; j++)
				_xvars(i,j) = pdata[j-1] - _means[j-1];
		}

		// Calculate the final covariance matrix
		if (flag)
			mCov = (_xvars.Transposed() * _xvars) / (m_currentsize); 
		else
			mCov = (_xvars.Transposed() * _xvars) / (m_currentsize-1);

		mCov.setOriginalVariancesMatrix(_xvars);

		return YARP_OK;
	}

	/** 
	  * Dynamic Time Warping algorithm.
	  * It performs the comparison between this template and another template beeing passed as
	  * a parameter. For a detailed description have a look at the next reference.
	  * \latexonly
	  * \bibitem{Rabiner93} Lawrence Rabiner,%
	  * 	\emph{Fundamental of Speech Recognition}, 1993, Prentice Hall Signal Processing Series
	  * \endlatexonly
	  * 
	  * @param unknow_template The unknown sound template to be compared
	  * @param result 
	  * 	-# YARP_OK if the result is ok
	  * 	-# YARP_FAIL if there is some problem comparing the templates
	  * @return optimal distance measurement between the local and the external templates
	  */
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
