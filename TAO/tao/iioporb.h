// This may look like C, but it's really -*- C++ -*-

// ============================================================================
//
// = LIBRARY
//    TAO
// 
// = FILENAME
//    iiopobj.h
//
// = AUTHOR
//     Copyright 1994-1995 by Sun Microsystems Inc.
// 
// ============================================================================

#if !defined (TAO_IIOPORB_H)
#  define TAO_IIOPORB_H

#  include "ace/OS.h"
#  include "ace/Singleton.h"
#  include "orbobj.h"

typedef class IIOP_ORB *IIOP_ORB_ptr;

extern "C" const IID IID_IIOP_ORB;

// ORB pseudo-objref

class ACE_Svc_Export IIOP_ORB : public CORBA_ORB
// = TITLE
//    Bridge implementation of IIOP_ORB
//
// = DESCRIPTION
//    This is one of the two main hooks into IIOP in this Win32
//    C/C++/COM based implementation; the other being IIOP_Object.
{
public:
  IIOP_ORB (void);
  // constructor

  ~IIOP_ORB (void);
  // destructor

  CORBA_Object_ptr string_to_object (CORBA_String str,
				     CORBA_Environment &env);
  // Get an object reference from the IOR string

  CORBA_String object_to_string (CORBA_Object_ptr obj,
				 CORBA_Environment &env);
  // Convert an object reference to an IOR stringified form

  HRESULT __stdcall QueryInterface (REFIID riid, 
				    void **ppv);
  // COM stuff - get the underlying IUnknown object based on the riid

  // = ACCESSORS
  void use_omg_ior_format (CORBA_Boolean ior);
  // Set the IOR flag.
  CORBA_Boolean use_omg_ior_format (void);
  // Get the IOR flag.
  
private:
  CORBA_Boolean use_omg_ior_format_;
  // decides whether to use the URL notation or to use IOR notation

  // These are not provided.
  IIOP_ORB (const IIOP_ORB &);
  IIOP_ORB &operator = (const IIOP_ORB &);
};

typedef IIOP_ORB *IIOP_ORB_ptr;

// Create a type for the singleton. 
typedef ACE_Singleton<IIOP_ORB, ACE_SYNCH_MUTEX> 
	TAO_ORB; 

#  if defined(__ACE_INLINE__)
#    include "iioporb.i"
#  endif

#endif	/* TAO_IIOPORB_H */
