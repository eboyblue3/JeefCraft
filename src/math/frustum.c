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

#include <math.h>
#include "math/frustum.h"

// Code taken to compute the frustum is from the famous
// paper by Gil Gribb and Klaus Hartmann that a lot of engines seem to use.
// Reference: http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
//
// Note: This code assumes OpenGL model for now. If "D3D space" is needed, we will
// need to implement it. Note that element access to the combo matrix is in column major order.
void computeFrustum(mat4 mvp, Frustum *frustum) {
   frustum->planes[FRUSTUM_LEFT].x = mvp[0][3] + mvp[0][0];
   frustum->planes[FRUSTUM_LEFT].y = mvp[1][3] + mvp[1][0];
   frustum->planes[FRUSTUM_LEFT].z = mvp[2][3] + mvp[2][0];
   frustum->planes[FRUSTUM_LEFT].n = mvp[3][3] + mvp[3][0];

   frustum->planes[FRUSTUM_RIGHT].x = mvp[0][3] - mvp[0][0];
   frustum->planes[FRUSTUM_RIGHT].y = mvp[1][3] - mvp[1][0];
   frustum->planes[FRUSTUM_RIGHT].z = mvp[2][3] - mvp[2][0];
   frustum->planes[FRUSTUM_RIGHT].n = mvp[3][3] - mvp[3][0];

   frustum->planes[FRUSTUM_TOP].x = mvp[0][3] - mvp[0][1];
   frustum->planes[FRUSTUM_TOP].y = mvp[1][3] - mvp[1][1];
   frustum->planes[FRUSTUM_TOP].z = mvp[2][3] - mvp[2][1];
   frustum->planes[FRUSTUM_TOP].n = mvp[3][3] - mvp[3][1];

   frustum->planes[FRUSTUM_BOTTOM].x = mvp[0][3] + mvp[0][1];
   frustum->planes[FRUSTUM_BOTTOM].y = mvp[1][3] + mvp[1][1];
   frustum->planes[FRUSTUM_BOTTOM].z = mvp[2][3] + mvp[2][1];
   frustum->planes[FRUSTUM_BOTTOM].n = mvp[3][3] + mvp[3][1];

   frustum->planes[FRUSTUM_NEAR].x = mvp[0][3] + mvp[0][2];
   frustum->planes[FRUSTUM_NEAR].y = mvp[1][3] + mvp[1][2];
   frustum->planes[FRUSTUM_NEAR].z = mvp[2][3] + mvp[2][2];
   frustum->planes[FRUSTUM_NEAR].n = mvp[3][3] + mvp[3][2];

   frustum->planes[FRUSTUM_FAR].x = mvp[0][3] - mvp[0][2];
   frustum->planes[FRUSTUM_FAR].y = mvp[1][3] - mvp[1][2];
   frustum->planes[FRUSTUM_FAR].z = mvp[2][3] - mvp[2][2];
   frustum->planes[FRUSTUM_FAR].n = mvp[3][3] - mvp[3][2];

   // Normalize.
   for (S32 i = 0; i < FRUSTUM_LOOP_COUNT; ++i) {
      FrustumPlane *plane = &frustum->planes[i];
      F32 mag = sqrtf(plane->x * plane->x + plane->y * plane->y + plane->z * plane->z);
      plane->x /= mag;
      plane->y /= mag;
      plane->z /= mag;
      plane->n /= mag;
   }
}

// Plane Distance function.
// Source code referenced from Ogre Rendering Engine's codebase.
//
// Copyright (c) 2000-2014 Torus Knot Software Ltd
// License: MIT
static inline float planeDistance(Vec3 vert, FrustumPlane *plane) {
   return plane->x * vert.x + plane->y * vert.y + plane->z * vert.z + plane->n;
}

// Culls square box in world-space
// Source code referenced from Ogre Rendering Engine's codebase.
//
// Copyright (c) 2000-2014 Torus Knot Software Ltd
// License: MIT
bool FrustumCullSquareBox(Frustum *frustum, Vec3 center, float halfExtent) {
   for (S32 i = 0; i < FRUSTUM_LOOP_COUNT; ++i) {
      FrustumPlane *plane = &frustum->planes[i];

      F32 dist = planeDistance(center, plane);
      F32 maxAbsDist = fabsf(plane->x) * halfExtent + fabsf(plane->y) * halfExtent + fabsf(plane->z) * halfExtent;
      if (dist < -maxAbsDist)
         return false;
   }
   return true;
}
