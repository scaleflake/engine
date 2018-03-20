#ifndef PRECOMPILED_H
#define PRECOMPILED_H

#define GLEW_STATIC
#include <glew.h>

#include <glfw3.h>

// WINAPI
//#define NOGDI
//#include <Windows.h>

// STL
#include <memory>
using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;
#include <vector>
using std::vector;
#include <map>
using std::map;
#include <thread>

#include <string>
using std::string;
#include <fstream>
using std::ifstream;
#include <cassert>
// assert
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;

#define ENGINE_WIDTH 1600
#define ENGINE_HEIGHT 900

#include "Mathematics.h"
#include "Windows.hpp"
#include "OpenGL.hpp"
//#include "ObjectPool.hpp"

#include "Physics/Shape.h"
#include "Physics/Body.h"
#include "Physics/Algorithms.h"
#include "Physics/Collision.h"
#include "Physics/Collider.h"

#endif // PRECOMPILED_H
