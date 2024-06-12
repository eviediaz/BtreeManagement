#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <algorithm> // para std::lower_bound y std::upper_bound

class BTreeNode 
{
public:
    int minimunDegree;
    std::vector<int> pagesID;
    std::vector<std::string> dnis; // this is the "key"
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
        // Inicializa i con el �ndice del elemento m�s a la derecha
        int i = actualNumberKeys - 1;

        // Si este es un nodo hoja
        if (isLeaf) {
            // El siguiente bucle hace dos cosas:
            // a) Encuentra la ubicaci�n de la nueva clave a insertar
            // b) Mueve todas las claves mayores un lugar hacia adelante

            while (i >= 0 && (std::memcmp(dnis[i].c_str(), dni, 8) > 0)) {
                dnis[i + 1] = dnis[i];
                pagesID[i + 1] = pagesID[i];
                i--;
            }

            // Inserta la nueva clave en la ubicaci�n encontrada
            dnis[i + 1] = std::string(dni);
            pagesID[i + 1] = pageID;
            actualNumberKeys = actualNumberKeys + 1;
        }
        else { // Si este nodo no es hoja
            // Encuentra el hijo que va a tener la nueva clave
            while (i >= 0 && (std::memcmp(dnis[i].c_str(), dni, 8) > 0)) i--;

            // Ver si el hijo encontrado est� lleno
            if (children[i + 1]->actualNumberKeys == 2 * minimunDegree - 1) {
                // Si el hijo est� lleno, entonces div�delo
                SplitChild(i + 1, children[i + 1]);

                // Despu�s de dividir, la clave del medio de children[i] sube y children[i]
                // se divide en dos. Ver cu�l de los dos va a tener la nueva clave
                if (std::memcmp(dnis[i + 1].c_str(), dni, 8) < 0) i++;
            }
            children[i + 1]->InsertNonFull(dni, pageID);
        }
        
    }
    

    void SplitChild(int i, BTreeNode* y)
    {
        // Crea un nuevo nodo que va a almacenar (t-1) claves de y
        BTreeNode* z = new BTreeNode(y->minimunDegree, y->isLeaf);
        z->actualNumberKeys = minimunDegree - 1;

        // Copia las �ltimas (t-1) claves de y a z
        for (int j = 0; j < minimunDegree - 1; j++)
        {
            z->dnis[j] = y->dnis[j + minimunDegree];
            z->pagesID[j] = y->pagesID[j + minimunDegree];
        }

        // Copia los �ltimos t hijos de y a z
        if (!y->isLeaf) {
            for (int j = 0; j < minimunDegree; j++)
                z->children[j] = y->children[j + minimunDegree];
        }

        // Reduce el n�mero de claves en y
        y->actualNumberKeys = minimunDegree - 1;

        // Dado que este nodo va a tener un nuevo hijo,
        // crea espacio para el nuevo hijo
        for (int j = actualNumberKeys; j >= i + 1; j--)
            children[j + 1] = children[j];

        // Enlaza el nuevo hijo a este nodo
        children[i + 1] = z;

        // Una clave de y se mover� a este nodo. Mueve todas las claves mayores
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

    void Traverse() {
        int i;
        for (i = 0; i < actualNumberKeys; i++) {
            // Si este nodo no es hoja, entonces antes de imprimir la clave[i],
            // recorre el sub�rbol enraizado con el hijo children[i].
            if (!isLeaf) children[i]->Traverse();
            std::cout << " " << dnis[i] << " -page id: " << pagesID[i] << std::endl;
        }

        // Imprime el sub�rbol enraizado con el �ltimo hijo
        if (!isLeaf) children[i]->Traverse();
    }

    void printBTree(int level = 0, int index = 0) {
        // Imprime el nivel y el �ndice del nodo
        std::cout << std::string(level * 2, ' ') << "Nivel " << level << " - Nodo " << index << ":\n";

        // Imprime las claves y los ID de p�gina del nodo
        std::cout << std::string(level * 2, ' ') << "Claves: ";
        for (int i = 0; i < actualNumberKeys; ++i) {
            std::cout << dnis[i] << " (Pagina ID: " << pagesID[i] << ") ";
        }
        std::cout << "\n";

        // Recorre los hijos del nodo
        for (int i = 0; i <= actualNumberKeys; ++i) {
            if (children[i] != nullptr) {
                children[i]->printBTree(level + 1, i);
            }
        }
    }
};

/*
#include <iostream>
#include <vector>

using namespace std;

class BTreeNode {
    int t;  // Grado m�nimo (define el rango de n�mero de claves)
    vector<int> keys; // Vector de claves
    vector<BTreeNode*> children; // Vector de punteros a los hijos
    int n;     // N�mero actual de claves
    bool leaf; // Es verdadero cuando el nodo es hoja
public:
    BTreeNode(int _t, bool _leaf);

    // Funci�n para recorrer todos los nodos en un sub�rbol enraizado con este nodo
    void traverse();

    // Funci�n para insertar una nueva clave en el sub�rbol enraizado con este nodo
    void insertNonFull(int k);

    // Funci�n para dividir el hijo y de este nodo. i es el �ndice de y en el vector children
    void splitChild(int i, BTreeNode* y);

    friend class BTree;
};

class BTree {
    BTreeNode* root;
    int t;  // Grado m�nimo
public:
    BTree(int _t) {
        root = nullptr;
        t = _t;
    }

    void traverse() {
        if (root != nullptr) root->traverse();
    }

    void insert(int k);
};

BTreeNode::BTreeNode(int t1, bool leaf1) {
    t = t1;
    leaf = leaf1;
    n = 0;
    keys.resize(2*t-1);
    children.resize(2*t);
}

void BTreeNode::traverse() {
    int i;
    for (i = 0; i < n; i++) {
        // Si este nodo no es hoja, entonces antes de imprimir la clave[i],
        // recorre el sub�rbol enraizado con el hijo children[i].
        if (!leaf) children[i]->traverse();
        cout << " " << keys[i];
    }

    // Imprime el sub�rbol enraizado con el �ltimo hijo
    if (!leaf) children[i]->traverse();
}

void BTreeNode::insertNonFull(int k) {
    // Inicializa i con el �ndice del elemento m�s a la derecha
    int i = n-1;

    // Si este es un nodo hoja
    if (leaf) {
        // El siguiente bucle hace dos cosas:
        // a) Encuentra la ubicaci�n de la nueva clave a insertar
        // b) Mueve todas las claves mayores un lugar hacia adelante
        while (i >= 0 && keys[i] > k) {
            keys[i+1] = keys[i];
            i--;
        }

        // Inserta la nueva clave en la ubicaci�n encontrada
        keys[i+1] = k;
        n = n+1;
    } else { // Si este nodo no es hoja
        // Encuentra el hijo que va a tener la nueva clave
        while (i >= 0 && keys[i] > k) i--;

        // Ver si el hijo encontrado est� lleno
        if (children[i+1]->n == 2*t-1) {
            // Si el hijo est� lleno, entonces div�delo
            splitChild(i+1, children[i+1]);

            // Despu�s de dividir, la clave del medio de children[i] sube y children[i]
            // se divide en dos. Ver cu�l de los dos va a tener la nueva clave
            if (keys[i+1] < k) i++;
        }
        children[i+1]->insertNonFull(k);
    }
}

void BTreeNode::splitChild(int i, BTreeNode* y) {
    // Crea un nuevo nodo que va a almacenar (t-1) claves de y
    BTreeNode* z = new BTreeNode(y->t, y->leaf);
    z->n = t - 1;

    // Copia las �ltimas (t-1) claves de y a z
    for (int j = 0; j < t-1; j++)
        z->keys[j] = y->keys[j+t];

    // Copia los �ltimos t hijos de y a z
    if (!y->leaf) {
        for (int j = 0; j < t; j++)
            z->children[j] = y->children[j+t];
    }

    // Reduce el n�mero de claves en y
    y->n = t - 1;

    // Dado que este nodo va a tener un nuevo hijo,
    // crea espacio para el nuevo hijo
    for (int j = n; j >= i+1; j--)
        children[j+1] = children[j];

    // Enlaza el nuevo hijo a este nodo
    children[i+1] = z;

    // Una clave de y se mover� a este nodo. Mueve todas las claves mayores
    // un lugar hacia adelante
    for (int j = n-1; j >= i; j--)
        keys[j+1] = keys[j];

    // Copia la clave del medio de y a este nodo
    keys[i] = y->keys[t-1];

    // Incrementa el conteo de claves en este nodo
    n = n + 1;
}

void BTree::insert(int k) {
    // Si el �rbol est� vac�o
    if (root == nullptr) {
        // Asigna memoria para la ra�z
        root = new BTreeNode(t, true);
        root->keys[0] = k;  // Inserta la clave
        root->n = 1;  // Actualiza el n�mero de claves en la ra�z
    } else { // Si el �rbol no est� vac�o
        // Si la ra�z est� llena, entonces el �rbol crece en altura
        if (root->n == 2*t-1) {
            // Asigna memoria para la nueva ra�z
            BTreeNode* s = new BTreeNode(t, false);

            // Hace la antigua ra�z como hijo de la nueva ra�z
            s->children[0] = root;

            // Divide la antigua ra�z y mueve 1 clave a la nueva ra�z
            s->splitChild(0, root);

            // La nueva ra�z tiene dos hijos ahora. Decide cu�l de los
            // dos hijos va a tener la nueva clave
            int i = 0;
            if (s->keys[0] < k) i++;
            s->children[i]->insertNonFull(k);

            // Cambia la ra�z
            root = s;
        } else { // Si la ra�z no est� llena, llama a insertNonFull para la ra�z
            root->insertNonFull(k);
        }
    }
}

// Programa de prueba
int main() {
    BTree t(4); // Un �rbol B con grado m�nimo 4

    t.insert(70);
    t.insert(50);
    t.insert(30);
    t.insert(40);
    t.insert(20);
    t.insert(80);
    t.insert(25);
    t.insert(90);
    t.insert(75);
    t.insert(10);
    t.insert(15);

    cout << "El recorrido del arbol construido es:" << endl;
    t.traverse();
    cout << endl;

    return 0;
}
*/