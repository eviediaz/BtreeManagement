#include "BPlusTreeNode.h"
#include "peruvian.h"

class BPlusTree {
private:
    int order;
    BPlusTreeNode* root;
    
    /// @brief helper function to recursively serialize a node and its children
    /// @param outFile 
    /// @param node 
    void serializeNode(std::ofstream& outFile, BPlusTreeNode* node) {
        // Write node information to the file
        // 1. Write the boolean flag indicating whether the node is a leaf
        // 2. Write the number of keys in the node
        // 3. Write the keys themselves
        // 4. Write any additional data associated with the keys (e.g., pointers to citizen objects)
        // 5. Recursively serialize children (if internal node)
    }

    /// @brief Helper function to recursively deserialize a node and its children
    /// @param inFile 
    /// @return 
    BPlusTreeNode* deserializeNode(std::ifstream& inFile) {
        // Read node information from the file
        // 1. Read the boolean flag indicating whether the node is a leaf
        // 2. Read the number of keys in the node
        // 3. Read the keys themselves
        // 4. Read any additional data associated with the keys
        // 5. Recursively deserialize children (if internal node)
    }

public:
    /// @brief initialize with a leaf node
    BPlusTree() {
        root = new BPlusTreeNode(true);
    }

    /// @brief insert a peruvian record into the B+Tree
    /// @param dni 
    /// @param peruvian 
    void insert(uint64_t dni, Peruvian* peruvian) {
        // find the appropriate leaf node and insert the record
        // handle splitting if necessary
    }

    /// @brief search for a Peruvian by DNI
    /// @param dni 
    /// @return 
    Peruvian* search(uint64_t dni) {
        // traverse the B+Tree to find the correct leaf node
        // call the search method on that node
    }

    /// @brief save the B+Tree to disk
    /// @param filename 
    void saveToDisk(const std::string& filename) {
        // serialize the B+Tree and write it to a file
        // 1. Open the file for writing ( I guess by using std::ofstream)
        // 2. Traverse the B+Tree (e.g., using preorder traversal)
        // 3. Write each node's information to the file (keys, data, etc.)
        // 4. Close the file
        /*
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile) {
            std::cerr << "Error opening file for writing: " << filename << std::endl;
            return;
        }

        // Serialize the B+Tree structure and write it to the file
        serializeNode(outFile, root);

        outFile.close();
        */
    }

    /// @brief load the B+Tree from disk
    /// @param filename 
    void loadFromDisk(const std::string& filename) {
        // read the serialized B+Tree from the file and reconstruct it
        // 1. Open the file for reading ( I guess by using std::ifstream)
        // 2. Read each node's information from the file
        // 3. Create new nodes and link them together (parent-child relationships)
        // 4. Update the B+Tree's root pointer
        // 5. Close the file
        /*
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cerr << "Error opening file for reading: " << filename << std::endl;
            return;
        }

        // Read the serialized B+Tree structure from the file and reconstruct it
        root = deserializeNode(inFile);

        inFile.close();
        */
    }
};
