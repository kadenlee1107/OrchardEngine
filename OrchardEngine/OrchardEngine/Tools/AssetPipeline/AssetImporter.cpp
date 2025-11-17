#include "AssetImporter.hpp"
#include <iostream>
#include <algorithm>

namespace Orchard::Tools {

AssetType AssetImporter::DetectAssetType(const std::string& path) {
    std::string extension = path.substr(path.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "fbx" || extension == "obj" || extension == "usd") {
        return AssetType::Mesh;
    }
    
    if (extension == "png" || extension == "jpg" || extension == "jpeg" || 
        extension == "exr" || extension == "hdr") {
        return AssetType::Texture;
    }
    
    if (extension == "wav" || extension == "mp3" || extension == "flac") {
        return AssetType::Audio;
    }
    
    if (extension == "mat") {
        return AssetType::Material;
    }
    
    if (extension == "metal" || extension == "shader") {
        return AssetType::Shader;
    }
    
    if (extension == "scene") {
        return AssetType::Scene;
    }
    
    return AssetType::Unknown;
}

std::shared_ptr<Asset> AssetImporter::Import(
    const std::string& sourcePath,
    const std::string& destinationPath,
    const ImportOptions& options) {
    
    AssetType type = DetectAssetType(sourcePath);
    
    switch (type) {
        case AssetType::Mesh:
            return ImportMesh(sourcePath, options);
        case AssetType::Texture:
            return ImportTexture(sourcePath, options);
        case AssetType::Audio:
            return ImportAudio(sourcePath, options);
        default:
            std::cerr << "Unsupported asset type: " << sourcePath << std::endl;
            return nullptr;
    }
}

std::shared_ptr<Asset> AssetImporter::ImportMesh(
    const std::string& path,
    const ImportOptions& options) {
    
    std::cout << "Importing mesh: " << path << std::endl;
    return nullptr;
}

std::shared_ptr<Asset> AssetImporter::ImportTexture(
    const std::string& path,
    const ImportOptions& options) {
    
    std::cout << "Importing texture: " << path << std::endl;
    return nullptr;
}

std::shared_ptr<Asset> AssetImporter::ImportAudio(
    const std::string& path,
    const ImportOptions& options) {
    
    std::cout << "Importing audio: " << path << std::endl;
    return nullptr;
}

}
