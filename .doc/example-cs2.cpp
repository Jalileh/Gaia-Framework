

#include "ecl.h"


#include "cm/cm.h"


class BPlayer {
 public:
   int hp;

   vec3 origin;
   vec3 angles;
   vec3 punchAngle;
   vec3 viewEyePosition;
   float Sensitivity;

   class Addrs {
    public:
      u64 BaseEntity, Controller, BasePawn,   // csPlayerController is C_BaseEntity,
         Weapon,
         BodyComponentGraph, CSkeletonInstance, ModelState, BoneidMatrix,
         CameraServices,
         AngleBase;
   };

   Bones bones = Bones(50);
};

class gameinfo {
 public:
   u64 gameSystem;
   u32 getHighestEntityIndex;
   int fov;
   ViewMatrix viewMatrix;
};


Entitylist elist(BPlayer{}, gameinfo{}, BPlayer::Addrs{});
Client client(BPlayer{}, BPlayer::Addrs{});
using player = Entity< BPlayer, BPlayer::Addrs >;


auto impl_features = [] {
   cmui.TabVisuals = [] {
      gmElements gme;

      gme.designateFamily(" y.10 top.35", {"v.checkbox name='ESP Outline'", "v.checkbox name='ESP Healthbar'", "v.checkbox name='ESP Skeleton'"

                                          });
   };

   cmui.TabAim = [] {
      gmElements gme;

      gme.designateFamily(" y.10 top.35", {"v.checkbox name='stealthy Aim'", "v.checkbox name='Aggressive Mode'", "v.checkbox name='Trigger Bot'"});
   };

   cme.fnroutes.OVERLAYT1_inf.Link([] {
      // ESP - feature scripting
      for (player player : elist.GetList()) {
         if (!player.hp or elist.m_isclient(player) or !player.m_isGoodEntity() /* or player.teamid == client.teamid */)
            continue;

         cmapi::visware vis(
            elist.GetUserMeta().viewMatrix,
            client.origin,
            player.origin,
            30
         );

         if (!vis.bTranslated())
            continue;

         if (gm.awaitEnabled("ESP Outline"))
            vis.DrawOutline();


         if (gm.awaitEnabled("ESP Healthbar"))
            vis.DrawHPBar(player.hp);

         if (gm.awaitEnabled("ESP Skeleton"))
            vis.DrawBones(player.bones);
      }


      return;
   });

   cme.fnroutes.CheatT2_inf.Link([] {
      for (player &player : elist.GetList()) {
         if (elist.m_isbadEntOrClient(player) /* or player.teamid == client.teamid */)
            continue;


         cmapi::Aimware< BPlayer, player::Addrs > aimware(
            client, player,
            &player::bones,
            &player::origin, &player::angles,
            &player::Addrs::AngleBase,
            elist.GetUserMeta().viewMatrix
         );

         if (gm.awaitEnabled("stealthy Aim")) {

            class RecoilSupressor {
             public:
            };

            static vec3 old_punchAngle;
            aimware.cb_on_aim([_ref] (::player me, ::player enemy) {
               auto my_View = client.origin + client.viewEyePosition;

               vec3 punch_angle_recoil = client.punchAngle;


               auto enemy_Head = enemy.bones.id("LEFT_ARM");   // left arm is head lol

               vec3 wangle = aimware.resolver.BaseConvToAimAngle(my_View, enemy_Head);


               auto lock_Factor = gm.awaitFloat("Lock Smoothing");


               wangle = aimware.aim_angle_smoother(lock_Factor, wangle, me.angles);

               if (punch_angle_recoil.x != 0 and punch_angle_recoil.y != 0) {
                  wangle.x -= punch_angle_recoil.x / gm.awaitFloat("Recoil");
                  wangle.y -= punch_angle_recoil.y / gm.awaitFloat("Recoil");
               }


               return wangle;
            });
         }
      }
   });
};


noret gaia::fn::Route() {


   impl_features();
   // client parsing
   client.Serve([] (player &client) {
      client.m_GetAddress().BasePawn = cm.sig.findObject("localPlayerPawn", cm.sig.findOffset< u8 >("LocalPlayerPawn_pointer"));
      client.m_GetAddress().AngleBase = cm.sig.findObject("viewAngleBase") + sig.findOffset< u16 >("viewAngles");
      elist.GetUserMeta().gameSystem = cm.sig.findObject("entityGameSystem", 0);

      rat.steal(
         cm.sig.findObject("viewMatrix"), elist.GetUserMeta().viewMatrix, "viewmatrix-alt"
      );

      auto PlayerPawn = client.m_GetAddress().BasePawn;


      return rat.readPointers(
         client.origin, PlayerPawn, sig.findOffset< u16 >("Pawn.BodyPosition"), "body pos",
         client.teamid, PlayerPawn, sig.findOffset< u16 >("Pawn.teamid"), "team id",
         client.angles, client.m_GetAddress().AngleBase, 0, "local r/w view Angles",
         client.hp, PlayerPawn, sig.findOffset< u16 >("Pawn.Health"), "hp local",
         client.punchAngle, PlayerPawn, sig.findOffset< u16 >("Pawn.BodyPosition") + 0x258, "recoil",
         client.viewEyePosition, PlayerPawn, sig.findOffset< u16 >("Pawn.Health") + 0x92C, "view"
      );
   });


   // locate entitysystem
   elist.provideInfo([] (auto &entitybase, auto &num) {
      entitybase = elist.GetUserMeta().gameSystem + 0x10;
      entitybase = rat.read< u64 >(entitybase, sizeof(u64), "E-Entry");
      num = (64);
   });


   // parsing player datas
   elist.ServeInstances(
      [_ref] (u64 &entity_base, player &player) {
         auto provideEnt = entity_base;
         auto gameSystem = elist.GetUserMeta().gameSystem;

         auto i = elist.GetPlayerAccessNumber();


         player.m_GetAddress().BaseEntity = provideEnt + (i & 0x1FF);

         u64 controller;
         if (!(controller = rat.read< u64 >(provideEnt + 0x78 * (i & 0x1FF), sizeof(u64), "E-Entry")))
            return false;

         u32 pawnId;
         if (!(pawnId = rat.read< u32 >(controller + sig.findOffset< u16 >("controller.mhPlayerPawn"), sizeof(u32), "pawnID")))
            return false;


         u64 pawn_entry;
         if (!(pawn_entry = rat.readPointer< u64 >(gameSystem, 0x8 * ((pawnId & 0x7FFF) >> 0x9) + 0x10, "pawn_entry")))
            return false;

         u64 current_pawn;
         if (!(current_pawn = rat.readPointer< u64 >(pawn_entry, (0x78 * (pawnId & 0x1FF)), "pawnID")))
            return false;
         else if (current_pawn == client.m_GetAddress().BasePawn)
            elist.MarkAsClient(player);

         auto &map = player.ml();
         map.BasePawn = current_pawn;


         bool result = rat.readPointers(
            map.BodyComponentGraph, map.BasePawn, sig.findOffset< u8 >("Pawn.BodyComponentGraph"), "BodyComponent",
            map.CSkeletonInstance, map.BodyComponentGraph, sig.findOffset< u8 >("BodyComponent.skeletonInstance"), "CSkeletonInstance",
            map.ModelState, map.CSkeletonInstance, 0x30 + sig.findOffset< u16 >("SkeletonInstance.modelState"), "ModelState",
            map.BoneidMatrix, map.ModelState, 0x80, "boneMatrix"
         );


         return result;
      },

      // parsing entity goes here
      [_ref] (player &player) {
         u64 Pawn = player.m_GetAddress().BasePawn;
         u64 controller = player.m_GetAddress().Controller;

         rat.stealv(
            Pawn + sig.findOffset< u16 >("Pawn.BodyPosition"), player.origin, "vec3 position -head",
            Pawn + sig.findOffset< u16 >("Pawn.teamid"), player.teamid, "team id",
            Pawn + sig.findOffset< u16 >("Pawn.Health"), player.hp, "player HP"
         );


         player.bones.ReadVector3FromPage(player.m_GetAddress().BoneidMatrix, 0x20);

         player.bones.MapBones({"id.head.6 to.NECK", "id.NECK.5 to.SPINE", "id.SPINE.4 to.SPINE_1", "id.SPINE_1.2 to.HIP", "id.HIP.0", "id.LEFT_SHOULDER.8 to.LEFT_ARM", "id.LEFT_ARM.9 to.LEFT_HAND", "id.LEFT_HAND.10", "id.RIGHT_SHOULDER.13 to.RIGHT_ARM", "id.RIGHT_ARM.14 to.RIGHT_HAND", "id.RIGHT_HAND.15",

                                "id.LEFT_HIP.22 to.LEFT_KNEE", "id.LEFT_KNEE.23 to.LEFT_FEET", "id.LEFT_FEET.24",

                                "id.RIGHT_HIP.25  to.RIGHT_KNEE", "id.RIGHT_KNEE.26 to.RIGHT_FEET", "id.RIGHT_FEET.27 ",

                                "id.BIND_RIGHT_SHOULDER.13 to.LEFT_SHOULDER", "id.BIND_RIGHT_HIP.25  to.LEFT_HIP", "id.BIND_RIGHT_HIP1.25  to.penis", "id.BIND_LEFT_HIP.22 to.penis",

                                "id.groin.1 to.SPINE_1"

         });
      },
      [_ref] (player &player) {

      }
   );
}

noret cma::config() {


   cm.sig.addObject(
      {

         "id.entityGameSystem  xxx????xxxxxxxxx     \\x48\\x8B\\x0D\\x65\\x09\\x44\\x01\\x8B\\xF3\\xC1\\xEB\\x0E\\x81\\xE6\\xFF\\x3F",


         "id.viewMatrix  xxx????xxxx \\x48\\x8D\\x0D\\x16\\x4A\\x70\\x01\\x48\\xC1\\xE0\\x06  ",

         "id.viewAngleBase xx????xxxxxxx????x????xxxx????xxxx????xxxx???? \\xFF\\x15\\x00\\x00\\x00\\x00\\x84\\xC0\\x74\\x0C\\x48\\x8D\\x0D\\x00\\x00\\x00\\x00\\xE8\\x00\\x00\\x00\\x00\\x48\\x8B\\xB4\\x24\\x00\\x00\\x00\\x00\\x48\\x8B\\xCE\\xE8\\x00\\x00\\x00\\x00\\x48\\x8B\\xCE\\xE8\\x00\\x00\\x00\\x00 dpos.13",

         "id.localPlayerPawn   xxx????xxx????x????x?   \\x48\\x89\\x0D\\x40\\xDB\\x69\\x01\\x48\\x89\\x0D\\x41\\xDB\\x69\\x01\\x89\\x0D\\x43\\xDB\\x69\\x01\\x48\\x89\\x0D"
      },
      "client.dll"
   );


   cm.sig.addOffset(
      {"id.Pawn.BodyComponentGraph    xxx?xxxxxxxx?xxxxxx \\x48\\x8B\\x49\\x00\\x48\\x85\\xC9\\x74\\x39\\x48\\x8B\\x49\\x00\\x48\\x8B\\x01\\xFF\\x50\\x48 dpos.3",
       "id.BodyComponent.skeletonInstance xxx?xxxxxxxx?xxxxxx \\x48\\x8B\\x49\\x00\\x48\\x85\\xC9\\x74\\x39\\x48\\x8B\\x49\\x00\\x48\\x8B\\x01\\xFF\\x50\\x48 dpos.12",


       "id.SkeletonInstance.modelState xxx????xxxxx?xxxxxxxx???? \\x48\\x8B\\x89\\x00\\x00\\x00\\x00\\x48\\x8B\\x01\\xFF\\x50\\x00\\x48\\x85\\xC0\\x74\\x17\\x48\\x8B\\x80\\x00\\x00\\x00\\x00 pos.3",
       "id.Pawn.Health xxxx????xxxxxxxxxx????xxxxxxxxx?xxxxx?xxxxxxxxxxxxxxxx \\xF3\\x0F\\x10\\x81\\x00\\x00\\x00\\x00\\x0F\\x57\\xD2\\x0F\\x5A\\xF8\\xF3\\x0F\\x10\\x81\\x00\\x00\\x00\\x00\\x48\\x8D\\x4D\\xD0\\xF2\\x0F\\x11\\x7C\\x24\\x00\\xF2\\x0F\\x11\\x74\\x24\\x00\\x0F\\xC6\\xC9\\x55\\xF3\\x0F\\x5A\\xD9\\x0F\\x5A\\xE0\\xF3\\x41\\x0F\\x5A\\xD0 dpos.4",

       "id.Pawn.BodyPosition xxxx????xxxxx????xx????xx???? \\xF2\\x0F\\x11\\x83\\x00\\x00\\x00\\x00\\x8B\\x40\\x08\\x89\\x83\\x00\\x00\\x00\\x00\\x8B\\x83\\x00\\x00\\x00\\x00\\x89\\x83\\x00\\x00\\x00\\x00 dpos.4",

       "id.LocalPlayerPawn_pointer xxx?xxxxxxxx???? \\x45\\x03\\x4F\\x30\\x02\\xDA\\x8B\\x04\\xAF\\x41\\x81\\xC1\\x00\\x00\\x00\\x00 dpos.3",   // this ones very bad
       "id.Pawn.teamid xx?????xx????xxx \\x80\\xBE\\x00\\x00\\x00\\x00\\x00\\x0F\\x84\\x00\\x00\\x00\\x00\\x49\\x8B\\xCE pos.2",
       "id.viewAngles xxxxxx????xxxxxxxxx????xxxx?xxxx?xxxx?xxxxxxxxxxxxxx \\xF2\\x41\\x0F\\x10\\x84\\x30\\x00\\x00\\x00\\x00\\xF2\\x41\\x0F\\x11\\x01\\x41\\x8B\\x84\\x30\\x00\\x00\\x00\\x00\\x48\\x8B\\x5C\\x24\\x00\\x48\\x8B\\x6C\\x24\\x00\\x48\\x8B\\x74\\x24\\x00\\x41\\x89\\x41\\x08\\x48\\x83\\xC4\\x20\\x41\\x5F\\x41\\x5E\\x5F\\xC3  dpos.6",
       "id.controller.mhPlayerPawn  xxx???xxxxxx???xxx????x????xx?????xxx??   \\x0F\\x84\\xC3\\x00\\x00\\x00\\x4D\\x85\\xFF\\x0F\\x84\\xBA\\x00\\x00\\x00\\x41\\x8B\\x8F\\xE4\\x07\\x00\\x00\\x83\\xF9\\xFF\\x74\\x7F\\x4C\\x8B\\x0D\\x96\\xC1\\xD7\\x00\\x4D\\x85\\xC9\\x74\\x73   dpos.18"

      },
      "client.dll"
   );
}

cm_app("cs2.exe", " con2   epfile tkey.9");
