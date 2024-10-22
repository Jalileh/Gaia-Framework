#pragma once

typedef struct vec3_3d {
   float x;
   float y;
   float z;

} AVEC3, *PAVEC3, POS3, *PPOS3;
typedef struct vec2_3d {
   float x;
   float y;
} AVEC2, *PAVEC2, POS2, *PPOS2;
typedef struct Dvec2_3d {
   double x, y, z;
} DAVEC2, DPOS2, *DPPOS2;
typedef struct vec6_3d {
   float x;
   float y;
   float z;
   float w;
   float d;
   float b;
} AVEC6, POS6, *PPOS6;

#define M_PI 3.14159265358979323846f

namespace c3d {
   POS2 TranslatePosition (POS3 TargetObject, POS3 SourceObject,
                           POS3 SourceRotation, float FOV, AVEC2 Magic_Custom,
                           bool &OUTSIDE_VIEW);
   POS2 GetPC_Resolution ();
   float GetAspectRatio (POS2 Resolution);
   void BoxDraw (AVEC2 Pos, float width, float height, float color[3]);
   float twoRelativeDist (POS3 *object1, POS3 *object2);
   float scaleByDistance (POS3 *dst1, POS3 *dist2,
                          float optional_Scalor = 180.f);
   void LineDraw (POS2 *screen, float Scale);
   void HPBAR (POS2 *Screen, float health, float scale);
} // namespace c3d

#define _CD3_WIDTH 0
#define _CD3_HEIGHT 1

void setnextspace (float gap);

struct vec2 {
   float x, y;
};

float inline ToMeters (float x) { return x / 39.62f; }

struct Vector3 {
   float x, y, z;
   Vector3 operator- (Vector3 ape) { return {x - ape.x, y - ape.y, z - ape.z}; }

   Vector3 operator+ (Vector3 ape) { return {x + ape.x, y + ape.y, z + ape.z}; }

   Vector3 operator* (float ape) { return {x * ape, y * ape, z * ape}; }

   Vector3 operator/ (float ape) { return {x / ape, y / ape, z / ape}; }

   Vector3 operator/= (float ape) {
      x /= ape;
      y /= ape;
      z /= ape;

      return *this;
   }

   Vector3 operator+= (Vector3 ape) {
      return {x += ape.x, y += ape.y, z += ape.z};
   }

   Vector3 operator-= (Vector3 ape) {
      return {x -= ape.x, y -= ape.y, z -= ape.z};
   }

   float Length () { return sqrt ((x * x) + (y * y) + (z * z)); }

   float Length2D () { return sqrt ((x * x) + (y * y)); }

   float DistTo (Vector3 ape) { return (*this - ape).Length (); }

   float Dist2D (Vector3 ape) { return (*this - ape).Length2D (); }

   float Dot (Vector3 &v) { return x * v.x + y * v.y + z * v.z; }
};
struct Vector4 {
   float w, x, y, z;
};
struct FMinimalViewInfo {
   Vector3 Location;
   Vector3 Rotation;
   float FOV;
};

#include <d3d9.h>

typedef struct vector_3_cd3 {
   float x;
   float y;
   float z;
} avec3_cd3;
typedef struct vector_2_cd3 {
   float x;
   float y;
} avec2_cd3;

class Prediction {
 public:
   avec2_cd3 aimAngle3D (pvoid Source_from, pvoid To_object);
   void normalize (float &x);

 private:
   avec3_cd3 Uorigin3d;
   avec3_cd3 Uorigin2d;

 private:
   avec2_cd3 Pangle3D ();
   avec2_cd3 Pangle2D ();
};
