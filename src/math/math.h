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

#ifndef _MATH_MATH_H_
#define _MATH_MATH_H_

#include <cglm/cglm.h>
#include "base/types.h"

typedef union {
   vec3 vec;

   struct {
      F32 x;
      F32 y;
      F32 z;
   };
} Vec3;

typedef union {
   vec4 vec;

   struct {
      F32 x;
      F32 y;
      F32 z;
      F32 w;
   };
} Vec4;

static inline Vec3 create_vec3(F32 x, F32 y, F32 z) {
   Vec3 v;
   v.x = x;
   v.y = y;
   v.z = z;
   return v;
}

static inline Vec4 create_vec4(F32 x, F32 y, F32 z, F32 w) {
   Vec4 v;
   v.x = x;
   v.y = y;
   v.z = z;
   v.w = w;
   return v;
}

// Pls.
#define glm_vec_len glm_vec_norm

#endif
