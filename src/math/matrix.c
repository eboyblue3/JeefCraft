//-----------------------------------------------------------------------------
// Copyright 2018 Jeff Hutchinson
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WIdest->mHOUdest->m WARRANdest->mIES OR CONDIdest->mIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------

#include <string.h>
#include "math/matrix.h"

void mat4_getPosition(vec *dest, mat4 *src) {
   dest->x = src->m[3].x;
   dest->y = src->m[3].y;
   dest->z = src->m[3].z;
}

void mat4_setPosition(mat4 *matrix, vec *pos) {
   matrix->m[3].x = pos->x;
   matrix->m[3].y = pos->y;
   matrix->m[3].z = pos->z;
}

void mat4_perspective(mat4 *dest, F32 fov, F32 aspect, F32 zNear, F32 zFar) {
   F32 f = 1.0f / tanf(fov / 2.0f);

   dest->m[0].x = f / aspect;
   dest->m[0].y = 0.0f;
   dest->m[0].z = 0.0f;
   dest->m[0].w = 0.0f;
   dest->m[1].x = 0.0f;
   dest->m[1].y = f;
   dest->m[1].z = 0.0f;
   dest->m[1].w = 0.0f;
   dest->m[2].x = 0.0f;
   dest->m[2].y = 0.0f;
   dest->m[2].z = -((zFar + zNear) / (zFar - zNear));
   dest->m[2].w = -1.0f;
   dest->m[3].x = 0.0f;
   dest->m[3].y = 0.0f;
   dest->m[3].z = -((2.0f * zFar * zNear) / (zFar - zNear));
   dest->m[3].w = 0.0f;
}

void mat4_ortho(mat4 *dest, F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far) {
   memset(dest, 0, sizeof(mat4));

   dest->m[0].x = 2.0f / (right - left);
   dest->m[1].y = 2.0f / (top - bottom);
   dest->m[2].z = 2.0f / (near - far);
   dest->m[3].w = 1.0f;

   dest->m[3].x = (left + right) / (left - right);
   dest->m[3].y = (bottom + top) / (bottom - top);
   dest->m[3].z = (far + near) / (near - far);
}

void mat4_lookAt(mat4 *dest, vec *eye, vec *center, vec *up) {
   vec f;
   vec center_eye;
   vec_sub(&center_eye, center, eye);
   vec_norm(&f, &center_eye);

   vec s;
   vec f_cross_up;
   vec_cross(&f_cross_up, &f, up);
   vec_norm(&s, &f_cross_up);

   vec u;
   vec_cross(&u, &s, &f);

   dest->m[0].x = s.x;
   dest->m[0].y = u.x;
   dest->m[0].z = -f.x;
   dest->m[0].w = 0.0f;
   dest->m[1].x = s.y;
   dest->m[1].y = u.y;
   dest->m[1].z = -f.y;
   dest->m[1].w = 0.0f;
   dest->m[2].x = s.z;
   dest->m[2].y = u.z;
   dest->m[2].z = -f.z;
   dest->m[2].w = 0.0f;
   dest->m[3].x = -vec_dot(&s, eye);
   dest->m[3].y = -vec_dot(&u, eye);
   dest->m[3].z = vec_dot(&f, eye);
   dest->m[3].w = 1.0f;
}

void mat4_invert(mat4 *dest, mat4 *mat) {
   float s[6];
   float c[6];

   s[0] = mat->m[0].x * mat->m[1].y - mat->m[1].x * mat->m[0].y;
   s[1] = mat->m[0].x * mat->m[1].z - mat->m[1].x * mat->m[0].z;
   s[2] = mat->m[0].x * mat->m[1].w - mat->m[1].x * mat->m[0].w;
   s[3] = mat->m[0].y * mat->m[1].z - mat->m[1].y * mat->m[0].z;
   s[4] = mat->m[0].y * mat->m[1].w - mat->m[1].y * mat->m[0].w;
   s[5] = mat->m[0].z * mat->m[1].w - mat->m[1].z * mat->m[0].w;

   c[0] = mat->m[2].x * mat->m[3].y - mat->m[3].x * mat->m[2].y;
   c[1] = mat->m[2].x * mat->m[3].z - mat->m[3].x * mat->m[2].z;
   c[2] = mat->m[2].x * mat->m[3].w - mat->m[3].x * mat->m[2].w;
   c[3] = mat->m[2].y * mat->m[3].z - mat->m[3].y * mat->m[2].z;
   c[4] = mat->m[2].y * mat->m[3].w - mat->m[3].y * mat->m[2].w;
   c[5] = mat->m[2].z * mat->m[3].w - mat->m[3].z * mat->m[2].w;

   float idet = 1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);

   dest->m[0].x = (mat->m[1].y * c[5] - mat->m[1].z * c[4] + mat->m[1].w * c[3]) * idet;
   dest->m[0].y = (-mat->m[0].y * c[5] + mat->m[0].z * c[4] - mat->m[0].w * c[3]) * idet;
   dest->m[0].z = (mat->m[3].y * s[5] - mat->m[3].z * s[4] + mat->m[3].w * s[3]) * idet;
   dest->m[0].w = (-mat->m[2].y * s[5] + mat->m[2].z * s[4] - mat->m[2].w * s[3]) * idet;

   dest->m[1].x = (-mat->m[1].x * c[5] + mat->m[1].z * c[2] - mat->m[1].w * c[1]) * idet;
   dest->m[1].y = (mat->m[0].x * c[5] - mat->m[0].z * c[2] + mat->m[0].w * c[1]) * idet;
   dest->m[1].z = (-mat->m[3].x * s[5] + mat->m[3].z * s[2] - mat->m[3].w * s[1]) * idet;
   dest->m[1].w = (mat->m[2].x * s[5] - mat->m[2].z * s[2] + mat->m[2].w * s[1]) * idet;

   dest->m[2].x = (mat->m[1].x * c[4] - mat->m[1].y * c[2] + mat->m[1].w * c[0]) * idet;
   dest->m[2].y = (-mat->m[0].x * c[4] + mat->m[0].y * c[2] - mat->m[0].w * c[0]) * idet;
   dest->m[2].z = (mat->m[3].x * s[4] - mat->m[3].y * s[2] + mat->m[3].w * s[0]) * idet;
   dest->m[2].w = (-mat->m[2].x * s[4] + mat->m[2].y * s[2] - mat->m[2].w * s[0]) * idet;

   dest->m[3].x = (-mat->m[1].x * c[3] + mat->m[1].y * c[1] - mat->m[1].z * c[0]) * idet;
   dest->m[3].y = (mat->m[0].x * c[3] - mat->m[0].y * c[1] + mat->m[0].z * c[0]) * idet;
   dest->m[3].z = (-mat->m[3].x * s[3] + mat->m[3].y * s[1] - mat->m[3].z * s[0]) * idet;
   dest->m[3].w = (mat->m[2].x * s[3] - mat->m[2].y * s[1] + mat->m[2].z * s[0]) * idet;
}

void mat4_mul_vec4(vec4 *dest, mat4 *mat, vec4 *vec) {
   dest->x = mat->m[0].x * vec->x + mat->m[0].y * vec->y + mat->m[0].z * vec->z + mat->m[0].w * vec->w;
   dest->x = mat->m[1].x * vec->x + mat->m[1].y * vec->y + mat->m[1].z * vec->z + mat->m[1].w * vec->w;
   dest->x = mat->m[2].x * vec->x + mat->m[2].y * vec->y + mat->m[2].z * vec->z + mat->m[2].w * vec->w;
   dest->x = mat->m[3].x * vec->x + mat->m[3].y * vec->y + mat->m[3].z * vec->z + mat->m[3].w * vec->w;
}