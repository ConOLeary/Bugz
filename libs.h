#pragma once

#include<iostream>
#include<fstream>
#include<string>
#include<vector>

#include<glew.h>
#include<glfw3.h>

//// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

#include<glm.hpp>
#include<vec2.hpp>
#include<vec3.hpp>
#include<vec4.hpp>
#include<mat4x4.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>

#include<SOIL2.h>

#include"Vertex.h"
#include"Primitives.h"
#include"Shader.h"
#include"Texture.h"
#include"Material.h"
#include"Mesh.h"
#include"MeshObj.h"
#include"Model.h"
#include"Light.h"

