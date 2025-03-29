#pragma once

#include "Vertex.hpp"

struct PrimitiveCube {
    const Vertex vertices[8];
    const uint16_t indices[36] = {
        0, 1, 2, // 0
        1, 3, 2, //
        4, 6, 5, // 2
        5, 6, 7, //
        0, 2, 4, // 4
        4, 2, 6, //
        1, 5, 3, // 6
        5, 7, 3, //
        0, 4, 1, // 8
        4, 5, 1, //
        2, 3, 6, // 10
        6, 3, 7, //
    };

    inline PrimitiveCube(uint32_t abgr)
        : vertices{
              {{-1.0f, 1.0f, 1.0f}, abgr},   {{1.0f, 1.0f, 1.0f}, abgr},
              {{-1.0f, -1.0f, 1.0f}, abgr},  {{1.0f, -1.0f, 1.0f}, abgr},
              {{-1.0f, 1.0f, -1.0f}, abgr},  {{1.0f, 1.0f, -1.0f}, abgr},
              {{-1.0f, -1.0f, -1.0f}, abgr}, {{1.0f, -1.0f, -1.0f}, abgr},
          } {};
};

// plane with face up
struct PrimitiveQuad {
    const Vertex vertices[4];
    const uint16_t indices[6] = {
        0, 2, 1, // 0
        1, 2, 3, //
    };

    inline PrimitiveQuad(uint32_t abgr)
        : vertices{
              {{-1.0f, 0.0f, 1.0f}, abgr},
              {{1.0f, 0.0f, 1.0f}, abgr},
              {{-1.0f, 0.0f, -1.0f}, abgr},
              {{1.0f, 0.0f, -1.0f}, abgr},
          } {};
};

// Icosphere
struct PrimitiveIcosphere {
    const Vertex vertices[12];
    const uint16_t indices[60] = {
        1,  6,  3,  //
        0,  6,  1,  //
        3,  4,  1,  //
        3,  6,  8,  //
        6,  0,  7,  //
        2,  0,  1,  //
        4,  3,  5,  //
        4,  2,  1,  //
        7,  8,  6,  //
        5,  3,  8,  //
        0,  11, 7,  //
        11, 0,  2,  //
        4,  5,  10, //
        10, 2,  4,  //
        8,  7,  9, //
        8,  9, 5,  //
        7,  11, 9, //
        11, 2,  10, //
        10, 5,  9, //
        11, 10, 9, //
    };

    inline PrimitiveIcosphere(uint32_t abgr)
        : vertices{
              {{-1.0f, 0.0f, -1.61803f}, abgr},
              {{0.0f, 1.61803f, -1.0f}, abgr},
              {{1.0f, 0.0f, -1.61803f}, abgr},
              {{0.0f, 1.61803f, 1.0f}, abgr},
              {{1.61803f, 1.0f, 0.0f}, abgr},
              {{1.0f, 0.0f, 1.61803f}, abgr},
              {{-1.61803f, 1.0f, 0.0f}, abgr},
              {{-1.61803f, -1.0f, 0.0f}, abgr},
              {{-1.0f, 0.0f, 1.61803f}, abgr},
              {{0.0f, -1.61803f, 1.0f}, abgr},
              {{1.61803f, -1.0f, 0.0f}, abgr},
              {{0.0f, -1.61803f, -1.0f}, abgr},
          } {};
};
