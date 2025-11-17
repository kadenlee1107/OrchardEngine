#pragma once

#include <string>
#include <memory>
#include <vector>

namespace Orchard::Tools {

class Asset;

enum class AssetType {
    Unknown,
    Mesh,
    Texture,
    Audio,
    Material,
    Shader,
    Scene
};

struct ImportOptions {
    bool generateMipmaps = true;
    bool compressTextures = true;
    bool optimizeMeshes = true;
    bool calculateNormals = true;
    bool calculateTangents = true;
    float scale = 1.0f;
};

class AssetImporter {
public:
    static AssetType DetectAssetType(const std::string& path);
    
    static std::shared_ptr<Asset> Import(
        const std::string& sourcePath,
        const std::string& destinationPath,
        const ImportOptions& options = ImportOptions()
    );
    
private:
    static std::shared_ptr<Asset> ImportMesh(
        const std::string& path,
        const ImportOptions& options
    );
    
    static std::shared_ptr<Asset> ImportTexture(
        const std::string& path,
        const ImportOptions& options
    );
    
    static std::shared_ptr<Asset> ImportAudio(
        const std::string& path,
        const ImportOptions& options
    );
};

}
