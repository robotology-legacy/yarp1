/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: mesh.h,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///

/*--Include File Header--------------------------------------------------------

Module		mesh
                                                                           
Author		Paul Fitzpatrick, 4th Comp Eng, 9117261
Version		1.0
Date		15/11/94
Description This file declares classes to be used when setting up a Mesh
			data structure.

			A mesh is a loosely structured collections of meshlinks.
			Each meshlink has a source and a destination.
			The source and destination are other meshlinks.
			New meshlink classes can be derived to hold user data.

			The classes in this header implement meshes with the following
			properties
			-	For a meshlink L, all meshlinks that have L as their source
				can be found efficiently, without a search
			-	All meshlinks that have L as their destination can be found
				efficiently, without a search
			-	All meshlinks in a given mesh can be found efficiently
			-	Deleting or modifying a meshlink can be done safely - any
				meshlinks connected to the changing meshlink are updated
				automatically

			Implementing these properties requires a meshlink to be a node
			on three separate double-linked lists -
			-	A list of all meshlinks in a mesh
			-	A list of all meshlinks with a given meshlink as their
				source
			-	A list of all meshlinks with a given meshlink as their
				destination


			Using the classes:

			Suppose a homogeneous mesh is being set up - a mesh in which all
			nodes are of the same class. The declarations would be as
			follows:

			class UserMeshLink : public BasedLink<UserMeshLink>
			{
				//	New user-defined members
				.
				.
				.
			};


			class UserMesh : public MeshOf<UserMeshLink>
			{
				//	New user-defined members
				.
				.
				.
			};


			All UserMesh and UserMeshLink information functions will return
			pointers to UserMeshLinks, so typecasts in application code
			will seldom be necessary.
-----------------------------------------------------------------------------*/


#ifndef MESH_INC
#define MESH_INC


//--Includes-------------------------------------------------------------------
//-----------------------------------------------------------------------------

#include <iostream>
#include <assert.h>

#include <yarp/YARPString.h>

//--Forward Declarations-------------------------------------------------------
//-----------------------------------------------------------------------------

//	Forward declarations
class MeshLink;
class Mesh;


//--Class Declarations---------------------------------------------------------
//-----------------------------------------------------------------------------

/*-----------------------------------------------------------------------------
Class		MeshLink
                                                                           
Description	A MeshLink object is a single node in a mesh.
-----------------------------------------------------------------------------*/
class MeshLink
{
protected:
  int pruneId;
	YARPString
		label;			//	Name of node
	MeshLink
		*src,			//	Source of link
		*dest;			//	Destination of link		
	MeshLink
		*asSrcRoot,		//	Root of list of links with this link as
            			//	their source
		*asDestRoot;	//	Root of list of links with this link as
						//	their destination
	MeshLink
		*srcPrev,		//	Previous link with the same source
		*srcNext,		//	Next link with the same source
		*destPrev,		//	Previous link with the same destination
		*destNext;		//	Next link with the same destination
	MeshLink
		*meshPrev,		//	Previous link in the mesh
		*meshNext;		//	Next link in the mesh
	Mesh
		*owner;			//	Mesh in which this link is contained

public:
  int GetId()
  { return pruneId; }

  void SetId(int id)
  { pruneId = id; }

	//	Constructor
	MeshLink();

	//	Destructor
	virtual ~MeshLink();

	//true if anyone sources from this, or acts as a dest for it
	int HasOutput()
	{ return ( dest != NULL ) || ( asSrcRoot != NULL ); }

	int HasInput()
	{ return ( src != NULL ) || ( asDestRoot != NULL ); }

	//	Sets the source of this link, disconnecting from any previously
	//	connected source
	void SetSource ( MeshLink *link );

	//	Sets the destination of this link, disconnecting from any
	//	previously connected destination
	void SetDest ( MeshLink *link );

	//	Sets the mesh in which this link is contained, disconnecting
	//	from any previously connected mesh
	void SetOwner ( Mesh *mesh );

	//	Sets or changes the name of the link
	void SetLabel ( const YARPString& lbl );
	void SetLabel ( const char *lbl );

	//	Disconnects any links using this link as their source
	void RemoveAsSource();

	//	Disconnects any links using this link as their destination
	void RemoveAsDest();

	//	Gets the name of the link
	const YARPString& GetLabel()	{	return label;		}

	Mesh *GetOwner()	{ return owner; }

#ifndef ROBOT
	//	Reads the user-data component of the link in verbose format
	//	(For example, from cin)
	virtual int Read ( istream& is, int style )	{ ACE_UNUSED_ARG(is); ACE_UNUSED_ARG(style); return 1;	}

	//	Writes the user-data component of the link in verbose format
	//	(For example, to cout)
	virtual int Write ( ostream& os, int style )	{ ACE_UNUSED_ARG(os); ACE_UNUSED_ARG(style); return 1;	}
#endif

	//	Traversal functions

	//	Gets current source of link
	MeshLink *BaseGetSource()	{	return src;			}

	//	Gets current destination of link
	MeshLink *BaseGetDest()		{	return dest;		}

	//	Gets previous link in the mesh
	MeshLink *BaseGetMeshPrev()	{	return meshPrev;		}

	//	Gets next link in the mesh
	MeshLink *BaseGetMeshNext()	{	return meshNext;		}

	//	Gets root of list of all links that use this link as their
	//	source
	MeshLink *BaseGetAsSrcRoot()	{	return asSrcRoot;		}

	//	Gets previous link that uses this link as its source
	MeshLink *BaseGetAsSrcPrev()	{	return srcPrev;		}

	//	Gets next link that uses this link as its source
	MeshLink *BaseGetAsSrcNext()	{	return srcNext;		}

	//	Gets root of list of all links that use this link as their
	//	destination
	MeshLink *BaseGetAsDestRoot()	{	return asDestRoot;	}

	//	Gets previous link that uses this link as its destination
	MeshLink *BaseGetAsDestPrev()	{	return destPrev;		}

	//	Gets next link that uses this link as its destination
	MeshLink *BaseGetAsDestNext()	{	return destNext;		}

	void Mark(int id);
};



extern const YARPString
	NullName;		//	Sentinel name to signal the end of a list of
					//	links		 

/*-----------------------------------------------------------------------------
Class		Mesh
                                                                           
Description	Manager for MeshLinks.
			Suitable for I/O and deletion.
			Targetted to solve problem of meshes not necessarily being fully
			connected.
			Each MeshLink can be given a tag for identification.
-----------------------------------------------------------------------------*/
class Mesh
{
	friend class MeshLink;

protected:
	MeshLink
		*root;			//	First link in Mesh

	int autoClear;

	//	Creates a link of the default type.
    //	This is overridden for Meshes of user-defined Links
	virtual MeshLink *CreateLink()	{	return new MeshLink;	}

	//	Makes sure that a link with the given label exists. If it doesn't,
    //	create it
	MeshLink *LabelledLink ( const YARPString& label );

    //	Adds a new link from a given source to a given destination
	MeshLink *AddLink ( const YARPString& label, const YARPString& srcLabel, const YARPString& destLabel );

public:

    //	Styles to control input and output of the mesh
	enum StyleConstants
	{
		CONCISE_FORMAT		= 0x0000,
		PROSE_FORMAT		= 0x0001,
		FORMAT				= 0x0001,
		MINIMAL_CONTENT		= 0x0000,
		EXHAUSTIVE_CONTENT	= 0x0002,
		CONTENT				= 0x0002,
		ARCHIVE_INPUT_STYLE	= CONCISE_FORMAT + EXHAUSTIVE_CONTENT,
		ARCHIVE_OUTPUT_STYLE= ARCHIVE_INPUT_STYLE,
		USER_INPUT_STYLE	= CONCISE_FORMAT + MINIMAL_CONTENT,
		USER_OUTPUT_STYLE	= PROSE_FORMAT + EXHAUSTIVE_CONTENT
	};

    //	Constructor
	Mesh();

    //	Destructor
	virtual ~Mesh();

    //	Finds the link in the mesh with the given label
	MeshLink *FindByLabel ( const YARPString& label );

	//	Deletes all the links in the mesh
	void Clear();

        void Prune(MeshLink *link);

	void SetAutoClear ( int val) { autoClear = val; }

  int Size();

#ifndef ROBOT
	//	Inputs mesh from a stream using the given style
	int Load ( istream& is, int style = ARCHIVE_INPUT_STYLE );

	//	Outputs mesh to a stream using the given style
	int Store ( ostream& os, int style = ARCHIVE_OUTPUT_STYLE );

	//	Inputs mesh from a stream (Concise format, Minimal content)
	friend istream& operator >> ( istream& is, Mesh& mesh );

	//	Outputs mesh to a stream (Prose format, Exhaustive content)
	friend ostream& operator << ( ostream& os, Mesh& mesh );
#endif

};


//--Templated Class Declarations-----------------------------------------------
//-----------------------------------------------------------------------------

/*-----------------------------------------------------------------------------
Class		BasedLink
                                                                           
Description	BasedLink is a templated shell for MeshLink used when all the
			links in a mesh have a common base class other than MeshLink
			itself.
			In homogenous meshes, where all the links are of type UserLink,
			UserLink could be declared as:
				class UserLink : public BasedLink<UserLink>

Template	BaseType		The common ancestor of all the MeshLinks in the
							mesh.
-----------------------------------------------------------------------------*/

template <class BaseType>
class BasedLink : public MeshLink
{
public:
	//	Tracing functions

	//	Gets current source of link
	BaseType *GetSource()		{	return ( BaseType * ) src;		}
	//	Gets current destination of link
	BaseType *GetDest()		{	return ( BaseType * ) dest;		}

	//	Gets previous link in the mesh
	BaseType *GetMeshPrev()		{	return ( BaseType * ) meshPrev; 	}
	//	Gets next link in the mesh
	BaseType *GetMeshNext()		{	return ( BaseType * ) meshNext;	}

	//	Gets root of list of all links that use this link as their
      //	source
	BaseType *GetAsSrcRoot()	{	return ( BaseType * ) asSrcRoot;	}
	//	Gets previous link that uses this link as its source
	BaseType *GetAsSrcPrev()	{	return ( BaseType * ) srcPrev;	}
	//	Gets next link that uses this link as its source
	BaseType *GetAsSrcNext()	{	return ( BaseType * ) srcNext;	}

	//	Gets root of list of all links that use this link as their
      //	destination
	BaseType *GetAsDestRoot()	{	return ( BaseType * ) asDestRoot;	}
	//	Gets previous link that uses this link as its destination
	BaseType *GetAsDestPrev()	{	return ( BaseType * ) destPrev;	}
	//	Gets next link that uses this link as its destination
	BaseType *GetAsDestNext()	{	return ( BaseType * ) destNext;	}
};


/*-----------------------------------------------------------------------------
Class		MeshOf
                                                                           
Description	BasedLink is a templated shell for MeshLink used when all the
			links in a mesh have a common base class other than MeshLink
			itself.
			In homogenous meshes, where all the links are of type UserLink,
			UserLink could be declared as:
				class UserLink : public BasedLink<UserLink>

Template	LinkType		The type of link to be generated by and stored
							in the mesh.
                            Must be descended from MeshLink.
-----------------------------------------------------------------------------*/
//	LinkType must be descended from MeshLink
template <class LinkType>
class MeshOf : public Mesh
{
private:
	virtual MeshLink *CreateLink()
	{	return new LinkType;		}

public:
	LinkType *NewLink ( const YARPString& label, const YARPString& srcLabel, const YARPString& destLabel )
	{	return ( LinkType * ) AddLink ( label, srcLabel, destLabel );	}

	LinkType *GetRoot()		{	return ( LinkType * ) root;				}

	LinkType *GetByLabel ( const YARPString& label )
	{	return ( LinkType * ) FindByLabel ( label );	}

	LinkType *NewLink()
	{	
	  LinkType *link = (LinkType *)CreateLink();
	  assert(link!=NULL);
	  link->SetOwner(this);
	  return link;
	}

	LinkType *NewLink(const char *name)
	{
	  LinkType *link = NewLink();
	  link->SetLabel(name);
	  return link;
	}
};



//--Friend Function Prototypes-------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef ROBOT

//	Inputs mesh from a stream (Concise format, Minimal content)
istream& operator >> ( istream& is, Mesh& mesh );

//	Outputs mesh to a stream (Prose format, Exhaustive content)
ostream& operator << ( ostream& os, Mesh& mesh );

#endif

#endif
