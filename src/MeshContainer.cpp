#include "MeshContainer.hpp"

#include "bx/debug.h"
#include <unordered_map>
#include "Vertex.hpp"

namespace std {
template <> struct hash<Vertex> {
    size_t operator()(Vertex const& vertex) const {
        return ((hash<glm::vec3>()(vertex.pos) ^
                 (hash<glm::vec3>()(vertex.normal) << 1)) >>
                1) ^
               (hash<glm::vec2>()(vertex.texCoord) << 1) ^
               (hash<glm::vec3>()(vertex.tangent) << 1);
    }
};
} // namespace std

// Calculate tangents for a triangle
void calculateTangent(const glm::vec3& pos1, const glm::vec3& pos2,
                      const glm::vec3& pos3, const glm::vec2& uv1,
                      const glm::vec2& uv2, const glm::vec2& uv3,
                      glm::vec3& tangent) {

    // Calculate edges of the triangle
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;

    // Calculate delta UV coordinates
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    // Calculate tangent vector
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    // Normalize the tangent
    tangent = glm::normalize(tangent);
}

MeshContainer::MeshContainer(const std::string& path) : path(path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                          path.c_str());
    if (!ret) {
        bx::debugPrintf("Failed to load OBJ file: %s %s\n", err.c_str(), warn.c_str());
        return;
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    // Temporary storage to accumulate tangents for vertices that appear in
    // multiple triangles
    std::unordered_map<glm::vec3, glm::vec3> accumulatedTangents;

    // First pass: Calculate tangents for each triangle
    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = shape.mesh.num_face_vertices[f];

            // Skip if not a triangle
            if (fv != 3) {
                index_offset += fv;
                continue;
            }

            // Get vertices of the triangle
            std::vector<glm::vec3> positions(3);
            std::vector<glm::vec2> texCoords(3);

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                positions[v] = {attrib.vertices[3 * idx.vertex_index + 0],
                                attrib.vertices[3 * idx.vertex_index + 1],
                                attrib.vertices[3 * idx.vertex_index + 2]};

                texCoords[v] = {
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * idx.texcoord_index + 1]};
            }

            // Calculate tangent for this triangle
            glm::vec3 tangent;
            calculateTangent(positions[0], positions[1], positions[2],
                             texCoords[0], texCoords[1], texCoords[2], tangent);

            // Accumulate tangents by vertex position (for averaging later)
            for (size_t v = 0; v < 3; v++) {
                if (accumulatedTangents.find(positions[v]) ==
                    accumulatedTangents.end()) {
                    accumulatedTangents[positions[v]] = tangent;
                } else {
                    accumulatedTangents[positions[v]] += tangent;
                }
            }

            index_offset += fv;
        }
    }

    // Normalize all accumulated tangents
    for (auto& pair : accumulatedTangents) {
        pair.second = glm::normalize(pair.second);
    }

    // Second pass: Build vertex and index buffers with tangents
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2],
            };

            vertex.normal = {
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2],
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
            };

            // Assign the pre-calculated tangent
            if (accumulatedTangents.find(vertex.pos) !=
                accumulatedTangents.end()) {
                vertex.tangent = accumulatedTangents[vertex.pos];
            } else {
                // Fallback if no tangent was calculated
                vertex.tangent = {1.0f, 0.0f, 0.0f};
                bx::debugPrintf(
                    "Warning: No tangent found for vertex at position: %f, %f, "
                    "%f\n", vertex.pos.x, vertex.pos.y, vertex.pos.z);
            }

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

MeshContainer::MeshContainer(MeshContainer&& other) noexcept {
    path = std::move(other.path);
    vertices = std::move(other.vertices);
    indices = std::move(other.indices);
    other.vertices.clear();
    other.indices.clear();
}

MeshContainer& MeshContainer::operator=(MeshContainer&& other) noexcept {
    if (this != &other) {
        path = std::move(other.path);
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        other.vertices.clear();
        other.indices.clear();
    }
    return *this;
}
MeshContainer::~MeshContainer() {}
