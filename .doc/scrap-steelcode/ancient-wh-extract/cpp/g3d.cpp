using namespace std;
#include "c3d.h"
#include <Windows.h>

#define cd3_debug 1

#if cd3_debug == 1
#   define cd3log(content) std::cout << content << std::endl;
#else
#   define cd3log(content)
#endif

avec2_cd3 Prediction::aimAngle3D(pvoid Source_from, pvoid To_object) {
   avec3_cd3 *from = recast< avec3_cd3 * >(Source_from);
   avec3_cd3 *object = recast< avec3_cd3 * >(To_object);

   Uorigin3d = {from->x - object->x, from->y - object->y, from->z - object->z};
   return Pangle3D();
}
avec2_cd3 Prediction::Pangle3D() {
   float hypnotize =
      sqrtf((Uorigin3d.x * Uorigin3d.x) + (Uorigin3d.y * Uorigin3d.y) + (Uorigin3d.z * Uorigin3d.z));
   auto Yaw = atanf(Uorigin3d.y / Uorigin3d.x);
   Yaw = Yaw * 180 / M_PI;
   if (Uorigin3d.x > 0) {
      Yaw += -180;
   }
   return avec2_cd3{Yaw, hypnotize};
}

void Prediction::normalize(float &x) {
   if (x > 0) {
      x += -180;
   }
}

POS2 c3d::GetPC_Resolution() {
   return {(float)GetSystemMetrics(_CD3_WIDTH), (float)GetSystemMetrics(_CD3_HEIGHT)};
}
float c3d::GetAspectRatio(POS2 Resolution) {
   return (Resolution.x / Resolution.y);
}

void _BoxDraw (AVEC2 Box, float width, float height, float align_center) {
   // make the linewidth a bit thicker
   glLineWidth(2.0f);
   glBegin(GL_LINE_LOOP);
   glVertex2f(Box.x - width, Box.y - height);   // Bottom-left vertex
   glVertex2f(Box.x + width, Box.y - height);   // Bottom-right vertex
   glVertex2f(Box.x + width, Box.y + height);   // Top-right vertex
   glVertex2f(Box.x - width, Box.y + height);   // Top-right vertex
   glEnd();
   glLineWidth(1.0f);
}
void c3d::LineDraw(POS2 *Screen, float Scale) {
   glBegin(GL_LINE_LOOP);

   glVertex2f(0.f, -1.f);
   glVertex2f(Screen->x, Screen->y - (0.758f / Scale));
   glEnd();
}
void c3d::HPBAR(POS2 *Screen, float health, float scale) {
   float inc_hp = 0.3f / scale;
   float stat_hp = 0.375f / scale;
   float stat_hpy = 0.01f / scale;

   POS2 screen = {Screen->x + (stat_hpy * 20.83f), Screen->y};

   glColor3f(0.8f, 0.0f, 0.0f);
   glBegin(GL_QUADS);
   glVertex2f(screen.x - (stat_hpy * 2), screen.y - (stat_hp * 2));
   glVertex2f(screen.x - (stat_hpy * 2), screen.y + (stat_hp * 2));
   glVertex2f(screen.x + (stat_hpy * 2), screen.y + (stat_hp * 2));
   glVertex2f(screen.x + (stat_hpy * 2), screen.y - (stat_hp * 2));
   glEnd();

   float greenHeight = (2 * stat_hp) * (health / 100.0f);

   glColor3f(0.0f, 1.0f, 0.0f);
   glBegin(GL_QUADS);
   glVertex2f(screen.x - (stat_hpy * 2), screen.y - (stat_hp * 2));
   glVertex2f(screen.x - (stat_hpy * 2), screen.y - (stat_hp * 2) + greenHeight * 2);
   glVertex2f(screen.x + (stat_hpy * 2), screen.y - (stat_hp * 2) + greenHeight * 2);
   glVertex2f(screen.x + (stat_hpy * 2), screen.y - (stat_hp * 2));
   glEnd();
}

void c3d::BoxDraw(AVEC2 Pos, float width, float height, float color[3]) {
   if (width == 0.0f) {
      width = 0.05f;
   }
   if (height == 0.0f)
      height = 0.09f;

   glColor3f(color[0], color[1], color[2]);
   _BoxDraw(Pos, width, height, 0.f);
}

float c3d::twoRelativeDist(POS3 *object1, POS3 *object2) {
   float sum = sqrtf(powf(object1->x - object2->x, 2) + powf(object1->y - object2->y, 2) + powf(object2->z - object2->z, 2));
   return sum;
}
float c3d::scaleByDistance(POS3 *dst1, POS3 *dist2, float optional_Scalor) {
   return {(c3d::twoRelativeDist(dst1, dist2)) / optional_Scalor};
}
#include <d3d9.h>

bool WorldToScreenX (Vector3 WorldLocation, FMinimalViewInfo CameraCacheL, POS2 &Screenlocation, AVEC2 Magic_Custom);

POS2 c3d::TranslatePosition(POS3 TargetObject, POS3 SourceObject, POS3 SourceRotation, float FOV, AVEC2 Magic_Custom, bool &OUTSIDE_VIEW) {
   POS2 Result = {0};
   FMinimalViewInfo Form;
   Form.Location = {SourceObject.x, SourceObject.y, SourceObject.z};
   Form.Rotation = {SourceRotation.x, SourceRotation.y, SourceRotation.z};
   Form.FOV = FOV;
   Vector3 wlc = {TargetObject.x, TargetObject.y, TargetObject.z};

   if (!WorldToScreenX(wlc, Form, Result, Magic_Custom)) {
      OUTSIDE_VIEW = false;
      return POS2{0};
   }
   else {
      OUTSIDE_VIEW = true;
      return Result;
   }
}
float gap_ = 0.0f;
void setnextspace (float gap) { gap_ = gap; }

D3DMATRIX Matrix (Vector3 rot, Vector3 origin);

#define MAGIC_TRANSFORMATION_NUMBER 0.5f   //?! WTF WORKS LMAO
bool WorldToScreenX (Vector3 WorldLocation, FMinimalViewInfo CameraCacheL, POS2 &Screenlocation, AVEC2 Magic_Custom) {
   if (!Magic_Custom.x) {
      Magic_Custom.x = MAGIC_TRANSFORMATION_NUMBER;
      Magic_Custom.y = MAGIC_TRANSFORMATION_NUMBER;
   }
   auto POV = CameraCacheL;
   Vector3 Rotation = POV.Rotation;
   D3DMATRIX tempMatrix = Matrix(Rotation, Rotation);

   Vector3 vAxisX, vAxisY, vAxisZ;

   vAxisX = Vector3{tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]};
   vAxisY = Vector3{tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]};
   vAxisZ = Vector3{tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]};

   Vector3 vDelta = WorldLocation - POV.Location;
   Vector3 vTransformed =
      Vector3{vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX)};

   if (vTransformed.z < 1.f)
      return false;

   float FovAngle = POV.FOV;

   // ?!  0.5 MAGIC NUMBER WTF? DONT EVER TRANSFORM WITH HEIGHT AND BASE, BREAKS
   // EVERYTHING

   Screenlocation.x = vTransformed.x *
                      (Magic_Custom.x / tanf(FovAngle * (float)M_PI / 360.f)) /
                      vTransformed.z;
   Screenlocation.y = vTransformed.y *
                      (Magic_Custom.y / tanf(FovAngle * (float)M_PI / 360.f)) /
                      vTransformed.z;

   return true;
}
