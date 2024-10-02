#pragma once
#define CM_ENGINE_H

#include "_cmarch.h"

#include "include\fnTemps.h"

#include "cm-metadata.h"

#include "_engine-abs.h"

#include "cm-ui.h"

#include "frameworks/fw-api.h"


class cma::CMEngine {
 public:
   singleton_member fnObject AppEntry = appEntry;


   const singleton_member fnObject baseConfig = cma::config;


   auto &GetCMUI () {
      return cmui;
   }
   auto &GetCMUIConfig () {
      return cmui.InitRoutes();
   }
   auto &GetCMUISettings () {
      return cmui.settings;
   }


   FNROUTES fnroutes;


   enum class estatus { pre_emptive,
                        await_nextloads,
                        runtime,
                        exit,
                        cleanup   // SET THIS IF YOU WANT TO TRIGGER CME TO EXIT.
   };


   class Status {
    public:
      bool check (estatus stateof) {
         return stateof == m_status;
      }

      noret set (estatus stateof) {
         m_status = stateof;
      }
      auto get () {
         return m_status;
      }


    private:
      estatus m_status;
   };
   Status status;

   // requests CME to enter stage clean up-
   // and get the swear-word outta here
   auto AskTermination () {
      status.set(estatus::cleanup);
   }


   // loads guimaster, siglib and comlib for frameworks
};


using CME = cma::CMEngine;
tag_global static cma::CMEngine cme;

// responsible for extending cm macro and very early data
namespace {
   struct Worker {
    public:
      static inline cryptInput Application_Target;
      bool target_supplied;

      Worker() : target_supplied(false) {}
   };
   tag_global static Worker worker;
}   // namespace

class CMEWorker {
 public:
   singleton_member auto StoreHostApp = [] (crypt NAME) {
      // set none if you do not want to attach.


      auto cryptos = NAME.decrypt();

      worker.Application_Target = cryptos;
      worker.target_supplied = true;
      return 0;
   };

   // call the cm_app target application if it exists.
   // [ WARNING ] Make sure its called in NAME_REDACTED thread
   singleton_member auto AutoHostAppCall = [] () {
      if (worker.target_supplied) {
         // confirm is .exe otherwise we return;
         //
         astr Name(worker.Application_Target.decrypt());
         if (Name.pos(-4) != '.') {
            worker.target_supplied = false;
            return true;
         }

         astr try_proc = worker.Application_Target.decrypt();
         // call targetApplication
         cm.rat.SetProcessModule(try_proc);
         // recrypt AYYLmao
         worker.Application_Target.RECRYPT();
         return true;

      }   // - returns true if we never used cm_app
      return true;
   };
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.runtime-engine
////   - IMPLEMENTATION AND PROCEDURES
////
////
////
////

namespace {
   using estatus = CME::estatus;
}


CM_IMPL auto
isol_CMRoute_Procedure (auto &CMRoute) {
   try {


      cme.fnroutes.NAME_REDACTEDT2_init.run(false);

      // call main route entry
      CMRoute();

      cme.fnroutes.NAME_REDACTEDT2_inf.run();


   } catch (astr &e) {

      cmverbose(e, "[ CME ] CM::fn general Exceptions Caught");
      return;

   } catch (exceptions::Entity &e) {
      cmverbose(e.Get(), "[ CME : ENTITYEXCEPTION ]");
      return;
   } catch (RatExceptions &e) {
      cmverbose(e.GetAny(), "[ CME : ] FW-RT EXCEPTION ]");

      return;
   }

   cma::fn::BaseRoute_PostRan = true;

   return;
}

namespace {

   CM_IMPL auto Runtime_t2 () {
      try {
         // this one is handled by this function
         CMEWorker::AutoHostAppCall();   // fed from macro


      } catch (RatExceptions &e) {
         e.Get();
         return;
      } catch (RatExceptions e) {
         e.Get();
         return;
      }


      isol_CMRoute_Procedure(CM_1337_ROUTE);
   }

}   // namespace

CM_IMPL bool NAME_REDACTEDThread_Main () {

   switch (cme.status.get()) {

      case estatus::await_nextloads: {
         fw::frameworkLoader().load_Native_Components();
         cme.status.set(estatus::runtime);
      }
      case estatus::runtime: {
         // CME main runtime
         Runtime_t2();


         break;
      }
      default: {
         // clean up we exit this thread here.


         cme.status.set(estatus::cleanup);
      }
   }

   return true;
}

CM_IMPL void cma::appEntry() {


   cme.status.set(estatus::pre_emptive);
   cme.baseConfig();


   fw::frameworkLoader().load_Graphical_Components();

   // init CMUI::Binds. And call CMUI init
   {
      cma::ui::SetFNROUTESPTR(cme.fnroutes);
      ui::CMUI &cmui = cme.GetCMUI();


      auto x = cmui.InitRoutes();

      x.Containers();
      x.Canvases();
   }
   sleeper().delay("10ms");

   while (cm.gui.getSessionInfo()->Session) {

      try {
         // NAME_REDACTED thread is started .
         if (cme.status.check(estatus::pre_emptive)) {

            cme.status.set(estatus::await_nextloads);

            lib::threadmaster().setup.newEternal(NAME_REDACTEDThread_Main, "x-2934f");
            lib::threadmaster().GlobalSleep("1ms");
         }
         else if (cme.status.check(estatus::cleanup)) {

            break;
         }

         cm.gui.run("all");

         if (cm.gui.pressed("X")) {
            cme.status.set(estatus::cleanup);
         }

         sleep_for(10ms);
      } catch (astr &e) {
         e.print("exce caught t1-gaia");
      }
   }

   cme.status.set(estatus::exit);

   while (cme.status.check(estatus::exit)) {
      sleeper().delay("1ms");
   }
}
