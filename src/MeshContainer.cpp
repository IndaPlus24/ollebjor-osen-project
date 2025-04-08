#include "MeshContainer.hpp"

#include "bx/debug.h"
#include <iostream>
#include <unordered_map>
#include "Vertex.hpp"

namespace std {
template <> struct hash<Vertex> {
    size_t operator()(Vertex const& vertex) const {
        return ((hash<glm::vec3>()(vertex.pos) ^
                 (hash<glm::vec3>()(vertex.normal) << 1)) >>
                1) ^
               (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};
} // namespace std

MeshContainer::MeshContainer(const std::string& path) : path(path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &warn,
                                path.c_str(), nullptr, true);
    if (!ret) {
        std::cerr << "Error loading OBJ file: " << path << std::endl;
    }

    bx::debugPrintf("Mesh loading: vertices: %d, normals: %d, shapes: %d",
                    attrib.vertices.size() / 3, attrib.normals.size() / 3,
                    shapes.size());
    std::unordered_map<Vertex, uint32_t> vertexMap;

    for (const auto& shape : shapes) {
        bx::debugPrintf("Mesh loading: Shape loading: name: %s",
                        shape.name.c_str());
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex;
            vertex.pos = {attrib.vertices[3 * index.vertex_index],
                          attrib.vertices[3 * index.vertex_index + 1],
                          -attrib.vertices[3 * index.vertex_index + 2]};
            if (index.normal_index >= 0) {
                vertex.normal = {attrib.normals[3 * index.normal_index],
                                 attrib.normals[3 * index.normal_index + 1],
                                 attrib.normals[3 * index.normal_index + 2]};
            } else {
                vertex.normal = {0.0f, 1.0f, 0.0f}; // Fallback
            }

            if (index.texcoord_index >= 0) {
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index],
                    attrib.texcoords[2 * index.texcoord_index + 1]};
            } else {
                vertex.texCoord = {0.0f, 0.0f}; // Fallback
            }

            if (vertexMap.count(vertex) == 0) {
                vertexMap[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(vertexMap[vertex]);
        }
    }
}

MeshContainer::~MeshContainer() {}
