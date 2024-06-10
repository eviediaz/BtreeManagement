#include <vector>
#include <algorithm> // para std::lower_bound y std::upper_bound

struct Peruvian {
    char dni[8];
    uint64_t idPosition;

    Peruvian(const char* dni_input, uint64_t id) : idPosition(id) {
        std::strncpy(dni, dni_input, 8);
    }
};

template <typename T, typename GenericData>
class BPlusTreeNode {
public:
    bool isLeaf; // indica si este nodo es una hoja o un nodo interno
    std::vector<T> keys; // DNIs (ordenados)
    std::vector<GenericData*> data; // punteros a objetos de ciudadanos (ordenados por keys)
    BPlusTreeNode* parent; // puntero al nodo padre
    std::vector<BPlusTreeNode*> children; // punteros a nodos hijos

    // Constructor
    BPlusTreeNode(bool isLeafNode) : isLeaf(isLeafNode), parent(nullptr) {}

    /// @brief Insertar un nuevo registro de ciudadano en este nodo
    /// @param dni
    /// @param citizen 
    void insert(const T& key, GenericData* value) {
        // Búsqueda binaria para encontrar la posición correcta para la inserción
        auto it = std::lower_bound(keys.begin(), keys.end(), key);

        // Calcular la posición para insertar el ciudadano
        size_t index = it - keys.begin();

        // Insertar el DNI en la posición correcta
        keys.insert(it, key);

        // Insertar el ciudadano en la posición correcta
        data.insert(data.begin() + index, value);

        // Si el nodo se sobrepasa, hay que dividirlo
        if (keys.size() > maxKeys) {
            BPlusTreeNode* newNode = split();

            // Actualizar el padre si es necesario
            if (parent == nullptr) {
                parent = new BPlusTreeNode(false);
                parent->children.push_back(this);
            }
            newNode->parent = parent;
            parent->children.push_back(newNode);
        }
    }

    /// @brief Buscar un peruano por DNI dentro de este nodo
    /// @param dni 
    /// @return el puntero correspondiente a Citizen* si se encuentra, de lo contrario nullptr
    GenericData* search(const T& key) {
        // Búsqueda binaria para encontrar la posición correcta
        auto it = std::lower_bound(keys.begin(), keys.end(), key);
        size_t index = it - keys.begin();

        // Comprobar si el DNI se encuentra en las keys
        if (it != keys.end() && *it == key) {
            return data[index];
        }

        // Si es un nodo hoja, devolver nullptr si no se encuentra el DNI
        if (isLeaf) {
            return nullptr;
        } else {
            // Buscar en el nodo hijo correspondiente
            return children[index]->search(key);
        }
    }

    /// @brief Dividir este nodo en dos nodos (utilizado durante la inserción)
    /// @return el nuevo nodo
    BPlusTreeNode* split() {
        // Crear un nuevo nodo (hoja o interno)
        BPlusTreeNode* newNode = new BPlusTreeNode(isLeaf);

        // Mover la mitad de las keys y data al nuevo nodo
        size_t mid = keys.size() / 2;
        newNode->keys.assign(keys.begin() + mid, keys.end());
        keys.erase(keys.begin() + mid, keys.end());

        if (isLeaf) {
            newNode->data.assign(data.begin() + mid, data.end());
            data.erase(data.begin() + mid, data.end());

            // Actualizar el puntero next
            newNode->next = next;
            next = newNode;
        } else {
            newNode->children.assign(children.begin() + mid, children.end());
            children.erase(children.begin() + mid, children.end());
        }

        return newNode;
    }

    /// @brief Load data from a file then create a b+tree from it, should use utilities method to calculate numPages
    /// @tparam T 
    /// @tparam U 
    /// @param pageManager 
    /// @param numPages 
    /// @return B+tree  from file
    template <typename T, typename U>
    BPlusTreeNode<T, U>* createBPlusTreeFromPageManager(PageManager &pageManager, long numPages) {
        BPlusTreeNode<T, U>* root = new BPlusTreeNode<T, U>(true);

        for (long i = 0; i < numPages; ++i) {
            U object = pageManager.readPage<U>(i);
            root->insert(object.dni, new U(object));
        }

        return root;
    }

private:
    static const size_t maxKeys = 4; // Número máximo de claves por nodo antes de dividir
    BPlusTreeNode* next; // Puntero al siguiente nodo (solo para nodos hoja)
};
