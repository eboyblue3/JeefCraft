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

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <assert.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "platform/window.h"
#include "platform/glfw3/glfw3Input.h"

GLFWwindow *gGLFW3PrimaryWindow;

WindowData createWindow(const char *title, S32 width, S32 height, WindowCreationData *data) {
   // For now we only support OpenGL (non core).
   assert(data->api == OpenGL);

   // We only support one window.
   assert(gGLFW3PrimaryWindow == NULL);

   // Set window hints
   if (data->api == OpenGL) {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, data->versionMajor);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, data->versionMinor);
   }

   WindowData window;
   memset(&window, 0, sizeof(WindowData));
   window.windowHandle = glfwCreateWindow(width, height, title, NULL, NULL);

#ifdef _WIN32
   window.nativeHandle = glfwGetWin32Window((GLFWwindow*)window.windowHandle);
#endif
   if (data->api == OpenGL || data->api == OpenGLCore) {
      glfwMakeContextCurrent((GLFWwindow*)window.windowHandle);
   }

   gGLFW3PrimaryWindow = (GLFWwindow*)window.windowHandle;
   return window;
}

void freeWindow(WindowData *window) {
   glfwDestroyWindow((GLFWwindow*)window->windowHandle);
   memset(window, 0, sizeof(WindowData));
   gGLFW3PrimaryWindow = NULL;
}

void setWindowTitle(WindowData *window, const char *title) {
   glfwSetWindowTitle((GLFWwindow*)window->windowHandle, title);
}

void swapBuffers(WindowData *window) {
   glfwSwapBuffers((GLFWwindow*)window->windowHandle);
}