#include <cstdlib>
#include <vector>
#include "peruvian.h"

class BPlusTreeNode {
public:
    bool isLeaf; // indicates whether this node is a leaf or internal node
    std::vector<uint64_t> keys; // DNIs (sorted)
    std::vector<Peruvian*> data; // pointers to citizen objects (sorted by keys)
    BPlusTreeNode* parent; // pointer to the parent node

    // Constructor
    BPlusTreeNode(bool isLeafNode) : isLeaf(isLeafNode), parent(nullptr) {}

    /// @brief Insert a new citizen record into this node
    /// @param dni
    /// @param citizen 
    void insert(uint64_t dni, Peruvian* citizen) {
        // binary search to find the correct position for insertion
        // update keys and data vectors accordingly
    }

    /// @brief Search for a peruvian by DNI within this node
    /// @param dni 
    /// @return the corresponding Citizen* if found, else nullptr
    Peruvian* search(uint64_t dni) {
        // binary search to find the correct position
        // return the corresponding Citizen* if found, else nullptr
    }

    /// @brief Split this node into two nodes (used during insertion)
    /// @return the new node
    BPlusTreeNode* split() {
        // create a new node (either leaf or internal)
        // move half of the keys and data to the new node
        // update parent pointers
        // return the new node
    }
};
