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

#include <box2d/box2d.h>
#include "draw.h"

#include <cstdlib>

struct Settings;
class Test;

#define RAND_LIMIT 32767

/// Random number in range [-1,1]
inline float RandomFloat()
{
    float r = (float)(rand() & (RAND_LIMIT));
    r /= RAND_LIMIT;
    r = 2.0f * r - 1.0f;
    return r;
}

/// Random floating point number in range [lo, hi]
inline float RandomFloat(float lo, float hi)
{
    float r = (float)(rand() & (RAND_LIMIT));
    r /= RAND_LIMIT;
    r = (hi - lo) * r + lo;
    return r;
}

// This is called when a joint in the world is implicitly destroyed
// because an attached body is destroyed. This gives us a chance to
// nullify the mouse joint.
class DestructionListener : public b2DestructionListener
{
public:
    void SayGoodbye(b2Fixture* fixture) override { (void)fixture; }
    void SayGoodbye(b2Joint* joint) override;

    Test* test;
};

const std::int32_t k_maxContactPoints = 2048;

struct ContactPoint
{
    b2Fixture* fixtureA;
    b2Fixture* fixtureB;
    b2Vec2 normal;
    b2Vec2 position;
    b2PointState state;
    float normalImpulse;
    float tangentImpulse;
    float separation;
};

class Test : public b2ContactListener
{
public:

    Test();
    virtual ~Test();

    void DrawTitle(const char* string);
    virtual void Step(Settings& settings);
    virtual void UpdateUI() {}
    virtual void Keyboard(int key) { (void)key; }
    virtual void KeyboardUp(int key) { (void)key; }
    void ShiftMouseDown(const b2Vec2& p);
    virtual void MouseDown(const b2Vec2& p);
    virtual void MouseUp(const b2Vec2& p);
    virtual void MouseMove(const b2Vec2& p);
    void LaunchBomb();
    void LaunchBomb(const b2Vec2& position, const b2Vec2& velocity);

    void SpawnBomb(const b2Vec2& worldPt);
    void CompleteBombSpawn(const b2Vec2& p);

    // Let derived tests know that a joint was destroyed.
    virtual void JointDestroyed(b2Joint* joint) { (void)joint; }

    // Callbacks for derived classes.
    virtual void BeginContact(b2Contact* contact)  override { (void)contact; }
    virtual void EndContact(b2Contact* contact)  override { (void)contact; }
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override
    {
        (void)contact;
        (void)impulse;
    }

    void ShiftOrigin(const b2Vec2& newOrigin);

protected:
    friend class DestructionListener;
    friend class BoundaryListener;
    friend class ContactListener;

    b2Body* m_groundBody;
    b2AABB m_worldAABB;
    ContactPoint m_points[k_maxContactPoints];
    std::int32_t m_pointCount;
    DestructionListener m_destructionListener;
    std::int32_t m_textLine;
    b2World* m_world;
    b2Body* m_bomb;
    b2MouseJoint* m_mouseJoint;
    b2Vec2 m_bombSpawnPoint;
    bool m_bombSpawning;
    b2Vec2 m_mouseWorld;
    std::int32_t m_stepCount;
    std::int32_t m_textIncrement;
    b2Profile m_maxProfile;
    b2Profile m_totalProfile;
};

typedef Test* TestCreateFcn();

int RegisterTest(const char* category, const char* name, TestCreateFcn* fcn);

//
struct TestEntry
{
    const char* category;
    const char* name;
    TestCreateFcn* createFcn;
};

#define MAX_TESTS 256
extern TestEntry g_testEntries[MAX_TESTS];
extern int g_testCount;
