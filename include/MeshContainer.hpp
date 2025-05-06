#pragma once

#include "Vertex.hpp"
#include <bgfx/bgfx.h>
#include <string>
#include <vector>
#include "tiny_obj_loader.h"

class MeshContainer {
  private:
    std::string path;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

  public:
    MeshContainer(std::string path, std::vector<Vertex> vertices,
                  std::vector<uint32_t> indices)
        : path(std::move(path)), vertices(std::move(vertices)),
          indices(std::move(indices)) {}
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

    inline const std::string& GetPath() const { return path; }
};
