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
#include <string>
#include <stdlib.h>
#include "base/io.h"

bool readTextFile(const char *fileName, char **contents, WordSize *length) {
   FILE *f = fopen(fileName, "r");
   if (!f)
      return false;

   fseek(f, 0, SEEK_END);
   WordSize size = (WordSize)ftell(f);
   rewind(f);

   *contents = (char*)calloc(size + 1, sizeof(char));
   fread(*contents, sizeof(char), size, f);
   *length = size;

   fclose(f);
   return true;
}

bool readBinaryFile(const char *fileName, U8 **contents, WordSize *length) {
   FILE *f = fopen(fileName, "rb");
   if (!f)
      return false;

   fseek(f, 0, SEEK_END);
   WordSize size = (WordSize)ftell(f);
   rewind(f);

   *contents = (U8*)calloc(size + 1, sizeof(U8));
   fread(*contents, sizeof(U8), size, f);
   *length = size;

   fclose(f);
   return true;
}