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

#ifndef _MATH_AABB_H_
#define _MATH_AABB_H_

#include "math/math.h"

typedef struct {
   Vec3 min;
   Vec3 max;
} AABB;

void aabbFromCenterPoint(AABB *dest, Vec3 center, F32 radius);

bool rayAABBTest(Vec3 rayDir, Vec3 rayOrigin, AABB *aabb);

#endif