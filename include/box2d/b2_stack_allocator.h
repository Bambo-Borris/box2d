// MIT License

// Copyright (c) 2019 Erin Catto

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <box2d/b2_api.h>
#include <box2d/b2_settings.h>

#include <array>

constexpr std::size_t b2_stackSize = 100 * 1024; // 100k
constexpr std::size_t b2_maxStackEntries = 32;

struct B2_API b2StackEntry
{
    char* data;
    std::size_t size;
    bool usedMalloc;
};

// This is a stack allocator used for fast per step allocations.
// You must nest allocate/free pairs. The code will assert
// if you try to interleave multiple allocate/free pairs.
class B2_API b2StackAllocator
{
public:
    b2StackAllocator();
    ~b2StackAllocator();

    template<typename T> 
    [[nodiscard]] T* Allocate(std::size_t count = 1); 

    template<typename T>
    void Free(T* ptr);

    std::size_t GetMaxAllocation() const;

private:

    void* HandleAllocate(std::size_t size);
    void HandleFree(void* p);

    std::array<char, b2_stackSize> m_data;
    std::size_t m_index;

    std::size_t m_allocation;
    std::size_t m_maxAllocation;

    std::array<b2StackEntry, b2_maxStackEntries> m_entries;
    std::size_t m_entryCount;
};

template<typename T>
T* b2StackAllocator::Allocate(std::size_t count)
{
    return (T*)HandleAllocate(sizeof(T) * count);
}

template<typename T>
void b2StackAllocator::Free(T* ptr)
{
    HandleFree(static_cast<void*>(ptr));
}
