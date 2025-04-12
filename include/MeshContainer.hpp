#pragma once

#include "tiny_obj_loader.h"
#include <bgfx/bgfx.h>
#include "Vertex.hpp"
#include <string>
#include <vector>

class MeshContainer {
  private:
    std::string path;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

  public:
    MeshContainer(const std::string& path);
    MeshContainer(MeshContainer&& other) noexcept;
    MeshContainer& operator=(MeshContainer&& other) noexcept;
    ~MeshContainer();

    inline const std::vector<Vertex>& GetVertices() const { return vertices; }
    inline const std::vector<uint32_t>& GetIndices() const { return indices; }

    inline void GetMeshData(const bgfx::Memory*& vertMem,
                     const bgfx::Memory*& indiMem) const {
        vertMem = bgfx::copy(vertices.data(), vertices.size() * sizeof(Vertex));
        indiMem = bgfx::copy(indices.data(), indices.size() * sizeof(uint32_t));
    }
};
