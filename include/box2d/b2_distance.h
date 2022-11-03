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
#include <box2d/b2_math.h>

#include <array>

class b2Shape;

/// A distance proxy is used by the GJK algorithm.
/// It encapsulates any shape.
struct B2_API b2DistanceProxy
{
    b2DistanceProxy() : m_vertices(nullptr), m_count(0), m_radius(0.0f) {}

    /// Initialize the proxy using the given shape. The shape
    /// must remain in scope while the proxy is in use.
    void Set(const b2Shape* shape, std::int32_t index);

    /// Initialize the proxy using a vertex cloud and radius. The vertices
    /// must remain in scope while the proxy is in use.
    void Set(const b2Vec2* vertices, std::int32_t count, float radius);

    /// Get the supporting vertex index in the given direction.
    std::int32_t GetSupport(const b2Vec2& d) const;

    /// Get the supporting vertex in the given direction.
    const b2Vec2& GetSupportVertex(const b2Vec2& d) const;

    /// Get the vertex count.
    std::int32_t GetVertexCount() const;

    /// Get a vertex by index. Used by b2Distance.
    const b2Vec2& GetVertex(std::int32_t index) const;

    std::array<b2Vec2, 2> m_buffer;
    const b2Vec2* m_vertices;
    std::int32_t m_count;
    float m_radius;
};

/// Used to warm start b2Distance.
/// Set count to zero on first call.
struct B2_API b2SimplexCache
{
    float metric;       ///< length or area
    std::uint16_t count;
    std::array<std::uint8_t, 3> indexA; ///< vertices on shape A
    std::array<std::uint8_t, 3> indexB; ///< vertices on shape B
};

/// Input for b2Distance.
/// You have to option to use the shape radii
/// in the computation. Even
struct B2_API b2DistanceInput
{
    b2DistanceProxy proxyA;
    b2DistanceProxy proxyB;
    b2Transform transformA;
    b2Transform transformB;
    bool useRadii;
};

/// Output for b2Distance.
struct B2_API b2DistanceOutput
{
    b2Vec2 pointA;      ///< closest point on shapeA
    b2Vec2 pointB;      ///< closest point on shapeB
    float distance;
    std::int32_t iterations;    ///< number of GJK iterations used
};

/// Compute the closest points between two shapes. Supports any combination of:
/// b2CircleShape, b2PolygonShape, b2EdgeShape. The simplex cache is input/output.
/// On the first call set b2SimplexCache.count to zero.
B2_API void b2Distance(b2DistanceOutput* output,
                b2SimplexCache* cache,
                const b2DistanceInput* input);

/// Input parameters for b2ShapeCast
struct B2_API b2ShapeCastInput
{
    b2DistanceProxy proxyA;
    b2DistanceProxy proxyB;
    b2Transform transformA;
    b2Transform transformB;
    b2Vec2 translationB;
};

/// Output results for b2ShapeCast
struct B2_API b2ShapeCastOutput
{
    b2Vec2 point;
    b2Vec2 normal;
    float lambda;
    std::int32_t iterations;
};

/// Perform a linear shape cast of shape B moving and shape A fixed. Determines the hit point, normal, and translation fraction.
/// @returns true if hit, false if there is no hit or an initial overlap
B2_API bool b2ShapeCast(b2ShapeCastOutput* output, const b2ShapeCastInput* input);

//////////////////////////////////////////////////////////////////////////

inline std::int32_t b2DistanceProxy::GetVertexCount() const
{
    return m_count;
}

inline const b2Vec2& b2DistanceProxy::GetVertex(const std::int32_t index) const
{
    assert(0 <= index && index < m_count);
    return m_vertices[index];
}

inline std::int32_t b2DistanceProxy::GetSupport(const b2Vec2& d) const
{
    std::int32_t bestIndex = 0;
    float bestValue = b2Dot(m_vertices[0], d);
    for (std::int32_t i = 1; i < m_count; ++i)
    {
	    const float value = b2Dot(m_vertices[i], d);
        if (value > bestValue)
        {
            bestIndex = i;
            bestValue = value;
        }
    }

    return bestIndex;
}

inline const b2Vec2& b2DistanceProxy::GetSupportVertex(const b2Vec2& d) const
{
    std::int32_t bestIndex = 0;
    float bestValue = b2Dot(m_vertices[0], d);
    for (std::int32_t i = 1; i < m_count; ++i)
    {
	    const float value = b2Dot(m_vertices[i], d);
        if (value > bestValue)
        {
            bestIndex = i;
            bestValue = value;
        }
    }

    return m_vertices[bestIndex];
}
