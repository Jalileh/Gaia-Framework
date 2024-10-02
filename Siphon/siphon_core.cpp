
#include "Siphon.h"


class SiphonCore {
 public:
   auto DumpVirtualizeSpace () {
   }
};

SiphonCore cs;


auto siphon_main () {

   fw::frameworkLoader().load_Graphical_Components();
   
   
 
   run_SiphonGui(cm.gui);
}

#include "api/epworker.h"
ep_winapp("siphon", siphon_main, "epfile con");
