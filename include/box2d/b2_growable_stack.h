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

#include <cstring>
#include <array>

#include <box2d/b2_settings.h>

/// This is a growable LIFO stack with an initial capacity of N.
/// If the stack size exceeds the initial capacity, the heap is used
/// to increase the size of the stack.
template <typename T, std::int32_t N>
class b2GrowableStack
{
public:
    b2GrowableStack()
    {
        m_stack = m_array.data();
        m_count = 0;
        m_capacity = N;
    }

    ~b2GrowableStack()
    {
        if (m_stack != m_array.data())
        {
            b2Free(m_stack);
            m_stack = nullptr;
        }
    }

    void Push(const T& element)
    {
        if (m_count == m_capacity)
        {
            T* old = m_stack;
            m_capacity *= 2;
            m_stack = (T*)b2Alloc(m_capacity * sizeof(T));
            memcpy(m_stack, old, m_count * sizeof(T));
            if (old != m_array.data())
            {
                b2Free(old);
            }
        }

        m_stack[m_count] = element;
        ++m_count;
    }

    T Pop()
    {
        assert(m_count > 0);
        --m_count;
        return m_stack[m_count];
    }

    std::int32_t GetCount()
    {
        return m_count;
    }

private:
    T* m_stack;
    std::array<T,N> m_array;
    std::int32_t m_count;
    std::int32_t m_capacity;
};
