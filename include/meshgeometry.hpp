#pragma once

#include <vector>
#include "graphics.h"

namespace MeshGen {

    // Function declarations
    Mesh CreateQuad(float width = 1.0f, float height = 1.0f);
    Mesh CreateCube(float size = 1.0f); 
    Mesh CreateSphere(float radius = 1.0f, unsigned int segments = 24, unsigned int rings = 24);

};