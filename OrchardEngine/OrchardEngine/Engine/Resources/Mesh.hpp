#pragma once

#include "../Math/Vector.hpp"
#include "../Utils/UUID.hpp"
#include "../Core/ResourceManager.hpp"
#include <vector>
#include <string>

namespace Orchard {

struct Vertex {
    Math::Vector3 position;
    Math::Vector3 normal;
    Math::Vector2 texcoord;
    Math::Vector3 tangent;
    
    Vertex() 
        : position(0, 0, 0)
        , normal(0, 1, 0)
        , texcoord(0, 0)
        , tangent(1, 0, 0)
    {}
};

struct SubMesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t materialIndex = 0;
    
#ifdef __APPLE__
    id<MTLBuffer> vertexBuffer = nil;
    id<MTLBuffer> indexBuffer = nil;
#endif
};

class Mesh : public Resource {
public:
    Mesh();
    ~Mesh();
    
    bool LoadFromFile(const std::string& path);
    
    void AddSubMesh(const SubMesh& submesh);
    const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }
    
    void CalculateNormals();
    void CalculateTangents();
    
    void UploadToGPU();
    
private:
    std::vector<SubMesh> m_SubMeshes;
};

class MeshImporter {
public:
    static std::shared_ptr<Mesh> ImportFBX(const std::string& path);
    static std::shared_ptr<Mesh> ImportOBJ(const std::string& path);
    static std::shared_ptr<Mesh> ImportUSD(const std::string& path);
};

}
