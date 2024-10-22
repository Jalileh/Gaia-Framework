#pragma once
#include "..\include\easymath.h"


namespace cmapi {
   namespace aimCerebium {}
}   // namespace cmapi

using namespace cmapi;

namespace cmapi::aimCerebium {
   class AimResolver;
   class inputSystem;


}   // namespace cmapi::aimCerebium

#define CM_AIM_PROTO_H


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.SmoothAim api
////
////
////
////


class aimCerebium::inputSystem {

   u32 m_TriggerKey;   // user input key for algo
   u32 m_key_cycles;   // amount of cycles of it triggering


 public:               // call sys to resolve key shenanigans
   bool when_key_held () {
      // check if key is pressed
      auto result = (GetAsyncKeyState(m_TriggerKey) & 0x8000) > 0;
      return result;
   }


 public:
   auto SetTriggerKey (u32 TriggerKey) {
      m_TriggerKey = TriggerKey;
   }
   auto GetUsedKey () {
      return m_TriggerKey;
   }
};

class CAngles {
 public:
   float pitch, yaw, roll;

 public:
   CAngles () : pitch(unset), yaw(unset), roll(unset) {}

   operator vec3() {
      return {pitch, yaw, roll};
   }

   operator vec3() const {
      return {pitch, yaw, roll};
   }
   vec3 vec () {
      return *this;
   }

   CAngles (const vec3 _ref v_angles) {
      pitch = v_angles.x;
      yaw = v_angles.y;
      roll = v_angles.z;
   };
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.Aim Resolver : Gaia Source 1.0
////
////  this gem of codebase should in theory generate us aim angles.
////  and ontop of that provide us with utility to determine the most reasonable
class aimCerebium::AimResolver {
 public:
   // some games check angles, you might want to be wary if you are making stealthy feature
   bool CheckRestrictedAngle (vec2 GeneratedAngles, vec2 MaxRestriction) {
      if (GeneratedAngles >= MaxRestriction)
         return false;
      else
         return true;
   }


 public:
   // This Code is certified and robust - let it age [CENSORED] [LET IT [CENSORED]ING AGE]
   //
   // [UNWANTED RESULTS WARNNING ]  THIS IS ONLY FOR BODY ORIGIN CALCULATIONS
   // YOU MUST GO ON TO CALL OTHER FUNCTIONS FOR BONE RESOLVER
   CAngles BaseConvToAimAngle (vec3 Observer, vec3 Acknowledged) {
      vec3 Origin = (Acknowledged - Observer);

      CAngles angles;

      VectorAngles((float _ptr)_ref Origin, (float _ptr)_ref angles);


      return angles;
   }


 private:
   void VectorAngles (const float *forward, float *angles) {

      float tmp, yaw, pitch;

      if (forward[1] == 0 && forward[0] == 0) {
         yaw = 0;
         if (forward[2] > 0)
            pitch = 270;
         else
            pitch = 90;
      }
      else {
         yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
         if (yaw < 0)
            yaw += 360;

         tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);


         pitch = (atan2(-forward[2], tmp) * 180 / M_PI);

         /*  why did the [CENSORED] put this here ?
            if (pitch < 0)
             wtf ? pitch += 360;
         */
      }

      angles[0] = pitch;
      angles[1] = yaw;
      angles[2] = 0;
   }
};
