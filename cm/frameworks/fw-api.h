
#pragma once
#define CM_FRAMEWORK_API_H
#if !defined(CM_KERNEL) and !defined(CM_USERMODE)
   pjc_assert(0, "PLEASE DEFINE CM_KERNEL OR CM_USERMODE FOR CRUCIAL PLATFORM");
#endif


#include "../_cmarch.h"
#include "fw-impl.h"

class cmLibs {

#if defined(CM_KERNEL)
   using ProxyLibraries = PROXY_API::PCENTAURI_API;
#elif defined(CM_USERMODE)
   using ProxyLibraries = pumlib;
#endif

   using apiSigLib = papiSigMaster;

 public:
   ProxyLibraries LoadedMemoryLibrary;
   apiSigLib siglib;
};

class cma::CM {
 public:
   systems::Gui gui;
   systems::Rat rat;
   systems::Sig sig;


 public:
   cmLibs libs;
};


tag_global static cma::CM cm;

#ifndef CM_FRAMEWORK_LOADER_H
#   include "fw-loader.h"
#endif   // !j
