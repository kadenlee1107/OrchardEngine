#include "ShaderCompiler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace Orchard::Tools {

ShaderCompileResult ShaderCompiler::CompileFromFile(
    const std::string& sourcePath,
    const ShaderCompileOptions& options) {
    
    std::ifstream file(sourcePath);
    if (!file.is_open()) {
        ShaderCompileResult result;
        result.success = false;
        result.errorMessage = "Failed to open file: " + sourcePath;
        return result;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    return CompileFromSource(source, sourcePath, options);
}

ShaderCompileResult ShaderCompiler::CompileFromSource(
    const std::string& source,
    const std::string& filename,
    const ShaderCompileOptions& options) {
    
    ShaderCompileResult result;
    
    std::string preprocessed = PreprocessShader(source, options);
    
    std::string tempFile = "/tmp/shader_temp.metal";
    std::ofstream out(tempFile);
    out << preprocessed;
    out.close();
    
    std::string outputPath = "/tmp/shader_compiled.air";
    std::string errorOutput;
    
    if (InvokeMetalCompiler(preprocessed, outputPath, options, errorOutput)) {
        std::ifstream compiled(outputPath, std::ios::binary);
        if (compiled.is_open()) {
            compiled.seekg(0, std::ios::end);
            size_t size = compiled.tellg();
            compiled.seekg(0, std::ios::beg);
            
            result.bytecode.resize(size);
            compiled.read(reinterpret_cast<char*>(result.bytecode.data()), size);
            
            result.success = true;
        }
    } else {
        result.success = false;
        result.errorMessage = errorOutput;
    }
    
    return result;
}

bool ShaderCompiler::CompileShaderLibrary(
    const std::vector<std::string>& shaderPaths,
    const std::string& outputPath,
    const ShaderCompileOptions& options) {
    
    std::vector<std::string> airFiles;
    
    for (const auto& shaderPath : shaderPaths) {
        auto result = CompileFromFile(shaderPath, options);
        if (!result.success) {
            std::cerr << "Failed to compile: " << shaderPath << std::endl;
            std::cerr << result.errorMessage << std::endl;
            return false;
        }
        
        std::string airFile = shaderPath + ".air";
        std::ofstream out(airFile, std::ios::binary);
        out.write(reinterpret_cast<const char*>(result.bytecode.data()), result.bytecode.size());
        out.close();
        
        airFiles.push_back(airFile);
    }
    
    std::string command = "xcrun -sdk macosx metallib";
    for (const auto& airFile : airFiles) {
        command += " " + airFile;
    }
    command += " -o " + outputPath;
    
    int exitCode = system(command.c_str());
    
    return (exitCode == 0);
}

std::string ShaderCompiler::PreprocessShader(
    const std::string& source,
    const ShaderCompileOptions& options) {
    
    std::string result = source;
    
    for (const auto& define : options.defines) {
        result = "#define " + define + "\n" + result;
    }
    
    return result;
}

bool ShaderCompiler::InvokeMetalCompiler(
    const std::string& source,
    const std::string& outputPath,
    const ShaderCompileOptions& options,
    std::string& errorOutput) {
    
    std::string command = "xcrun -sdk macosx metal";
    
    if (options.optimize) {
        command += " -O2";
    }
    
    if (options.generateDebugInfo) {
        command += " -g";
    }
    
    command += " -c /tmp/shader_temp.metal -o " + outputPath;
    
    command += " 2>&1";
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        errorOutput = "Failed to invoke Metal compiler";
        return false;
    }
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        errorOutput += buffer;
    }
    
    int exitCode = pclose(pipe);
    
    return (exitCode == 0);
}

}
