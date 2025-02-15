// AVL_Database.cpp
#include "AVL_Database.hpp"
#include <algorithm>
#include <cmath>

Record::Record(const std::string &k, int v) : key(k), value(v) {}

AVLNode::AVLNode(Record *r) : record(r), left(nullptr), right(nullptr), height(1) {}

AVLTree::AVLTree() : root(nullptr), nodeCount(0), searchComparisonCount(0) {}

int AVLTree::height(AVLNode *node)
{
    return node ? node->height : 0;
}

void AVLTree::updateHeight(AVLNode *node)
{
    if (node)
    {
        node->height = 1 + std::max(height(node->left), height(node->right));
    }
}

int AVLTree::getBalance(AVLNode *node)
{
    return node ? height(node->left) - height(node->right) : 0;
}

/*
Can be used to balance AVL trees
*/
AVLNode *AVLTree::rotateRight(AVLNode *y)
{
    if (!y || !y->left)
        return y; // if y is null or y does not have a left child, rotation is not possible
    AVLNode *l = y->left;
    y->left = l->right; // the left child of y takes the place of y and y is moved as the right child of l. Since l's right child cannot be on the left of l, l->left is attached to the free hand of y instead.
    l->right = y;
    updateHeight(y), updateHeight(l); // Heights at node y and node l (which is the new root) are updated
    return l;
}

/*
Can be used to balance AVL trees
*/
AVLNode *AVLTree::rotateLeft(AVLNode *x)
{
    if (!x || !x->right)
        return x; // rotated in the same way as rotateRight only the opposite
    AVLNode *r = x->right;
    x->right = r->left;
    r->left = x;
    updateHeight(x), updateHeight(r);
    return r;
}

AVLNode *AVLTree::minValueNode(AVLNode *node)
{
    while (node->left != nullptr)
    {
        node = node->left; // Finds the minimum value node
    }
    return node;
}

AVLNode *AVLTree::searchHelper(AVLNode *node, const std::string &key, int value) const
{
    AVLNode *current = node;
    while (current)
    {
        if (key < current->record->key)
        {
            current = current->left; // if the key is less than the current node's key, move to the left
        }
        else if (key > current->record->key)
        {
            current = current->right; // if the key is greater than the current node's key, move to the right
        }
        else
        {
            return current; // in this case, they key is equal to the current node's key, so we return the current node
        }
    }
    return nullptr; // if the key is not found, return nullptr
}

AVLNode *AVLTree::reBalance(AVLNode *node)
{
    if (!node)
        return node; // if the node is null, return null

    updateHeight(node); // update the height in each iteration

    int balance = getBalance(node); // get the balance on the given node

    if (balance > 1 && getBalance(node->left) >= 0)
    {
        return rotateRight(node); // Left-Left (LL) case
    }

    if (balance < -1 && getBalance(node->right) <= 0)
    {
        return rotateLeft(node); // Right-Right (RR) case
    }

    if (balance > 1 && getBalance(node->left) < 0)
    {
        node->left = rotateLeft(node->left);
        return rotateRight(node); // Left-Right (LR) case
    }

    if (balance < -1 && getBalance(node->right) > 0)
    {
        node->right = rotateRight(node->right);
        return rotateLeft(node); // Right-Left (RL) case
    }

    return node; // if the balance is not greater than 1 or less than -1, return the node
}

AVLNode *AVLTree::insertHelper(AVLNode *node, Record *record)
{
    if (!node)
        return new AVLNode(record); // In the correct position, the node is null, this is where the record is inserted

    if (record->key < node->record->key)
    {
        node->left = insertHelper(node->left, record); // Recursion happens until the correct poistion is found
    }
    else if (record->key > node->record->key)
    {
        node->right = insertHelper(node->right, record); // In this case, inserthelper finds the position in the right subtree since the key is greater
    }

    updateHeight(node); // After every insertion, height is updated and the tree is rebalanced
    return reBalance(node);
}

void AVLTree::insert(Record *record)
{
    if (!root)
    {
        root = new AVLNode(record);
        nodeCount++;
        searchComparisonCount++;
        return;
    }
    root = insertHelper(root, record); // After inserting, node count is updated
    nodeCount++;
}

void AVLTree::deleteNode(const std::string &key, int value)
{
    if (!root)
        return;

    AVLNode *nodeToDelete = searchHelper(root, key, value); // Node is found using searchHelper
    if (!nodeToDelete || nodeToDelete->record->value != value)
    { // If no node matches the key and value, return
        return;
    }

    if (!nodeToDelete->left && !nodeToDelete->right)
    {
        if (nodeToDelete == root)
        {
            delete root; // Case 1: No children - Root deleted and set to nullptr
            root = nullptr;
        }
        else
        {
            AVLNode *parent = deleteHelper(root, nodeToDelete); // Locate parent
            if (parent->left == nodeToDelete)
                parent->left = nullptr;
            else
                parent->right = nullptr;
            delete nodeToDelete;
        }
    }
    else if (!nodeToDelete->left || !nodeToDelete->right)
    {
        AVLNode *child = nodeToDelete->left ? nodeToDelete->left : nodeToDelete->right; // Case 2: One child - whichever child exists is set to child

        if (nodeToDelete == root)
        {
            root = child;
        }
        else
        {
            AVLNode *parent = deleteHelper(root, nodeToDelete);
            if (parent->left == nodeToDelete)
                parent->left = child;
            else
                parent->right = child;
        }
        delete nodeToDelete; // node is deleted and its parent is set to the child
    }
    else
    {
        AVLNode *minNode = minValueNode(nodeToDelete->right); // Case 3: Two children - Find the in-order successor (minNode)
        nodeToDelete->record = minNode->record;

        AVLNode *parent = deleteHelper(root, minNode); // Delete the in-order successor (minNode)
        if (parent->left == minNode)
            parent->left = minNode->right;
        else
            parent->right = minNode->right;
        delete minNode;
    }

    updateHeight(root); // After each deletion, height is updated and the tree is rebalanced
    root = reBalance(root);

    nodeCount--; // After each deletion, node count is updated
}

AVLNode *AVLTree::deleteHelper(AVLNode *root, AVLNode *child) const
{
    AVLNode *parent = nullptr;
    AVLNode *current = root; // The function finds the parent of the node to be deleted

    while (current && current != child)
    {
        parent = current;
        if (child->record->key < current->record->key)
            current = current->left; // if the key is less than the current node's key, move to the left
        else
            current = current->right; // if the key is greater than the current node's key, move to the right
    }
    return parent;
}

Record *AVLTree::search(const std::string &key, int value)
{

    AVLNode *found = searchHelper(root, key, value);

    if (!found)
    {
        Record record = Record("", 0);
        AVLNode *node = new AVLNode(&record);           //If the key is not found, a node with key "" and value 0 is returned
        found = node;
    }
    return found->record;
}

// IndexedDatabase Implementation
void IndexedDatabase::insert(Record *record)
{
    index.insert(record);
}

Record *IndexedDatabase::search(const std::string &key, int value)
{
    return index.search(key, value);
}

void IndexedDatabase::deleteRecord(const std::string &key, int value)
{
    index.deleteNode(key, value);
}

/* RangeQuery Hints
1. Base: if (!node) return
2. Key Traversal Logic:
   - If value >= start: check left subtree
   - If start <= value <= end: add to result
   - If value <= end: check right subtree
*/

void IndexedDatabase::rangeQueryHelper(AVLNode *node, int start, int end, std::vector<Record *> &result) const
{
    if (!node)
        return;

    if (node->record->value > start)
    {
        rangeQueryHelper(node->left, start, end, result);       //If value greater than start, check left subtree
    }

    if (node->record->value >= start && node->record->value <= end)
    {
        result.push_back(node->record);                         //If value is between start and end, add to result
    }

    if (node->record->value < end)
    {
        rangeQueryHelper(node->right, start, end, result);              //If value less than end, check right subtree
    }
}

std::vector<Record *> IndexedDatabase::rangeQuery(int start, int end)
{
    std::vector<Record *> result;
    rangeQueryHelper(index.root, start, end, result);               
    return result;
}

void IndexedDatabase::clearHelper(AVLNode *node)
{
    if (!node)
        return;
    clearHelper(node->left);
    clearHelper(node->right);
    delete node->record;
    delete node;
}

void IndexedDatabase::clearDatabase()
{
    clearHelper(index.root);
    index.root = nullptr;
}

int IndexedDatabase::calculateHeight(AVLNode *node) const
{
    if (!node)
        return 0;
    return 1 + std::max(calculateHeight(node->left), calculateHeight(node->right));
}

int IndexedDatabase::getTreeHeight() const
{
    return calculateHeight(index.root);
}

int IndexedDatabase::getSearchComparisons(const std::string &key, int value)
{
    search(key, value);
    return index.getLastSearchComparisons();
}
