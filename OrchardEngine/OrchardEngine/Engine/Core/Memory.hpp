#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>

namespace Orchard::Memory {

constexpr size_t KB = 1024;
constexpr size_t MB = 1024 * KB;
constexpr size_t GB = 1024 * MB;

class MemoryArena {
public:
    explicit MemoryArena(size_t size);
    ~MemoryArena();
    
    void* Allocate(size_t size, size_t alignment = 16);
    void Clear();
    void Reset();
    
    size_t GetSize() const { return m_Size; }
    size_t GetUsed() const { return m_Used; }
    size_t GetAvailable() const { return m_Size - m_Used; }
    
private:
    uint8_t* m_Memory = nullptr;
    size_t m_Size = 0;
    size_t m_Used = 0;
};

class PoolAllocator {
public:
    PoolAllocator(size_t elementSize, size_t elementCount);
    ~PoolAllocator();
    
    void* Allocate();
    void Deallocate(void* ptr);
    
    size_t GetElementSize() const { return m_ElementSize; }
    size_t GetCapacity() const { return m_ElementCount; }
    size_t GetUsedCount() const { return m_UsedCount; }
    
private:
    struct FreeNode {
        FreeNode* next;
    };
    
    uint8_t* m_Memory = nullptr;
    FreeNode* m_FreeList = nullptr;
    size_t m_ElementSize = 0;
    size_t m_ElementCount = 0;
    size_t m_UsedCount = 0;
};

template<typename T>
class STLArenaAllocator {
public:
    using value_type = T;
    
    explicit STLArenaAllocator(MemoryArena* arena) : m_Arena(arena) {}
    
    template<typename U>
    STLArenaAllocator(const STLArenaAllocator<U>& other) : m_Arena(other.m_Arena) {}
    
    T* allocate(size_t n) {
        return static_cast<T*>(m_Arena->Allocate(n * sizeof(T), alignof(T)));
    }
    
    void deallocate(T*, size_t) {
    }
    
    template<typename U>
    struct rebind {
        using other = STLArenaAllocator<U>;
    };
    
private:
    template<typename U>
    friend class STLArenaAllocator;
    
    MemoryArena* m_Arena;
};

inline void* AlignForward(void* ptr, size_t alignment) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t aligned = (addr + (alignment - 1)) & ~(alignment - 1);
    return reinterpret_cast<void*>(aligned);
}

inline size_t AlignForwardSize(size_t size, size_t alignment) {
    return (size + (alignment - 1)) & ~(alignment - 1);
}

}
