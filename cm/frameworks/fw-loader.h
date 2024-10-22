#pragma once

#define CM_FRAMEWORK_LOADER_H


#include "../_cmarch.h"

#include "fw-impl.h"

#include "fw-api.h"

#include "umlib.h"
namespace fw {


   class frameworkLoader {
    public:
      frameworkLoader () {
#if CM_FW_LOCALNET == true
         astr local_host("http://localhost:3004/");
         auto net = lib::netcorp();

         net->config.domain(local_host);

#endif
      };

      template < typename framework >
      framework loadObject (astr fnInitExport, astr name) {


         loader::CoreObject load(loader::autoconf::COMPONENT, (!PJC_CONSUMERBUILD));

         auto initFn = load.loadComponent(name.cstr(), fnInitExport.cstr());

         if (initFn == null) {
            cle_private("FRAMEWORK LOAD-FAILURE");
         }

         using MakeFuncRet = framework (*)();
         return rcast< MakeFuncRet >(initFn)();
      }

      // pre-defined loads
      noret load_Native_Components ();
      noret load_Graphical_Components ();
   };


   // this is loaded by our r/w thread
   // Do NOT use in Main Thread
   CM_IMPL noret frameworkLoader::load_Native_Components () {   // aple


      frameworkLoader fwl;

      //   this explains why your [CENSORED] ass has to write 400000 wrappers...
      //   [ make the fucking dlls reasonable, have it configure itself.]


      astr export_names[] = {
         astrc("Centurion"),
         astrc("examina")

      };


      try {

#if CM_CONSUMERBUILD == true or CM_FW_FORCENET == true

         cm.libs.LoadedMemoryLibrary = fwl.loadObject< decltype(cm.libs.LoadedMemoryLibrary) >(
            export_names[0],
            astrc("comlib")
         );

         cm.libs.siglib = fwl.loadObject< decltype(cm.libs.siglib) >(
            export_names[1],
            astrc("sigmaster")
         );


#else

#   if defined(CM_KERNEL)
         cm.libs.LoadedMemoryLibrary = loader::local::exportDllFn< decltype(cm.libs.LoadedMemoryLibrary) >(
            astrc("c:/users/fayar/desktop/clib/comlib/build/Debug/comlib.dll"),
            export_names[0],
            true

         );
#   elif defined(CM_USERMODE)

         cm.libs.LoadedMemoryLibrary = loader::local::exportDllFn< decltype(cm.libs.LoadedMemoryLibrary) >(
            acstr("c:/users/fayar/desktop/clib/umlib/build/Debug/umlib.dll"),
            umlib_exportname,
            true

         );

#   endif

         cm.libs.siglib = loader::local::exportDllFn< decltype(cm.libs.siglib) >(
            acstr("c:/users/fayar/desktop/clib/sigmaster/build/Debug/sigmaster.dll"),
            export_names[1],
            true

         );


#endif
      } catch (astr &e) {
         e.sprint("sleep.1s ERROR LOADING");
         return;
      }
      // why dosent the [CENSORED]ing library configure itself on return? [CENSORED]...
      // or atleast make it so we call this shit from header, jesus


#if defined(CM_KERNEL)
      fsl.proxima = cm.libs.LoadedMemoryLibrary->Eprox->getEproxv2();

      cm.sig.sigmaster = cm.libs.siglib->getKernel_comlib(cm.libs.LoadedMemoryLibrary->Eprox);

      if (cm.libs.LoadedMemoryLibrary->Eprox->IsDriverOnline() == false) {
#   if PJC_CONSUMERBUILD == false
         astr notify_first_driver_load = acstr(" [ CM ] DRIVER IS NOT LOADED .. PROCEEDING IN ATTEMPT OF LOADING DRIVER");
         notify_first_driver_load.print("CM-KERNEL LOG");


#   endif
         if (loader::HasAdminPrivlidge() == false) {
            loader::ReloadHubLoaderAsAdmin();
            exit(0);
         }
         else {

            NetCorp::loadDriver();
         }


         if (cm.libs.LoadedMemoryLibrary->Eprox->IsDriverOnline() == false)
            cmverbose("tried loading driver and checking, didnt go online", "POST LOAD DRIVER OFFLINE STATUS");
      }


#elif defined(CM_USERMODE)
      fsl.proxima = cm.libs.LoadedMemoryLibrary;
      cm.sig.sigmaster = cm.libs.siglib->getUsermode_umlib(fsl.proxima);
#endif

      return;
   }

   // Graphics should always be ran on main thread
   CM_IMPL noret frameworkLoader::load_Graphical_Components () {
      try {
         cm.gui.load();
      } catch (astr &e) {
         e.sprint("sleep.5s");
      }

      return;
   }
}   // namespace fw
