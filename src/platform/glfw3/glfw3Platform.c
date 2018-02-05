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
//-----------------------------------------------------------------------------

#include <GLFW/glfw3.h>
#include "platform/platform.h"

bool gRunning;

bool initPlatform() {
   gRunning = true;
   return glfwInit() == GLFW_TRUE;
}

void pollEvents(WindowData *window) {
   glfwPollEvents();
   if (glfwWindowShouldClose((GLFWwindow*)window->windowHandle))
      gRunning = false;
}

void shutdownPlatform() {
   glfwTerminate();
}

F64 getRealTime() {
   // glfwGetTime() returns time in seconds since glfw startup.
   // It automatically detects the highest resolution timer.
   return glfwGetTime();
}