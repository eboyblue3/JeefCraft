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

#include <stdio.h>
#include <string.h>
#include <stb_vec.h>
#include <open-simplex-noise.h>
#include "base/types.h"
#include "platform/window.h"
#include "platform/platform.h"
#include "platform/input.h"
#include "graphics/shader.h"
#include "math/matrix.h"
#include "game/camera.h"

#include <GL/glew.h>

// Taken from std_voxel_render.h, from the public domain
static F32 cubes[6][4][4] = {
   { { 1,0,1,0 },{ 1,1,1,0 },{ 1,1,0,0 },{ 1,0,0,0 } }, // east
   { { 1,1,1,1 },{ 0,1,1,1 },{ 0,1,0,1 },{ 1,1,0,1 } }, // north
   { { 0,1,1,2 },{ 0,0,1,2 },{ 0,0,0,2 },{ 0,1,0,2 } }, // west
   { { 0,0,1,3 },{ 1,0,1,3 },{ 1,0,0,3 },{ 0,0,0,3 } }, // south
   { { 0,1,1,4 },{ 1,1,1,4 },{ 1,0,1,4 },{ 0,0,1,4 } }, // up
   { { 0,0,0,5 },{ 1,0,0,5 },{ 1,1,0,5 },{ 0,1,0,5 } }, // down
};

int main(int argc, char **argv) {
   if (!initPlatform())
      return -1;

   WindowCreationData createWindowData;
   memset(&createWindowData, 0, sizeof(WindowCreationData));
   createWindowData.api = OpenGL;
   createWindowData.versionMajor = 2;
   createWindowData.versionMinor = 1;
   createWindowData.fullscreen = false;

   WindowData window = createWindow("JeefCraft", 1440, 900, &createWindowData);

   if (glewInit() != GLEW_OK)
      return -2;
   initCamera();

   printf("OpenGL Driver Information:\n");
   printf("   Vendor:   %s\n", glGetString(GL_VENDOR));
   printf("   Renderer: %s\n", glGetString(GL_RENDERER));
   printf("   Version:  %s\n", glGetString(GL_VERSION));
   printf("   Shading:  %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

   // Create a cube.
   GLuint vbo;
   glGenBuffers(1, &vbo);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(F32) * 24 * 4, cubes, GL_STATIC_DRAW);

   // index buffer
   U16 indices[36];
   int id = 0;
   for (int i = 0; i < 36; i += 6) {
      indices[i] = id;
      indices[i + 1] = id + 2;
      indices[i + 2] = id + 1;
      indices[i + 3] = id + 0;
      indices[i + 4] = id + 3;
      indices[i + 5] = id + 2;

      id += 4;
   }
   GLuint ibo;
   glGenBuffers(1, &ibo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(U16) * 36, indices, GL_STATIC_DRAW);

   // Create shader
   U32 program;
   generateShaderProgram("Shaders/basic.vert", "Shaders/basic.frag", &program);

   // bind attrib locations
   glBindAttribLocation(program, 0, "position");

   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);

   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);

   F64 lastTime = getRealTime();
   F64 secondTime = lastTime;

   S32 fpsCounter = 0;
#define FPS_BUFFER_SIZE 128
   char fpsBuffer[FPS_BUFFER_SIZE];

   // Set initial camera position
   vec cameraPos = vec3(25.0f, 10.0f, 10.0f);
   setCameraPosition(&cameraPos);

   GLint projMatrixLoc = glGetUniformLocation(program, "projViewMatrix");
   GLint modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");

   osn_context *osn;
   open_simplex_noise((U64)0xDEADBEEF, &osn);

   while (gRunning) {
      // Calculate mouse movement for frame.
      inputCacheMouseMovementForCurrentFrame();

      // Get delta time in milliseconds.
      F64 current = getRealTime();
      F32 delta = F32(current - lastTime) * 1000.0f;
      lastTime = current;

      vec pos;
      getCameraPosition(&pos);

      if ((current - secondTime) >= 1.0) { // 1 second.
         memset(fpsBuffer, 0, FPS_BUFFER_SIZE);
         snprintf(fpsBuffer, FPS_BUFFER_SIZE, "JeefCraft - FPS: %d mspf: %f Camerapos: %f %f %f", fpsCounter, delta, pos.x, pos.y, pos.z);
         setWindowTitle(&window, fpsBuffer);

         // Reset
         fpsCounter = 0;
         secondTime = current;
      }

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(program);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

      // proj/view matrix
      mat4 proj, view, projView;
      mat4_perspective(&proj, 1.5708f, 1440.0f / 900.0f, 0.01f, 200.0f);

      calculateFreecamViewMatrix(&view, delta);

      mat4_mul(&projView, &proj, &view);
      glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &(projView.m[0].x));

      for (S32 x = 0; x < 100; ++x) {
         for (S32 z = 0; z < 100; ++z) {
            mat4 modelMatrix;
            mat4_identity(&modelMatrix);

            // position offset
            vec pos = vec3((F32)x, 0.0f, (F32)z);
            // calcuate it with 2d noise. This is the y.

            // So this is how noise works.
            // 16.0 is stretch factor.
            // noise returns between -1 to +1.
            // 5 is the amplitude. So min/max will be no more than 10 units apart. But since we take
            //   the absolute value, we are only the range between 0-1, so its only 5 units apart.
            pos.y = fabsf(floorf((F32)open_simplex_noise2(osn, (F64)x / 16.0, (F64)z / 16.0) * 5.0f));

            mat4_setPosition(&modelMatrix, &pos);

            glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &(modelMatrix.m[0].x));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
         }
      }

      swapBuffers(&window);
      pollEvents(&window);

      // We completed a frame!
      fpsCounter++;
   }

   glDeleteBuffers(1, &vbo);
   glDeleteBuffers(1, &ibo);

   freeWindow(&window);
   shutdownPlatform();
	return 0;
}