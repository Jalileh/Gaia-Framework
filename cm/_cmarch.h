#pragma once


#define CM_CONSUMERBUILD PJC_CONSUMERBUILD

#if CM_NOSHADOWLIBS == true
#   define PJC_NOSHADOWLIBS CM_NOSHADOWLIBS
#endif

#ifndef CM_IMPL
#   define CM_IMPL inline
#   define CM_IMPL_S static
#endif

#define CM_IMPL_SI static inline

#if ENABLE_CM_VERBOSE == true
#   define cmverbose cd
#else
#   define cmverbose(in, ...)

#endif


#ifndef CM_IMPL_S
#   define CM_IMPL_S static
#endif

// cma cm-application
// - framework
namespace cma {
   // external data-manip - comlib/umlib
   // pattern-sig framework - sigmaster-lib/dll
   // guimaster framework - guimaster-lib/dll
   namespace systems {
      class Gui;
      class Sig;
      class Rat;

   }   // namespace systems
   namespace ui {}

   namespace fn {

      // Gaia Route-1337 |  make history | do it now
      #ifndef  CM_NO_ROUTE 
       noret Route ();
       #define CM_1337_ROUTE cma::fn::Route 
       #else 
       CM_IMPL noret empty_route (){} 
       #define CM_1337_ROUTE cma::fn::empty_route
      #endif 
      // not necessary to have code in it,

      singleton_member bool BaseRoute_PostRan = false;


   }   // namespace fn

   class CMEngine;
   class CM;

   // just define it if you do not want to override the engine.
   noret config ();
   CM_IMPL void appEntry ();

   namespace exceptions {}
}   // namespace cma

namespace fw {
   class frameworkLoader;
}

using namespace cma;

namespace gaia = cma;

using cmMarkID = crypt;


#include "include\easymath.h"
