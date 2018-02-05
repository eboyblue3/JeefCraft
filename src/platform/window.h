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

#ifndef _PLATFORM_WINDOW_H_
#define _PLATFORM_WINDOW_H_

#include "base/types.h"

/// Structure that holds information about the Window object
/// This structure is API agnostic.
typedef struct {
   void *windowHandle;
   void *nativeHandle;
} WindowData;

typedef enum {
   D3D9,
   D3D11,
   Metal,
   OpenGL,
   OpenGLCore,
   Vulkan
} WindowAPI;

typedef struct {
   WindowAPI api;
   S32 versionMajor;
   S32 versionMinor;
   bool fullscreen;
} WindowCreationData;

WindowData createWindow(const char *title, S32 width, S32 height, WindowCreationData *data);
void freeWindow(WindowData *window);
void setWindowTitle(WindowData *window, const char *title);
void swapBuffers(WindowData *window);

#endif