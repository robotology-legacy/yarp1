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
/// $Id: YARPSoundTemplate.h,v 1.18 2004-12-30 16:50:46 beltran Exp $
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
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include "soundidentificationprocessing.h"

#define ARRAY_MAX 30 // Aproximatelly 5 seconds sound (22 frames/second)

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

    unsigned int m_totalsize;       /** This is the real total size of the m_parray.                         */
    int m_vectors_length;  /** This is the size of the internal vectors; This length is defined by. */
                            /** the first vector introduced in the template.                         */
    //YVector ** m_parray;   /** This is the actual pointer to the array of vectors.                  */
	YARPList<YVector> m_parray; /** This is the list of pointers to vectors. */

public:
	/** 
	 * Constructor.
	 */
	YARPSoundTemplate()
	{
		//YARPSoundTemplate(ARRAY_MAX);
		m_vectors_length = 0;
		m_totalsize      = ARRAY_MAX;
	}
	/** 
	 * Overloaded constructor.
	 * 
	 * @param size the size of the internal vectors array
	 */
	YARPSoundTemplate(int size)
	{
		m_vectors_length = 0;
		m_totalsize      = size;
	}
	/** 
	 * Destructor.
	 */
	~YARPSoundTemplate()
	{
		if ( !m_parray.empty())
			m_parray.clear();
	}

	/** 
	 * Returns a pointer to the array of vectors.
	 * 
	 * @return 
	 */
	//inline YVector ** data(void) const { return m_parray; }

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
	Resize( unsigned int new_size) {

		ACE_ASSERT ( new_size != 0 );
		if ( m_totalsize > new_size) {

			if ( m_parray.size() > new_size ) {
				while ( m_parray.size() != new_size) {
					m_parray.pop_back();
				}
			}
			m_totalsize = new_size;
		}
		else {
			m_totalsize = new_size; 
		}
	}

	/** 
	  * Checks whether the template if full or not.
	  * 
	  * @return 
	  * 	-# True is the template is full.
	  * 	-# False otherwise.
	  */
	bool isFull () const
	{
		if (m_parray.size() >= m_totalsize)
			return true;
		else 
			return false;
	}


	/** 
	 * Returns the "current" size of the template.
	 * 
	 * @return  the current size of the template
	 */
	inline int
	Length() const { return m_parray.size(); }

	/** 
	 * Returns the total available size of the template.
	 * 
	 * @return the total available size of the template
	 */
	inline unsigned int
	Size() const { return m_totalsize; }
	
	/** 
	 * Returns the length of the vectors inside the template.
	 * 
	 * @return the length of the vector inside the template
	 */
	inline int
	VectorLength() const { return m_vectors_length; }

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
		LOCAL_TRACE("YARPSoundTemplate: Entering Add");
        int vectorsize        = 0;    /** The size of the vector we want to add.             */

		vectorsize = in.Length();
		ACE_ASSERT( vectorsize != 0 );
		
		if (m_parray.size() == 0)                 // This is the first vector in the template
			m_vectors_length = vectorsize;      // Define the template vector length
		else                                    // There are some other vectors to compare with
			if (vectorsize != m_vectors_length) {
				ACE_OS::fprintf(stdout,"YARPSoundTemplate: Error, adding a vector with an erroneous vector size\n"); 
				return (YARP_FAIL);                    // of the other vectors in the template
			}

        if (m_parray.size() == m_totalsize)       // The Template is full
		{
			switch(flag)
			{
				case 0: return(0);break;                // Nothing to do just returning with error
				case 1: Resize(m_totalsize+1);break;    // Making just one space for one new vector
				case 2: m_parray.pop_front();break; // Delete the firts vector to make space. 
			}
		}

		m_parray.push_back( in );

		return 1;
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
	int Delete(unsigned int index)
	{
		LOCAL_TRACE("YARPSoundTemplate: Entering Delete");
		YARPListIterator<YVector> soundTemplateIterator(m_parray);
		unsigned int i = 0;

		if (index >= m_parray.size())
		{
			ACE_OS::fprintf(stdout, "YARPSoundTemplate: Sorry! Element %d can not be removed \n",index);
			return YARP_FAIL;
		}

		soundTemplateIterator.go_head();
		for ( i = 0; i < index; i++) {
			soundTemplateIterator++; 
		}
		m_parray.erase( soundTemplateIterator );


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
		LOCAL_TRACE("YARPSoundTemplate: Entering Save");
		FILE * pfile;
		unsigned int i = 0;
		int j = 0;
		double  * pdata   = NULL;
		YARPListIterator<YVector> soundTemplateIterator(m_parray);

		// Open file
		pfile = fopen(name.c_str(),"w");
		ACE_ASSERT(pfile != NULL);

		soundTemplateIterator.go_head();
		
		for ( i = 0; i < m_parray.size(); i++)
		{
			YVector &pvector = *soundTemplateIterator;
			*soundTemplateIterator++;
			pdata = pvector.data();
			for ( j = 0; j < pvector.Length(); j++)
				fprintf(pfile,"%f ",pdata[j]);
			fprintf(pfile,"\n");
		}

		fclose(pfile);
	}

	/** 
	  * Loads a sound template from a file.
	  * 
	  * @param name The name of the file.
	  */
	int 
	Load(YARPString name) {

		LOCAL_TRACE("YARPSoundTemplate: Entering Load");
		//ACE_OS::fprintf(stdout, "YARPSoundTemplate: Entering Load\n");

		char line[256];
		YVector valuesVector;
		YARPVector<double> temporalVector;

		ifstream infile;
	   	infile.open( name.c_str() );

		Destroy();

		if ( infile.is_open() ) {

			while (infile.getline(line, 256)) {

				YARPString linestring(line);
				YARPString numberString;
				int startposition = 0;
				int endposition   = 0;

				while ( (endposition = linestring.find( " ", startposition)) != -1) {

					double dValue = 0.0;
					int lenght = endposition - startposition;

					numberString = linestring.substring(
						startposition, 
						lenght
						);

					dValue = atof( numberString.c_str() );

					temporalVector.add_tail( dValue );
					startposition = endposition+1;
				}

				////Fill the YARP vector
				if ( temporalVector.size() > 0 ) {

					valuesVector.Resize( temporalVector.size() );

					YARPVectorIterator<double> theTemporalVectorIterator( temporalVector );

					theTemporalVectorIterator.go_head(); 

					for (unsigned int i = 0; i < temporalVector.size(); i++) {
						valuesVector[i] = *theTemporalVectorIterator;
						theTemporalVectorIterator++;
					}

					Add(valuesVector);
					//Clean the temporal vector.
					temporalVector.size(0);
				}
			}

			infile.close();
			return YARP_OK;
		}
		else {
			 return YARP_FAIL;
		}
	}

	/** 
	  * Copies sound templates.
	  * 
	  * @param soundTemplate The template to be copied.
	  * 
	  * @return The reference to the copied template.
	  */
	YARPSoundTemplate& operator=(const YARPSoundTemplate& soundTemplate)
	{
		YVector tempVector;

		Destroy();
		if ( m_totalsize != soundTemplate.Size() ) 
			Resize(soundTemplate.Size());

		
		m_parray = soundTemplate.m_parray;
		m_vectors_length = soundTemplate.m_vectors_length;
		return *this;
	}

	/** 
	  * Delete all the content of the buffer and deallocated the memory.
	  */
	void 
	Destroy()
	{
		LOCAL_TRACE("YARPSoundTemplate: Entering Destroy");

		int size = m_parray.size();
		//ACE_ASSERT( size != 0 );

		if ( size != 0)
			m_parray.clear();

		m_vectors_length = 0;
		m_totalsize      = ARRAY_MAX;
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
		LOCAL_TRACE("YARPSoundTemplate: Entering Dtw");
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
		//YVector **punkvectorarray = NULL;
		YARPListIterator<YVector> unknownIterator(unknow_template.m_parray);
		YARPListIterator<YVector> localIterator(m_parray);
		YMatrix   localDist;
		YMatrix   globalDist;

		nunkTemp = unknow_template.Length();
		ACE_ASSERT( nunkTemp != 0 );
		nrefTemp = Length();
		ACE_ASSERT( nrefTemp != 0 );
        tempSize = m_vectors_length; // This assigns the length of the template vectors. This length has to be the same in both templates
		ACE_ASSERT( tempSize != 0 );

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
		//  Compute all the local distances
		//----------------------------------------------------------------------
		unknownIterator.go_head();
		for(i = 1; i <= nunkTemp; i++) {
			localIterator.go_head();
			for(j = 1; j <= nrefTemp; j++)
			{
				sum = 0.0;
				//p_unkTemp = punkvectorarray[i-1]->data();
				//p_refTemp = m_parray[j-1]->data();
				YVector &unknownVectorReference = *unknownIterator;
				YVector &localVectorReference   = *localIterator;
				localIterator++;
				p_unkTemp = unknownVectorReference.data(); 
				p_refTemp = localVectorReference.data();

				//Just calculate the distance between the vectors
				for ( k = 0; k < tempSize; k++)
				{
					diff = p_unkTemp[k] - p_refTemp[k];
					sum += (diff*diff);
				}
				localDist(i,j) = sum;
			}
			unknownIterator++;
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
