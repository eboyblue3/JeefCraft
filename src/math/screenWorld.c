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

void raycastScreenToWorld(F32 mouseX, F32 mouseY, F32 width, F32 height, mat4 vp, Vec3 *rayOrigin, Vec3 *rayDir) {
   // Normalize the rayStart and rayEnd -1.0 to 1.0
   Vec4 rayStart;
   rayStart.x = (mouseX / width - 0.5f) * 2.0f;
   rayStart.y = (mouseY / height - 0.5f) * 2.0f;
   rayStart.z = -1.0f;
   rayStart.w = 1.0f;

   Vec4 rayEnd;
   rayEnd.x = (mouseX / width - 0.5f) * 2.0f;
   rayEnd.y = (mouseY / height - 0.5f) * 2.0f;
   rayEnd.z = 0.0f;
   rayEnd.w = 1.0f;

   mat4 invert;
   glm_mat4_inv(vp, invert);

   Vec4 rayWorldStart, rayWorldEnd;
   glm_mat4_mulv(invert, rayStart.vec, rayWorldStart.vec);
   glm_mat4_mulv(invert, rayEnd.vec, rayWorldEnd.vec);

   Vec3 rwStart, rwEnd;
   glm_vec3(rayWorldStart.vec, rwStart.vec);
   glm_vec3(rayWorldEnd.vec, rwEnd.vec);

   glm_vec_scale(rwStart.vec, 1.0f / rayWorldStart.w, rwStart.vec);
   glm_vec_scale(rwEnd.vec, 1.0f / rayWorldEnd.w, rwEnd.vec);

   // Output
   rayOrigin->x = rwStart.x;
   rayOrigin->y = rwStart.y;
   rayOrigin->z = rwStart.z;

   // Output
   glm_vec_sub(rwStart.vec, rwEnd.vec, rayDir->vec);
   glm_vec_norm(rayDir->vec);
}