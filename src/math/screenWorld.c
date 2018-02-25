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

#include "math/screenWorld.h"
#include "math/matrix.h"

void raycastScreenToWorld(S32 mouseX, S32 mouseY, S32 width, S32 height, mat4 *mvp, vec *rayOrigin, vec *rayDir) {
   // Normalize the rayStart and rayEnd -1.0 to 1.0
   vec4 rayStart;
   rayStart.x = (mouseX / width - 0.5f) * 2.0f;
   rayStart.y = (mouseY / height - 0.5f) * 2.0f;
   rayStart.z = -1.0f;
   rayStart.w = 1.0f;

   vec4 rayEnd;
   rayStart.x = (mouseX / width - 0.5f) * 2.0f;
   rayStart.y = (mouseY / height - 0.5f) * 2.0f;
   rayStart.z = 0.0f;
   rayStart.w = 1.0f;

   mat4 invert;
   mat4_invert(&invert, mvp);

   vec4 rayWorldStart, rayWorldEnd;
   mat4_mul_vec4(&rayWorldStart, &invert, &rayStart);
   mat4_mul_vec4(&rayWorldEnd, &invert, &rayEnd);
   vec rwStart = vec3(rayWorldStart.x, rayWorldStart.y, rayWorldStart.z);
   vec rwEnd = vec3(rayWorldEnd.x, rayWorldEnd.y, rayWorldEnd.z);
   vec_scaleeq(&rwStart, 1.0f / rayWorldStart.w);
   vec_scaleeq(&rwEnd, 1.0f / rayWorldEnd.w);

   // Output
   rayOrigin->x = rwStart.x;
   rayOrigin->y = rwStart.y;
   rayOrigin->z = rwStart.z;

   // Output
   vec rayDirWorld;
   vec_sub(&rayDirWorld, &rwStart, &rwEnd);
   vec_norm(rayDir, &rayDirWorld);
}