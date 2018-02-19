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
void computeFrustum(mat4 *mvp, Frustum *frustum) {
   frustum->planes[FRUSTUM_LEFT].x = mvp->m[0].w + mvp->m[0].x;
   frustum->planes[FRUSTUM_LEFT].y = mvp->m[1].w + mvp->m[1].x;
   frustum->planes[FRUSTUM_LEFT].z = mvp->m[2].w + mvp->m[2].x;
   frustum->planes[FRUSTUM_LEFT].n = mvp->m[3].w + mvp->m[3].x;

   frustum->planes[FRUSTUM_RIGHT].x = mvp->m[0].w - mvp->m[0].x;
   frustum->planes[FRUSTUM_RIGHT].y = mvp->m[1].w - mvp->m[1].x;
   frustum->planes[FRUSTUM_RIGHT].z = mvp->m[2].w - mvp->m[2].x;
   frustum->planes[FRUSTUM_RIGHT].n = mvp->m[3].w - mvp->m[3].x;

   frustum->planes[FRUSTUM_TOP].x = mvp->m[0].w - mvp->m[0].y;
   frustum->planes[FRUSTUM_TOP].y = mvp->m[1].w - mvp->m[1].y;
   frustum->planes[FRUSTUM_TOP].z = mvp->m[2].w - mvp->m[2].y;
   frustum->planes[FRUSTUM_TOP].n = mvp->m[3].w - mvp->m[3].y;

   frustum->planes[FRUSTUM_BOTTOM].x = mvp->m[0].w + mvp->m[0].y;
   frustum->planes[FRUSTUM_BOTTOM].y = mvp->m[1].w + mvp->m[1].y;
   frustum->planes[FRUSTUM_BOTTOM].z = mvp->m[2].w + mvp->m[2].y;
   frustum->planes[FRUSTUM_BOTTOM].n = mvp->m[3].w + mvp->m[3].y;

   frustum->planes[FRUSTUM_NEAR].x = mvp->m[0].w + mvp->m[0].z;
   frustum->planes[FRUSTUM_NEAR].y = mvp->m[1].w + mvp->m[1].z;
   frustum->planes[FRUSTUM_NEAR].z = mvp->m[2].w + mvp->m[2].z;
   frustum->planes[FRUSTUM_NEAR].n = mvp->m[3].w + mvp->m[3].z;

   frustum->planes[FRUSTUM_FAR].x = mvp->m[0].w - mvp->m[0].z;
   frustum->planes[FRUSTUM_FAR].y = mvp->m[1].w - mvp->m[1].z;
   frustum->planes[FRUSTUM_FAR].z = mvp->m[2].w - mvp->m[2].z;
   frustum->planes[FRUSTUM_FAR].n = mvp->m[3].w - mvp->m[3].z;

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
static inline float planeDistance(vec *vert, FrustumPlane *plane) {
   return plane->x * vert->x + plane->y * vert->y + plane->z * vert->z + plane->n;
}

// Culls square box in world-space
// Source code referenced from Ogre Rendering Engine's codebase.
//
// Copyright (c) 2000-2014 Torus Knot Software Ltd
// License: MIT
bool FrustumCullSquareBox(Frustum *frustum, vec *center, float halfExtent) {
   for (S32 i = 0; i < FRUSTUM_LOOP_COUNT; ++i) {
      FrustumPlane *plane = &frustum->planes[i];

      F32 dist = planeDistance(center, plane);
      F32 maxAbsDist = fabsf(plane->x) * halfExtent + fabsf(plane->y) * halfExtent + fabsf(plane->z) * halfExtent;
      if (dist < -maxAbsDist)
         return false;
   }
   return true;
}
