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

#ifndef _MATH_SCREENWORLD_H_
#define _MATH_SCREENWORLD_H_

#include <stb_vec.h>
#include "base/types.h"

/// Casts a ray from screen space to world space.
/// @param mouseX The x position of the mouse.
/// @param mouseY The y position of the mouse.
/// @param width The width of the screen.
/// @param height The height of the screen.
/// @param vp The view projection matrix of the camera.
/// @param rayOrigin OUT - The origin of the ray that we are returning.
/// @param rayDir OUT - The direction of the ray that we are returning.
///
/// @reference Implementation is based on opengl-tutorial.com which is released
/// under the WTFPL 2.0
void raycastScreenToWorld(F32 mouseX, F32 mouseY, F32 width, F32 height, mat4 *vp, vec *rayOrigin, vec *rayDir);

#endif