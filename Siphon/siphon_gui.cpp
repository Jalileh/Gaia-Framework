
#include "Siphon.h"


auto BaseFrame (){

   gmBuilderHere("baseFrame")

}


noret run_SiphonGui(systems::Gui gui) {

 
  
   gui.designContainer(" visual.empty name.siphon w.800 y.900");

   gui.designCanvas("name.SiphonMain   w.800 h.900  flags.nonav owner.siphon ", BaseFrame);


  while(true){
  
    gui.run("all");
  }
}
