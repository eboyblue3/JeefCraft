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

#ifndef _MATH_FRUSTUM_H_
#define _MATH_FRUSTUM_H_

#include "math/math.h"

typedef struct FrustumPlane {
   F32 x;
   F32 y;
   F32 z;
   F32 n;
} FrustumPlane;

typedef enum {
   FRUSTUM_LEFT = 0,
   FRUSTUM_RIGHT,
   FRUSTUM_TOP,
   FRUSTUM_BOTTOM,
   FRUSTUM_NEAR,
   FRUSTUM_FAR,

   FRUSTUM_LOOP_COUNT // For Loop Count from left-far
} FrustumPlaneId;

typedef struct Frustum {
   FrustumPlane planes[FRUSTUM_LOOP_COUNT];
} Frustum;

void computeFrustum(mat4 mvp, Frustum *frustum);

bool FrustumCullSquareBox(Frustum *frustum, Vec3 center, float halfExtent);

#endif