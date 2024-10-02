#pragma once
#define CM_H

#include "api/epworker.h"


#if PJC_CONSUMERBUILD == false and CM_FW_FORCENET == false
#   define GMAPI_DLL true
#else
#   define GMAPI_SHADOWNET true
#endif
#include "gmapi.h"

/*
      ---      CM             ---
    everything comes together here.
*/

#include "_cmarch.h"

#include "cm-metadata.h"

#include "frameworks/fw-api.h"

#include "cm-ui.h"

#include "cm-engine.h"


//  TargetApplication : set none if you do not want to attach.
//    + epworker commands: you know them commands dog.
#ifdef pjc_build_winapp
#   define _trycmapp ep_winapp
#else
#   define _trycmapp ep_conapp
#endif

#define cm_app(TargetApplication, epworker_commands)                   \
   auto res = CMEWorker::StoreHostApp(crypt(WSTR(TargetApplication))); \
   _trycmapp("Gaiacon", cme.AppEntry, epworker_commands);


CM_IMPL cma::systems::Rat rat;
CM_IMPL cma::systems::Sig sig;
