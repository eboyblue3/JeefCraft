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

#include "graphics/texture2d.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static bool uploadTexture2DToGL(U8 *data, Texture2D *tex) {
   GLenum format;
   switch (tex->channels) {
   case 3:
      // JPG
      format = GL_RGB;
      break;
   case 4:
      // PNG
      format = GL_RGBA;
      break;
   default:
      format = GL_RGBA;
   }

   GLuint id;
   glEnable(GL_TEXTURE_2D); // Old drivers might need this in legacy GL.
   glGenTextures(1, &id);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, id);
   glTexImage2D(GL_TEXTURE_2D, 0, format, tex->width, tex->height, 0, format, GL_UNSIGNED_BYTE, data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, tex->maxMipLevel);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
   glGenerateMipmapEXT(GL_TEXTURE_2D);

   // Attach the id to tex
   tex->glId = id;
   return true;
}

bool createTexture2D(const char *file, S32 channels, S32 numMipLevels, Texture2D *tex) {
   // Must be not higher than 4.
   assert(channels <= 4);

   S32 x, y, n;
   U8 *data = stbi_load(file, &x, &y, &n, channels);
   if (data == NULL)
      return false;

   memset(tex, 0, sizeof(Texture2D));
   tex->width = x;
   tex->height = y;
   tex->channels = channels;
   tex->maxMipLevel = numMipLevels;
   bool result = uploadTexture2DToGL(data, tex);

   // Free data image data.
   stbi_image_free(data);
   return result;
}

void freeTexture2D(Texture2D *tex) {
   if (tex->glId)
      glDeleteTextures(1, &tex->glId);
   memset(tex, 0, sizeof(Texture2D));
}