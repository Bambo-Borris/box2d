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

#include "b2_chain_polygon_contact.h"
#include <box2d/b2_block_allocator.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_chain_shape.h>
#include <box2d/b2_edge_shape.h>

#include <new>

b2Contact* b2ChainAndPolygonContact::Create(b2Fixture* fixtureA, std::int32_t indexA, b2Fixture* fixtureB, std::int32_t indexB, b2BlockAllocator* allocator)
{
    auto* mem = allocator->Allocate<b2ChainAndPolygonContact>();
    return new (mem) b2ChainAndPolygonContact(fixtureA, indexA, fixtureB, indexB);
}

void b2ChainAndPolygonContact::Destroy(b2Contact* contact, b2BlockAllocator* allocator)
{
    ((b2ChainAndPolygonContact*)contact)->~b2ChainAndPolygonContact();
    allocator->Free(contact);
}

b2ChainAndPolygonContact::b2ChainAndPolygonContact(b2Fixture* fixtureA, std::int32_t indexA, b2Fixture* fixtureB, std::int32_t indexB)
: b2Contact(fixtureA, indexA, fixtureB, indexB)
{
    assert(m_fixtureA->GetType() == b2Shape::e_chain);
    assert(m_fixtureB->GetType() == b2Shape::e_polygon);
}

void b2ChainAndPolygonContact::Evaluate(b2Manifold* manifold, const b2Transform& xfA, const b2Transform& xfB)
{
    b2ChainShape* chain = (b2ChainShape*)m_fixtureA->GetShape();
    b2EdgeShape edge;
    chain->GetChildEdge(&edge, m_indexA);
    b2CollideEdgeAndPolygon(    manifold, &edge, xfA,
                                (b2PolygonShape*)m_fixtureB->GetShape(), xfB);
}
