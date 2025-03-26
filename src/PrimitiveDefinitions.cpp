#include "PrimitiveDefinitions.hpp"

#include "Vertex.hpp"

Vertex cube_vertices[] = {
    {{-1.0f, 1.0f, 1.0f}, 0xff000000},   {{1.0f, 1.0f, 1.0f}, 0xff0000ff},
    {{-1.0f, -1.0f, 1.0f}, 0xff00ff00},  {{1.0f, -1.0f, 1.0f}, 0xff00ffff},
    {{-1.0f, 1.0f, -1.0f}, 0xffff0000},  {{1.0f, 1.0f, -1.0f}, 0xffff00ff},
    {{-1.0f, -1.0f, -1.0f}, 0xffffff00}, {{1.0f, -1.0f, -1.0f}, 0xffffffff},
};

uint16_t cube_indices[] = {
    0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 2, 4, 4, 2, 6,
    1, 5, 3, 5, 7, 3, 0, 4, 1, 4, 5, 1, 2, 3, 6, 6, 3, 7,
};

Vertex quad_vertices[] = {
    {{-1.0f, -1.0f, 0.0f}, 0xff00ff00},  {{1.0f, -1.0f, 0.0f}, 0xff00ffff},
    {{-1.0f, 1.0f, 0.0f}, 0xffff0000},   {{1.0f, 1.0f, 0.0f}, 0xffff00ff},
};

uint16_t quad_indices[] = {
    0, 1, 2, 1, 3, 2,
};

Vertex icoshere_vertices[] = {
    {{-0.525731f, 0.0f, 0.850651f}, 0xff00ff00},  {{0.525731f, 0.0f, 0.850651f}, 0xff00ffff},
    {{-0.525731f, 0.0f, -0.850651f}, 0xffff0000}, {{0.525731f, 0.0f, -0.850651f}, 0xffff00ff},
    {{0.0f, 0.850651f, 0.525731f}, 0xffffff00},   {{0.0f, 0.850651f, -0.525731f}, 0xffffffff},
    {{0.0f, -0.850651f, 0.525731f}, 0xff0000ff},  {{0.0f, -0.850651f, -0.525731f}, 0xff00ff00},
    {{0.850651f, 0.525731f, 0.0f}, 0xff00ffff},   {{-0.850651f, 0.525731f, 0.0f}, 0xffff0000},
    {{0.850651f, -0.525731f, 0.0f}, 0xffff00ff},  {{-0.850651f, -0.525731f, 0.0f}, 0xffffff00},
};

uint16_t icoshere_indices[] = {
    0, 1, 4, 0, 4, 9, 9, 4, 5, 4, 8, 5, 4, 1, 8, 8, 1, 10, 8, 10, 3, 5, 8, 3, 5, 3, 2, 2, 3, 7, 7, 3, 10,
    7, 10, 6, 7, 6, 11, 11, 6, 0, 0, 6, 1, 6, 10, 1, 9, 11, 0, 9, 2, 11, 9, 5, 2, 7, 11,
};
