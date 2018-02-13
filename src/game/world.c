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

#include <math.h>
#include <assert.h>
#include <string.h>
#include <GL/glew.h>
#include <stb_vec.h>
#include <stretchy_buffer.h>
#include <open-simplex-noise.h>
#include "game/world.h"
#include "math/matrix.h"
#include "game/camera.h"
#include "graphics/shader.h"
#include "graphics/texture2d.h"

#define CHUNK_WIDTH 16
#define MAX_CHUNK_HEIGHT 256
#define RENDER_CHUNK_HEIGHT 16
#define CHUNK_SIZE (S32)(MAX_CHUNK_HEIGHT * CHUNK_WIDTH * CHUNK_WIDTH)
#define CHUNK_SPLITS (S32)(MAX_CHUNK_HEIGHT / RENDER_CHUNK_HEIGHT)

// Taken from std_voxel_render.h, from the public domain
static F32 cubes[6][4][4] = {
   { { 1,0,1,0 },{ 1,1,1,0 },{ 1,1,0,0 },{ 1,0,0,0 } }, // east
   { { 1,1,1,1 },{ 0,1,1,1 },{ 0,1,0,1 },{ 1,1,0,1 } }, // up
   { { 0,1,1,2 },{ 0,0,1,2 },{ 0,0,0,2 },{ 0,1,0,2 } }, // west
   { { 0,0,1,3 },{ 1,0,1,3 },{ 1,0,0,3 },{ 0,0,0,3 } }, // down
   { { 0,1,1,4 },{ 1,1,1,4 },{ 1,0,1,4 },{ 0,0,1,4 } }, // north
   { { 0,0,0,5 },{ 1,0,0,5 },{ 1,1,0,5 },{ 0,1,0,5 } }, // south
};

static F32 cubeUVs[6][4][2] = {
   { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } }, // East
   { { 1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 } }, // up
   { { 1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 } }, // west
   { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } }, // down
   { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } }, // north
   { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }  // south
};

typedef enum CubeSides {
   CubeSides_East,
   CubeSides_Up,
   CubeSides_West,
   CubeSides_Down,
   CubeSides_North,
   CubeSides_South,
} CubeSides;

static struct osn_context *osn;

typedef struct GPUVertex {
   vec4 position;
   vec2 uvs;
} GPUVertex;

typedef U32 GPUIndex;

typedef struct Cube {
   U16 material : 10; // 1024 material types
   U16 light : 4;     // 0-15 light level
   U16 flag1 : 1;     // 1-bit extra flag
   U16 flag2 : 1;     // 1-bit extra flag
} Cube;

typedef enum Materials {
   Material_Air,
   Material_Bedrock,
   Material_Dirt,
   Material_Grass,     // Also note that bottoms of grass have dirt blocks.
   Material_Grass_Side // Sides of grass have a special texture.
} Material;

// TODO: store a list of pointers of RenderChunk array (RenderChunk**)
// into a Chunk datastructure. That way we can access the RenderChunk
// and update it accordingly when we break a block. We can calculate
// based on the position of the block breaking what position the RenderChunk
// is in.

typedef struct RenderChunk {
   GPUVertex *vertexData; /// stretchy buffer
   GPUIndex *indices;          /// stretchy buffer
   GPUIndex currentIndex;      /// Current index offset
   GPUIndex indiceCount;       /// Indice Size
   S32 vertexCount;       /// VertexData Count 

   GLuint vbo;            /// OpenGL Vertex Buffer Object
   GLuint ibo;            /// OpenGL Index Buffer Object
} RenderChunk;

typedef struct Chunk {
   Cube *cubeData;                         /// Cube data for full chunk
   RenderChunk renderChunks[CHUNK_SPLITS]; /// Per-render chunk data.
} Chunk;

/// ChunkWorld is a flat 2D array that represents the entire
/// world based upon
Chunk *gChunkWorld = NULL;

// Grid size but should be variable. This is the 'chunk distance'.
S32 worldSize = 16;

Chunk* getChunkAt(S32 x, S32 z) {
   return &gChunkWorld[(z * (worldSize)) + x];
}

GLuint projMatrixLoc;
GLuint modelMatrixLoc;
GLuint textureLoc;
U32 program;
Texture2D textureAtlas;

Cube* getCubeAt(Cube *cubeData, S32 x, S32 y, S32 z) {
   return &cubeData[x * (MAX_CHUNK_HEIGHT) * (CHUNK_WIDTH) + z * (MAX_CHUNK_HEIGHT) + y];
}

#define TEXTURE_ATLAS_COUNT_I 32
#define TEXTURE_ATLAS_COUNT_F 32.0f

void buildFace(Chunk *chunk, S32 index, S32 side, S32 material, vec *localPos) {
   // Vertex data first, then index data.

   RenderChunk *renderChunk = &chunk->renderChunks[index];

   for (S32 i = 0; i < 4; ++i) {
      GPUVertex v;
      v.position.x = cubes[side][i][0] + localPos->x;
      v.position.y = cubes[side][i][1] + localPos->y;
      v.position.z = cubes[side][i][2] + localPos->z;
      v.position.w = cubes[side][i][3];
      v.uvs.x = (cubeUVs[side][i][0] + ((F32)(material % TEXTURE_ATLAS_COUNT_I))) / TEXTURE_ATLAS_COUNT_F;
      v.uvs.y = (cubeUVs[side][i][1] + ((F32)(material / TEXTURE_ATLAS_COUNT_I))) / TEXTURE_ATLAS_COUNT_F;
      sb_push(renderChunk->vertexData, v);
   }
   renderChunk->vertexCount += 4;

   GPUIndex in = renderChunk->currentIndex;
   sb_push(renderChunk->indices, in);
   sb_push(renderChunk->indices, in + 2);
   sb_push(renderChunk->indices, in + 1);
   sb_push(renderChunk->indices, in);
   sb_push(renderChunk->indices, in + 3);
   sb_push(renderChunk->indices, in + 2);
   renderChunk->currentIndex += 4;
   renderChunk->indiceCount += 6;
}

void generateWorld(S32 chunkX, S32 chunkZ, S32 worldX, S32 worldZ) {
   // We modulate divide CHUNK_WIDTH as we keep the grid static
   // no matter where we move around on the map.
   // Say we go west, well, the first 'west' chunks are replaced by new chunks.
   // TODO: This doesn't work quite yet, and we might need to use chunk
   // pointers so we can easilly rearrange the world.
   // Or maybe a memcpy will be fine, who knows.

   Chunk *chunk = getChunkAt(chunkX, chunkZ);
   chunk->cubeData = (Cube*)calloc(CHUNK_SIZE, sizeof(Cube));
   Cube *cubeData = chunk->cubeData;

   F64 stretchFactor = 16.0;

   for (S32 x = 0; x < CHUNK_WIDTH; ++x) {
      for (S32 z = 0; z < CHUNK_WIDTH; ++z) {
         // calculate height for each cube.
         // Taking absolute value will allow for only 0-1 scaling.
         // also make sure to use the world coordinates
         F64 noise = fabs(open_simplex_noise2(osn, (F64)(x + worldX) / stretchFactor, (F64)(z + worldZ) / stretchFactor) * 40.0);
         S32 height = (S32)(noise);

         // Make block at height level grass.
         getCubeAt(cubeData, x, height, z)->material = Material_Grass;

         // Need to make air for anything above height.
         // Anything below height is just block the whole way till last couple rows which
         // are bedrock.
         for (S32 y = height + 1; y < MAX_CHUNK_HEIGHT; ++y) {
            // All air
            getCubeAt(cubeData, x, y, z)->material = Material_Air;
         }
         for (S32 y = 4; y < height; ++y) {
            // All dirt.
            // todo: this is where we do stuff like caves!
            // but of course in a second pass, or third pass...!
            getCubeAt(cubeData, x, y, z)->material = Material_Dirt;
         }
         for (S32 y = 0; y < 4; ++y) {
            // All bedrock
            getCubeAt(cubeData, x, y, z)->material = Material_Bedrock;
         }
      }
   }

}

inline bool isTransparent(Cube *cubeData, S32 x, S32 y, S32 z) {
   return getCubeAt(cubeData, x, y, z)->material == Material_Air;
}

void generateGeometry(S32 chunkX, S32 chunkZ) {
   Chunk *chunk = getChunkAt(chunkX, chunkZ);
   Cube *cubeData = chunk->cubeData;

   // Generate geometry.
   for (S32 x = 0; x < CHUNK_WIDTH; ++x) {
      for (S32 z = 0; z < CHUNK_WIDTH; ++z) {
         // Split up y axis into render chunks and calc y
         for (S32 i = 0; i < CHUNK_SPLITS; ++i) {
            for (S32 j = 0; j < RENDER_CHUNK_HEIGHT; ++j) {
               S32 y = (RENDER_CHUNK_HEIGHT * i) + j;
               vec localPos = vec3(x, y, z);

               // skip if current block is transparent.
               if (isTransparent(cubeData, x, y, z))
                  continue;

               // Cross chunk checking. Only need to check x and z axes.
               // If the next *chunk* over is is transparent then ya we have
               // to render regardless.
               bool isOpaqueNegativeX = false;
               bool isOpaquePositiveX = false;
               bool isOpaqueNegativeZ = false;
               bool isOpaquePositiveZ = false;

               if (x == 0 && chunkX > 0) {
                  Cube *behindData = getChunkAt(chunkX - 1, chunkZ)->cubeData;
                  if (!isTransparent(behindData, CHUNK_WIDTH - 1, y, z)) {
                     // The cube behind us on the previous chunk is in fact
                     // transparent. We need to render this face.
                     isOpaqueNegativeX = true;
                  }
               }
               if (x == (CHUNK_WIDTH - 1) && (chunkX + 1) < worldSize) {
                  Cube *behindData = getChunkAt(chunkX + 1, chunkZ)->cubeData;
                  if (!isTransparent(behindData, 0, y, z)) {
                     // The cube behind us on the previous chunk is in fact
                     // transparent. We need to render this face.
                     isOpaquePositiveX = true;
                  }
               }
               if (z == 0 && chunkZ > 0) {
                  Cube *behindData = getChunkAt(chunkX, chunkZ - 1)->cubeData;
                  if (!isTransparent(behindData, x, y, CHUNK_WIDTH - 1)) {
                     // The cube behind us on the previous chunk is in fact
                     // transparent. We need to render this face.
                     isOpaqueNegativeZ = true;
                  }
               }
               if (z == (CHUNK_WIDTH - 1) && (chunkZ + 1) < worldSize) {
                  Cube *behindData = getChunkAt(chunkX, chunkZ + 1)->cubeData;
                  if (!isTransparent(behindData, x, y, 0)) {
                     // The cube behind us on the previous chunk is in fact
                     // transparent. We need to render this face.
                     isOpaquePositiveZ = true;
                  }
               }

               // check all 6 directions to see if the cube is exposed.
               // If the cube is exposed in that direction, render that face.

               S32 material = getCubeAt(cubeData, x, y, z)->material;

               if (y == 0 || isTransparent(cubeData, x, y - 1, z))
                  buildFace(chunk, i, CubeSides_Down, material, &localPos);

               if (y >= (MAX_CHUNK_HEIGHT - 1) || isTransparent(cubeData, x, y + 1, z))
                  buildFace(chunk, i, CubeSides_Up, material, &localPos);

               if ((!isOpaqueNegativeX && x == 0) || (x > 0 && isTransparent(cubeData, x - 1, y, z)))
                  buildFace(chunk, i, CubeSides_West, material, &localPos);

               if ((!isOpaquePositiveX && x >= (CHUNK_WIDTH - 1)) || (x < (CHUNK_WIDTH - 1) && isTransparent(cubeData, x + 1, y, z)))
                  buildFace(chunk, i, CubeSides_East, material, &localPos);

               if ((!isOpaqueNegativeZ && z == 0) || (z > 0 && isTransparent(cubeData, x, y, z - 1)))
                  buildFace(chunk, i, CubeSides_South, material, &localPos);

               if ((!isOpaquePositiveZ && z >= (CHUNK_WIDTH - 1)) || (z < (CHUNK_WIDTH - 1) && isTransparent(cubeData, x, y, z + 1)))
                  buildFace(chunk, i, CubeSides_North, material, &localPos);
            }
         }
      }
   }
}

void uploadGeometryToGL() {
   // *Single threaded*
   //
   // Upload to the GL
   // Now that we generated all of the geometry, upload to the GL.
   // Note if a chunk has no geometry we don't create a vbo
   //
   // TODO: use VAO if extension is supported??
   for (S32 x = 0; x < worldSize; ++x) {
      for (S32 z = 0; z < worldSize; ++z) {
         for (S32 i = 0; i < CHUNK_SPLITS; ++i) {
            RenderChunk *r = &getChunkAt(x, z)->renderChunks[i];
            if (r->vertexCount > 0) {
               glGenBuffers(1, &r->vbo);
               glGenBuffers(1, &r->ibo);

               glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
               glBufferData(GL_ARRAY_BUFFER, sizeof(GPUVertex) * r->vertexCount, r->vertexData, GL_STATIC_DRAW);

               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->ibo);
               glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GPUIndex) * r->indiceCount, r->indices, GL_STATIC_DRAW);
            }

            // Free right after uploading to the GL. We don't need gpu data
            // in both system and gpu ram.
            sb_free(r->vertexData);
            sb_free(r->indices);
         }
      }
   }
}

void initWorld() {
   // Only 2 mip levels.
   bool ret = createTexture2D("Assets/block_atlas.png", 4, 2, &textureAtlas);
   if (!ret) {
      exit(-3);
   }

   // Create shader
   generateShaderProgram("Shaders/basic.vert", "Shaders/basic.frag", &program);
   // bind attrib locations
   glBindAttribLocation(program, 0, "position");
   glBindAttribLocation(program, 1, "uvs");
   projMatrixLoc = glGetUniformLocation(program, "projViewMatrix");
   modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
   textureLoc = glGetUniformLocation(program, "textureAtlas");

   open_simplex_noise((U64)0xDEADBEEF, &osn);

   // world grid
   gChunkWorld = (Chunk*)calloc(worldSize * worldSize, sizeof(Chunk));

   // Easilly put each chunk in a thread in here.
   // nothing OpenGL, all calculation and world generation.
   for (S32 x = 0; x < worldSize; ++x) {
      for (S32 z = 0; z < worldSize; ++z) {
         // World position calcuation before passing.
         generateWorld(x, z, x * CHUNK_WIDTH, z * CHUNK_WIDTH);
      }
   }

   // TODO MULTITHREADED: sync here before generating the Geometry.

   // Easilly put each chunk in a thread in here.
   // nothing OpenGL, all calculation and world generation.
   for (S32 x = 0; x < worldSize; ++x) {
      for (S32 z = 0; z < worldSize; ++z) {
         generateGeometry(x, z);
      }
   }

   // TODO MULTITHREADED: sync here before GL upload.

   uploadGeometryToGL();
}

void freeWorld() {
   for (S32 x = 0; x < worldSize; ++x) {
      for (S32 z = 0; z < worldSize; ++z) {
         Chunk *c = getChunkAt(x, z);
         free(c->cubeData);
         for (S32 i = 0; i < CHUNK_SPLITS; ++i) {
            RenderChunk *r = &c->renderChunks[i];
            if (r->vertexCount > 0) {
               glDeleteBuffers(1, &r->vbo);
               glDeleteBuffers(1, &r->ibo);
            }
         }
      }
   }

   free(gChunkWorld);
   open_simplex_noise_free(osn);
}

void renderWorld(F32 dt) {
   // Set GL State
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);

   glUseProgram(program);

   // proj/view matrix
   mat4 proj, view, projView;
   mat4_perspective(&proj, 1.5708f, 1440.0f / 900.0f, 0.01f, (F32)(CHUNK_WIDTH * worldSize));
   getCurrentViewMatrix(&view);
   mat4_mul(&projView, &proj, &view);
   glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &(projView.m[0].x));

   // Bind our texture atlas to texture unit 0
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, textureAtlas.glId);
   glUniform1i(textureLoc, 0);

   for (S32 x = 0; x < worldSize; ++x) {
      for (S32 z = 0; z < worldSize; ++z) {
         Chunk *c = getChunkAt(x, z);
         for (S32 i = 0; i < CHUNK_SPLITS; ++i) {
            if (c->renderChunks[i].vertexCount > 0) {

               // Set position.
               vec pos = vec3(x * CHUNK_WIDTH, 0, z * CHUNK_WIDTH);
               mat4 modelMatrix;
               mat4_identity(&modelMatrix);
               mat4_setPosition(&modelMatrix, &pos);
               glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &(modelMatrix.m[0].x));
               glBindBuffer(GL_ARRAY_BUFFER, c->renderChunks[i].vbo);
               glEnableVertexAttribArray(0);
               glEnableVertexAttribArray(1);
               glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GPUVertex), (void*)offsetof(GPUVertex, position));
               glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GPUVertex), (void*)offsetof(GPUVertex, uvs));
               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->renderChunks[i].ibo);
               glDrawElements(GL_TRIANGLES, (GLsizei)c->renderChunks[i].indiceCount, GL_UNSIGNED_INT, (void*)0);
            }
         }
      }
   }
}
