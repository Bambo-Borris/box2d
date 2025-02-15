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

const std::int32_t b2_blockSizeCount = 14;

struct b2Block;
struct b2Chunk;

/// This is a small object allocator used for allocating small
/// objects that persist for more than one time step.
/// See: http://www.codeproject.com/useritems/Small_Block_Allocator.asp
class B2_API b2BlockAllocator
{
public:
    b2BlockAllocator();
    ~b2BlockAllocator();

    /// Allocate memory. This will use b2Alloc if the size is larger than b2_maxBlockSize.
    template <typename T>
    [[nodiscard]] T* Allocate(std::size_t count = 1);


    /// Free memory. This will use b2Free if the size is larger than b2_maxBlockSize.
    template <typename T>
    void Free(T* ptr, std::size_t count = 1);

    void Clear();

private:
    void* HandleAllocate(std::size_t size);
    void HandleFree(void* p, std::size_t size);

    b2Chunk* m_chunks;
    std::int32_t m_chunkCount;
    std::size_t m_chunkSpace;

    std::array<b2Block*, b2_blockSizeCount> m_freeLists;
};

template <typename T>
T* b2BlockAllocator::Allocate(std::size_t count)
{
    assert(count > 0);
    return (T*)HandleAllocate(sizeof(T) * count);
}

template <typename T>
void b2BlockAllocator::Free(T* ptr, std::size_t count)
{
    assert(count > 0);
    HandleFree(static_cast<void*>(ptr), sizeof(T) * count);
}
