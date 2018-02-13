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

#ifndef _GRAPHICS_TEXTURE2D_H_
#define _GRAPHICS_TEXTURE2D_H_

#include <GL/glew.h>
#include "base/types.h"

typedef struct Texture2D {
   S32 channels;
   S32 width;
   S32 height;
   S32 maxMipLevel;
   GLuint glId;
} Texture2D;

/// Loads a 2D texture off of the disk and uploads it to the GL as a static image.
/// @param file The file location of where the texture is on the disk.
/// @param bits The requested number of channels (1-4).
/// @param maxMip The maximum number of mipmap levels, or -1 if unlimited.
/// @param tex The output data of the texture.
/// @return true if successful in loading and uploading the texture to the GL.
bool createTexture2D(const char *file, S32 channels, S32 maxMip, Texture2D *tex);

/// Frees the texture from the GL and any additional resources it would have.
/// @param tex The Texture2D structure which has the texture we wish to free.
void freeTexture2D(Texture2D *tex);

#endif