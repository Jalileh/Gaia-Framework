#pragma once
#include <math.h>

#define _mvar_pi 3.14159265358979323846f
inline const float GetPi () { return _mvar_pi; };

#define PI GetPi()
#define M_PI PI

class Math {
 public:
   const float pi = _mvar_pi;

   float ToDegrees (float value) {
      return value * 180 / PI;
   }

   float ToRadians (float value) {
      return value * PI / 180;
   }

   // the unknown angle of a triangle
   float Hypotenuse (float x, float y) {
      return std::sqrt(x * x + y * y);
   }

   // Best Resolver for vec maths
   float Atan (float a) {
      return atan(a);
   }
   float Atan2 (float a, float b) {
      return atan2(a, b);
   }


   float _cosf (float a) {
      return cosf(a);
   }

   auto square (float squaring_num) {
      return squaring_num * squaring_num;
   }
   // this can be used to i.e normalie (-180/180 to 0-360 and so on)
   auto Clamp (float value, float range) {
      return value + range;
   }
};
tag_global static Math math;

template < typename T >
class t_vec2 {
 public:
   T x;
   T y;

   t_vec2() : x(0.f), y(0.f) {}
   t_vec2(T x, T y) : x(x), y(y) {}

   template < typename other_type >
   t_vec2(t_vec2< other_type > other) : x(T(other.x)), y(T(other.y)) {}

   t_vec2 operator+(const t_vec2 &other) const {
      return t_vec2(x + other.x, y + other.y);
   }

   t_vec2 operator-(const t_vec2 &other) const {
      return t_vec2(x - other.x, y - other.y);
   }

   t_vec2 operator*(T scalar) const {
      return t_vec2(x * scalar, y * scalar);
   }

   t_vec2 operator/(T scalar) const {
      if (scalar == 0) {
         throw std::invalid_argument("Division by zero.");
      }
      return t_vec2(x / scalar, y / scalar);
   }

   bool operator==(const t_vec2 &other) const {
      return (x == other.x) && (y == other.y);
   }

   bool operator!=(const t_vec2 &other) const {
      return !(*this == other);
   }

   bool operator<(const t_vec2 &other) const {
      return hypotenuse() < other.hypotenuse();
   }

   bool operator<=(const t_vec2 &other) const {
      return hypotenuse() <= other.hypotenuse();
   }

   bool operator>(const t_vec2 &other) const {
      return hypotenuse() > other.hypotenuse();
   }

   bool operator>=(const t_vec2 &other) const {
      return hypotenuse() >= other.hypotenuse();
   }

   T hypotenuse () const {
      return std::sqrt(x * x + y * y);
   }


   /*
    * This function calculates the 2D length of a vector by considering only the x and y components, ignoring the z component.
    * This can be visualized as the length of the projection of the 3D vector onto the xy-plane.
    * */
   T Length2D () const { return sqrt((x * x) + (y * y)); }


   /*    You might use this function when you need to find out how far apart two points are in a flat plane,
    *    such as measuring the distance between two locations on a map, ignoring altitude. */
   T Dist2D (t_vec2< T > other) const { return (*this - other).Length2D(); }

   /*
      This function calculates the dot product of the current vector and another vector other.

      The dot product is a way of combining two vectors that gives a single number.
      It's calculated by multiplying the corresponding components of the vectors and then adding those products together.
    *
    * You might use this function when you want to determine how similar two directions are.
    * The dot product is particularly useful in computer graphics for lighting calculations,
    * as it helps to determine the angle between the light source and a surface.
  */
   T Dot (t_vec2< T > &other) const { return x * other.x + y * other.y; }


   t_vec2< T > Radian () {
      return {x * PI / 180, y * PI / 180};
   }

   t_vec2< T > Angle () {
      return {x * 180 / PI, y * 180 / PI};
   }
};

template < typename T >
class t_vec3 {
 public:
   T x;
   T y;
   T z;

   t_vec3() : x(0.f), y(0.f), z(0.f) {}


   t_vec3(T x, T y, T z) : x(x), y(y), z(z) {}

   t_vec3 operator+(const t_vec3 &other) const {
      return t_vec3(x + other.x, y + other.y, z + other.z);
   }

   t_vec3 operator-(const t_vec3 &other) const {
      return t_vec3(x - other.x, y - other.y, z - other.z);
   }

   t_vec3 operator*(T scalar) const {
      return t_vec3(x * scalar, y * scalar, z * scalar);
   }


   t_vec3 operator/(T scalar) const {
      if (scalar == 0) {
         throw std::invalid_argument("Division by zero.");
      }
      return t_vec3(x / scalar, y / scalar, z / scalar);
   }

   bool operator==(const t_vec3 &other) const {
      return (x == other.x) && (y == other.y) && (z == other.z);
   }

   bool operator!=(const t_vec3 &other) const {
      return !(*this == other);
   }

   bool operator<(const t_vec3 &other) const {
      return hypotenuse() < other.hypotenuse();
   }

   bool operator<=(const t_vec3 &other) const {
      return hypotenuse() <= other.hypotenuse();
   }

   bool operator>(const t_vec3 &other) const {
      return hypotenuse() > other.hypotenuse();
   }

   bool operator>=(const t_vec3 &other) const {
      return hypotenuse() >= other.hypotenuse();
   }

   T hypotenuse () const {
      return std::sqrt(x * x + y * y);
   }

   bool NormalizeVector () noexcept {
      if (x != x || y != y || z != z)
         return false;

      if (x > 180)
         x -= 360.f;
      if (x < -180)
         x += 360.f;
      if (y > 180.f)
         y -= 360.f;
      if (y < -180.f)
         y += 360.f;

      return x >= -180.f && x <= 180.f && y >= -180.f && y <= 180.f;
   }

   bool ClampAngle () noexcept {
      if (x > 89.f)
         x = 89.f;
      if (x < -89.f)
         x = -89.f;
      z = 0;

      return x >= -89.f && x <= 89.f && y >= -180.f && y <= 180.f && z == 0.f;
   }

   /*
      This function calculates the 3D length (also called the hypotenuse) of a vector.
      Imagine a vector as an arrow pointing from the origin (0, 0, 0)
      to a point (x, y, z) in 3D space. The length of this arrow is the distance from the origin to the point.
   */
   T Length () const { return sqrt((x * x) + (y * y) + (z * z)); }
   /*
    * This function calculates the 2D length of a vector by considering only the x and y components, ignoring the z component.
    * This can be visualized as the length of the projection of the 3D vector onto the xy-plane.
    * */
   T Length2D () const { return sqrt((x * x) + (y * y)); }

   /* This function calculates the distance between the current vector and another vector other in 3D space.
    * It subtracts the coordinates of the other vector from the current vector,
    * and then calculates the length of the resulting vector.
    *
    * You might use this function when you need to find out how far apart two points are in 3D space,
    * such as measuring the distance between two objects in a game or simulation.
    * */
   T DistTo (t_vec3< T > other) const { return (*this - other).Length(); }

   /*    You might use this function when you need to find out how far apart two points are in a flat plane,
    *    such as measuring the distance between two locations on a map, ignoring altitude. */
   T Dist2D (t_vec3< T > other) const { return (*this - other).Length2D(); }

   /*
      This function calculates the dot product of the current vector and another vector other.

      The dot product is a way of combining two vectors that gives a single number.
      It's calculated by multiplying the corresponding components of the vectors and then adding those products together.
    *
    * You might use this function when you want to determine how similar two directions are.
    * The dot product is particularly useful in computer graphics for lighting calculations,
    * as it helps to determine the angle between the light source and a surface.
  */

   T Dot (t_vec3< T > &other) const { return x * other.x + y * other.y + z * other.z; }


   t_vec3< T > Radian () {
      return {x * PI / 180, y * PI / 180, z * PI / 180};
   }


   t_vec3< T > Angle () {
      return {x * 180 / PI, y * 180 / PI, z * 180 / PI};
   }
};

template < typename T >
class t_vec4 {
 public:
   T x;
   T y;
   T z;
   T w;

   t_vec4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
   t_vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

   t_vec4 operator+(const t_vec4 &other) const {
      return t_vec4(x + other.x, y + other.y, z + other.z, w + other.w);
   }

   t_vec4 operator-(const t_vec4 &other) const {
      return t_vec4(x - other.x, y - other.y, z - other.z, w - other.w);
   }


   t_vec4 operator*(T scalar) const {
      return t_vec4(x * scalar, y * scalar, z * scalar, w * scalar);
   }

   t_vec4 operator/(T scalar) const {
      if (scalar == 0) {
         throw std::invalid_argument("Division by zero.");
      }
      return t_vec4(x / scalar, y / scalar, z / scalar, w / scalar);
   }

   bool operator==(const t_vec4 &other) const {
      return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
   }

   bool operator!=(const t_vec4 &other) const {
      return !(*this == other);
   }

   bool operator<(const t_vec4 &other) const {
      return hypotenuse() < other.hypotenuse();
   }

   bool operator<=(const t_vec4 &other) const {
      return hypotenuse() <= other.hypotenuse();
   }

   bool operator>(const t_vec4 &other) const {
      return hypotenuse() > other.hypotenuse();
   }

   bool operator>=(const t_vec4 &other) const {
      return hypotenuse() >= other.hypotenuse();
   }

   T hypotenuse () const {
      return std::sqrt(x * x + y * y);
   }


   t_vec4< T > m_Radians () {
      return {x * PI / 180, y * PI / 180, z * PI / 180, w * PI / 180};
   }


   t_vec4< T > m_Degrees () {
      return {x * 180 / PI, y * 180 / PI, z * 180 / PI, w * 180 / PI};
   }

   float ToDegrees (float value) {
      return value * 180 / PI;
   }

   float ToRadians (float value) {
      return value * PI / 180;
   }
};


template < typename T >
class t_matrix_4x4 {
 public:
   T m[4][4] = {0.0};

 public:
   // Constructors
   t_matrix_4x4() {
   }

   t_matrix_4x4(const t_matrix_4x4< T > &other) {
      for (int i = 0; i < 4; ++i) {
         for (int j = 0; j < 4; ++j) {
            m[i][j] = other.m[i][j];
         }
      }
   }

   t_matrix_4x4(T *initial_value) {
      for (int i = 0; i < 4; ++i) {
         for (int j = 0; j < 4; ++j) {
            m[i][j] = initial_value[i * 4 + j];
         }
      }
   }

   // Overloaded operator[] to access a row
   T *operator[](int row) {
      if (row >= 0 && row < 4) {
         return m[row];
      }
      else {
         throw std::out_of_range("Matrix row index out of range");
      }
   }

   t_vec4< T > GetRowAsVector (int row) {
      if (row >= 0 && row < 4) {
         return t_vec4< T >(m[row][0], m[row][1], m[row][2], m[row][3]);
      }
      else {
         throw std::out_of_range("Matrix row index out of range");
      }
   }
   t_vec3< T > GetRowAsVector3 (int row) {
      if (row >= 0 && row < 4) {
         return t_vec3< T >(m[row][0], m[row][1], m[row][2]);
      }
      else {
         throw std::out_of_range("Matrix row index out of range");
      }
   }

   const T *operator[](int row) const {
      if (row >= 0 && row < 4) {
         return m[row];
      }
      else {
         throw std::out_of_range("Matrix row index out of range");
      }
   }
};

using baseType = float;

using ViewMatrix = t_matrix_4x4< float >;
using RotationMatrix = t_matrix_4x4< float >;

using ViewMatrix_Percise = t_matrix_4x4< double >;

using vec2 = t_vec2< baseType >;
using vec3 = t_vec3< baseType >;
using vec4 = t_vec4< baseType >;
