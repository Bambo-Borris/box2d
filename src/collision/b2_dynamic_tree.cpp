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
#include <box2d/b2_dynamic_tree.h>
#include <cstring>

b2DynamicTree::b2DynamicTree()
{
    m_root = b2_nullNode;

    m_nodeCapacity = 16;
    m_nodeCount = 0;
    m_nodes = (b2TreeNode*)b2Alloc(m_nodeCapacity * sizeof(b2TreeNode));
    memset(m_nodes, 0, m_nodeCapacity * sizeof(b2TreeNode));

    // Build a linked list for the free list.
    for (std::int32_t i = 0; i < m_nodeCapacity - 1; ++i)
    {
        m_nodes[i].next = i + 1;
        m_nodes[i].height = -1;
    }
    m_nodes[m_nodeCapacity-1].next = b2_nullNode;
    m_nodes[m_nodeCapacity-1].height = -1;
    m_freeList = 0;

    m_insertionCount = 0;
}

b2DynamicTree::~b2DynamicTree()
{
    // This frees the entire tree in one shot.
    b2Free(m_nodes);
}

// Allocate a node from the pool. Grow the pool if necessary.
std::int32_t b2DynamicTree::AllocateNode()
{
    // Expand the node pool as needed.
    if (m_freeList == b2_nullNode)
    {
        assert(m_nodeCount == m_nodeCapacity);

        // The free list is empty. Rebuild a bigger pool.
        b2TreeNode* oldNodes = m_nodes;
        m_nodeCapacity *= 2;
        m_nodes = (b2TreeNode*)b2Alloc(m_nodeCapacity * sizeof(b2TreeNode));
        memcpy(m_nodes, oldNodes, m_nodeCount * sizeof(b2TreeNode));
        b2Free(oldNodes);

        // Build a linked list for the free list. The parent
        // pointer becomes the "next" pointer.
        for (std::int32_t i = m_nodeCount; i < m_nodeCapacity - 1; ++i)
        {
            m_nodes[i].next = i + 1;
            m_nodes[i].height = -1;
        }
        m_nodes[m_nodeCapacity-1].next = b2_nullNode;
        m_nodes[m_nodeCapacity-1].height = -1;
        m_freeList = m_nodeCount;
    }

    // Peel a node off the free list.
    std::int32_t nodeId = m_freeList;
    m_freeList = m_nodes[nodeId].next;
    m_nodes[nodeId].parent = b2_nullNode;
    m_nodes[nodeId].child1 = b2_nullNode;
    m_nodes[nodeId].child2 = b2_nullNode;
    m_nodes[nodeId].height = 0;
    m_nodes[nodeId].userData = nullptr;
    m_nodes[nodeId].moved = false;
    ++m_nodeCount;
    return nodeId;
}

// Return a node to the pool.
void b2DynamicTree::FreeNode(std::int32_t nodeId)
{
    assert(0 <= nodeId && nodeId < m_nodeCapacity);
    assert(0 < m_nodeCount);
    m_nodes[nodeId].next = m_freeList;
    m_nodes[nodeId].height = -1;
    m_freeList = nodeId;
    --m_nodeCount;
}

// Create a proxy in the tree as a leaf node. We return the index
// of the node instead of a pointer so that we can grow
// the node pool.
std::int32_t b2DynamicTree::CreateProxy(const b2AABB& aabb, void* userData)
{
    std::int32_t proxyId = AllocateNode();

    // Fatten the aabb.
    b2Vec2 r(b2_aabbExtension, b2_aabbExtension);
    m_nodes[proxyId].aabb.lowerBound = aabb.lowerBound - r;
    m_nodes[proxyId].aabb.upperBound = aabb.upperBound + r;
    m_nodes[proxyId].userData = userData;
    m_nodes[proxyId].height = 0;
    m_nodes[proxyId].moved = true;

    InsertLeaf(proxyId);

    return proxyId;
}

void b2DynamicTree::DestroyProxy(std::int32_t proxyId)
{
    assert(0 <= proxyId && proxyId < m_nodeCapacity);
    assert(m_nodes[proxyId].IsLeaf());

    RemoveLeaf(proxyId);
    FreeNode(proxyId);
}

bool b2DynamicTree::MoveProxy(std::int32_t proxyId, const b2AABB& aabb, const b2Vec2& displacement)
{
    assert(0 <= proxyId && proxyId < m_nodeCapacity);

    assert(m_nodes[proxyId].IsLeaf());

    // Extend AABB
    b2AABB fatAABB;
    b2Vec2 r(b2_aabbExtension, b2_aabbExtension);
    fatAABB.lowerBound = aabb.lowerBound - r;
    fatAABB.upperBound = aabb.upperBound + r;

    // Predict AABB movement
    b2Vec2 d = b2_aabbMultiplier * displacement;

    if (d.x < 0.0f)
    {
        fatAABB.lowerBound.x += d.x;
    }
    else
    {
        fatAABB.upperBound.x += d.x;
    }

    if (d.y < 0.0f)
    {
        fatAABB.lowerBound.y += d.y;
    }
    else
    {
        fatAABB.upperBound.y += d.y;
    }

    const b2AABB& treeAABB = m_nodes[proxyId].aabb;
    if (treeAABB.Contains(aabb))
    {
        // The tree AABB still contains the object, but it might be too large.
        // Perhaps the object was moving fast but has since gone to sleep.
        // The huge AABB is larger than the new fat AABB.
        b2AABB hugeAABB;
        hugeAABB.lowerBound = fatAABB.lowerBound - 4.0f * r;
        hugeAABB.upperBound = fatAABB.upperBound + 4.0f * r;

        if (hugeAABB.Contains(treeAABB))
        {
            // The tree AABB contains the object AABB and the tree AABB is
            // not too large. No tree update needed.
            return false;
        }

        // Otherwise the tree AABB is huge and needs to be shrunk
    }

    RemoveLeaf(proxyId);

    m_nodes[proxyId].aabb = fatAABB;

    InsertLeaf(proxyId);

    m_nodes[proxyId].moved = true;

    return true;
}

void b2DynamicTree::InsertLeaf(std::int32_t leaf)
{
    ++m_insertionCount;

    if (m_root == b2_nullNode)
    {
        m_root = leaf;
        m_nodes[m_root].parent = b2_nullNode;
        return;
    }

    // Find the best sibling for this node
    b2AABB leafAABB = m_nodes[leaf].aabb;
    std::int32_t index = m_root;
    while (m_nodes[index].IsLeaf() == false)
    {
        std::int32_t child1 = m_nodes[index].child1;
        std::int32_t child2 = m_nodes[index].child2;

        float area = m_nodes[index].aabb.GetPerimeter();

        b2AABB combinedAABB;
        combinedAABB.Combine(m_nodes[index].aabb, leafAABB);
        float combinedArea = combinedAABB.GetPerimeter();

        // Cost of creating a new parent for this node and the new leaf
        float cost = 2.0f * combinedArea;

        // Minimum cost of pushing the leaf further down the tree
        float inheritanceCost = 2.0f * (combinedArea - area);

        // Cost of descending into child1
        float cost1;
        if (m_nodes[child1].IsLeaf())
        {
            b2AABB aabb;
            aabb.Combine(leafAABB, m_nodes[child1].aabb);
            cost1 = aabb.GetPerimeter() + inheritanceCost;
        }
        else
        {
            b2AABB aabb;
            aabb.Combine(leafAABB, m_nodes[child1].aabb);
            float oldArea = m_nodes[child1].aabb.GetPerimeter();
            float newArea = aabb.GetPerimeter();
            cost1 = (newArea - oldArea) + inheritanceCost;
        }

        // Cost of descending into child2
        float cost2;
        if (m_nodes[child2].IsLeaf())
        {
            b2AABB aabb;
            aabb.Combine(leafAABB, m_nodes[child2].aabb);
            cost2 = aabb.GetPerimeter() + inheritanceCost;
        }
        else
        {
            b2AABB aabb;
            aabb.Combine(leafAABB, m_nodes[child2].aabb);
            float oldArea = m_nodes[child2].aabb.GetPerimeter();
            float newArea = aabb.GetPerimeter();
            cost2 = newArea - oldArea + inheritanceCost;
        }

        // Descend according to the minimum cost.
        if (cost < cost1 && cost < cost2)
        {
            break;
        }

        // Descend
        if (cost1 < cost2)
        {
            index = child1;
        }
        else
        {
            index = child2;
        }
    }

    std::int32_t sibling = index;

    // Create a new parent.
    std::int32_t oldParent = m_nodes[sibling].parent;
    std::int32_t newParent = AllocateNode();
    m_nodes[newParent].parent = oldParent;
    m_nodes[newParent].userData = nullptr;
    m_nodes[newParent].aabb.Combine(leafAABB, m_nodes[sibling].aabb);
    m_nodes[newParent].height = m_nodes[sibling].height + 1;

    if (oldParent != b2_nullNode)
    {
        // The sibling was not the root.
        if (m_nodes[oldParent].child1 == sibling)
        {
            m_nodes[oldParent].child1 = newParent;
        }
        else
        {
            m_nodes[oldParent].child2 = newParent;
        }

        m_nodes[newParent].child1 = sibling;
        m_nodes[newParent].child2 = leaf;
        m_nodes[sibling].parent = newParent;
        m_nodes[leaf].parent = newParent;
    }
    else
    {
        // The sibling was the root.
        m_nodes[newParent].child1 = sibling;
        m_nodes[newParent].child2 = leaf;
        m_nodes[sibling].parent = newParent;
        m_nodes[leaf].parent = newParent;
        m_root = newParent;
    }

    // Walk back up the tree fixing heights and AABBs
    index = m_nodes[leaf].parent;
    while (index != b2_nullNode)
    {
        index = Balance(index);

        std::int32_t child1 = m_nodes[index].child1;
        std::int32_t child2 = m_nodes[index].child2;

        assert(child1 != b2_nullNode);
        assert(child2 != b2_nullNode);

        m_nodes[index].height = 1 + b2Max(m_nodes[child1].height, m_nodes[child2].height);
        m_nodes[index].aabb.Combine(m_nodes[child1].aabb, m_nodes[child2].aabb);

        index = m_nodes[index].parent;
    }

    //Validate();
}

void b2DynamicTree::RemoveLeaf(std::int32_t leaf)
{
    if (leaf == m_root)
    {
        m_root = b2_nullNode;
        return;
    }

    std::int32_t parent = m_nodes[leaf].parent;
    std::int32_t grandParent = m_nodes[parent].parent;
    std::int32_t sibling;
    if (m_nodes[parent].child1 == leaf)
    {
        sibling = m_nodes[parent].child2;
    }
    else
    {
        sibling = m_nodes[parent].child1;
    }

    if (grandParent != b2_nullNode)
    {
        // Destroy parent and connect sibling to grandParent.
        if (m_nodes[grandParent].child1 == parent)
        {
            m_nodes[grandParent].child1 = sibling;
        }
        else
        {
            m_nodes[grandParent].child2 = sibling;
        }
        m_nodes[sibling].parent = grandParent;
        FreeNode(parent);

        // Adjust ancestor bounds.
        std::int32_t index = grandParent;
        while (index != b2_nullNode)
        {
            index = Balance(index);

            std::int32_t child1 = m_nodes[index].child1;
            std::int32_t child2 = m_nodes[index].child2;

            m_nodes[index].aabb.Combine(m_nodes[child1].aabb, m_nodes[child2].aabb);
            m_nodes[index].height = 1 + b2Max(m_nodes[child1].height, m_nodes[child2].height);

            index = m_nodes[index].parent;
        }
    }
    else
    {
        m_root = sibling;
        m_nodes[sibling].parent = b2_nullNode;
        FreeNode(parent);
    }

    //Validate();
}

// Perform a left or right rotation if node A is imbalanced.
// Returns the new root index.
std::int32_t b2DynamicTree::Balance(std::int32_t iA)
{
    assert(iA != b2_nullNode);

    b2TreeNode* A = m_nodes + iA;
    if (A->IsLeaf() || A->height < 2)
    {
        return iA;
    }

    std::int32_t iB = A->child1;
    std::int32_t iC = A->child2;
    assert(0 <= iB && iB < m_nodeCapacity);
    assert(0 <= iC && iC < m_nodeCapacity);

    b2TreeNode* B = m_nodes + iB;
    b2TreeNode* C = m_nodes + iC;

    std::int32_t balance = C->height - B->height;

    // Rotate C up
    if (balance > 1)
    {
        std::int32_t iF = C->child1;
        std::int32_t iG = C->child2;
        b2TreeNode* F = m_nodes + iF;
        b2TreeNode* G = m_nodes + iG;
        assert(0 <= iF && iF < m_nodeCapacity);
        assert(0 <= iG && iG < m_nodeCapacity);

        // Swap A and C
        C->child1 = iA;
        C->parent = A->parent;
        A->parent = iC;

        // A's old parent should point to C
        if (C->parent != b2_nullNode)
        {
            if (m_nodes[C->parent].child1 == iA)
            {
                m_nodes[C->parent].child1 = iC;
            }
            else
            {
                assert(m_nodes[C->parent].child2 == iA);
                m_nodes[C->parent].child2 = iC;
            }
        }
        else
        {
            m_root = iC;
        }

        // Rotate
        if (F->height > G->height)
        {
            C->child2 = iF;
            A->child2 = iG;
            G->parent = iA;
            A->aabb.Combine(B->aabb, G->aabb);
            C->aabb.Combine(A->aabb, F->aabb);

            A->height = 1 + b2Max(B->height, G->height);
            C->height = 1 + b2Max(A->height, F->height);
        }
        else
        {
            C->child2 = iG;
            A->child2 = iF;
            F->parent = iA;
            A->aabb.Combine(B->aabb, F->aabb);
            C->aabb.Combine(A->aabb, G->aabb);

            A->height = 1 + b2Max(B->height, F->height);
            C->height = 1 + b2Max(A->height, G->height);
        }

        return iC;
    }

    // Rotate B up
    if (balance < -1)
    {
        std::int32_t iD = B->child1;
        std::int32_t iE = B->child2;
        b2TreeNode* D = m_nodes + iD;
        b2TreeNode* E = m_nodes + iE;
        assert(0 <= iD && iD < m_nodeCapacity);
        assert(0 <= iE && iE < m_nodeCapacity);

        // Swap A and B
        B->child1 = iA;
        B->parent = A->parent;
        A->parent = iB;

        // A's old parent should point to B
        if (B->parent != b2_nullNode)
        {
            if (m_nodes[B->parent].child1 == iA)
            {
                m_nodes[B->parent].child1 = iB;
            }
            else
            {
                assert(m_nodes[B->parent].child2 == iA);
                m_nodes[B->parent].child2 = iB;
            }
        }
        else
        {
            m_root = iB;
        }

        // Rotate
        if (D->height > E->height)
        {
            B->child2 = iD;
            A->child1 = iE;
            E->parent = iA;
            A->aabb.Combine(C->aabb, E->aabb);
            B->aabb.Combine(A->aabb, D->aabb);

            A->height = 1 + b2Max(C->height, E->height);
            B->height = 1 + b2Max(A->height, D->height);
        }
        else
        {
            B->child2 = iE;
            A->child1 = iD;
            D->parent = iA;
            A->aabb.Combine(C->aabb, D->aabb);
            B->aabb.Combine(A->aabb, E->aabb);

            A->height = 1 + b2Max(C->height, D->height);
            B->height = 1 + b2Max(A->height, E->height);
        }

        return iB;
    }

    return iA;
}

std::int32_t b2DynamicTree::GetHeight() const
{
    if (m_root == b2_nullNode)
    {
        return 0;
    }

    return m_nodes[m_root].height;
}

//
float b2DynamicTree::GetAreaRatio() const
{
    if (m_root == b2_nullNode)
    {
        return 0.0f;
    }

    const b2TreeNode* root = m_nodes + m_root;
    float rootArea = root->aabb.GetPerimeter();

    float totalArea = 0.0f;
    for (std::int32_t i = 0; i < m_nodeCapacity; ++i)
    {
        const b2TreeNode* node = m_nodes + i;
        if (node->height < 0)
        {
            // Free node in pool
            continue;
        }

        totalArea += node->aabb.GetPerimeter();
    }

    return totalArea / rootArea;
}

// Compute the height of a sub-tree.
std::int32_t b2DynamicTree::ComputeHeight(std::int32_t nodeId) const
{
    assert(0 <= nodeId && nodeId < m_nodeCapacity);
    b2TreeNode* node = m_nodes + nodeId;

    if (node->IsLeaf())
    {
        return 0;
    }

    std::int32_t height1 = ComputeHeight(node->child1);
    std::int32_t height2 = ComputeHeight(node->child2);
    return 1 + b2Max(height1, height2);
}

std::int32_t b2DynamicTree::ComputeHeight() const
{
    std::int32_t height = ComputeHeight(m_root);
    return height;
}

void b2DynamicTree::ValidateStructure(std::int32_t index) const
{
    if (index == b2_nullNode)
    {
        return;
    }

    if (index == m_root)
    {
        assert(m_nodes[index].parent == b2_nullNode);
    }

    const b2TreeNode* node = m_nodes + index;

    std::int32_t child1 = node->child1;
    std::int32_t child2 = node->child2;

    if (node->IsLeaf())
    {
        assert(child1 == b2_nullNode);
        assert(child2 == b2_nullNode);
        assert(node->height == 0);
        return;
    }

    assert(0 <= child1 && child1 < m_nodeCapacity);
    assert(0 <= child2 && child2 < m_nodeCapacity);

    assert(m_nodes[child1].parent == index);
    assert(m_nodes[child2].parent == index);

    ValidateStructure(child1);
    ValidateStructure(child2);
}

void b2DynamicTree::ValidateMetrics(std::int32_t index) const
{
    if (index == b2_nullNode)
    {
        return;
    }

    const b2TreeNode* node = m_nodes + index;

    std::int32_t child1 = node->child1;
    std::int32_t child2 = node->child2;

    if (node->IsLeaf())
    {
        assert(child1 == b2_nullNode);
        assert(child2 == b2_nullNode);
        assert(node->height == 0);
        return;
    }

    assert(0 <= child1 && child1 < m_nodeCapacity);
    assert(0 <= child2 && child2 < m_nodeCapacity);

    std::int32_t height1 = m_nodes[child1].height;
    std::int32_t height2 = m_nodes[child2].height;
    std::int32_t height;
    height = 1 + b2Max(height1, height2);
    assert(node->height == height);

    b2AABB aabb;
    aabb.Combine(m_nodes[child1].aabb, m_nodes[child2].aabb);

    assert(aabb.lowerBound == node->aabb.lowerBound);
    assert(aabb.upperBound == node->aabb.upperBound);

    ValidateMetrics(child1);
    ValidateMetrics(child2);
}

void b2DynamicTree::Validate() const
{
#if defined(b2DEBUG)
    ValidateStructure(m_root);
    ValidateMetrics(m_root);

    std::int32_t freeCount = 0;
    std::int32_t freeIndex = m_freeList;
    while (freeIndex != b2_nullNode)
    {
        assert(0 <= freeIndex && freeIndex < m_nodeCapacity);
        freeIndex = m_nodes[freeIndex].next;
        ++freeCount;
    }

    assert(GetHeight() == ComputeHeight());

    assert(m_nodeCount + freeCount == m_nodeCapacity);
#endif
}

std::int32_t b2DynamicTree::GetMaxBalance() const
{
    std::int32_t maxBalance = 0;
    for (std::int32_t i = 0; i < m_nodeCapacity; ++i)
    {
        const b2TreeNode* node = m_nodes + i;
        if (node->height <= 1)
        {
            continue;
        }

        assert(node->IsLeaf() == false);

        std::int32_t child1 = node->child1;
        std::int32_t child2 = node->child2;
        std::int32_t balance = b2Abs(m_nodes[child2].height - m_nodes[child1].height);
        maxBalance = b2Max(maxBalance, balance);
    }

    return maxBalance;
}

void b2DynamicTree::RebuildBottomUp()
{
    std::int32_t* nodes = (std::int32_t*)b2Alloc(m_nodeCount * sizeof(std::int32_t));
    std::int32_t count = 0;

    // Build array of leaves. Free the rest.
    for (std::int32_t i = 0; i < m_nodeCapacity; ++i)
    {
        if (m_nodes[i].height < 0)
        {
            // free node in pool
            continue;
        }

        if (m_nodes[i].IsLeaf())
        {
            m_nodes[i].parent = b2_nullNode;
            nodes[count] = i;
            ++count;
        }
        else
        {
            FreeNode(i);
        }
    }

    while (count > 1)
    {
        float minCost = FLT_MAX;
        std::int32_t iMin = -1, jMin = -1;
        for (std::int32_t i = 0; i < count; ++i)
        {
            b2AABB aabbi = m_nodes[nodes[i]].aabb;

            for (std::int32_t j = i + 1; j < count; ++j)
            {
                b2AABB aabbj = m_nodes[nodes[j]].aabb;
                b2AABB b;
                b.Combine(aabbi, aabbj);
                float cost = b.GetPerimeter();
                if (cost < minCost)
                {
                    iMin = i;
                    jMin = j;
                    minCost = cost;
                }
            }
        }

        std::int32_t index1 = nodes[iMin];
        std::int32_t index2 = nodes[jMin];
        b2TreeNode* child1 = m_nodes + index1;
        b2TreeNode* child2 = m_nodes + index2;

        std::int32_t parentIndex = AllocateNode();
        b2TreeNode* parent = m_nodes + parentIndex;
        parent->child1 = index1;
        parent->child2 = index2;
        parent->height = 1 + b2Max(child1->height, child2->height);
        parent->aabb.Combine(child1->aabb, child2->aabb);
        parent->parent = b2_nullNode;

        child1->parent = parentIndex;
        child2->parent = parentIndex;

        nodes[jMin] = nodes[count-1];
        nodes[iMin] = parentIndex;
        --count;
    }

    m_root = nodes[0];
    b2Free(nodes);

    Validate();
}

void b2DynamicTree::ShiftOrigin(const b2Vec2& newOrigin)
{
    // Build array of leaves. Free the rest.
    for (std::int32_t i = 0; i < m_nodeCapacity; ++i)
    {
        m_nodes[i].aabb.lowerBound -= newOrigin;
        m_nodes[i].aabb.upperBound -= newOrigin;
    }
}
