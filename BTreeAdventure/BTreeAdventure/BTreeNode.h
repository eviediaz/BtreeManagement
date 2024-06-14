#include <iostream>
#include <vector>
#include <array>
#include <cstring>
#include <string>
#include <algorithm> // para std::lower_bound y std::upper_bound

class BTreeNode 
{
public:
    int minimunDegree;
    std::vector<int> pagesID;
    std::vector<std::array<char, 9>> dnis; // this is the "key"
    std::vector<BTreeNode*> children;
    int actualNumberKeys;
    bool isLeaf;

    BTreeNode(int minimunDegree, bool isLeaf) 
    {
        this->minimunDegree = minimunDegree;
        this->isLeaf = isLeaf;
        actualNumberKeys = 0;
        pagesID.resize(2 * minimunDegree - 1);
        dnis.resize(2 * minimunDegree - 1);
        children.resize(2 * minimunDegree);
    }    
    
    void InsertNonFull(const char* dni, int pageID) {
        // Inicializa i con el índice del elemento más a la derecha
        int i = actualNumberKeys - 1;

        // Si este es un nodo hoja
        if (isLeaf) {
            // El siguiente bucle hace dos cosas:
            // a) Encuentra la ubicación de la nueva clave a insertar
            // b) Mueve todas las claves mayores un lugar hacia adelante

            while (i >= 0 && (std::memcmp(dnis[i].data(), dni, 8) > 0)) {
                dnis[i + 1] = dnis[i];
                pagesID[i + 1] = pagesID[i];
                i--;
            }

            // Inserta la nueva clave en la ubicación encontrada
            std::memcpy(dnis[i + 1].data(), dni, 8);
            pagesID[i + 1] = pageID;
            actualNumberKeys = actualNumberKeys + 1;
        }
        else { // Si este nodo no es hoja
            // Encuentra el hijo que va a tener la nueva clave
            while (i >= 0 && (std::memcmp(dnis[i].data(), dni, 8) > 0)) i--;

            // Ver si el hijo encontrado está lleno
            if (children[i + 1]->actualNumberKeys == 2 * minimunDegree - 1) {
                // Si el hijo está lleno, entonces divídelo
                SplitChild(i + 1, children[i + 1]);

                // Después de dividir, la clave del medio de children[i] sube y children[i]
                // se divide en dos. Ver cuál de los dos va a tener la nueva clave
                if (std::memcmp(dnis[i + 1].data(), dni, 8) < 0) i++;
            }
            children[i + 1]->InsertNonFull(dni, pageID);
        }
        
    }
    

    void SplitChild(int i, BTreeNode* y)
    {
        // Crea un nuevo nodo que va a almacenar (t-1) claves de y
        BTreeNode* z = new BTreeNode(y->minimunDegree, y->isLeaf);
        z->actualNumberKeys = minimunDegree - 1;

        // Copia las últimas (t-1) claves de y a z
        for (int j = 0; j < minimunDegree - 1; j++)
        {
            z->dnis[j] = y->dnis[j + minimunDegree];
            z->pagesID[j] = y->pagesID[j + minimunDegree];
        }

        // Copia los últimos t hijos de y a z
        if (!y->isLeaf) {
            for (int j = 0; j < minimunDegree; j++)
                z->children[j] = y->children[j + minimunDegree];
        }

        // Reduce el número de claves en y
        y->actualNumberKeys = minimunDegree - 1;

        // Dado que este nodo va a tener un nuevo hijo,
        // crea espacio para el nuevo hijo
        for (int j = actualNumberKeys; j >= i + 1; j--)
            children[j + 1] = children[j];

        // Enlaza el nuevo hijo a este nodo
        children[i + 1] = z;

        // Una clave de y se moverá a este nodo. Mueve todas las claves mayores
        // un lugar hacia adelante
        for (int j = actualNumberKeys - 1; j >= i; j--)
        {
            dnis[j + 1] = dnis[j];
            pagesID[j + 1] = pagesID[j];
        }

        // Copia la clave del medio de y a este nodo
        dnis[i] = y->dnis[minimunDegree - 1];
        pagesID[i] = y->pagesID[minimunDegree - 1];

        // Incrementa el conteo de claves en este nodo
        actualNumberKeys = actualNumberKeys + 1;
    }

    int search(const char* dni) {
        // Encuentra la primera clave mayor o igual a dni
        int i = 0;
        while (i < actualNumberKeys && (std::memcmp(dnis[i].data(), dni, 8) < 0)) {
            i++;
        }

        // Si la clave se encuentra en este nodo, devuelve su page ID
        if (i < actualNumberKeys && std::memcmp(dnis[i].data(), dni, 8) == 0) {
            return pagesID[i];
        }

        // Si la clave no se encuentra aquí y este es un nodo hoja
        if (isLeaf) {
            std::cout << "Clave " << dni << " no encontrada en el B-Tree." << std::endl;
            return -1; // Indicador de que la clave no se encontró
        }

        // Va al subárbol adecuado
        return children[i]->search(dni);
    }

    void Remove(const char* dni) {
        int idx = 0;
        while (idx < actualNumberKeys && (std::memcmp(dnis[idx].data(), dni, 8) < 0)) {
            idx++;
        }

        if (idx < actualNumberKeys && std::memcmp(dnis[idx].data(), dni, 8) == 0) {
            if (isLeaf) {
                RemoveFromLeaf(idx);
            }
            else {
                RemoveFromNonLeaf(idx);
            }
        }
        else {
            if (isLeaf) {
                std::cout << "La clave " << dni << " no existe en el arbol." << std::endl;
                return;
            }

            bool flag = (idx == actualNumberKeys);
            if (children[idx]->actualNumberKeys < minimunDegree) {
                Fill(idx);
            }

            if (flag && idx > actualNumberKeys) {
                children[idx - 1]->Remove(dni);
            }
            else {
                children[idx]->Remove(dni);
            }
        }
    }

    void Traverse() {
        int i;
        for (i = 0; i < actualNumberKeys; i++) {
            // Si este nodo no es hoja, entonces antes de imprimir la clave[i],
            // recorre el subárbol enraizado con el hijo children[i].
            if (!isLeaf) children[i]->Traverse();
            std::cout << " " << dnis[i].data() << " -page id: " << pagesID[i] << std::endl;
        }

        // Imprime el subárbol enraizado con el último hijo
        if (!isLeaf) children[i]->Traverse();
    }

    void printBTree(int level = 0, int index = 0) {
        // Imprime el nivel y el índice del nodo
        std::cout << std::string(level * 2, ' ') << "Nivel " << level << " - Nodo " << index << ":\n";

        // Imprime las claves y los ID de página del nodo
        std::cout << std::string(level * 2, ' ') << "Claves: ";
        for (int i = 0; i < actualNumberKeys; ++i) {
            std::cout << dnis[i].data() << " (Pagina ID: " << pagesID[i] << ") ";
        }
        std::cout << "\n";

        // Recorre los hijos del nodo
        for (int i = 0; i <= actualNumberKeys; ++i) {
            if (children[i] != nullptr) {
                children[i]->printBTree(level + 1, i);
            }
        }
    }

private:
    void RemoveFromLeaf(int idx) {
        for (int i = idx + 1; i < actualNumberKeys; ++i) {
            dnis[i - 1] = dnis[i];
            pagesID[i - 1] = pagesID[i];
        }
        actualNumberKeys--;
    }

    void RemoveFromNonLeaf(int idx) {
        std::array<char, 9> k = dnis[idx];
        if (children[idx]->actualNumberKeys >= minimunDegree) {
            std::array<char, 9> predDni = GetPredecessor(idx);
            dnis[idx] = predDni;
            pagesID[idx] = children[idx]->GetPredecessorPageID(idx);
            children[idx]->Remove(predDni.data());
        }
        else if (children[idx + 1]->actualNumberKeys >= minimunDegree) {
            std::array<char, 9> succDni = GetSuccessor(idx);
            dnis[idx] = succDni;
            pagesID[idx] = children[idx + 1]->GetSuccessorPageID(idx);
            children[idx + 1]->Remove(succDni.data());
        }
        else {
            Merge(idx);
            children[idx]->Remove(k.data());
        }
    }

    std::array<char, 9> GetPredecessor(int idx) {
        BTreeNode* cur = children[idx];
        while (!cur->isLeaf) {
            cur = cur->children[cur->actualNumberKeys];
        }
        return cur->dnis[cur->actualNumberKeys - 1];
    }

    std::array<char, 9> GetSuccessor(int idx) {
        BTreeNode* cur = children[idx + 1];
        while (!cur->isLeaf) {
            cur = cur->children[0];
        }
        return cur->dnis[0];
    }

    int GetPredecessorPageID(int idx) {
        BTreeNode* cur = children[idx];
        while (!cur->isLeaf) {
            cur = cur->children[cur->actualNumberKeys];
        }
        return cur->pagesID[cur->actualNumberKeys - 1];
    }

    int GetSuccessorPageID(int idx) {
        BTreeNode* cur = children[idx + 1];
        while (!cur->isLeaf) {
            cur = cur->children[0];
        }
        return cur->pagesID[0];
    }

    void Fill(int idx) {
        if (idx != 0 && children[idx - 1]->actualNumberKeys >= minimunDegree) {
            BorrowFromPrev(idx);
        }
        else if (idx != actualNumberKeys && children[idx + 1]->actualNumberKeys >= minimunDegree) {
            BorrowFromNext(idx);
        }
        else {
            if (idx != actualNumberKeys) {
                Merge(idx);
            }
            else {
                Merge(idx - 1);
            }
        }
    }

    void BorrowFromPrev(int idx) {
        BTreeNode* child = children[idx];
        BTreeNode* sibling = children[idx - 1];

        for (int i = child->actualNumberKeys - 1; i >= 0; --i) {
            child->dnis[i + 1] = child->dnis[i];
            child->pagesID[i + 1] = child->pagesID[i];
        }

        if (!child->isLeaf) {
            for (int i = child->actualNumberKeys; i >= 0; --i) {
                child->children[i + 1] = child->children[i];
            }
        }

        child->dnis[0] = dnis[idx - 1];
        child->pagesID[0] = pagesID[idx - 1];

        if (!isLeaf) {
            child->children[0] = sibling->children[sibling->actualNumberKeys];
        }

        dnis[idx - 1] = sibling->dnis[sibling->actualNumberKeys - 1];
        pagesID[idx - 1] = sibling->pagesID[sibling->actualNumberKeys - 1];

        child->actualNumberKeys += 1;
        sibling->actualNumberKeys -= 1;
    }

    void BorrowFromNext(int idx) {
        BTreeNode* child = children[idx];
        BTreeNode* sibling = children[idx + 1];

        child->dnis[child->actualNumberKeys] = dnis[idx];
        child->pagesID[child->actualNumberKeys] = pagesID[idx];

        if (!child->isLeaf) {
            child->children[child->actualNumberKeys + 1] = sibling->children[0];
        }

        dnis[idx] = sibling->dnis[0];
        pagesID[idx] = sibling->pagesID[0];

        for (int i = 1; i < sibling->actualNumberKeys; ++i) {
            sibling->dnis[i - 1] = sibling->dnis[i];
            sibling->pagesID[i - 1] = sibling->pagesID[i];
        }

        if (!sibling->isLeaf) {
            for (int i = 1; i <= sibling->actualNumberKeys; ++i) {
                sibling->children[i - 1] = sibling->children[i];
            }
        }

        child->actualNumberKeys += 1;
        sibling->actualNumberKeys -= 1;
    }

    void Merge(int idx) {
        BTreeNode* child = children[idx];
        BTreeNode* sibling = children[idx + 1];

        child->dnis[minimunDegree - 1] = dnis[idx];
        child->pagesID[minimunDegree - 1] = pagesID[idx];

        for (int i = 0; i < sibling->actualNumberKeys; ++i) {
            child->dnis[i + minimunDegree] = sibling->dnis[i];
            child->pagesID[i + minimunDegree] = sibling->pagesID[i];
        }

        if (!child->isLeaf) {
            for (int i = 0; i <= sibling->actualNumberKeys; ++i) {
                child->children[i + minimunDegree] = sibling->children[i];
            }
        }

        for (int i = idx + 1; i < actualNumberKeys; ++i) {
            dnis[i - 1] = dnis[i];
            pagesID[i - 1] = pagesID[i];
        }

        for (int i = idx + 2; i <= actualNumberKeys; ++i) {
            children[i - 1] = children[i];
        }

        child->actualNumberKeys += sibling->actualNumberKeys + 1;
        actualNumberKeys--;

        delete sibling;
    }
};