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

#include "math/matrix.h"
#include "game/camera.h"
#include "platform/input.h"

#define CAMERA_SPEED 4.0f
#define MOUSE_SPEED 0.005f
#define PI 3.14159f
#define PI_2 1.570796f

#define PITCH_MIN -PI_2
#define PITCH_MAX (PI_2 - 0.2f)

typedef struct CameraInfo {
   vec position;
   F32 horiziontalAngle;
   F32 verticalAngle;
} CameraInfo;
CameraInfo gCameraInfo;

void initCamera() {
   gCameraInfo.position = vec3(0.0f, 0.0f, 0.0f);
   gCameraInfo.horiziontalAngle = 0.0f;
   gCameraInfo.verticalAngle = -0.45f;
}

void getCameraPosition(vec *pos) {
   pos->x = gCameraInfo.position.x;
   pos->y = gCameraInfo.position.y;
   pos->z = gCameraInfo.position.z;
}

void setCameraPosition(vec *pos) {
   gCameraInfo.position.x = pos->x;
   gCameraInfo.position.y = pos->y;
   gCameraInfo.position.z = pos->z;
}

// Method based on the camera control inside of opengl-tutorial.com. As of [2/4/2018]
// source code for that tutorial is released under the WTFPL version 2.0
void calculateFreecamViewMatrix(mat4 *viewMatrix, F32 delta) {
   delta /= 1000.0f;

   F64 mouseX;
   F64 mouseY;
   inputGetMouseMovementForCurrentFrame(&mouseX, &mouseY);

   gCameraInfo.horiziontalAngle += MOUSE_SPEED * (F32)mouseX;
   gCameraInfo.verticalAngle += MOUSE_SPEED * (F32)mouseY;

   // Clamp pitch
   if (gCameraInfo.verticalAngle < PITCH_MIN)
      gCameraInfo.verticalAngle = PITCH_MIN;
   else if (gCameraInfo.verticalAngle > PITCH_MAX)
      gCameraInfo.verticalAngle = PITCH_MAX;

   // Direction
   vec direction;
   direction.x = cosf(gCameraInfo.verticalAngle) * sinf(gCameraInfo.horiziontalAngle);
   direction.y = sinf(gCameraInfo.verticalAngle);
   direction.z = cosf(gCameraInfo.verticalAngle) * cosf(gCameraInfo.horiziontalAngle);

   // Right vector
   vec right;
   right.x = sinf(gCameraInfo.horiziontalAngle - PI / 2.0f);
   right.y = 0.0f;
   right.z = cosf(gCameraInfo.horiziontalAngle - PI / 2.0f);

   // up
   vec up;
   vec_cross(&up, &right, &direction);

   // Process Movement with key checks.
   if (inputGetKeyStatus(KEY_W) == PRESSED) {
      gCameraInfo.position.x += direction.x * delta * CAMERA_SPEED;
      gCameraInfo.position.y += direction.y * delta * CAMERA_SPEED;
      gCameraInfo.position.z += direction.z * delta * CAMERA_SPEED;
   }
   if (inputGetKeyStatus(KEY_S) == PRESSED) {
      gCameraInfo.position.x -= direction.x * delta * CAMERA_SPEED;
      gCameraInfo.position.y -= direction.y * delta * CAMERA_SPEED;
      gCameraInfo.position.z -= direction.z * delta * CAMERA_SPEED;
   }
   if (inputGetKeyStatus(KEY_D) == PRESSED) {
      gCameraInfo.position.x += right.x * delta * CAMERA_SPEED;
      gCameraInfo.position.y += right.y * delta * CAMERA_SPEED;
      gCameraInfo.position.z += right.z * delta * CAMERA_SPEED;
   }
   if (inputGetKeyStatus(KEY_A) == PRESSED) {
      gCameraInfo.position.x -= right.x * delta * CAMERA_SPEED;
      gCameraInfo.position.y -= right.y * delta * CAMERA_SPEED;
      gCameraInfo.position.z -= right.z * delta * CAMERA_SPEED;
   }

   // Finally calcuate the view matrix.
   vec center;
   vec_add(&center, &gCameraInfo.position, &direction);
   mat4_lookAt(viewMatrix, &gCameraInfo.position, &center, &up);
}