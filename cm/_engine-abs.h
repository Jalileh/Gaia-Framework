#pragma once

#include "include/fnTemps.h"
#include "api/templates.h"

template < typename CBType >
class core_CBlist : private class_templ::GetSet< bool > {
 public:
   core_CBlist() {
      Set(true);   // is_infinite
   }
   template < typename... Args >
   auto run (bool is_infinite = true, Args &...uh) {
      if (!is_infinite) {
         if (!Get())
            return false;
         else
            Set(is_infinite);
      }

      bool ran = false;
      for (auto lambdas : fnlist) {
         lambdas.fn(uh...);
         ran = true;
      }
      return ran;
   }


   // supplied
   auto Has () {
      return this->fnlist.Length();
   }

   const auto &_abstract () const {
      return fnlist;
   }

 protected:
   nodem< fnObject< CBType > > fnlist;
   bool ThisFnAlreadyExists(CBType fn){
      for (auto fn_in_registry : fnlist) {
          if(*pqword(&fn) == *pqword(&fn_in_registry)){
            return true;
          }
      }
      
      return false;
   }
    
 
    
};
 
template < typename CBType >
class CustomCBlist : public core_CBlist< CBType > {
 public:
   template < typename fn >
   inline noret Link (  fn config_fn_link) {
      if(this->ThisFnAlreadyExists(FnRoute_links)) {
          return;
      }
      
      this->fnlist.addNew(CBType(config_fn_link));
   };
   
  template < typename fn >
    auto operator=(fn Fn){
        this->Link(Fn);
    }
    
};
template < typename CBType >
class FnRouteList : public core_CBlist< CBType > {
 public:
   // links are made once do not worry
   template < typename fn >
   inline noret Link (  fn FnRoute_links) {
      if (cma::fn::BaseRoute_PostRan)   // this makes sure its not registered twice
         return;

      if(this->ThisFnAlreadyExists(FnRoute_links)) {
          return;
      }
      
      this->fnlist.addNew(CBType(FnRoute_links));
   };
   
    template < typename fn >
    auto operator=(fn Fn){
        this->Link(Fn);
    }
    
};


using EngineRoutelist = FnRouteList< void (*)(void) >;


struct FNROUTES {

   // fn route calls :
   // - occurs on Gui : Main Thread
   // - routes are called INFINTELY
   EngineRoutelist GuiT1_inf;
   // overlay window
   EngineRoutelist OVERLAYT1_inf;

   // fn route calls :
   // - occurs on Gui : Main Thread
   // - routes are called ONLY ONCE.
   EngineRoutelist GuiT1_init;

   // fn route calls :
   // - occurs on NAME_REDACTED thread 2
   // - routes are called ONLY ONCE.
   EngineRoutelist NAME_REDACTEDT2_init;

   // fn route calls :
   // - occurs on NAME_REDACTED thread 2
   // - routes are called INFINTELY
   EngineRoutelist NAME_REDACTEDT2_inf;
};
// extended fn callbacks please use these

 