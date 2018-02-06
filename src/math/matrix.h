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
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------

#ifndef _MATH_MATRIX_H_
#define _MATH_MATRIX_H_

#include <math.h>
#include <stb_vec.h>
#include "base/types.h"

inline void mat4_getPosition(vec *dest, mat4 *src) {
   dest->x = src->m[3].x;
   dest->x = src->m[3].y;
   dest->x = src->m[3].z;
}

inline void mat4_setPosition(mat4 *matrix, vec *pos) {
   matrix->m[3].x = pos->x;
   matrix->m[3].y = pos->y;
   matrix->m[3].z = pos->z;
}

inline void mat4_perspective(mat4 *dest, F32 fov, F32 aspect, F32 zNear, F32 zFar) {
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

inline void mat4_lookAt(mat4 *dest, vec *eye, vec *center, vec *up) {
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

#endif