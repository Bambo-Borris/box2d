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

#include "test.h"

class MobileUnbalanced : public Test
{
public:

    enum
    {
        e_depth = 4
    };

    MobileUnbalanced()
    {
        b2Body* ground;

        // Create ground body.
        {
            b2BodyDef bodyDef;
            bodyDef.position.Set(0.0f, 20.0f);
            ground = m_world->CreateBody(&bodyDef);
        }

        float a = 0.5f;
        b2Vec2 h(0.0f, a);

        b2Body* root = AddNode(ground, b2Vec2_zero, 0, 3.0f, a);

        b2RevoluteJointDef jointDef;
        jointDef.bodyA = ground;
        jointDef.bodyB = root;
        jointDef.localAnchorA.SetZero();
        jointDef.localAnchorB = h;
        m_world->CreateJoint(&jointDef);
    }

    b2Body* AddNode(b2Body* parent, const b2Vec2& localAnchor, std::int32_t depth, float offset, float a)
    {
        float density = 20.0f;
        b2Vec2 h(0.0f, a);

        b2Vec2 p = parent->GetPosition() + localAnchor - h;

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = p;
        b2Body* body = m_world->CreateBody(&bodyDef);

        b2PolygonShape shape;
        shape.SetAsBox(0.25f * a, a);
        body->CreateFixture(&shape, density);

        if (depth == e_depth)
        {
            return body;
        }

        b2Vec2 a1 = b2Vec2(offset, -a);
        b2Vec2 a2 = b2Vec2(-offset, -a);
        b2Body* body1 = AddNode(body, a1, depth + 1, 0.5f * offset, a);
        b2Body* body2 = AddNode(body, a2, depth + 1, 0.5f * offset, a);

        b2RevoluteJointDef jointDef;
        jointDef.bodyA = body;
        jointDef.localAnchorB = h;

        jointDef.localAnchorA = a1;
        jointDef.bodyB = body1;
        m_world->CreateJoint(&jointDef);

        jointDef.localAnchorA = a2;
        jointDef.bodyB = body2;
        m_world->CreateJoint(&jointDef);

        return body;
    }

    static Test* Create()
    {
        return new MobileUnbalanced;
    }
};

static int testIndex = RegisterTest("Solver", "Mobile Unbalanced", MobileUnbalanced::Create);
