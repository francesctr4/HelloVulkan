#pragma once

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs)

#pragma comment (lib, "Source/External/Assimp/libx64/assimp-vc143-mt.lib")

#include "External/Assimp/include/cimport.h"
#include "External/Assimp/include/scene.h"
#include "External/Assimp/include/postprocess.h"