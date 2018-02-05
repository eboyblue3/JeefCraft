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

#ifndef _PLATFORM_INPUT_H_
#define _PLATFORM_INPUT_H_

#include "base/types.h"

// Key list takes the same name types as GLFW 3.x
typedef S32 Key;

extern Key KEY_UNKNOWN;
extern Key KEY_SPACE;
extern Key KEY_APOSTROPHE;
extern Key KEY_COMMA;
extern Key KEY_MINUS;
extern Key KEY_PERIOD;
extern Key KEY_SLASH;
extern Key KEY_0;
extern Key KEY_1;
extern Key KEY_2;
extern Key KEY_3;
extern Key KEY_4;
extern Key KEY_5;
extern Key KEY_6;
extern Key KEY_7;
extern Key KEY_8;
extern Key KEY_9;
extern Key KEY_SEMICOLON;
extern Key KEY_EQUAL;
extern Key KEY_A;
extern Key KEY_B;
extern Key KEY_C;
extern Key KEY_D;
extern Key KEY_E;
extern Key KEY_F;
extern Key KEY_G;
extern Key KEY_H;
extern Key KEY_I;
extern Key KEY_J;
extern Key KEY_K;
extern Key KEY_L;
extern Key KEY_M;
extern Key KEY_N;
extern Key KEY_O;
extern Key KEY_P;
extern Key KEY_Q;
extern Key KEY_R;
extern Key KEY_S;
extern Key KEY_T;
extern Key KEY_U;
extern Key KEY_V;
extern Key KEY_W;
extern Key KEY_X;
extern Key KEY_Y;
extern Key KEY_Z;
extern Key KEY_LEFT_BRACKET;
extern Key KEY_BACKSLASH;
extern Key KEY_RIGHT_BRACKET;
extern Key KEY_GRAVE_ACCENT;
extern Key KEY_WORLD_1;
extern Key KEY_WORLD_2;
extern Key KEY_ESCAPE;
extern Key KEY_ENTER;
extern Key KEY_TAB;
extern Key KEY_BACKSPACE;
extern Key KEY_INSERT;
extern Key KEY_DELETE;
extern Key KEY_RIGHT;
extern Key KEY_LEFT;
extern Key KEY_DOWN;
extern Key KEY_UP;
extern Key KEY_PAGE_UP;
extern Key KEY_PAGE_DOWN;
extern Key KEY_HOME;
extern Key KEY_END;
extern Key KEY_CAPS_LOCK;
extern Key KEY_SCROLL_LOCK;
extern Key KEY_NUM_LOCK;
extern Key KEY_PRINT_SCREEN;
extern Key KEY_PAUSE;

// Function Keys.
extern Key KEY_F1;
extern Key KEY_F2;
extern Key KEY_F3;
extern Key KEY_F4;
extern Key KEY_F5;
extern Key KEY_F6;
extern Key KEY_F7;
extern Key KEY_F8;
extern Key KEY_F9;
extern Key KEY_F10;
extern Key KEY_F11;
extern Key KEY_F12;
extern Key KEY_F13;
extern Key KEY_F14;
extern Key KEY_F15;

// Keypad Keys.
extern Key KEY_KEYPAD_0;
extern Key KEY_KEYPAD_1;
extern Key KEY_KEYPAD_2;
extern Key KEY_KEYPAD_3;
extern Key KEY_KEYPAD_4;
extern Key KEY_KEYPAD_5;
extern Key KEY_KEYPAD_6;
extern Key KEY_KEYPAD_7;
extern Key KEY_KEYPAD_8;
extern Key KEY_KEYPAD_9;
extern Key KEY_KEYPAD_DECIMAL;
extern Key KEY_KEYPAD_DIVIDE;
extern Key KEY_KEYPAD_MULTIPLY;
extern Key KEY_KEYPAD_SUBTRACT;
extern Key KEY_KEYPAD_ADD;
extern Key KEY_KEYPAD_ENTER;
extern Key KEY_KEYPAD_EQUAL;

// Modifier Keys
extern Key KEY_LEFT_SHIFT;
extern Key KEY_LEFT_CONTROL;
extern Key KEY_LEFT_ALT;
extern Key KEY_LEFT_SUPER;
extern Key KEY_RIGHT_SHIFT;
extern Key KEY__RIGHT_CONTROL;
extern Key KEY_RIGHT_ALT;
extern Key KEY_RIGHT_SUPER;

extern Key KEY_MENU;

typedef enum KeyState {
   PRESSED = 0,
   RELEASED = 1
} KeyState;

KeyState inputGetKeyStatus(Key key);

void inputCacheMouseMovementForCurrentFrame();

/// The idea is that the mouse movement is cached for a given frame.
/// This is done by calling inputGetCursorPosition and inputSetCursorPosition
/// At the beginning of each frame in the render loop. Then, to use that
/// value throughout the frame, you can call this function here.
void inputGetMouseMovementForCurrentFrame(F64 *x, F64 *y);

#endif