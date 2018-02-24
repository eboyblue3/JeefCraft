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

#ifndef _MATH_MATRIX_H_
#define _MATH_MATRIX_H_

#include <math.h>
#include <stb_vec.h>
#include "base/types.h"

void mat4_getPosition(vec *dest, mat4 *src);

void mat4_setPosition(mat4 *matrix, vec *pos);

void mat4_perspective(mat4 *dest, F32 fov, F32 aspect, F32 zNear, F32 zFar);

void mat4_ortho(mat4 *dest, F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far);

void mat4_lookAt(mat4 *dest, vec *eye, vec *center, vec *up);

/// Reference is based off of datenwolf's linmath library released under WTFPL 2.0
void mat4_invert(mat4 *dest, mat4 *mat);

#endif