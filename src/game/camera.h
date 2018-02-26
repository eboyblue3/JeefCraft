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

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "math/frustum.h"

void initCamera();
void getCameraPosition(Vec3 *pos);
void getCurrentViewMatrix(mat4 *mat);
void getCurrentProjMatrix(mat4 *mat);
void setCameraProjMatrix(mat4 mat);
void setCameraPosition(Vec3 pos);
void calculateFreecamViewMatrix(F32 dt);
void getCameraFrustum(Frustum *frustum);

#endif