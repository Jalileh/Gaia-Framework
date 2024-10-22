#pragma once

#include "cm/cm.h"

#include "aim-cerebium.h"
#ifndef CM_VISWARE_H
#   include "vis-ware.h"
#endif
namespace cmapi {
   template < typename T, typename T_Addrs >
   class Aimware;
}


namespace ui_aimware {
   CM_IMPL astr name_lock_factor = "Lock Smoothing",   // for smoothing aim
      name_fov_area = "Fov Area",
                name_RCS = "Recoil";


}   // namespace ui_aimware

class UIAimwareConfig {
 public:
   inline auto GetLockFactor () {
      return gm.Vector2(ui_aimware::name_lock_factor);
   }

   inline auto GetRecoil () {
      return gm.awaitFloat(ui_aimware::name_RCS);
   }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.aimware implementation highest level and clean code
////
////
////
////

template < typename T, typename TADDR >
class t_aimEntity {
 private:
   vec3 *viewAngles;
   Bones *bones;
   vec3 *headpos;
   Entity< T, TADDR > _ptr managed_entity;


 public:
   t_aimEntity (Entity< T, TADDR > &base_entity, Bones T::*bones, vec3 T::*viewAnglesVector, vec3 T::*headPos)
      : managed_entity(&base_entity), bones(&(base_entity.*bones)),
        viewAngles(&(base_entity.*viewAnglesVector)), headpos(&(base_entity.*headPos)) {}

   t_aimEntity () : managed_entity(null) {};

 public:
   auto ViewAngles () {
      return deref viewAngles;
   }
   auto &Bones () {
      return deref bones;
   }
   auto Origin () {
      return deref headpos;
   }
   auto _ref EObject () {
      return deref managed_entity;
   }

   // if we even have an entity wrapped in our logic, do not run any algo using this object if false
   bool isManagedEntity () {
      return (static_cast< bool >(managed_entity) > null);
   }
};


class CrossHairFov {
 public:
   noret initSystem () {
      fovdata.BasePosition = NativeDisplay().GetCenterWH();

      cme.fnroutes.OVERLAYT1_inf = [] () {
         if (!cm.gui.awaitEnabled("stealthy Aim") or cm.gui.awaitEnabled("Hide Fov Helper"))
            return;

         try {
            RunGuiSystem(fovdata);
         } catch (astr &e) {
            cle_private(e, "CM-AIMWARE-CROSSHAIRFOV");
         } catch (const std::exception &e) {
            cle_private(e.what(), "CM-AIMWARE-CROSSHAIRFOV");
         }

         return;
      };

      // cm-ui shit
      cmui.TabAim = [] {
         gmElements gme;
         gme.designateFamily("y.120 top.45", {

                                                "visual.slider1 name='Lock Smoothing' min.1 base.5   max.300 ",
                                                "visual.slider name='Fov Area' min.1 base.10 max.300 ",
                                                "visual.slider1 name='Recoil' min.1 base.1.704 max.300 ",

                                                "visual.checkbox name='Hide Fov Helper'",
                                             });


         return;
      };

      return;
   }
   struct FovData {
      vec2 BasePosition;

      const float BasePoints = 30.f,
                  BaseRadius = 50.f;

      bool disableFovDraw = false;

      gmVec2 modifier;

      float GetRadius () {
         return modifier.x + BaseRadius;
      }

      FovData () noexcept {}
   };

   const FovData &GetFovData () {
      return fovdata;
   }


 private:
   singleton_member FovData fovdata;

 public:
   static noret RunGuiSystem (FovData &fovdata) {
      if (fovdata.modifier = gm.awaitVector2("Fov Area"); fovdata.modifier.x > 0) {
         DisplayFOVCircle(fovdata.modifier.x + fovdata.BaseRadius, fovdata.BasePoints);
      }
      else {
         DisplayFOVCircle(fovdata.BaseRadius, fovdata.BasePoints);
      }


      return;
   }

   static noret DisplayFOVCircle (float radius, float points) {
      vec2 center = NativeDisplay().GetCenterWH();

      gmGL().color3f(1.0f, 0.0f, 0.0f);

#ifdef CM_AIMWARE_CROSSHAIR_ALIGN
      center.x -= CM_AIMWARE_CROSSHAIR_ALIGN,
         center.y -= CM_AIMWARE_CROSSHAIR_ALIGN;
#endif

      FDraws().DrawCircle(center, points, radius);
   }
};


// AIMWARE 06/06 cs2 GAIA robust
template < typename T, typename TADDR >
class cmapi::Aimware : public CrossHairFov, public UIAimwareConfig {
 public:
   aimCerebium::AimResolver resolver;

 protected:
   aimCerebium::inputSystem userinput;
   modelScalar m_scalar;

   using aimEntity = t_aimEntity< T, TADDR >;
   using CMEntity = Entity< T, TADDR >;


 protected:
   u64 m_ADDR_ViewAngles;
   ViewMatrix &m_viewMatrix;

 protected:
   aimEntity m_player, m_client;
   singleton_member aimEntity m_LastBestAimedPlayer;   // last player used to circumvent the aim


 public:
   Aimware (CMEntity &Client, CMEntity &Player, Bones T::*bones, vec3 T::*HEADPOS, vec3 T::*viewAnglesVec3, u64 TADDR::*ViewAngles, ViewMatrix &viewMatrix) : m_viewMatrix(viewMatrix) {

      m_player = aimEntity(Player, bones, viewAnglesVec3, HEADPOS);

      m_client = aimEntity(Client, bones, viewAnglesVec3, HEADPOS);

      // used only in fov feature
      m_viewMatrix = viewMatrix;

      // fetch the writeable angles
      m_ADDR_ViewAngles = Client.m_GetAddress().*ViewAngles;

      // start up ov links and the system

      if (!m_LastBestAimedPlayer.isManagedEntity()) {
         SetBestAimedPlayer(m_player);

         CrossHairFov fov_object;
         fov_object.initSystem();
      }


      this->userinput.SetTriggerKey(keycode_mouseL);
   }

   // non best aim play possibly
   auto &player () {
      return this->m_player;
   }


   auto &BestAimedPlayer () {
      return m_LastBestAimedPlayer;
   }

   noret SetBestAimedPlayer (aimEntity &NewBestPlayer) {
      m_LastBestAimedPlayer = NewBestPlayer;
   }

   // just check if someone is inside circle
   bool GOOD_PLAYER_IN_FOV (aimEntity &Player) {
      CrossHairFov fov;
      float discard_distance;

      if (!Player.EObject().hp)
         return false;

      return IsEntityinFovContext(Player, m_viewMatrix, this->m_scalar, discard_distance);
   }

   bool GOOD_PLAYER_IN_FOV (aimEntity &Player, float &distanceToFovCenter) {
      CrossHairFov fov;

      if (!Player.EObject().hp)
         return false;

      return IsEntityinFovContext(Player, m_viewMatrix, this->m_scalar, distanceToFovCenter);
   }

   // resolves closest to circle this is where we determine it
   // knownBestPlayer is just extra flavour for this function conditions


   bool IsEntityinFovContext (aimEntity WorldEntity, ViewMatrix &ViewMatrix, modelScalar &scalar, float &out_distanceToFovCenter) {
      World tran;

      vec2 screen_point;

      auto HeadPosition = WorldEntity.Origin();
      HeadPosition.z += 40;

      if (!tran.ToScreenbyVMatrix(HeadPosition, screen_point, ViewMatrix, NativeDisplay().GetPixelRes()))
         return false;


      auto fovdata = CrossHairFov().GetFovData();

      ModelBoundAreaScalar bounds;


      auto box = bounds.GenerateBoxBounds(screen_point, scalar.GetScaleFactor());


      bool box_is_inside = isBoxInsideCircle(
         box,
         scalar.GetScaleFactor(),
         fovdata.BasePosition,
         fovdata.modifier.x + fovdata.BaseRadius,
         out_distanceToFovCenter

      );

      if (!box_is_inside and isPointsInsideBox2(
                                {{fovdata.BasePosition.x - fovdata.GetRadius(), fovdata.BasePosition.y + fovdata.GetRadius()},
                                 {fovdata.BasePosition.x + fovdata.GetRadius(), fovdata.BasePosition.y - fovdata.GetRadius()},

                                 {fovdata.BasePosition.x, fovdata.BasePosition.y}

                                },
                                box
                             ))
         return true;

      return box_is_inside;
   }


   float DistanceToClient (aimEntity &Player) {
      return m_client.Origin().DistTo(Player.Origin());
   }

   float DistanceBetween (vec3 &angle_1, vec3 &angle_2) {
      return angle_1.DistTo(angle_2);
   }

   bool isClientTeammate (aimEntity &Player) {
      return Player.EObject()->teamid == m_client.EObject()->teamid;
   }

   bool IS_LAST_BESTPLAYER (aimEntity &AimingEntity) {
      return (AimingEntity.EObject().m_intel().slot == m_LastBestAimedPlayer.EObject().m_intel().slot);
   }

 private:
   // A utility function to wrap angles to the range [-180, 180]
   float WrapAngle (float angle) {
      angle = fmod(angle + 180.0f, 360.0f);
      if (angle < 0)
         angle += 360.0f;
      return angle - 180.0f;
   }

   // Linear interpolation (LERP) function for smooth transition
   float Lerp (float start, float end, float t) {
      return start + t * (end - start);
   }

 public:
   CM_IMPL vec3 aim_angle_smoother (float modifier, const CAngles &targetAngles, CAngles m_clientViewAngles) {
      vec3 smoothed_angles;

      // Calculate the difference between the target and current angles
      float deltaX = WrapAngle(targetAngles.pitch - m_client.ViewAngles().x);
      float deltaY = WrapAngle(targetAngles.yaw - m_client.ViewAngles().y);

      // Apply smoothing using LERP or simple division
      // Modifier here can be used as a factor for the smoothing effect
      float smoothingFactor = 1.0f / modifier;   // Adjust this to fine-tune the smoothing

      smoothed_angles.x = m_client.ViewAngles().x + deltaX * smoothingFactor;
      smoothed_angles.y = m_client.ViewAngles().y + deltaY * smoothingFactor;

      return smoothed_angles;
   }
   CM_IMPL vec3 aim_angle_smootherEx (float modifier, const CAngles &targetAngles, vec3 m_clientViewAngles) {
      vec3 smoothed_angles;

      // Calculate the difference between the target and current angles
      float deltaX = WrapAngle(targetAngles.pitch - m_clientViewAngles.x);
      float deltaY = WrapAngle(targetAngles.yaw - m_clientViewAngles.y);

      // Apply smoothing using LERP or simple division
      // Modifier here can be used as a factor for the smoothing effect
      float smoothingFactor = 1.0f / modifier;   // Adjust this to fine-tune the smoothing

      smoothed_angles.x = m_clientViewAngles.x + deltaX * smoothingFactor;
      smoothed_angles.y = m_clientViewAngles.y + deltaY * smoothingFactor;

      return smoothed_angles;
   }

   CM_IMPL CAngles GenerateAimAngles (vec3 &clientHeadPos, vec3 &EnemyHeadPos) {
      return resolver.BaseConvToAimAngle(clientHeadPos, EnemyHeadPos);
   }

 public:
   CM_IMPL noret client_mouse_move (CAngles &push_angle) {
      rat.write(m_ADDR_ViewAngles, push_angle, "cangle-pushed");
      return;
   }


 private:
   // is needed to determine our sophisticated
   // and robust fovbased aim system
   bool On_aiming_fov_player (aimEntity &Player) {

      // // no team-mate aims [CENSORED]
      // if (isClientTeammate(Player))
      //    return false;

      auto Factor = m_scalar.GenerateScaleFactor(
         DistanceToClient(Player), BaseScaleModifier::MAP_TYPE_CSGO
      );

      m_scalar.SetScaleFactor(Factor);

      if (IS_LAST_BESTPLAYER(Player)) {
         // check if we still fucking inside it
         auto bIsInsideFov = GOOD_PLAYER_IN_FOV(Player);
         if (Player.EObject().hp == 0) {
            sleeper().delay("100ms");
         }
         return (bIsInsideFov) ? true : false;
      }

      float fov_dist_trial = 0, fov_dist_best = 0;

      if (GOOD_PLAYER_IN_FOV(Player, fov_dist_trial)) {

         // both players are caught
         if (GOOD_PLAYER_IN_FOV(BestAimedPlayer(), fov_dist_best)) {
            auto best_distance = DistanceToClient(BestAimedPlayer());
            auto trial_distance = DistanceToClient(Player);

            auto relative_best_from_client = (best_distance - trial_distance);
            if ((relative_best_from_client > 150))
               return false;
            else if (fov_dist_trial > fov_dist_best)
               return false;
         }

         SetBestAimedPlayer(Player);
         return true;
      }

      return false;
   }


 public:
   bool template_StealthyAim () {

      CAngles angles;


      if (On_aiming_fov_player(m_player)) {

         angles = GenerateAimAngles(m_client.headpos, BestAimedPlayer().headpos);

         if (this->userinput.when_key_held()) {
            auto ui_value = gm.awaitFloat("Lock Smoothing");

            angles = aim_angle_smoother(ui_value, angles);

            client_mouse_move(angles);
         }
      }
      else {

         return false;
      }

      return true;
   }

   bool cb_Angle_Edit_StealthyAim (auto fn_angle_modifier) {

      CAngles angles;


      if (On_aiming_fov_player(m_player)) {

         angles = GenerateAimAngles(m_client.headpos, BestAimedPlayer().headpos);
         // callback may change angle here


         if (this->userinput.when_key_held()) {
            auto ui_value = gm.awaitVector2("Lock Smoothing");

            angles = aim_angle_smoother(ui_value.x, angles);
            fn_angle_modifier(angles);

            client_mouse_move(angles);
         }
      }
      else {

         return false;
      }

      return true;
   }
   bool cb_on_aim (auto cb) {

      CAngles angles;


      if (On_aiming_fov_player(m_player)) {

         // callback may change angle here
         angles = cb(m_client.EObject(), BestAimedPlayer().EObject());

         if (this->userinput.when_key_held()) {

            client_mouse_move(angles);
         }
      }
      else {

         return false;
      }

      return true;
   }
};
