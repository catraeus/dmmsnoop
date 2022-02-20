// $Id: CaesCallBack.hpp 497 2019-08-17 20:46:44Z duncang $

//=================================================================================================
// Original File Name : CaesCallBack.hpp
// Original Author    : duncang
// Creation Date      : Oct 14, 2012
// Copyright          : Copyright © 2012 - 2022 by Catraeus and Duncan Gray
//
// Description        :
/*
  The whole point of callbacks is to invert the normal hierarchy of caller/called.  NOW, the called
    is above and the caller is in the "included" module of the called function.  Thus include hell
    doesn't happen.
  The magic of the present method is that the base CB class, which is put into the lower file,
    doesn't have the templating.  Therefore, it doesn't force the lower-level calling Member
    Function that will invoke the CB to need the upper level caller's class definition.
    No DANGEROUS Forward Referencing.
  Finally, since this is c++, the deadly function-pointer-to-a-member-function conundrum isn't
    raising its poisonous little head.
  The term "upper file" means the one that has the function to be called back and includes the
    "lower file's" header.
  The "lower file," duh, is the one that got included in the upper file and will execute the
    callback.
  The void pointer in the functor pattern allows for the knowledge-share so that is a signature
    that both the lower and upper must by MAGICK be same.
  1.  Put the pointer to CbV into the lower file as a public member variable.  It won't be
        "new'd" only set from above.
  2.  NULL out the pointer to the CbV in the lower file in its constructor.
  3.  Put a pointer to the CbT in the includer file's own header file "above."
        With the templating of its own self class.
        new it.
  4.  Make a function in the upper file that has a bool return and a void * input parameter.
  5.  "Set" the callback in the upper file with the functor/functee pattern.
  6.  Assign the public member variable of the lower file to the functor/functee from the upper file.
  7.  Execute() 'till the cows come home.
*/
//
//=================================================================================================


#ifdef _FIND_BADNESS_
#define PRINT_INOUT(msg) fprintf(stdout, msg); fflush(stdout);
#else
#define PRINT_INOUT(msg) (void)(msg)
#endif

#ifndef __CAEV_CALLBACK_HPP_
#define __CAEV_CALLBACK_HPP_
#include <stdio.h>

                    class CbV {
  public:
    virtual      ~CbV() {} // to keep compilers happy, is a trivial destructor that doesn't reduce pure virtuality.
    virtual bool  Execute(void *i_object) const = 0;
  };
template <class tI> class CbT : public CbV {
  public:
    typedef bool (tI::*tF)(void *i_object); // funny ha ha lessor/lessee <=> functor/functee
  public:
                  CbT()  {
      PRINT_INOUT("CbT::CbT()\n");
      pF = 0; // Important : zeroing the pointer so we later can check for errors
      pI = 0;
      }
                 ~CbT() {
    PRINT_INOUT("CbT::~CbT()\n");
    }
    virtual bool  Execute(void *i_object) const {
      if (pF != 0)   return (pI->*pF)(i_object);
      else           fprintf(stdout, "  **  ERROR ** Uninitialized functionality in CbT\n");
      return false;
      }
            void  SetCallback (tI  *i_pI, tF   i_pF) {
      pI = i_pI;
      pF = i_pF;
      }
            void  ClearCallback(void) {
      pF = 0; // Important : zeroing the pointer so we later can check for errors
      pI = 0;
      }
  private:
    tI  *pI; // The Instance that got templated down into us.
    tF   pF; // The function that the instance contains that got pointered down into us.
  };

#endif // __CAEV_CALLBACK_HPP_




