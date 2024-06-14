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
            std::memcpy(root->dnis[0].data(), dniToInsert, 8);
            root->dnis[0][8] = '\0';  // Inserta la clave
            root->pagesID[0] = pageID;
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
                
                if ((std::memcmp(s->dnis[0].data(), dniToInsert, 8) < 0)) i++;
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

    void Serialize(const std::string& filename) {
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "Error al abrir el archivo para serializar." << std::endl;
            return;
        }

        if (root != nullptr) {
            root->Serialize(outFile);
        }

        outFile.close();
    }

    void Deserialize(const std::string& filename) {
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile.is_open()) {
            std::cerr << "Error al abrir el archivo para deserializar." << std::endl;
            return;
        }

        root = new BTreeNode(minimunDegree, true); // Se debe saber si es hoja o no
        root->Deserialize(inFile);

        inFile.close();
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