#include "Memory.hpp"
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace Orchard::Memory {

MemoryArena::MemoryArena(size_t size) : m_Size(size) {
    m_Memory = static_cast<uint8_t*>(std::aligned_alloc(64, size));
    if (!m_Memory) {
        throw std::bad_alloc();
    }
    std::memset(m_Memory, 0, size);
}

MemoryArena::~MemoryArena() {
    if (m_Memory) {
        std::free(m_Memory);
    }
}

void* MemoryArena::Allocate(size_t size, size_t alignment) {
    void* current = m_Memory + m_Used;
    void* aligned = AlignForward(current, alignment);
    
    size_t alignmentOffset = static_cast<uint8_t*>(aligned) - static_cast<uint8_t*>(current);
    size_t totalSize = alignmentOffset + size;
    
    if (m_Used + totalSize > m_Size) {
        return nullptr;
    }
    
    m_Used += totalSize;
    return aligned;
}

void MemoryArena::Clear() {
    std::memset(m_Memory, 0, m_Size);
    m_Used = 0;
}

void MemoryArena::Reset() {
    m_Used = 0;
}

PoolAllocator::PoolAllocator(size_t elementSize, size_t elementCount)
    : m_ElementSize(AlignForwardSize(elementSize, 16))
    , m_ElementCount(elementCount)
{
    size_t totalSize = m_ElementSize * m_ElementCount;
    m_Memory = static_cast<uint8_t*>(std::aligned_alloc(64, totalSize));
    if (!m_Memory) {
        throw std::bad_alloc();
    }
    
    m_FreeList = nullptr;
    for (size_t i = 0; i < m_ElementCount; ++i) {
        FreeNode* node = reinterpret_cast<FreeNode*>(m_Memory + i * m_ElementSize);
        node->next = m_FreeList;
        m_FreeList = node;
    }
}

PoolAllocator::~PoolAllocator() {
    if (m_Memory) {
        std::free(m_Memory);
    }
}

void* PoolAllocator::Allocate() {
    if (!m_FreeList) {
        return nullptr;
    }
    
    FreeNode* node = m_FreeList;
    m_FreeList = m_FreeList->next;
    m_UsedCount++;
    
    return node;
}

void PoolAllocator::Deallocate(void* ptr) {
    if (!ptr) return;
    
    FreeNode* node = static_cast<FreeNode*>(ptr);
    node->next = m_FreeList;
    m_FreeList = node;
    m_UsedCount--;
}

}
