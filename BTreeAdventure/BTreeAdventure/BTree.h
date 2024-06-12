#pragma once
#include "BTreeNode.h"
class BTree 
{
    BTreeNode* root;
    int minimunDegree;  // Grado m�nimo
public:
    BTree(int minimunDegree) {
        root = nullptr;
        this->minimunDegree = minimunDegree;
    }

    void Insert(const char* dniToInsert, int pageID)
    {
        // Si el �rbol est� vac�o
        if (root == nullptr) {
            // Asigna memoria para la ra�z
            root = new BTreeNode(minimunDegree, true);
            root->dnis[0] = dniToInsert;  // Inserta la clave
            root->actualNumberKeys = 1;  // Actualiza el n�mero de claves en la ra�z
        }
        else { // Si el �rbol no est� vac�o
            // Si la ra�z est� llena, entonces el �rbol crece en altura
            if (root->actualNumberKeys == 2 * minimunDegree - 1) {
                // Asigna memoria para la nueva ra�z
                BTreeNode* s = new BTreeNode(minimunDegree, false);

                // Hace la antigua ra�z como hijo de la nueva ra�z
                s->children[0] = root;

                // Divide la antigua ra�z y mueve 1 clave a la nueva ra�z
                s->SplitChild(0, root);

                // La nueva ra�z tiene dos hijos ahora. Decide cu�l de los
                // dos hijos va a tener la nueva clave
                int i = 0;

                char currentDNI[9];
                std::memset(currentDNI, 0, sizeof(currentDNI)); // Inicializar dni con ceros
                std::strncpy(currentDNI, s->dnis[0].c_str(), sizeof(currentDNI));    // Copiar el DNI asegurando el terminador nulo
                currentDNI[sizeof(currentDNI) - 1] = '\0'; // Agregar car�cter nulo al final

                char dniToInsertTemp[9];
                std::memset(dniToInsertTemp, 0, sizeof(dniToInsertTemp)); // Inicializar dni con ceros
                std::strncpy(dniToInsertTemp, dniToInsert, sizeof(dniToInsertTemp));    // Copiar el DNI asegurando el terminador nulo
                dniToInsertTemp[sizeof(dniToInsertTemp) - 1] = '\0'; // Agregar car�cter nulo al final

                bool isLowerThanKey = std::memcmp(currentDNI, dniToInsertTemp, 8) < 0;

                if (isLowerThanKey) i++;
                s->children[i]->InsertNonFull(dniToInsert, pageID);

                // Cambia la ra�z
                root = s;
            }
            else { // Si la ra�z no est� llena, llama a insertNonFull para la ra�z
                root->InsertNonFull(dniToInsert, pageID);
            }
        }
    }

    int GetPageIDByDNI(const char* dni) {
        // Si la ra�z es nula, el �rbol est� vac�o, retorna -1
        if (root == nullptr) {
            std::cout << "El arbol B esta vacio. Clave " << dni << " no encontrada." << std::endl;
            return -1;
        }
        // Llama al m�todo search de la ra�z
        return root->search(dni);
    }

    void Remove(const char* dni) {
        if (!root) {
            std::cout << "El arbol esta vac�o. No se puede eliminar la clave: " << dni << std::endl;
            return;
        }

        root->Remove(dni);

        if (root->actualNumberKeys == 0) {
            BTreeNode* tmp = root;
            if (root->isLeaf) {
                root = nullptr;
            }
            else {
                root = root->children[0];
            }
            delete tmp;
        }
    }

    void TraverseTree() 
    {
        if (root != nullptr) root->Traverse();
    }

    void PrintBTree() {
        if (root != nullptr) root->printBTree();
    }

};