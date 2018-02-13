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

#include <assert.h>
#include <GLFW/glfw3.h>
#include "platform/input.h"

Key KEY_UNKNOWN = GLFW_KEY_UNKNOWN;
Key KEY_SPACE = GLFW_KEY_SPACE;
Key KEY_APOSTROPHE = GLFW_KEY_APOSTROPHE;
Key KEY_COMMA = GLFW_KEY_COMMA;
Key KEY_MINUS = GLFW_KEY_MINUS;
Key KEY_PERIOD = GLFW_KEY_PERIOD;
Key KEY_SLASH = GLFW_KEY_SLASH;
Key KEY_0 = GLFW_KEY_0;
Key KEY_1 = GLFW_KEY_1;
Key KEY_2 = GLFW_KEY_2;
Key KEY_3 = GLFW_KEY_3;
Key KEY_4 = GLFW_KEY_4;
Key KEY_5 = GLFW_KEY_5;
Key KEY_6 = GLFW_KEY_6;
Key KEY_7 = GLFW_KEY_7;
Key KEY_8 = GLFW_KEY_8;
Key KEY_9 = GLFW_KEY_9;
Key KEY_SEMICOLON = GLFW_KEY_SEMICOLON;
Key KEY_EQUAL = GLFW_KEY_EQUAL;
Key KEY_A = GLFW_KEY_A;
Key KEY_B = GLFW_KEY_B;
Key KEY_C = GLFW_KEY_C;
Key KEY_D = GLFW_KEY_D;
Key KEY_E = GLFW_KEY_E;
Key KEY_F = GLFW_KEY_F;
Key KEY_G = GLFW_KEY_G;
Key KEY_H = GLFW_KEY_H;
Key KEY_I = GLFW_KEY_I;
Key KEY_J = GLFW_KEY_J;
Key KEY_K = GLFW_KEY_K;
Key KEY_L = GLFW_KEY_L;
Key KEY_M = GLFW_KEY_M;
Key KEY_N = GLFW_KEY_N;
Key KEY_O = GLFW_KEY_O;
Key KEY_P = GLFW_KEY_P;
Key KEY_Q = GLFW_KEY_Q;
Key KEY_R = GLFW_KEY_R;
Key KEY_S = GLFW_KEY_S;
Key KEY_T = GLFW_KEY_T;
Key KEY_U = GLFW_KEY_U;
Key KEY_V = GLFW_KEY_V;
Key KEY_W = GLFW_KEY_W;
Key KEY_X = GLFW_KEY_X;
Key KEY_Y = GLFW_KEY_Y;
Key KEY_Z = GLFW_KEY_Z;
Key KEY_LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET;
Key KEY_BACKSLASH = GLFW_KEY_BACKSLASH;
Key KEY_RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET;
Key KEY_GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT;
Key KEY_WORLD_1 = GLFW_KEY_WORLD_1;
Key KEY_WORLD_2 = GLFW_KEY_WORLD_2;
Key KEY_ESCAPE = GLFW_KEY_ESCAPE;
Key KEY_ENTER = GLFW_KEY_ENTER;
Key KEY_TAB = GLFW_KEY_TAB;
Key KEY_BACKSPACE = GLFW_KEY_BACKSPACE;
Key KEY_INSERT = GLFW_KEY_INSERT;
Key KEY_DELETE = GLFW_KEY_DELETE;
Key KEY_RIGHT = GLFW_KEY_RIGHT;
Key KEY_LEFT = GLFW_KEY_LEFT;
Key KEY_DOWN = GLFW_KEY_DOWN;
Key KEY_UP = GLFW_KEY_UP;
Key KEY_PAGE_UP = GLFW_KEY_PAGE_UP;
Key KEY_PAGE_DOWN = GLFW_KEY_PAGE_DOWN;
Key KEY_HOME = GLFW_KEY_HOME;
Key KEY_END = GLFW_KEY_END;
Key KEY_CAPS_LOCK = GLFW_KEY_CAPS_LOCK;
Key KEY_SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK;
Key KEY_NUM_LOCK = GLFW_KEY_NUM_LOCK;
Key KEY_PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN;
Key KEY_PAUSE = GLFW_KEY_PAUSE;

// Function Keys.
Key KEY_F1 = GLFW_KEY_F1;
Key KEY_F2 = GLFW_KEY_F2;
Key KEY_F3 = GLFW_KEY_F3;
Key KEY_F4 = GLFW_KEY_F4;
Key KEY_F5 = GLFW_KEY_F5;
Key KEY_F6 = GLFW_KEY_F6;
Key KEY_F7 = GLFW_KEY_F7;
Key KEY_F8 = GLFW_KEY_F8;
Key KEY_F9 = GLFW_KEY_F9;
Key KEY_F10 = GLFW_KEY_F10;
Key KEY_F11 = GLFW_KEY_F11;
Key KEY_F12 = GLFW_KEY_F12;
Key KEY_F13 = GLFW_KEY_F13;
Key KEY_F14 = GLFW_KEY_F14;
Key KEY_F15 = GLFW_KEY_F15;

// Keypad Keys.
Key KEY_KEYPAD_0 = GLFW_KEY_KP_0;
Key KEY_KEYPAD_1 = GLFW_KEY_KP_1;
Key KEY_KEYPAD_2 = GLFW_KEY_KP_2;
Key KEY_KEYPAD_3 = GLFW_KEY_KP_3;
Key KEY_KEYPAD_4 = GLFW_KEY_KP_4;
Key KEY_KEYPAD_5 = GLFW_KEY_KP_5;
Key KEY_KEYPAD_6 = GLFW_KEY_KP_6;
Key KEY_KEYPAD_7 = GLFW_KEY_KP_7;
Key KEY_KEYPAD_8 = GLFW_KEY_KP_8;
Key KEY_KEYPAD_9 = GLFW_KEY_KP_9;
Key KEY_KEYPAD_DECIMAL = GLFW_KEY_KP_DECIMAL;
Key KEY_KEYPAD_DIVIDE = GLFW_KEY_KP_DIVIDE;
Key KEY_KEYPAD_MULTIPLY = GLFW_KEY_KP_MULTIPLY;
Key KEY_KEYPAD_SUBTRACT = GLFW_KEY_KP_SUBTRACT;
Key KEY_KEYPAD_ADD = GLFW_KEY_KP_ADD;
Key KEY_KEYPAD_ENTER = GLFW_KEY_KP_ENTER;
Key KEY_KEYPAD_EQUAL = GLFW_KEY_KP_EQUAL;

// Modifier Keys
Key KEY_LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT;
Key KEY_LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL;
Key KEY_LEFT_ALT = GLFW_KEY_LEFT_ALT;
Key KEY_LEFT_SUPER = GLFW_KEY_LEFT_SUPER;
Key KEY_RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT;
Key KEY_RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL;
Key KEY_RIGHT_ALT = GLFW_KEY_RIGHT_ALT;
Key KEY_RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER;

Key KEY_MENU = GLFW_KEY_MENU;

extern GLFWwindow *gGLFW3PrimaryWindow;

// Mouse movement cache
typedef struct MouseMovementCache {
   F64 x;
   F64 y;
} MouseMovementCache;
MouseMovementCache gMouseMovementCache = { 0.0, 0.0 };
MouseMovementCache gLastMousePosition = { 0.0, 0.0 };

KeyState inputGetKeyStatus(Key key) {
   // We must have a window in order to do key presses.
   assert(gGLFW3PrimaryWindow);

   int status = glfwGetKey(gGLFW3PrimaryWindow, (S32)key);
   if (status == GLFW_PRESS)
      return PRESSED;
   return RELEASED;
}

void inputCacheMouseMovementForCurrentFrame() {
   // We must have a window in order to do key presses.
   assert(gGLFW3PrimaryWindow);

   F64 x;
   F64 y;
   glfwGetCursorPos(gGLFW3PrimaryWindow, &x, &y);

	gMouseMovementCache.x = x - gLastMousePosition.x;
	gMouseMovementCache.y = y - gLastMousePosition.y;

   // Calculate offset from center.
   gLastMousePosition.x = x;
   gLastMousePosition.y = y;
}

/// The idea is that the mouse movement is cached for a given frame.
/// This is done by calling inputGetCursorPosition and inputSetCursorPosition
/// At the beginning of each frame in the render loop. Then, to use that
/// value throughout the frame, you can call this function here.
void inputGetMouseMovementForCurrentFrame(F64 *x, F64 *y) {
   *x = gMouseMovementCache.x;
   *y = gMouseMovementCache.y;
}
