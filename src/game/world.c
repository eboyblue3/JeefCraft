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
#include <stretchy_buffer.h>
#include <open-simplex-noise.h>
#include "game/world.h"
#include "game/camera.h"
#include "graphics/shader.h"
#include "graphics/texture2d.h"
#include "math/frustum.h"
#include "platform/input.h"
#include "math/screenWorld.h"
#include "math/aabb.h"

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
   { { 0, 1 }, { 0, 0 }, { 1, 0 }, { 1, 1 } }, // East
   { { 1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 } }, // up
   { { 1, 0 }, { 1, 1 }, { 0, 1 }, { 0, 0 } }, // west
   { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 0, 0 } }, // down
   { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } }, // north
   { { 1, 1 }, { 0, 1 }, { 0, 0 }, { 1, 0 } }  // south
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
   Vec4 position;
   F32 uvx;
   F32 uvy;
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
   Material_Grass,      // Also note that bottoms of grass have dirt blocks.
   Material_Grass_Side, // Sides of grass have a special texture.
   Material_Wood_Trunk,
   Material_Leaves
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
S32 worldSize = 2;

Chunk* getChunkAt(S32 x, S32 z) {
   // Since x and z can go from -worldSize to worldSize,
   // we need to normalize them so that they are always positive.
   x += worldSize;
   z += worldSize;

   S32 index = (z * (worldSize * 2)) + x;
   return &gChunkWorld[index];
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

void buildFace(Chunk *chunk, S32 index, S32 side, S32 material, Vec3 localPos) {
   // Vertex data first, then index data.

   RenderChunk *renderChunk = &chunk->renderChunks[index];

   for (S32 i = 0; i < 4; ++i) {
      GPUVertex v;
      v.position.x = cubes[side][i][0] + localPos.x;
      v.position.y = cubes[side][i][1] + localPos.y;
      v.position.z = cubes[side][i][2] + localPos.z;
      v.position.w = cubes[side][i][3];
      v.uvx = (F32)(cubeUVs[side][i][0] + ((F32)(material % TEXTURE_ATLAS_COUNT_I))) / TEXTURE_ATLAS_COUNT_F;
      v.uvy = (F32)(cubeUVs[side][i][1] + ((F32)(material / TEXTURE_ATLAS_COUNT_I))) / TEXTURE_ATLAS_COUNT_F;
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

F32 getViewDistance() {
   // Give 1 chunk 'padding' looking forward.
   return worldSize * CHUNK_WIDTH + CHUNK_WIDTH;
}

static inline bool isTransparent(Cube *cubeData, S32 x, S32 y, S32 z) {
   return getCubeAt(cubeData, x, y, z)->material == Material_Air;
}

static inline bool isTransparentAtCube(Cube *c) {
   if (c == NULL)
      return false;
   return c->material == Material_Air;
}

static inline Cube* getGlobalCubeAtWorldSpacePosition(S32 x, S32 y, S32 z) {
   // first calculate chunk based upon position.
   S32 chunkX = x / CHUNK_WIDTH;
   S32 chunkZ = z / CHUNK_WIDTH;

   if (x < 0)
      --chunkX;
   if (z < 0)
      --chunkZ;

   // Don't go past.
   if (chunkX < -worldSize || chunkX >= worldSize || chunkZ < -worldSize || chunkZ >= worldSize)
      return NULL;

   Chunk *chunk = getChunkAt(chunkX, chunkZ);

   S32 localChunkX = x - (chunkX * CHUNK_WIDTH);
   S32 localChunkZ = z - (chunkZ * CHUNK_WIDTH);

   return getCubeAt(chunk->cubeData, localChunkX, y, localChunkZ);
}

// Worldspace
static bool shouldCave(S32 x, S32 y, S32 z) {
   F64 cave_stretch = 24.0;

   F64 noise = 0.0;
   for (S32 i = 0; i < 6; ++i) {
      F64 factor = cave_stretch * ((F64)((1 << i) / 3) + 1.0);

      noise += (open_simplex_noise3(
         osn,
         (F64)(x) / factor * (F64)(1 << i),
         (F64)y / factor * (F64)(1 << (i + 1)),
         (F64)(z) / factor * (F64)(1 << i)
      ) + 1.0) / (F64)(1 << (i + 1));
   }

   return noise >= 1.33;
}

static S32 solidCubesAroundCubeAt(S32 x, S32 y, S32 z, S32 worldX, S32 worldZ) {
   S32 solidCount = 0;
   solidCount += !isTransparentAtCube(getGlobalCubeAtWorldSpacePosition(x + worldX - 1, y, z + worldZ)) && !shouldCave(x + worldX - 1, y, z + worldZ);
   solidCount += !isTransparentAtCube(getGlobalCubeAtWorldSpacePosition(x + worldX + 1, y, z + worldZ)) && !shouldCave(x + worldX + 1, y, z + worldZ);
   solidCount += !isTransparentAtCube(getGlobalCubeAtWorldSpacePosition(x + worldX, y - 1, z + worldZ)) && !shouldCave(x + worldX, y - 1, z + worldZ);
   solidCount += !isTransparentAtCube(getGlobalCubeAtWorldSpacePosition(x + worldX, y + 1, z + worldZ)) && !shouldCave(x + worldX, y + 1, z + worldZ);
   solidCount += !isTransparentAtCube(getGlobalCubeAtWorldSpacePosition(x + worldX, y, z - 1 + worldZ)) && !shouldCave(x + worldX, y, z - 1 + worldZ);
   solidCount += !isTransparentAtCube(getGlobalCubeAtWorldSpacePosition(x + worldX, y, z + 1 + worldZ)) && !shouldCave(x + worldX, y, z + 1 + worldZ);
   return solidCount;
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

   F64 stretchFactor = 20.0;

   for (S32 x = 0; x < CHUNK_WIDTH; ++x) {
      for (S32 z = 0; z < CHUNK_WIDTH; ++z) {
         // calculate height for each cube.
         // Taking absolute value will allow for only 0-1 scaling.
         // also make sure to use the world coordinates

         // Smoothen the noise based on 5 blocks surrounding it.
         F64 noise = (open_simplex_noise2(osn, (F64)(x + worldX) / stretchFactor, (F64)(z + worldZ) / stretchFactor)) * 10.0;
         for (S32 i = -5; i < 5; ++i) {
            for (S32 j = -5; j < 5; ++j) {
               noise += (open_simplex_noise2(osn, (F64)(x + i + worldX) / (stretchFactor + i), (F64)(z + j + worldZ) / (stretchFactor + j)) * (10.0 + j)) / 2.0f;
            }
            noise /= 10.f;
         }
         //F64 noise = fabs(open_simplex_noise2(osn, (F64)(x + worldX) / stretchFactor, (F64)(z + worldZ) / stretchFactor) * 10.0);
         S32 height = (S32)(noise) + 70.0f; // 70 as base height.

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

void generateCavesAndStructures(S32 chunkX, S32 chunkZ, S32 worldX, S32 worldZ) {

   Chunk *chunk = getChunkAt(chunkX, chunkZ);
   Cube *cubeData = chunk->cubeData;

   // Generate caves.
   for (S32 x = 0; x < CHUNK_WIDTH; ++x) {
      for (S32 z = 0; z < CHUNK_WIDTH; ++z) {
         for (S32 y = 0; y < MAX_CHUNK_HEIGHT; ++y) {
            Cube *c = getCubeAt(cubeData, x, y, z);
            if (c->material == Material_Bedrock)
               continue;
            if (c->material == Material_Air)
               break;

            if (shouldCave(x + worldX, y, z + worldZ)) {
               // Perform smothing.
               S32 solidCount = solidCubesAroundCubeAt(x, y, z, worldX, worldZ);
               if (solidCount < 4) {
                  // It's a cave, carve out air.
                  c->material = Material_Air;
               }
            }
         }
      }
   }

   // Generate Trees
   for (S32 x = 0; x < CHUNK_WIDTH; ++x) {
      for (S32 z = 0; z < CHUNK_WIDTH; ++z) {
         // Find the height. Skip over anything that isn't the height.
         // and we only care about grass.
         S32 height = MAX_CHUNK_HEIGHT - 1;
         for (; height >= 0; --height) {
            if (getCubeAt(cubeData, x, height, z)->material != Material_Air) {
               break;
            }
         }

         if (getCubeAt(cubeData, x, height, z)->material == Material_Grass) {
            // Lets generate some trees.
            //
            // Also, a tree only has a 1/10 chance of spawning on this block.
            S32 posX = x;
            S32 posZ = z;
            if (open_simplex_noise2(osn, (F64)posX + worldX, (F64)posZ + worldZ) >= 0.8) {
               getCubeAt(cubeData, x, height + 1, z)->material = Material_Wood_Trunk;
               getCubeAt(cubeData, x, height + 2, z)->material = Material_Wood_Trunk;
               getCubeAt(cubeData, x, height + 3, z)->material = Material_Wood_Trunk;
               for (S32 xxx = x - 3; xxx < x + 3; ++xxx) {
                  if (xxx >= CHUNK_WIDTH)
                     break;
                  else if (xxx < 0)
                     continue;
                  for (S32 zzz = z - 3; zzz < z + 3; ++zzz) {
                     if (zzz >= CHUNK_WIDTH)
                        break;
                     else if (zzz < 0)
                        continue;
                     getCubeAt(cubeData, xxx, height + 4, zzz)->material = Material_Leaves;
                  }
               }
            }
         }
      }
   }
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
               Vec3 localPos;
               localPos.x = (F32)x;
               localPos.y = (F32)y;
               localPos.z = (F32)z;

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

               if (x == 0 && chunkX > -worldSize) {
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
               if (z == 0 && chunkZ > -worldSize) {
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

               if (y >= (MAX_CHUNK_HEIGHT - 1) || isTransparent(cubeData, x, y + 1, z))
                  buildFace(chunk, i, CubeSides_Up, material, localPos);

               // If this is grass, bottom has to be dirt.

               if (y == 0 || isTransparent(cubeData, x, y - 1, z))
                  buildFace(chunk, i, CubeSides_Down, (material == Material_Grass ? Material_Dirt : material), localPos);

               // After we built the top, this is a special case for grass.
               // If we are actually building grass sides it has to be special.
               if (material == Material_Grass)
                  material = Material_Grass_Side;

               if ((!isOpaqueNegativeX && x == 0) || (x > 0 && isTransparent(cubeData, x - 1, y, z)))
                  buildFace(chunk, i, CubeSides_West, material, localPos);

               if ((!isOpaquePositiveX && x >= (CHUNK_WIDTH - 1)) || (x < (CHUNK_WIDTH - 1) && isTransparent(cubeData, x + 1, y, z)))
                  buildFace(chunk, i, CubeSides_East, material, localPos);

               if ((!isOpaqueNegativeZ && z == 0) || (z > 0 && isTransparent(cubeData, x, y, z - 1)))
                  buildFace(chunk, i, CubeSides_South, material, localPos);

               if ((!isOpaquePositiveZ && z >= (CHUNK_WIDTH - 1)) || (z < (CHUNK_WIDTH - 1) && isTransparent(cubeData, x, y, z + 1)))
                  buildFace(chunk, i, CubeSides_North, material, localPos);
            }
         }
      }
   }
}


GLuint singleBufferCubeVBO;
GLuint singleBufferCubeIBO;

void uploadGeometryToGL() {
   // *Single threaded*
   //
   // Upload to the GL
   // Now that we generated all of the geometry, upload to the GL.
   // Note if a chunk has no geometry we don't create a vbo
   //
   // TODO: use VAO if extension is supported??
   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
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

   // Single buffer cube vbo/ibo
   glGenBuffers(1, &singleBufferCubeVBO);
   glBindBuffer(GL_ARRAY_BUFFER, singleBufferCubeVBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(F32) * 6 * 4 * 4, cubes, GL_STATIC_DRAW);

   glGenBuffers(1, &singleBufferCubeIBO);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, singleBufferCubeIBO);
   
   S32 indices[36];
   S32 in = 0;
   for (S32 i = 0; i < 36; i += 6) {
      indices[i] = in;
      indices[i + 1] = in + 2;
      indices[i + 2] = in + 1;
      indices[i + 3] = in;
      indices[i + 4] = in + 3;
      indices[i + 5] = in + 2;

      in += 4;
   }
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(S32) * 36, indices, GL_STATIC_DRAW);
}

int gVisibleChunks = 0;
int gTotalVisibleChunks = 0;
int gTotalChunks = 0;

S32 pickerShaderProjMatrixLoc;
S32 pickerShaderModelMatrixLoc;
GLuint pickerProgram;

void initWorld() {
   // Only 2 mip levels.
   bool ret = createTexture2D("Assets/block_atlas.png", 4, 2, &textureAtlas);
   if (!ret) {
      exit(-3);
   }

   // Create shader
   generateShaderProgram("Shaders/basic.vert", "Shaders/basic.frag", &program);
   projMatrixLoc = glGetUniformLocation(program, "projViewMatrix");
   modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
   textureLoc = glGetUniformLocation(program, "textureAtlas");

   // Create shader for picker
   generateShaderProgram("Shaders/red.vert", "Shaders/red.frag", &pickerProgram);
   pickerShaderProjMatrixLoc = glGetUniformLocation(program, "projViewMatrix");
   pickerShaderModelMatrixLoc = glGetUniformLocation(program, "modelMatrix");

   open_simplex_noise((U64)0xDEADBEEF, &osn);

   // world grid
   gChunkWorld = (Chunk*)calloc((worldSize * 2) * (worldSize * 2), sizeof(Chunk));
   gTotalChunks = worldSize * 2 * worldSize * 2 * CHUNK_SPLITS;

   // Easilly put each chunk in a thread in here.
   // nothing OpenGL, all calculation and world generation.
//#pragma omp parallel for
   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
         // World position calcuation before passing.
         generateWorld(x, z, x * CHUNK_WIDTH, z * CHUNK_WIDTH);
      }
   }

   // TODO MULTITHREADED: sync here before generating the Geometry.

   // Generate caves and tree
//#pragma omp parallel for
   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
         generateCavesAndStructures(x, z, x * CHUNK_WIDTH, z * CHUNK_WIDTH);
      }
   }

   // Easilly put each chunk in a thread in here.
   // nothing OpenGL, all calculation and world generation.
//#pragma omp parallel for
   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
         generateGeometry(x, z);
      }
   }

   // TODO MULTITHREADED: sync here before GL upload.

   uploadGeometryToGL();
}

void freeWorld() {
   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
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

bool orthoFlag = false;

void renderWorld(F32 dt) {
   // Set GL State
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);

   glUseProgram(program);

   // proj/view matrix
   mat4 proj, view, projView;

   if (inputGetKeyStatus(KEY_V) == PRESSED)
      orthoFlag = true;
   else
      orthoFlag = false;

   // For debugging culling
   if (!orthoFlag) {
      getCurrentProjMatrix(&proj);
      getCurrentViewMatrix(&view);
   } else {
      glm_ortho(-256.0f, 256.0f, -256.0f / (1440.f / 900.f), 256.0f / (1440.f / 900.f), -200.0f, 200.0f, proj);
      Vec3 eye = create_vec3(0.0f, 0.0f, 0.0f);
      Vec3 center = create_vec3(0.0f, -1.0f, 0.0f);
      Vec3 up = create_vec3(1.0f, 0.0f, 0.0f);
      glm_lookat(eye.vec, center.vec, up.vec, view);
   }
   glm_mat4_mul(proj, view, projView);
   glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &(projView[0][0]));

   // Bind our texture atlas to texture unit 0
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, textureAtlas.glId);
   glUniform1i(textureLoc, 0);

   gVisibleChunks = 0;
   gTotalVisibleChunks = 0;

   Frustum frustum;
   getCameraFrustum(&frustum);

   for (S32 x = -worldSize; x < worldSize; ++x) {
      for (S32 z = -worldSize; z < worldSize; ++z) {
         Chunk *c = getChunkAt(x, z);
         for (S32 i = 0; i < CHUNK_SPLITS; ++i) {
            if (c->renderChunks[i].vertexCount > 0) {
               gTotalVisibleChunks++;

               // Set position.
               // Center y pos should actually be RENDER_CHUNK_HEIGHT * i
               // but pos should always be 0 for y since the pos is baked into the y coord.
               Vec3 pos = create_vec3(x * CHUNK_WIDTH, 0, z * CHUNK_WIDTH);
               Vec3 center;
               Vec3 halfExtents = create_vec3(CHUNK_WIDTH / 2.0f, RENDER_CHUNK_HEIGHT / 2.0f, CHUNK_WIDTH / 2.0f);
               glm_vec_add(pos.vec, halfExtents.vec, center.vec);
               center.y += (F32)(i * RENDER_CHUNK_HEIGHT); // We add since we already have RENDER_CHUNK_HEIGHT / 2.0

               if (FrustumCullSquareBox(&frustum, center, CHUNK_WIDTH / 2.0f)) {
                  mat4 modelMatrix;
                  glm_mat4_identity(modelMatrix);
                  glm_translate(modelMatrix, pos.vec);
                  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &(modelMatrix[0][0]));
                  glBindBuffer(GL_ARRAY_BUFFER, c->renderChunks[i].vbo);
                  glEnableVertexAttribArray(0);
                  glEnableVertexAttribArray(1);
                  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GPUVertex), (void*)offsetof(GPUVertex, position));
                  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GPUVertex), (void*)offsetof(GPUVertex, uvx));
                  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->renderChunks[i].ibo);
                  glDrawElements(GL_TRIANGLES, (GLsizei)c->renderChunks[i].indiceCount, GL_UNSIGNED_INT, (void*)0);
                  glDisableVertexAttribArray(0);
                  glDisableVertexAttribArray(1);

                  gVisibleChunks++;
               }
            }
         }
      }
   }

   // Do our raycast to screen world.
   Vec3 rayOrigin, rayDir;
   raycastScreenToWorld(1400.0f / 2.0f, 900.0f / 2.0f, 1400.0f, 900.0f, projView, &rayOrigin, &rayDir);

   // Check to see if we have something within 8 blocks away.
   Vec3 point = rayOrigin;
   Vec3 scalar;
   glm_vec_scale(rayDir.vec, 0.1f, scalar.vec);
   for (S32 i = 0; i < 800; ++i) {
      glm_vec_add(point.vec, scalar.vec, point.vec);

      Vec3 pos = create_vec3(roundf(point.x), roundf(point.y), roundf(point.z));

      // Calculate chunk at point.
      Cube *c = getGlobalCubeAtWorldSpacePosition((S32)pos.x, (S32)pos.y, (S32)pos.z);
      if (c->material != Material_Air) {
         glUseProgram(pickerProgram);

         glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &(projView[0][0]));
         mat4 modelMatrix;
         glm_mat4_identity(modelMatrix);
         glm_translate(modelMatrix, pos.vec);
         glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &(modelMatrix[0][0]));

         glBindBuffer(GL_ARRAY_BUFFER, singleBufferCubeVBO);
         glEnableVertexAttribArray(0);
         glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(F32) * 4, (void*)0);
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, singleBufferCubeIBO);
         glDrawElements(GL_TRIANGLES, (GLsizei)36, GL_UNSIGNED_INT, (void*)0);
         glDisableVertexAttribArray(0);
         break;
      }
   }
}
