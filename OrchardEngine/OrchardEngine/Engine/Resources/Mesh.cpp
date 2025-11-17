#include "Mesh.hpp"
#include <iostream>
#include <cstring>

namespace Orchard {

Mesh::Mesh() {
}

Mesh::~Mesh() {
}

bool Mesh::LoadFromFile(const std::string& path) {
    std::cout << "Loading mesh: " << path << std::endl;
    m_Path = path;
    return false;
}

void Mesh::AddSubMesh(const SubMesh& submesh) {
    m_SubMeshes.push_back(submesh);
}

void Mesh::CalculateNormals() {
    for (auto& submesh : m_SubMeshes) {
        for (auto& vertex : submesh.vertices) {
            vertex.normal = Math::Vector3(0, 0, 0);
        }
        
        for (size_t i = 0; i < submesh.indices.size(); i += 3) {
            uint32_t i0 = submesh.indices[i];
            uint32_t i1 = submesh.indices[i + 1];
            uint32_t i2 = submesh.indices[i + 2];
            
            Math::Vector3 v0 = submesh.vertices[i0].position;
            Math::Vector3 v1 = submesh.vertices[i1].position;
            Math::Vector3 v2 = submesh.vertices[i2].position;
            
            Math::Vector3 edge1 = v1 - v0;
            Math::Vector3 edge2 = v2 - v0;
            Math::Vector3 normal = Math::Vector3::Cross(edge1, edge2);
            
            submesh.vertices[i0].normal += normal;
            submesh.vertices[i1].normal += normal;
            submesh.vertices[i2].normal += normal;
        }
        
        for (auto& vertex : submesh.vertices) {
            vertex.normal.Normalize();
        }
    }
}

void Mesh::CalculateTangents() {
    for (auto& submesh : m_SubMeshes) {
        for (auto& vertex : submesh.vertices) {
            vertex.tangent = Math::Vector3(0, 0, 0);
        }
        
        for (size_t i = 0; i < submesh.indices.size(); i += 3) {
            uint32_t i0 = submesh.indices[i];
            uint32_t i1 = submesh.indices[i + 1];
            uint32_t i2 = submesh.indices[i + 2];
            
            Vertex& v0 = submesh.vertices[i0];
            Vertex& v1 = submesh.vertices[i1];
            Vertex& v2 = submesh.vertices[i2];
            
            Math::Vector3 edge1 = v1.position - v0.position;
            Math::Vector3 edge2 = v2.position - v0.position;
            
            Math::Vector2 deltaUV1(v1.texcoord.x - v0.texcoord.x, v1.texcoord.y - v0.texcoord.y);
            Math::Vector2 deltaUV2(v2.texcoord.x - v0.texcoord.x, v2.texcoord.y - v0.texcoord.y);
            
            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            
            Math::Vector3 tangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            
            v0.tangent += tangent;
            v1.tangent += tangent;
            v2.tangent += tangent;
        }
        
        for (auto& vertex : submesh.vertices) {
            vertex.tangent.Normalize();
        }
    }
}

void Mesh::UploadToGPU() {
#ifdef __APPLE__
    // Metal GPU upload would go here
#endif
}

std::shared_ptr<Mesh> MeshImporter::ImportFBX(const std::string& path) {
    std::cout << "Importing FBX: " << path << std::endl;
    return nullptr;
}

std::shared_ptr<Mesh> MeshImporter::ImportOBJ(const std::string& path) {
    std::cout << "Importing OBJ: " << path << std::endl;
    return nullptr;
}

std::shared_ptr<Mesh> MeshImporter::ImportUSD(const std::string& path) {
    std::cout << "Importing USD: " << path << std::endl;
    return nullptr;
}

}
