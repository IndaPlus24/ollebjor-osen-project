#pragma once

#include "Vertex.hpp"

struct PrimitiveCube {
    const Vertex vertices[8];
    const uint16_t indices[36] = {
        0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 2, 4, 4, 2, 6,
        1, 5, 3, 5, 7, 3, 0, 4, 1, 4, 5, 1, 2, 3, 6, 6, 3, 7,
    };

    inline PrimitiveCube(uint32_t abgr)
        : vertices{
              {{-1.0f, 1.0f, 1.0f}, abgr},   {{1.0f, 1.0f, 1.0f}, abgr},
              {{-1.0f, -1.0f, 1.0f}, abgr},  {{1.0f, -1.0f, 1.0f}, abgr},
              {{-1.0f, 1.0f, -1.0f}, abgr},  {{1.0f, 1.0f, -1.0f}, abgr},
              {{-1.0f, -1.0f, -1.0f}, abgr}, {{1.0f, -1.0f, -1.0f}, abgr},
          } {};
};

struct PrimitiveQuad {
    const Vertex vertices[4];
    const uint16_t indices[6] = {0, 1, 2, 1, 3, 2};

    inline PrimitiveQuad(uint32_t abgr)
        : vertices{
              {{-1.0f, -1.0f, 0.0f}, abgr},
              {{1.0f, -1.0f, 0.0f}, abgr},
              {{-1.0f, 1.0f, 0.0f}, abgr},
              {{1.0f, 1.0f, 0.0f}, abgr},
          } {};
};

struct PrimitiveIcosphere {
    const Vertex vertices[12];
    const uint16_t indices[59] = {
        0, 1, 4, 0, 4, 9, 9,  4, 5, 4,  8, 5, 4,  1,  8,  8, 1, 10, 8,  10,
        3, 5, 8, 3, 5, 3, 2,  2, 3, 7,  7, 3, 10, 7,  10, 6, 7, 6,  11, 11,
        6, 0, 0, 6, 1, 6, 10, 1, 9, 11, 0, 9, 2,  11, 9,  5, 2, 7,  11,
    };

    inline PrimitiveIcosphere(uint32_t abgr)
        : vertices{
              {{-0.525731f, 0.0f, 0.850651f}, abgr},
              {{0.525731f, 0.0f, 0.850651f}, abgr},
              {{-0.525731f, 0.0f, -0.850651f}, abgr},
              {{0.525731f, 0.0f, -0.850651f}, abgr},
              {{0.0f, 0.850651f, 0.525731f}, abgr},
              {{0.0f, 0.850651f, -0.525731f}, abgr},
              {{0.0f, -0.850651f, 0.525731f}, abgr},
              {{0.0f, -0.850651f, -0.525731f}, abgr},
              {{0.850651f, 0.525731f, 0.0f}, abgr},
              {{-0.850651f, 0.525731f, 0.0f}, abgr},
              {{0.850651f, -0.525731f, 0.0f}, abgr},
              {{-0.850651f, -0.525731f, 0.0f}, abgr},
          } {}
};
