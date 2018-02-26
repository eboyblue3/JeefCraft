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

void screenRayToWorld(mat4 viewMatrix, Vec3 *rayOrigin, Vec4 *rayDirection) {
   Vec4 forward = { { 0, 0, -1, 0 } };

   mat4 inverse_view;
   glm_mat4_inv(viewMatrix, inverse_view);

   rayOrigin->x = inverse_view[3][0];
   rayOrigin->y = inverse_view[3][1];
   rayOrigin->z = inverse_view[3][2];

   glm_mat4_mulv(inverse_view, forward.vec, rayDirection->vec);
}