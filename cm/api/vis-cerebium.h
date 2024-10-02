#include "..\include\easymath.h"
/* keep this file as mathematical and low level interface for -ware.h parent
 * vis-proto.h implementation for money
 */

namespace cmapi {
   namespace visCerebium {}
}   // namespace cmapi

using namespace cmapi;

namespace cmapi::visCerebium {
   class NativeDisplay;
   class World;

}   // namespace cmapi::visCerebium
using namespace visCerebium;

#define CM_VIS_PROTO_H

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.Native display code OS/CUSTOM usage
////
////
////
////

class visCerebium::NativeDisplay {
   vec2 PixelRes;

 public:
   // custom initialization with no syscall to get resolution
   NativeDisplay(vec2 Custom_Pixel_Resolution) : PixelRes(Custom_Pixel_Resolution) {
   }
   // Grabs The Resolution from OS. x2 Constructors
   NativeDisplay() {
      PixelRes = GetResolutionFromOS();
   }

   auto GetAspectRatio () {
      return (PixelRes.x / PixelRes.y);
   }
   vec2 GetCenterWH () {
      return {PixelRes.x / 2, PixelRes.y / 2};
   }

   auto GetPixelRes () {
      return PixelRes;
   }

 private:
   vec2 GetResolutionFromOS () {
      return {(float)GetSystemMetrics(0), (float)GetSystemMetrics(1)};
   }
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.ViewMatrices manifestor
////
////
////
////

class MatrixBuilder {
 public:
   static ViewMatrix Generate_Buggy_SandstormMatrix_4x4 (vec3 _ref Rotation, vec3 Origin) {
      float radPitch = (Rotation.x * GetPi() / 180.f);
      float radYaw = (Rotation.y * GetPi() / 180.f);
      float radRoll = (Rotation.z * GetPi() / 180.f);


      float SP = sinf(radPitch);
      float CP = cosf(radPitch);
      float SY = sinf(radYaw);
      float CY = cosf(radYaw);
      float SR = sinf(radRoll);
      float CR = cosf(radRoll);

      ViewMatrix matrix;
      matrix.m[0][0] = CP * CY;
      matrix.m[0][1] = CP * SY;
      matrix.m[0][2] = SP;
      matrix.m[0][3] = 0.f;

      matrix.m[1][0] = SR * SP * CY - CR * SY;
      matrix.m[1][1] = SR * SP * SY + CR * CY;
      matrix.m[1][2] = -SR * CP;
      matrix.m[1][3] = 0.f;

      matrix.m[2][0] = -(CR * SP * CY + SR * SY);
      matrix.m[2][1] = CY * SR - CR * SP * SY;
      matrix.m[2][2] = CR * CP;
      matrix.m[2][3] = 0.f;

      matrix.m[3][0] = Origin.x;
      matrix.m[3][1] = Origin.y;
      matrix.m[3][2] = Origin.z;
      matrix.m[3][3] = 1.f;

      return matrix;
   }


   // reversed incase some weird formatting occurs due to values?
   static ViewMatrix GenerateViewMatrix (const vec3 &Rotation, const vec3 &Origin) {

      // pitch
      const float SP = sin(Rotation.x * PI / 180.f);
      const float CP = cos(Rotation.x * PI / 180.f);

      // roll
      const float SR = sin(Rotation.z * PI / 180.f);
      const float CR = cos(Rotation.z * PI / 180.f);
      // yaw
      const float SY = sin(Rotation.y * PI / 180.f);
      const float CY = cos(Rotation.y * PI / 180.f);

      ViewMatrix matrix;
      int row = 0;

      matrix.m[row][0] = CP * CY;
      matrix.m[row][1] = CP * SY;
      matrix.m[row][2] = SP;


      row++;

      matrix.m[row][0] = SR * SP * CY - CR * SY;
      matrix.m[row][1] = SR * SP * SY + CR * CY;
      matrix.m[row][2] = -SR * CP;


      row++;

      matrix.m[row][0] = -(CR * SP * CY + SR * SY);
      matrix.m[row][1] = CY * SR - CR * SP * SY;
      matrix.m[row][2] = CR * CP;

      row++;

      matrix.m[row][0] = 0.f;
      matrix.m[row][1] = 0.f;
      matrix.m[row][2] = 0.f;
      matrix.m[row][3] = 1.f;

      return matrix;
   }
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.world projection
////
////   sandstorm magic number that was succesful = {0.568f, 0.900f}
////
////

// Core World Projection manifestor
class visCerebium::World {
 public:
   // To Screen Univeral for all games
   // Consuming this code requires the Camera Object, supplying the clients body positions assuming it's unaltered-
   // will not work.
   // YOU MUST FIND THE ( CAMERA X-Y-Z  OBJECT LOCATION ) THAT'S SEPERATE FROM THE LOCALPLAYER VECTOR OBJECT POSITION.
   // [ WARNING THE MATH IS CORRECT ] Your inputs are wrong [DO NOT USE YOUR WORLD LOCALPLAYER COORDINATES ]
   // if the generated output dosen't move the drawing down when you look up, you have not found the camera object.
   vec2 ToScreen_Universal (vec3 LocalCameraPositions, vec3 Angles, float Eye_FOV, vec3 WorldEntity) {
      _CachedScreenPos = world_to_screen_algorithm(LocalCameraPositions, Angles, Eye_FOV, WorldEntity);
      return _CachedScreenPos;
   }

   // mr robot viewmatrix pulled, feed it and magic occurs. [ TESTED AND IS WORKING ... ]
   // -- requires that you reverse viewmatrix and pull it from game.
   // -- If you cannot find it, you can surely transform one [ GAIA APPROVED]
   bool ToScreenbyVMatrix (const vec3 worldEntity, vec2 &screen, const ViewMatrix &vm, t_vec2< int > window_size) {

      float w = (vm[3][0] * worldEntity.x) + (vm[3][1] * worldEntity.y) + (vm[3][2] * worldEntity.z) + vm[3][3];

      const float WorldEntity_is_behind = 0.001f;

      if (w < WorldEntity_is_behind) {
         return false;
      }

      const float x = worldEntity.x * vm[0][0] + worldEntity.y * vm[0][1] + worldEntity.z * vm[0][2] + vm[0][3];
      const float y = worldEntity.x * vm[1][0] + worldEntity.y * vm[1][1] + worldEntity.z * vm[1][2] + vm[1][3];

      w = 1.f / w;
      float nx = x * w;
      float ny = y * w;


      screen.x = (window_size.x * 0.5f * nx) + (nx + window_size.x * 0.5f);
      screen.y = (window_size.y * 0.5f * ny) + (ny + window_size.y * 0.5f);

      return true;
   }

 private:
   // creates the ViewMatrix
   auto construct_viewMatrix (const vec3 &LocalCameraPositions, const vec3 &Eye_angles) {
      auto viewMatrix = MatrixBuilder::GenerateViewMatrix(Eye_angles, LocalCameraPositions);

      return viewMatrix;
   }

   // WARNING THIS CODE NEEDS swear-wordING THE CAMERA NOT AN ORIGIN VEC3 OF LOCALPLAYER!!!!
   // ORIGIN IS THE LOCATION OF THE PLATE UNDER A MODEL
   vec2 world_to_screen_algorithm (vec3 &LocalCameraPositions, vec3 &Eye_angles, float &FovAngle, vec3 &WorldEntity) {
      vec2 Screenlocation;   // return


      auto viewMatrix = construct_viewMatrix(LocalCameraPositions, Eye_angles);
      _CachedViewMatrix = viewMatrix;

      vec3 vAxisX, vAxisY, vAxisZ;

      vAxisX = viewMatrix.GetRowAsVector3(0);
      vAxisY = viewMatrix.GetRowAsVector3(1);
      vAxisZ = viewMatrix.GetRowAsVector3(2);

      vec3 vDelta = WorldEntity - LocalCameraPositions;
      vec3 vTransformed = vec3{vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX)};

      if (vTransformed.z < 0.1f)
         return Screenlocation;

      Screenlocation.x = -(vTransformed.x / vTransformed.z * tanf(FovAngle * GetPi() / 360.f));
      Screenlocation.y = (vTransformed.y / vTransformed.z * tanf(FovAngle * GetPi() / 360.f));

      return Screenlocation;
   }


 public:
   ViewMatrix _ref CachedViewMatrix () {
      return _CachedViewMatrix;
   }

   vec2 _ref CachedScreenPos () {
      return _CachedScreenPos;
   }


 protected:
   ViewMatrix _CachedViewMatrix;
   vec2 _CachedScreenPos;
};


struct Box {
   vec2 top_left, top_right, bottom_left, bottom_right;
   vec2 *GetCornerbyArray (u16 corner) {
      return scast< vec2 * >(&top_left) + corner;
   }
};


// gives us translated shapes with scaleFactor
class ModelBoundAreaScalar {
 private:
   const float base_w = 70, base_h = 160;

 public:
   auto GenerateBoxBounds (vec2 &PointOnScreen, float scaleFactor) {
      Box box;
      auto factor = scaleFactor;

      box.top_left = {PointOnScreen.x - (base_w / factor), PointOnScreen.y - (base_h / factor)};
      box.top_right = {PointOnScreen.x + (base_w / factor), PointOnScreen.y - (base_h / factor)};
      box.bottom_left = {PointOnScreen.x - (base_w / factor), PointOnScreen.y + (base_h / factor)};
      box.bottom_right = {PointOnScreen.x + (base_w / factor), PointOnScreen.y + (base_h / factor)};

      return box;
   };

#ifdef CM_API_DEBUG
   noret DrawBox (Box &box, float scaleFactor = 1) {
      gmGL gl;

      // Adjust the vertices by the scale factor
      auto top_left = *box.GetCornerbyArray(0);
      auto top_right = *box.GetCornerbyArray(1);
      auto bottom_left = *box.GetCornerbyArray(2);
      auto bottom_right = *box.GetCornerbyArray(3);

      // Apply scale factor
      auto scale = [scaleFactor] (vec2 &v) {
         v.x /= scaleFactor;
         v.y /= scaleFactor;
      };

      scale(top_left);
      scale(top_right);
      scale(bottom_left);
      scale(bottom_right);

      // Draw the box using the adjusted vertices
      gl.DrawByMode(gmGL::mode::line_loop, [&] {
         gl.VertexApi().VertexV2(
            bottom_left.x, bottom_left.y,     // Bottom-left vertex
            bottom_right.x, bottom_right.y,   // Bottom-right vertex
            top_right.x, top_right.y,         // Top-right vertex
            top_left.x, top_left.y            // Top-left vertex
         );
      });
   }
#endif
};


static bool isInsideCircleMath (vec2 &screen_Point, vec2 circle_Point, float circle_radius, float &out_distanceToCenter) {

   float x = (screen_Point.x - circle_Point.x);
   float y = (screen_Point.y - circle_Point.y);

   float Distance = math.square(x) + math.square(y);

   // and of course finish off with a squared Radius
   if (Distance < math.square(circle_radius)) {

      out_distanceToCenter = Distance;
      return true;
   }
   else {
      out_distanceToCenter = Distance;
      return false;
   }
}

inline bool isPointInsideBox (const vec2 &point, const vec2 &top_left, const vec2 &bottom_right) {
   return (point.x >= top_left.x && point.x <= bottom_right.x && point.y >= top_left.y && point.y <= bottom_right.y);
}
// this is vectors of points that may be inside
inline bool isPointsInsideBox2 (vector< vec2 > Points, Box &box) {
   for (auto point : Points) {

      if (isPointInsideBox(point, box.top_left, box.bottom_right))
         return true;
   }

   return false;
}


static bool isBoxInsideCircle (Box &box, float scaleFactor, vec2 circle_point, float circle_radius, float &distanceToCenter) {


   // Array of box corners
   vec2 corners[4] = {box.top_left, box.top_right, box.bottom_left, box.bottom_right};

   // Check each corner to see if it is inside the circle
   for (int i = 0; i < 4; ++i) {
      float distance_to_circle_center = corners[i].Dist2D(circle_point);
      if (distance_to_circle_center <= circle_radius) {
         distanceToCenter = distance_to_circle_center;
         return true;   // At least one corner is inside the circle
      }
   }


   return false;   // No corners are inside the circle
}
