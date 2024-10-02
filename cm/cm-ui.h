#pragma once
#define CM_UI_H

#include "_cmarch.h"

#include "_engine-abs.h"


#include "gmapi.h"

#include "include\fnTemps.h"


namespace cma::ui {
   class CMUI;

   tag_global static FNROUTES _ptr engine_routes = nullptr;

   auto inline SetFNROUTESPTR(FNROUTES &that) {
      engine_routes = &that;
   }

   struct _config {
      bool overlay = true;
      bool menu = true;
   };


   struct config_canvas {
      bool hidden_popup = true;
   };

   noret_s BaseContainers_init ();
   noret_s BaseCanvases_init ();

   noret_s overlay_routine ();

   noret_s CanvasMain ();
   noret_s Modifier_canvas_user ();
   noret_s Modifier_canvas_apis ();
   noret_s Canvas_Features_user ();
   noret_s Canvas_popup ();


}   // namespace cma::ui


class cma::ui::CMUI {

 public:
   class Settings {
    protected:
      friend noret cma::ui::BaseContainers_init();

      struct {
         bool hide_all;
      } actions;

      auto PostActionInitiate () {
         if (this->actions.hide_all) {
            gm.global_hide();
         }
      }

    public:
      Settings() : actions(unsets) {

                   };

      auto globalHide () {
         actions.hide_all = true;
      }
      auto globalShow () {
         actions.hide_all = false;
      }
   };
   Settings settings;

   enum class Current_Tab {
      VISUALS,
      AIM,
      MISC
   };
   singleton_member Current_Tab m_PageTab;

   EngineRoutelist TabVisuals;
   EngineRoutelist TabAim;
   EngineRoutelist TabMisc;

   struct Canvas_Routes {
      singleton_member fnObject
         canvasMain = ui::CanvasMain;

      singleton_member fnObject
         canvas_modifier_user = ui::Modifier_canvas_user;

      singleton_member fnObject
         overlay_route = ui::overlay_routine;


      singleton_member fnObject
         canvas_modifier_api = ui::Modifier_canvas_apis;


      singleton_member fnObject popup = ui::Canvas_popup;
   };

 private:
   Canvas_Routes Canvas_Actors_Routes;

 public:
   // these are the canvases running on all gaia-cm projects
   auto &CanvasRoutes () {
      return Canvas_Actors_Routes;
   }


   struct BaseInitRoutes {
      singleton_member auto Containers = fnObject2(
         ui::BaseContainers_init, cma::ui::_config{}
      );

      singleton_member auto Canvases = fnObject2(
         ui::BaseCanvases_init, cma::ui::config_canvas{}
      );
   };

 private:
   BaseInitRoutes initRoutes;

 public:
   auto &InitRoutes () {
      return this->initRoutes;
   }
};
tag_global static cma::ui::CMUI cmui;


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.cm-gui  callback instances below
////
////
////
////


noret_s cma::ui::Modifier_canvas_user(){
   gmBuilderHere("FEATURES-BASE_STYLE")
}


noret_s cma::ui::Modifier_canvas_apis() {
}

noret_s cma::ui::Canvas_Features_user() {


   switch (cmui.m_PageTab) {
      case CMUI::Current_Tab::VISUALS:
         cmui.TabVisuals.run();
         break;
      case CMUI::Current_Tab::AIM:
         cmui.TabAim.run();
         break;
      case CMUI::Current_Tab::MISC:
         cmui.TabMisc.run();
         break;
   };
}

class PopUpInstance {
};

noret_s cma::ui::Canvas_popup() {
}


noret_s cma::ui::CanvasMain() {

   // id :   MAIN-TOP

   gmElements gme;

   gme.designateFamily(
      "  font.3  top.80 width.160 rounding.8", {"visual.text name.label <text> PUBLIC_SOURCE_REDACTED.COM </text> font.15",
                                                "visual.text name.build x.20  y.-20<text> Powered by Gaia build 1.1 </text> font.1"

                                               }
   );


   gme.designateFamily(
      "  font.3 x.20 y.155 h.40  top.80 width.160 rounding.8", {
                                                                  "visual.button name.VISUALS",
                                                                  "visual.button name.AIMWARE",
                                                                  "visual.button name.OTHER",
                                                               }
   );
   gme.designateFamily(
      " name.customer-panel font.1 w.90 gap.100 rounding.20 x.660 y.10", {"visual.button name.Changelog",
                                                                          "visual.button name.Report-Bugs",
                                                                          "visual.button name.X width.-50"

                                                                         }
   );


   if (gm.awaitPressed("AIMWARE")) {
      cmui.m_PageTab = CMUI::Current_Tab::AIM;
   }
   else if (gm.awaitPressed("VISUALS"))
      cmui.m_PageTab = CMUI::Current_Tab::VISUALS;
   else if (gm.awaitPressed("OTHER"))
      cmui.m_PageTab = CMUI::Current_Tab::MISC;
}

noret_s cma::ui::BaseCanvases_init() {
   if (!cmui.InitRoutes().Containers.config().menu)
      return;

   gm.designCanvas("name.main  x.0 y.-1 w.914 h.607 flags.notab flags.impostor flags.noresize flags.fixed flags.static owner.cmui", cmui.CanvasRoutes().canvasMain);
 
   gm.designCanvas("name.gaia-features  w.680 h.486 flags.notab  flags.fixed  x.202 y.90  flags.noresize  owner.cmui", cma::ui::Canvas_Features_user);

   gm.designCanvas("name.gaia-popup flags.notab  width.400 height.600   owner.cmui flags.hide", cmui.CanvasRoutes().popup);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.Overlay ////
////
////
////


noret_s cma::ui::overlay_routine() {

   auto gl = gmGL::_abstract();

   auto run_projection_as_pixel = [_ref] {
      glViewport(0, 0, 1, -1);
      glLoadIdentity();

      glMatrixMode(GL_PROJECTION_MATRIX);
      glOrtho(0, 1910, 0, 1070, -1, 1);
   };

   run_projection_as_pixel();


   // call shit here [ gl is setup for pixels coordinates ]
   {

      cma::ui::engine_routes->OVERLAYT1_inf.run(true);
   }
}


noret_s cma::ui::BaseContainers_init() {

   astr win_menu = astrc("name.cmui visual.empty   w.914 h.607");

   auto volatile myconf = cmui.InitRoutes().Containers.config();


   if (myconf.menu)
      gm.designContainer(win_menu);

   if (myconf.overlay) {
      gm.designContainer("name.ov width.1910  height.1070 visual.overlay", overlay_routine);
   }


   cmui.settings.PostActionInitiate();
}
