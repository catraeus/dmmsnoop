
// $Id: CaesCallBack.hpp 141 2017-06-02 01:39:10Z duncang $
//  Callback.hpp

#ifdef _FIND_BADNESS_
#define PRINT_INOUT(msg) fprintf(stdout, msg); fflush(stdout);
#else
#define PRINT_INOUT(msg)
#endif

#ifndef __CAEV_CALLBACK_HPP_
#define __CAEV_CALLBACK_HPP_
#include <stdio.h>
/* \cond HIDDEN_SYMBOLS */
/*=====================================================================================================================
  The magic of this is that the base CB class doesn't have the templating.  Therefore, it doesn't force the
    lower-level called Member Function that will invoke the CB to need the upper level caller's class definition.

  I hereby declare two new C++ (maybe even object oriented) keywords:
    - Helper Callback
        A callback that is pushed, at call time, by a parent class into a contained class' Member Function as a
            Parameter so that the contained class doesn't have to have a Member Callback Field that is pre-set.
              Of course a contained class could have a member callback that gets set in the working called member
              function of the contained class, but I have always believed it to be sloppy functionality to have
              Operational Member Functions change its Setup Member Fields at the whim of operations.  The volatility
              of Member Fields is what i'm talking about here.  An int field that does fast work needs to be updated
              at every Operation Member Function call.  A Callback Member Field probably shouldn't be that volatile.
    - Contributor Callback
        A callback that is set, early in the life of a lower level class and only changes infrequently.  It is a Member
        Field of the lower level class.  It has things like Set, Get and Clear functions to hide it.
=======================================================================================================================*/
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
      //fprintf(stdout, "CbT::CbT()\n"); fflush(stdout);
      pF = 0; // Important : zeroing the pointer so we later can check for errors
      pI = 0;
      }
                 ~CbT() {
    //fprintf(stdout, "CbT::~CbT()\n"); fflush(stdout);
    }
    virtual bool  Execute(void *i_object) const {
      if (pF != 0)   return (pI->*pF)(i_object);
      else           PRINT_INOUT("  **  ERROR ** Uninitialized functionality in CbT\n")
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
/* \endcond HIDDEN_SYMBOLS */
#endif // __CAEV_CALLBACK_HPP_




