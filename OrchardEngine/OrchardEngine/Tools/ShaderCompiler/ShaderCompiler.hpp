#pragma once

#include <string>
#include <vector>

namespace Orchard::Tools {

struct ShaderCompileOptions {
    std::string entryPoint = "main";
    std::vector<std::string> defines;
    std::vector<std::string> includePaths;
    bool generateDebugInfo = false;
    bool optimize = true;
    std::string targetVersion = "metal3.0";
};

struct ShaderCompileResult {
    bool success = false;
    std::vector<uint8_t> bytecode;
    std::string errorMessage;
    std::vector<std::string> warnings;
};

class ShaderCompiler {
public:
    static ShaderCompileResult CompileFromFile(
        const std::string& sourcePath,
        const ShaderCompileOptions& options = ShaderCompileOptions()
    );
    
    static ShaderCompileResult CompileFromSource(
        const std::string& source,
        const std::string& filename,
        const ShaderCompileOptions& options = ShaderCompileOptions()
    );
    
    static bool CompileShaderLibrary(
        const std::vector<std::string>& shaderPaths,
        const std::string& outputPath,
        const ShaderCompileOptions& options = ShaderCompileOptions()
    );
    
private:
    static std::string PreprocessShader(
        const std::string& source,
        const ShaderCompileOptions& options
    );
    
    static bool InvokeMetalCompiler(
        const std::string& source,
        const std::string& outputPath,
        const ShaderCompileOptions& options,
        std::string& errorOutput
    );
};

}
