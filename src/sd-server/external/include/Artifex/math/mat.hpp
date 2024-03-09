#pragma once

#include <cmath>

#include "vec.hpp"

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a < b ? b : a)

// Matrix Size Type
typedef uint8_t mat_t;

// Variable Dimension Matrix
template <mat_t R = 4, mat_t C = R, typename T = float> struct mat {
  static_assert(R != 0 && C != 0, "Null-Size Matrices are not supported");

  T data[R][C];

  // Constructors & Destructor

  mat(T v = 0);
  mat(std::vector<std::vector<T>> m);
  template <mat_t inR, mat_t inC, typename inT> mat(mat<inR, inC, inT> m);

  ~mat();

  // Arithmetic Operators

  mat<R, C, T> operator+(mat<R, C, T> m) const;
  mat<R, C, T> operator-(mat<R, C, T> m) const;
  // matrix division is not defined
  template <mat_t inR, mat_t inC, typename inT> mat<MAX(R, inR), MAX(C, inC)> operator*(mat<inR, inC, inT> m) const;

  template <mat_t inD, typename inT> vec<inD, inT> operator*(vec<inD, inT> v) const;

  // Assignment Operators

  template <mat_t inR, mat_t inC, typename inT> void operator=(mat<inR, inC, inT> m);
  template <mat_t inR, mat_t inC, typename inT> void operator*=(mat<inR, inC, inT> m);

  // Relational Operators

  template <mat_t inR, mat_t inC, typename inT> bool operator==(mat<inR, inC, inT> m) const;
  template <mat_t inR, mat_t inC, typename inT> bool operator!=(mat<inR, inC, inT> m) const;

  // Other Operators

  T operator()(mat_t row, mat_t col) const;
  T &operator()(mat_t row, mat_t col);

  // Functions (Instance Methods)

  bool isSquare() const;
  void inverse();

  // Functions (Static)

  static mat<R> identity();

  static mat<4> rotationX(mat_t rad);
  static mat<4> rotationY(mat_t rad);
  static mat<4> rotationZ(mat_t rad);
  static mat<4> rotation(vec<3> rad);

  static mat<4> translation(vec<3> v);
  static mat<4> scale(vec<3> v);

  // Functions (Dimension-Specific)

  static mat<4> perspective(mat_t fovrads, mat_t aspect, mat_t near, mat_t far);

  static mat<4> ortho(mat_t left, mat_t right, mat_t bottom, mat_t top, mat_t fnear, mat_t ffar);

  static mat<4> lookat(vec<3> pos, vec<3> target, vec<3> up = {0.0f, 1.0f, 0.0f});
};

// Constructors & Destructor

template <mat_t R, mat_t C, typename T> mat<R, C, T>::mat(T v) {
  for (mat_t r = 0; r < R; r++)
    for (mat_t c = 0; c < C; c++)
      this->data[r][c] = v;
}

template <mat_t R, mat_t C, typename T> mat<R, C, T>::mat(std::vector<std::vector<T>> m) {
  for (mat_t r = 0; r < R; r++)
    for (mat_t c = 0; c < C; c++)
      this->data[r][c] = r < m.size() && c < m[r].size() ? m[r][c] : 0;
}

template <mat_t R, mat_t C, typename T> template <mat_t inR, mat_t inC, typename inT> mat<R, C, T>::mat(mat<inR, inC, inT> m) {
  *this = m;
}

template <mat_t R, mat_t C, typename T> mat<R, C, T>::~mat() {}

// Arithmetic Operators

template <mat_t R, mat_t C, typename T> mat<R, C, T> mat<R, C, T>::operator+(mat<R, C, T> m) const {
  mat<R, C, T> out;

  for (mat_t r = 0; r < R; r++)
    for (mat_t c = 0; c < C; c++)
      out.data[r][c] = this->data[r][c] + m.data[r][c];

  return out;
}

template <mat_t R, mat_t C, typename T> mat<R, C, T> mat<R, C, T>::operator-(mat<R, C, T> m) const {
  mat<R, C, T> out;

  for (mat_t r = 0; r < R; r++)
    for (mat_t c = 0; c < C; c++)
      out.data[r][c] = this->data[r][c] - m.data[r][c];

  return out;
}

template <mat_t R, mat_t C, typename T>
template <mat_t inR, mat_t inC, typename inT>
mat<MAX(R, inR), MAX(C, inC)> mat<R, C, T>::operator*(mat<inR, inC, inT> m) const {
  mat<MAX(R, inR), MAX(C, inC)> out;

  if (C != inR)
    return out;

  for (mat_t r = 0; r < MAX(R, inR); r++)
    for (mat_t c = 0; c < MAX(C, inC); c++)
      for (mat_t k = 0; k < C; k++)
        out.data[r][c] += this->data[r][k] * m.data[k][c];

  return out;
}

template <mat_t R, mat_t C, typename T>
template <mat_t inD, typename inT>
vec<inD, inT> mat<R, C, T>::operator*(vec<inD, inT> v) const {
  vec<inD, inT> out;

  // TODO

  return out;
}

// Assignment Operators

template <mat_t R, mat_t C, typename T>
template <mat_t inR, mat_t inC, typename inT>
void mat<R, C, T>::operator=(mat<inR, inC, inT> m) {
  for (mat_t r = 0; r < R; r++)
    for (mat_t c = 0; c < C; c++)
      this->data[r][c] = r < inR && c < inC ? m.data[r][c] : 0;
}

template <mat_t R, mat_t C, typename T>
template <mat_t inR, mat_t inC, typename inT>
void mat<R, C, T>::operator*=(mat<inR, inC, inT> m) {
  *this = *this * m;
}

// Relational Operators

template <mat_t R, mat_t C, typename T>
template <mat_t inR, mat_t inC, typename inT>
bool mat<R, C, T>::operator==(mat<inR, inC, inT> m) const {
  for (mat_t r = 0; r < std::min(R, inR); r++)
    for (mat_t c = 0; c < std::min(C, inC); c++)
      if (this->data[r][c] != m.data[r][c])
        return false;
  return true;
}

template <mat_t R, mat_t C, typename T>
template <mat_t inR, mat_t inC, typename inT>
bool mat<R, C, T>::operator!=(mat<inR, inC, inT> m) const {
  for (mat_t r = 0; r < std::min(R, inR); r++)
    for (mat_t c = 0; c < std::min(C, inC); c++)
      if (this->data[r][c] == m.data[r][c])
        return false;
  return true;
}

// Other Operators

template <mat_t R, mat_t C, typename T> T mat<R, C, T>::operator()(mat_t row, mat_t col) const {
  return this->data[row % R][col % C];
}

template <mat_t R, mat_t C, typename T> T &mat<R, C, T>::operator()(mat_t row, mat_t col) {
  return this->data[row % R][col % C];
}

// Functions (Instance Methods)

template <mat_t R, mat_t C, typename T> bool mat<R, C, T>::isSquare() const { return R == C; }

// Functions (Static)

template <mat_t R, mat_t C, typename T> mat<R, R> mat<R, C, T>::identity() {
  mat<R> out;

  for (mat_t rc = 0; rc < MIN(R, C); rc++)
    out.data[rc][rc] = 1;

  return out;
}

template <mat_t R, mat_t C, typename T> mat<4> mat<R, C, T>::rotationX(mat_t rad) {
  mat<4> matrix;
  matrix(0, 0) = 1.0f;
  matrix(1, 1) = cosf(rad);
  matrix(1, 2) = sinf(rad);
  matrix(2, 1) = -sinf(rad);
  matrix(2, 2) = cosf(rad);
  matrix(3, 3) = 1.0f;
  return matrix;
}

template <mat_t R, mat_t C, typename T> mat<4> mat<R, C, T>::rotationY(mat_t rad) {
  mat<4> matrix;
  matrix(0, 0) = cosf(rad);
  matrix(0, 2) = sinf(rad);
  matrix(2, 0) = -sinf(rad);
  matrix(1, 1) = 1.0f;
  matrix(2, 2) = cosf(rad);
  matrix(3, 3) = 1.0f;
  return matrix;
}

template <mat_t R, mat_t C, typename T> mat<4> mat<R, C, T>::rotationZ(mat_t rad) {
  mat<4> matrix;
  matrix(0, 0) = cosf(rad);
  matrix(0, 1) = sinf(rad);
  matrix(1, 0) = -sinf(rad);
  matrix(1, 1) = cosf(rad);
  matrix(2, 2) = 1.0f;
  matrix(3, 3) = 1.0f;
  return matrix;
}

template <mat_t R, mat_t C, typename T> mat<4> mat<R, C, T>::rotation(vec<3> rad) {
  mat<4> matrix, x, y, z;

  x = rotationX(rad->x);
  y = rotationY(rad->y);
  z = rotationZ(rad->z);

  matrix = identity();
  matrix = x * y;
  matrix = matrix * z;

  return matrix;
}

template <mat_t R, mat_t C, typename T> mat<4> translation(vec<3> v) {
  mat<4> out = mat<4>::identity();

  out.data[3][0] = v->x;
  out.data[3][1] = v->y;
  out.data[3][2] = v->z;

  return out;
}

template <mat_t R, mat_t C, typename T> mat<4> scale(vec<3> v) {
  mat<4> out;

  out.data[0][0] = v->x;
  out.data[1][1] = v->y;
  out.data[2][2] = v->z;
  out.data[3][3] = 1.0f;

  return out;
}

// Functions (Dimension-Specific)

template <mat_t R, mat_t C, typename T> mat<4> mat<R, C, T>::perspective(mat_t fovrads, mat_t aspect, mat_t near, mat_t far) {
  mat_t tanHalfFov = tan(fovrads / 2.0f);

  mat<4> out;
  out.data[0][0] = 1.0f / (aspect * tanHalfFov);
  out.data[1][1] = 1.0f / (tanHalfFov);
  out.data[2][2] = -(far + near) / (far - near);
  out.data[2][3] = -1.0f;
  out.data[3][2] = -(2.0f * far * near) / (far - near);
  return out;
}

template <mat_t R, mat_t C, typename T>
mat<4> mat<R, C, T>::ortho(mat_t left, mat_t right, mat_t bottom, mat_t top, mat_t near, mat_t far) {
  mat<4> out = identity();
  out.data[0][0] = 2.0f / (right - left);
  out.data[1][1] = 2.0f / (top - bottom);
  out.data[2][2] = -2.0f / (far - near);
  out.data[3][0] = -(right + left) / (right - left);
  out.data[3][1] = -(top + bottom) / (top - bottom);
  out.data[3][2] = -(far + near) / (far - near);
  return out;
}

template <mat_t R, mat_t C, typename T> mat<4> mat<R, C, T>::lookat(vec<3> pos, vec<3> target, vec<3> up) {
  vec<3> f = (target - pos).normalize();
  vec<3> s = vec<3>::cross(f, up).normalize();
  vec<3> u = vec<3>::cross(s, f);

  mat<4> out = identity();
  out.data[0][0] = s[0];
  out.data[1][0] = s[1];
  out.data[2][0] = s[2];
  out.data[0][1] = u[0];
  out.data[1][1] = u[1];
  out.data[2][1] = u[2];
  out.data[0][2] = -f[0];
  out.data[1][2] = -f[1];
  out.data[2][2] = -f[2];
  out.data[3][0] = -vec<3>::dot(s, pos);
  out.data[3][1] = -vec<3>::dot(u, pos);
  out.data[3][2] = vec<3>::dot(f, pos);
  return out;
}

#undef MIN
#undef MAX