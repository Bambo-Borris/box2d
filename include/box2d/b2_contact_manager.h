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
#include <box2d/b2_broad_phase.h>

class b2Contact;
class b2ContactFilter;
class b2ContactListener;
class b2BlockAllocator;

// Delegate of b2World.
class B2_API b2ContactManager
{
public:
	b2ContactManager();

	// Broad-phase callback.
	void AddPair(void* proxyUserDataA, void* proxyUserDataB);

	void FindNewContacts();

	void Destroy(b2Contact* c);

	void Collide();

	b2BroadPhase m_broadPhase;
	b2Contact* m_contactList;
	std::int32_t m_contactCount;
	b2ContactFilter* m_contactFilter;
	b2ContactListener* m_contactListener;
	b2BlockAllocator* m_allocator;
};
