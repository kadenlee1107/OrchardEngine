#!/bin/bash

echo "=================================="
echo "Orchard Engine Project Validator"
echo "=================================="
echo ""
echo "This is a macOS-native game engine that must be compiled on macOS with Xcode."
echo "This validation script checks that all project files are in place."
echo ""

TOTAL_FILES=0
MISSING_FILES=0

check_file() {
    if [ -f "$1" ]; then
        echo "✓ $1"
        ((TOTAL_FILES++))
    else
        echo "✗ MISSING: $1"
        ((MISSING_FILES++))
    fi
}

check_dir() {
    if [ -d "$1" ]; then
        echo "✓ $1/"
        ((TOTAL_FILES++))
    else
        echo "✗ MISSING DIR: $1/"
        ((MISSING_FILES++))
    fi
}

echo "Checking directory structure..."
echo ""

check_dir "OrchardEngine/Engine/Core"
check_dir "OrchardEngine/Engine/ECS"
check_dir "OrchardEngine/Engine/Math"
check_dir "OrchardEngine/Engine/Rendering/Metal"
check_dir "OrchardEngine/Engine/Physics"
check_dir "OrchardEngine/Engine/Audio"
check_dir "OrchardEngine/Editor"
check_dir "OrchardEngine/Samples"
check_dir "OrchardEngine/Tools"
check_dir "OrchardEngine/Documentation"

echo ""
echo "Checking core engine files..."
echo ""

check_file "OrchardEngine/Engine/Core/Engine.hpp"
check_file "OrchardEngine/Engine/Core/Engine.cpp"
check_file "OrchardEngine/Engine/Core/Memory.hpp"
check_file "OrchardEngine/Engine/Math/Vector.hpp"
check_file "OrchardEngine/Engine/Math/Matrix.hpp"
check_file "OrchardEngine/Engine/Math/Quaternion.hpp"
check_file "OrchardEngine/Engine/ECS/Entity.hpp"
check_file "OrchardEngine/Engine/ECS/World.hpp"

echo ""
echo "=================================="
echo "Validation Summary"
echo "=================================="
echo "Total items checked: $((TOTAL_FILES + MISSING_FILES))"
echo "Items found: $TOTAL_FILES"
echo "Missing items: $MISSING_FILES"
echo ""

if [ $MISSING_FILES -eq 0 ]; then
    echo "✓ Project structure is complete!"
else
    echo "✗ Some files are missing. Project may be incomplete."
fi

echo ""
echo "To compile this engine:"
echo "1. Transfer this project to a Mac with Apple Silicon (M1/M2/M3/M4)"
echo "2. Install Xcode 15+ with Command Line Tools"
echo "3. Run: cmake -B build -G Xcode"
echo "4. Open the generated Xcode project"
echo "5. Build and run from Xcode"
echo ""

find OrchardEngine -type f | wc -l | xargs -I {} echo "Total files in project: {}"
