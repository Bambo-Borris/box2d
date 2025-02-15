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
#include <box2d/b2_shape.h>

class b2EdgeShape;

/// A chain shape is a free form sequence of line segments.
/// The chain has one-sided collision, with the surface normal pointing to the right of the edge.
/// This provides a counter-clockwise winding like the polygon shape.
/// Connectivity information is used to create smooth collisions.
/// @warning the chain will not collide properly if there are self-intersections.
class B2_API b2ChainShape : public b2Shape
{
public:
    b2ChainShape();

    /// The destructor frees the vertices using b2Free.
    ~b2ChainShape();

    /// Clear all data.
    void Clear();

    /// Create a loop. This automatically adjusts connectivity.
    /// @param vertices an array of vertices, these are copied
    /// @param count the vertex count
    void CreateLoop(const b2Vec2* vertices, std::int32_t count);

    /// Create a chain with ghost vertices to connect multiple chains together.
    /// @param vertices an array of vertices, these are copied
    /// @param count the vertex count
    /// @param prevVertex previous vertex from chain that connects to the start
    /// @param nextVertex next vertex from chain that connects to the end
    void CreateChain(const b2Vec2* vertices, std::int32_t count,
        const b2Vec2& prevVertex, const b2Vec2& nextVertex);

    /// Implement b2Shape. Vertices are cloned using b2Alloc.
    b2Shape* Clone(b2BlockAllocator* allocator) const override;

    /// @see b2Shape::GetChildCount
    std::int32_t GetChildCount() const override;

    /// Get a child edge.
    void GetChildEdge(b2EdgeShape* edge, std::int32_t index) const;

    /// This always return false.
    /// @see b2Shape::TestPoint
    bool TestPoint(const b2Transform& transform, const b2Vec2& p) const override;

    /// Implement b2Shape.
    bool RayCast(b2RayCastOutput* output, const b2RayCastInput& input,
                    const b2Transform& transform, std::int32_t childIndex) const override;

    /// @see b2Shape::ComputeAABB
    void ComputeAABB(b2AABB* aabb, const b2Transform& transform, std::int32_t childIndex) const override;

    /// Chains have zero mass.
    /// @see b2Shape::ComputeMass
    void ComputeMass(b2MassData* massData, float density) const override;

    /// The vertices. Owned by this class.
    b2Vec2* m_vertices;

    /// The vertex count.
    std::int32_t m_count;

    b2Vec2 m_prevVertex, m_nextVertex;
};

inline b2ChainShape::b2ChainShape()
{
    m_type = e_chain;
    m_radius = b2_polygonRadius;
    m_vertices = nullptr;
    m_count = 0;
}
