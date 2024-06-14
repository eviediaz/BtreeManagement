#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include "BTree.h"
#include "Person.h"

class PageManager : protected std::fstream // FStream methods and attributes are going to be protected
{

public:
    // Constructor
    PageManager(const char* filename)
    {
        this->filename = filename;
        this->file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!this->file.is_open()) {
            std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        }
        else {
            std::cout << "archivo abierto good " << filename << std::endl;
        }
    };

    // Destructor
    ~PageManager()
    {
        if (this->file.is_open()) {
            this->file.close();
        }
    }

    /// @brief Get the size of a the file
    /// @return the size of a file
    std::uintmax_t GetFileSize() {
        std::ifstream in(this->filename, std::ios::binary | std::ios::ate);
        if (!in.is_open()) {
            std::cerr << "Error al abrir el archivo para obtener el tamani." << std::endl;
            return -1;
        }
        std::uintmax_t fileSize = in.tellg();
        in.close();
        std::cout << "Tamanio del archivo: " << fileSize << " bytes" << std::endl;
        return fileSize;
        /*
        clear();
        seekg(0, std::ios::end);
        return tellg();
        */
    }

    Personita ReadGetObjectByPageID(const long& pageID)
    {
        Personita person;
        clear();
        seekg(pageID * sizeof(person), std::ios::beg);
        read(reinterpret_cast<char*>(&person), sizeof(person));
        if (!good()) {
            std::cerr << "Error al leer page: " << pageID << std::endl;
        }
        return person;
    };

    void ReadFileAndLoadToBtree(BTree& tree) {
        if (!is_open()) {
            std::cerr << "Error: el archivo no esta abierto." << std::endl;
        }

        clear(); // Limpiar cualquier bandera de error anterior
        seekg(0, std::ios::beg); // Mover el puntero al inicio del archivo        
        
        Personita person;
        const size_t chunkSize = 10000; // Leer 10000 registros a la vez
        std::vector<Personita> buffer(chunkSize);

        while (true) {
            read(reinterpret_cast<char*>(buffer.data()), chunkSize * sizeof(Personita));
            std::streamsize bytesRead = gcount();

            if (bytesRead == 0) {
                break;
            }

            size_t recordsRead = bytesRead / sizeof(Personita);

            for (size_t i = 0; i < recordsRead; ++i) {
                person = buffer[i];
                tree.Insert(person.dni, person.pageID);
            }

            if (bytesRead < static_cast<std::streamsize>(chunkSize * sizeof(Personita))) {
                break;
            }
        }        

        if (fail() && !eof()) {
            std::cerr << "Error al leer el archivo." << std::endl;
        }
    }

    void LoadDataToBTree(BTree& tree)
    {
        std::uintmax_t fileSize = GetFileSize();
        if (fileSize <= 0) {
            std::cerr << "Error: tamanio del archivo no valido." << std::endl;
            return;
        }

        size_t numRecords = fileSize / sizeof(Personita);
        // numRecords / numThreads (4 125 000) -> se insta llena a 10gb
        // 1 000 000 -> se insta llena a 4gb
        //   100 000  -> comienza con 400mb aprox hasta 1.3gb
        size_t chunkSize = 100000;
        std::cout << "numRecords: " << numRecords << "\n";
        std::cout << "chunkSize: " << chunkSize << "\n";

        // Number of threads to use for parallel processing
        unsigned int numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        threads.reserve(numThreads);
        std::atomic<int> activeThreads(0);
        // esta wea es peligrosa xd
        //size_t chunkSize = numRecords / numThreads; // Tamanio del bloque a leer en cada iteración
        std::cout << "threads: " << numThreads << "\n";


        // Mutex for thread-safe insertion to BTree
        std::mutex treeMutex;

        
        // Lambda function for loading data in each thread
        auto loadChunk = [&](size_t start, size_t end) {
            std::ifstream file(this->filename, std::ios::in | std::ios::binary);
            if (!file.is_open()) {
                std::cerr << "Error al abrir el archivo en hilo." << std::endl;
                return;
            }
            else
            {
                std::cout << "\narchivo abierto buenardo" << "\n";
            }

            
            file.seekg(start * sizeof(Personita), std::ios::beg);
            std::vector<Personita> buffer(chunkSize);            
            
            for (size_t pos = start; pos < end; pos += chunkSize) {
                size_t recordsToRead = std::min(chunkSize, end - pos);
                file.read(reinterpret_cast<char*>(buffer.data()), recordsToRead * sizeof(Personita));

                // Verificar si la lectura fue exitosa
                if (!file) {
                    std::cerr << "Error al leer desde el archivo en posición " << pos << " (leídos " << file.gcount() << " bytes)" << std::endl;
                    break;
                }

                {
                    std::lock_guard<std::mutex> lock(treeMutex);
                    for (size_t i = 0; i < recordsToRead; ++i) {
                        // Insertar directamente en el BTree
                        if (buffer[i].dni[0] != '\0') {
                            tree.Insert(buffer[i].dni, buffer[i].pageID);
                        }
                    }
                }
            }
            activeThreads--;
            std::cout << "Hilo completado para posiciones de " << start << " a " << end << std::endl;
            
        };

        size_t chunkPerThread = numRecords / numThreads;
        for (unsigned int i = 0; i < numThreads; ++i) {
            size_t start = i * chunkPerThread;
            size_t end = (i == numThreads - 1) ? numRecords : start + chunkPerThread;
            std::cout << "haciendo thread emplace back [" << i+1 << "]" << "\n";
            threads.emplace_back(loadChunk, start, end);
        }

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        

        if (this->file.fail() && !this->file.eof()) {
            std::cerr << "Error al leer el archivo." << std::endl;
        }
    }

    void AddNewPerson(BTree& tree, Personita& person) {
        // Verificar el tamaño del archivo para determinar el nuevo pageID
        long newPageID = GetFileSize() / sizeof(Personita);

        // Asignar el nuevo pageID a la persona
        person.pageID = newPageID;

        WriteObjectInDisk(newPageID, person);

        if (!good()) {
            std::cerr << "Error al escribir el nuevo registro en el archivo." << std::endl;
            return;
        }

        // Insertar la nueva persona en el B-Tree
        tree.Insert(person.dni, person.pageID);
    }

    void WriteObjectInDisk(long newPageID, Personita& person)
    {
        // Escribir el nuevo registro en el archivo
        clear();
        seekp(newPageID * sizeof(Personita), std::ios::beg);
        write(reinterpret_cast<char*>(&person), sizeof(person));
    }

    void DeleteRecordFromDisk(long pageID, BTree& tree) {
        // Leer el registro del archivo para obtener el DNI antes de eliminarlo
        Personita personToRemove = ReadGetObjectByPageID(pageID);
        std::string dniToRemove = personToRemove.dni;

        // Nombre del archivo temporal
        const char* tempFilename = "temp.bin";

        // Crear archivo temporal
        std::fstream tempFile(tempFilename, std::ios::out | std::ios::binary);
        if (!tempFile.is_open()) {
            std::cerr << "Error al abrir el archivo temporal." << std::endl;
            return;
        }

        // Leer registros del archivo original y copiar los que no se van a eliminar
        clear();
        seekg(0, std::ios::beg); // Mover el puntero al inicio del archivo
        Personita person;
        long currentID = 0;

        while (read(reinterpret_cast<char*>(&person), sizeof(Personita))) {
            if (currentID != pageID) {
                tempFile.write(reinterpret_cast<char*>(&person), sizeof(person));
            }
            currentID++;
        }

        if (fail() && !eof()) {
            std::cerr << "Error al leer el archivo original." << std::endl;
            tempFile.close();
            std::remove(tempFilename); // Eliminar el archivo temporal en caso de error
            return;
        }

        // Cerrar archivos
        tempFile.close();
        close();

        // Eliminar el archivo original y renombrar el archivo temporal
        if (std::remove(filename) != 0) {
            std::cerr << "Error al eliminar el archivo original." << std::endl;
            std::remove(tempFilename); // Eliminar el archivo temporal si falla
            return;
        }

        if (std::rename(tempFilename, filename) != 0) {
            std::cerr << "Error al renombrar el archivo temporal." << std::endl;
            return;
        }

        // Reabrir el archivo original como binario
        open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!is_open()) {
            std::cerr << "Error al reabrir el archivo: " << filename << std::endl;
        }

        // Eliminar el registro del B-Tree utilizando el DNI obtenido
        tree.Remove(dniToRemove.c_str());
    }

private:
    // Private member variable to hold the value
    std::fstream file;
    const char* filename;
};