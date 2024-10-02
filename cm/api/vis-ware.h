#pragma once

#define CM_VISWARE_H
#include "cm/cm.h"

#include "vis-cerebium.h"

namespace cmapi {
   class visware;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.Fbone object
////
////
////
////

class Bones {
 public:
   class BoneInstance {
    public:
      BoneInstance() : index(-1) {}
      astr id;
      int index;
      vec3 vector;

      // this is the bonefriend that is locked for bone esp
      astr pair_bone;
   };

 private:
   CachedData< BoneInstance > BoneData;

   bool m_boneData_Mapped = false;
   u32 m_boneMapLength;

 public:
   auto _isBonesMapped () {
      return m_boneData_Mapped;
   }

   auto _GetBoneMapLength () {
      return m_boneMapLength;
   }

   auto _SetBoneMapLength (u16 length) {
      this->m_boneMapLength = length;
   }

   Bones() : m_boneMapLength(null), m_boneData_Mapped(false) {}

   constexpr Bones(auto num_of_bones) {
      for (int i = 0; i < num_of_bones; i++)
         BoneData.AddItem(BoneInstance{});
   }

   // the node lists of raw read bone matrix
   auto _ref BoneList () {
      return BoneData.GetList();
   }


   //[ THIS IS API FOR NON BONE CLASS DO NOT USE ]
   // access id string
   vec3 id (astr id) {
      if (this->_isBonesMapped()) {
         return BoneData.getbyID_s(id, _ref BoneInstance::id)->vector;
      }
      else {
         return vec3{};
      }
   }

   // [ THIS IS API FOR NON BONE CLASS DO NOT USE ]
   // access with index rather than string head for example
   auto _ref numBoneID (int id) {
      return BoneData.getbyID_s_nocle(id, _ref BoneInstance::index)->vector;
   }

   // accessid struct
   // access with index rather than string head for example
   BoneInstance _boneid (astr id) {
      auto ptr = BoneData.getbyID_s_nocle(id, _ref BoneInstance::id);
      return deref ptr;
   }
   // access with struct
   BoneInstance _numBoneID (int id) {
      return deref BoneData.getbyID_s_nocle(id, _ref BoneInstance::index);
   }

   // num_spacing incase the layout of the bonematrix has non-relevant floats between them
   // this data structure layout is of cs2 due to bone_index implementation occuring in 1-30 + spacing
   auto ReadVector3FromPage (const u64 Address, auto num_spacing) {

      auto &bones = BoneList();
      auto const numofBones = bones.Length();

      auto raw_ptr = rat._CReadPointerRaw< pbyte >(Address, (sizeof(vec3) + num_spacing) * numofBones, "bone-parse");

      for (u32 i = 0; i < numofBones; i++) {
         // bone index appears as i
         vec3 _bonevec = deref(vec3 *)(raw_ptr + (i * num_spacing));
         bones[i].vector = _bonevec;

         //  auto _bonevec = rat.read< vec3 >(Address + (i * num_spacing), sizeof(vec3), "bone-parse");
         //  bones[i].vector = _bonevec;
      }
   }


   auto MapBones (const List< astr > _ref BoneMapper) {
      if (this->m_boneData_Mapped)
         return;
      else {
         this->m_boneMapLength = BoneMapper.Length();
      }

      auto num_null_used = false;


      for (u32 i = 0; i < BoneMapper.Length(); i++) {
         astr desc = deref const_cast< astr * >(&BoneMapper[i]);
         astr id = desc.ExtractWord("id.\a");
         astr num_index, bone_to;

         if (!id) {
            cle_private("bone id not supplied", "MapBones vis-ware.h");
         }

         num_index = id.ExtractWord(".\a");

         if (num_index == "0") {
            num_null_used = true;
         }
         else if (!num_index and num_null_used) {
            cle_private("cant use bone-index null twice!", "MapBones vis-ware.h");
         }
         else if (num_index.toint() > BoneList().Length()) {
            cle_private("given index is greater than vec3list of bones", "MapBones vis-ware.h");
         }

         bone_to = desc.ExtractWord("to.\a");

         if (!bone_to and !BoneMapper.enumisLast(i))
            cle_private("no connector for bone! (bone to)", "MapBones vis-ware.h");

         auto &list = this->BoneList();
         {
            id = id.stripRightof(".", true);

            if (num_null_used)
               id = id.replace("0", "");

            list[i].id = id;
            list[i].index = num_index;
            list[i].pair_bone = bone_to;
         }
      }

      m_boneData_Mapped = true;
   }
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.FDraws - Advanced
////
////
////
////

class FDraws_Advanced {
 public:
   bool BoneDraw (ViewMatrix View, Bones _ref bones) {

      auto world = World{};
      auto v = gmGL::VertexApi();
      auto gl = gmGL{};


      vec2 screen[] = {vec2{}, vec2{}};


      if (!bones._isBonesMapped())
         return false;


      auto &boneList = bones.BoneList();
      auto bone_length = boneList.Length();


      gl.DrawByMode(gmGL::mode::line_linear, [_ref] {
         // resolve
         // make sure line is swear-wording there
         for (int i = 0; i < bones._GetBoneMapLength(); i++) {   // minus one cus head is accessed first above

            Bones::BoneInstance bone;
            Bones::BoneInstance connector_bone;

            bone = bones.BoneList()[i];

            if (bone.pair_bone.NotNull())
               connector_bone = bones._boneid(bone.pair_bone);
            else
               continue;

            // draw them
            auto ParentBoneVec_1 = boneList[bone.index].vector;
            auto ConnectedBoneVec_2 = boneList[connector_bone.index].vector;

            u32 conv_to_world_num = null;

            conv_to_world_num += world.ToScreenbyVMatrix(ParentBoneVec_1, screen[0], View, NativeDisplay().GetPixelRes());
            conv_to_world_num += world.ToScreenbyVMatrix(ConnectedBoneVec_2, screen[1], View, NativeDisplay().GetPixelRes());

            if (conv_to_world_num > 1) {

               v.VertexV2(screen[1].x, screen[1].y);

               v.VertexV2(screen[0].x, screen[0].y);
            }
         }
      });


      return true;
   }
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.Functional Draws  - basic
////
////
////
////

class FDraws : public FDraws_Advanced {
 public:
   noret DrawBox (const vec2 _ref DesiredPoint, float width, float height, float scaleFactor = 1) {
      gmGL gl;

      width /= scaleFactor, height /= scaleFactor;

      gl.DrawByMode(gmGL::mode::line_loop, [&] {
         gl.VertexApi().VertexV2(
            DesiredPoint.x - width, DesiredPoint.y - height,   // Bottom-left vertex
            DesiredPoint.x + width, DesiredPoint.y - height,   // Bottom-right vertex
            DesiredPoint.x + width, DesiredPoint.y + height,   // Top-right vertex
            DesiredPoint.x - width, DesiredPoint.y + height    // Top-right vertex
         );
      });
   }

   noret DrawHPBar (const vec2 _ref DesiredPoint, float width, float height, float health, float scaleFactor, float bar_postion_offset = 77.f) {

      float HP_BAR_HEIGHT = height / scaleFactor;
      float HP_BAR_WIDTH = width / scaleFactor;


      vec2 screen = {DesiredPoint.x + (bar_postion_offset / scaleFactor), DesiredPoint.y};

      // Drawing the background red rectangle

      gmGL gl;

      constexpr gmcolor color_badHP(0.8f, 0.0f, 0.0f);
      gl.DrawByMode(gmGL::mode::quads, color_badHP, [&] {
         gl.VertexApi().VertexV2(
            screen.x, screen.y - HP_BAR_HEIGHT,                  // Bottom-left
            screen.x, screen.y + HP_BAR_HEIGHT,                  // Top-left
            screen.x + HP_BAR_WIDTH, screen.y + HP_BAR_HEIGHT,   // Top-right
            screen.x + HP_BAR_WIDTH, screen.y - HP_BAR_HEIGHT    // Bottom-right
         );
      });

      float greenHeight = (HP_BAR_HEIGHT) * (health / 100.0f);

      // Drawing the health green rectangle

      constexpr gmcolor color(41, 187, 48);
      gl.DrawByMode(gmGL::mode::quads, color, [&] {
         gl.VertexApi().VertexV2(
            screen.x, screen.y - HP_BAR_HEIGHT,                                    // Bottom-left
            screen.x, screen.y - HP_BAR_HEIGHT + greenHeight * 2,                  // Top-left
            screen.x + HP_BAR_WIDTH, screen.y - HP_BAR_HEIGHT + greenHeight * 2,   // Top-right
            screen.x + HP_BAR_WIDTH, screen.y - HP_BAR_HEIGHT                      // Bottom-right
         );
      });
   }

   /*
       vec2 center = vis._abstract_display().GetCenterWH();

         float points = 30; [note amount of swear-wording points we draw around]
         float radius = 10; [radius dummy you never went to school]
         vec3 red = {1.0f, 0.0f, 0.0f};
         DrawCircle(center, points, radius, red);
   */
   noret DrawCircle (const vec2 &center, float points, float radius) {
      gmGL gl;

      float step = (float)M_PI * 2.0f / points;

      gl.DrawByMode(gmGL::mode::line_loop, [&] {
         for (float a = 0; a < (M_PI * 2.0f); a += step) {
            float x = radius * cosf(a) + center.x;
            float y = radius * sinf(a) + center.y;
            gl.VertexApi().VertexV2(x, y);
         }
      });
   }


   noret _DrawText (const vec2 _ref desiredPoint, astr Text) {
   }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.bone impl
////
//// csgo has 30
////
////

enum class BaseBoneIndex_cs2 : u16 {
   head = 1,
   arm_1 = 2,
   arm_2 = 3,
   torso = 4,
   penis = 8,
   leg_1 = 10,
   leg_2 = 12

};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.visware subs and core
////
////
////
////


// add preprocessors for this shite
enum class BaseScaleModifier : short int {
   MAP_TYPE_CSGO = 1,
   MAP_TYPE_GTA5 = 2,
   MAP_TYPE_DAYZ = 3
};


class modelScalar {
   float m_factor;

 public:
   // generates scale factor with e-c range.
   auto GenerateScaleFactor (float Distance, BaseScaleModifier map) {
      return (Distance) / 181.f;
   }

   auto SetScaleFactor (float Factor) {
      m_factor = Factor;
   }

   float GetScaleFactor () {
      return m_factor;
   }
};


// class is limited to non-global swear-word
class cmapi::visware {
   const ViewMatrix &m_viewMatrix;
   const vec3 &m_client_in_world;
   vec3 m_Entity_in_world;

 protected:
   visCerebium::World world;
   visCerebium::NativeDisplay native;

   bool m_bTranslated;
   vec2 m_TranslatedPoint;

 protected:
   FDraws draw;
   modelScalar scalar;

 public:
   visware(const ViewMatrix _ref viewMatrix, const vec3 _ref client_in_world, const vec3 _ref Entity_in_world, float addToHeadPos = 30.f)
      : m_viewMatrix(viewMatrix), m_client_in_world(client_in_world), m_Entity_in_world(Entity_in_world) {

      m_Entity_in_world.z += addToHeadPos;

      m_bTranslated = world.ToScreenbyVMatrix(m_Entity_in_world, m_TranslatedPoint, m_viewMatrix, native.GetPixelRes());

      auto Factor = scalar.GenerateScaleFactor(
         this->m_Entity_in_world.Dist2D(m_client_in_world), BaseScaleModifier::MAP_TYPE_CSGO
      );

      scalar.SetScaleFactor(Factor);
   }

   // if w2s was a success
   bool bTranslated () {
      return m_bTranslated;
   }

   // the scale factor generated with the API
   auto ScaleFactor () {
      return scalar.GetScaleFactor();
   }

   // the translated vector - fetch it
   vec2 W2ScreenVector () {
      return (m_bTranslated) ? m_TranslatedPoint : vec2{0.f, 0.f};
   }

   // abstract low level draw
   auto &_abstract_draw () {
      return draw;
   }
   auto &_abstract_display () {
      return native;
   }

   // HP DRAW DUH
   auto DrawHPBar (int HP) {

      draw.DrawHPBar(W2ScreenVector(), 5, 144, HP, scalar.GetScaleFactor());
   }

   // The classic BOX ESP
   auto DrawOutline () {
      glColor3f(0.5647f, 0.0706f, 0.6196f);

      glLineWidth(2.0f);
      draw.DrawBox(W2ScreenVector(), 70, 160, scalar.GetScaleFactor());
   }

   // dog we need this
   auto DrawHPText (astr HP) {
   }
   auto DrawBones (Bones bones) {
      glLineWidth(1.0f);
      glColor3f(1.0f, 0.502f, 0.0f);
      draw.BoneDraw(this->m_viewMatrix, bones);
   }
};
