#pragma once

#ifndef BTREE_DISK_ITERATOR
#define BTREE_DISK_ITERATOR

#include "BTreeNodeDisk.h"
#include "DiskPageManager.h"

namespace APP_CORE
{
	namespace DISK
	{
        template<class T, int B> // Define a template for the Iterator class with a type T and an integer B
        class Iterator // Define the Iterator class
        { 
        public:
            typedef Node<T, B> node; // Define a typedef for convenience
            int index = 0;  // Initialize the index variable to 0
            node currentPosition; // Declare a variable to hold the current position in the tree

            Iterator() {}

            /// <summary>
            /// Constructor that takes a shared pointer to a page manager
            /// </summary>
            /// <param name="pm"></param>
            Iterator(std::shared_ptr <DiskPageManager> pm) : pageManager{ pm } {}

            /// <summary>
            /// Overloaded dereference operator (like a pointer) to return the current data 
            /// </summary>
            /// <returns></returns>
            T operator*() {
                return currentPosition.data[index];
            }

            /// <summary>
            /// Overloaded incremeent operator for advancing the iterator
            /// </summary>
            /// <returns></returns>
            Iterator& operator++() {
                if (index < currentPosition.count - 1)  // If there are more elements in the current node
                {
                    index++; // Increment the index
                }
                else 
                {
                    index = 0; // reset the index to 0
                    node aux{ -1 }; // Create a sentinel node with page_id -1
                    if (currentPosition.right == 0) // Check if it's a sentinel node
                    {
                        currentPosition = aux; // Set the current position to the sentinel node
                    }
                    else {
                        this->pageManager->recover(currentPosition.right, currentPosition);
                    }
                }
                return *this;
            }

            /// <summary>
            /// // Overloaded assignment operator
            /// </summary>
            /// <param name="copy"></param>
            /// <returns></returns>
            Iterator& operator=(const Iterator& copy) 
            {
                this->currentPosition = copy.currentPosition;
                this->index = copy.index;
                this->pageManager = copy.pageManager;
                return *this;
            }

            /// <summary>
            /// Copy constructor
            /// </summary>
            /// <param name="copy"></param>
            Iterator(const Iterator& copy) 
            {
                this->currentPosition = copy.currentPosition;
                this->index = copy.index;
                this->pageManager = copy.pageManager;
            }

            /// <summary>
            /// Overloaded inequality operator
            /// </summary>
            /// <param name="other"></param>
            /// <returns></returns>
            bool operator!=(const Iterator& other) const 
            {
                if (this->currentPosition.page_id == other.currentPosition.page_id) 
                {
                    if (this->currentPosition.data[index] == 
                        other.currentPosition.data[other.index]) 
                    {
                        return false;
                    }
                    else return true;
                }
                return true;
            }

        private:
            std::shared_ptr <DiskPageManager> pageManager;
        };
	}
}

#endif // !BTREE_DISK_ITERATOR

/*
* #include "node.h"  // Include the node header file
#include "pagemanager.h"  // Include the page manager header file

namespace utec {
    namespace disk {
        template<class T, int B>  // Define a template for the Iterator class with a type T and an integer B
        class Iterator {  // Define the Iterator class
        public:
            typedef Node<T, B> node;  // Define a typedef for convenience
            int index = 0;  // Initialize the index variable to 0
            node currentPosition;  // Declare a variable to hold the current position in the tree

            Iterator() {}  // Default constructor

            // Constructor that takes a shared pointer to a page manager
            Iterator(std::shared_ptr<pagemanager> pm) : pm{pm} {}

            // Overloaded dereference operator to return the current data
            T operator*() {
                return currentPosition.data[index];  // Return the data at the current index
            }

            // Overloaded pre-increment operator for advancing the iterator
            Iterator &operator++() {
                if (index < currentPosition.count - 1) {  // If there are more elements in the current node
                    index++;  // Increment the index
                } else {
                    index = 0;  // Reset the index to 0
                    node aux{-1};  // Create a sentinel node with page_id -1
                    if (currentPosition.right == 0) {  // Check if it's a sentinel node
                        currentPosition = aux;  // Set the current position to the sentinel node
                    } else {
                        this->pm->recover(currentPosition.right, currentPosition);  // Move to the next node
                    }
                }
                return *this;  // Return the updated iterator
            }

            // Overloaded assignment operator
            Iterator &operator=(const Iterator &copy) {
                this->currentPosition = copy.currentPosition;  // Assign the current position
                this->index = copy.index;  // Assign the index
                this->pm = copy.pm;  // Assign the page manager
                return *this;  // Return the updated iterator
            }

            // Copy constructor
            Iterator(const Iterator &copy) {
                this->currentPosition = copy.currentPosition;  // Copy the current position
                this->index = copy.index;  // Copy the index
                this->pm = copy.pm;  // Copy the page manager
            }

            // Overloaded inequality operator
            bool operator!=(const Iterator &other) const {
                if (this->currentPosition.page_id == other.currentPosition.page_id) {  // If the page IDs are the same
                    if (this->currentPosition.data[index] == other.currentPosition.data[other.index]) {  // If the data at the current index is the same
                        return false;  // Return false
                    } else return true;  // Otherwise, return true
                }
                return true;  // If page IDs are different, return true
            }

        private:
            std::shared_ptr<pagemanager> pm;  // Declare a shared pointer to a page manager
        };
    }
}

*/