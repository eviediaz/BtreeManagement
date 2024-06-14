#pragma once
#include <iostream>
#include <cstring>

class Personita
{
public:
    int pageID;
    char dni[9]; // Cambiado a 9 para incluir el carácter nulo '\0'
    char name[100];
    int edad;
    char lastname[41];      // Apellido
    char nationality[21];   // Nacionalidad
    char placeOfBirth[21];  // Lugar de nacimiento
    char address[129];      // Dirección
    char phone[17];         // Teléfono
    char email[129];        // Correo electrónico
    char civilStatus[3];    // Estado civil (ej: "S" para soltero, "M" para casado)

    // Constructor por defecto
    Personita() : edad(0), pageID(0)
    {
        std::memset(name, 0, sizeof(name));
        std::memset(dni, 0, sizeof(dni)); // Inicializar dni con ceros
        std::memset(lastname, 0, sizeof(lastname));
        std::memset(nationality, 0, sizeof(nationality));
        std::memset(placeOfBirth, 0, sizeof(placeOfBirth));
        std::memset(address, 0, sizeof(address));
        std::memset(phone, 0, sizeof(phone));
        std::memset(email, 0, sizeof(email));
        std::memset(civilStatus, 0, sizeof(civilStatus));
    }

    // Constructor parametrizado
    Personita(const char* name, int edad, const char* dni, int pageId, const char* lastname, const char* nationality,
        const char* placeOfBirth, const char* address, const char* phone, const char* email, const char* civilStatus)
        : edad(edad), pageID(pageId)
    {
        std::strncpy(this->name, name, sizeof(this->name) - 1);
        this->name[sizeof(this->name) - 1] = '\0';

        std::strncpy(this->dni, dni, sizeof(this->dni) - 1);
        this->dni[sizeof(this->dni) - 1] = '\0';

        std::strncpy(this->lastname, lastname, sizeof(this->lastname) - 1);
        this->lastname[sizeof(this->lastname) - 1] = '\0';

        std::strncpy(this->nationality, nationality, sizeof(this->nationality) - 1);
        this->nationality[sizeof(this->nationality) - 1] = '\0';

        std::strncpy(this->placeOfBirth, placeOfBirth, sizeof(this->placeOfBirth) - 1);
        this->placeOfBirth[sizeof(this->placeOfBirth) - 1] = '\0';

        std::strncpy(this->address, address, sizeof(this->address) - 1);
        this->address[sizeof(this->address) - 1] = '\0';

        std::strncpy(this->phone, phone, sizeof(this->phone) - 1);
        this->phone[sizeof(this->phone) - 1] = '\0';

        std::strncpy(this->email, email, sizeof(this->email) - 1);
        this->email[sizeof(this->email) - 1] = '\0';

        std::strncpy(this->civilStatus, civilStatus, sizeof(this->civilStatus) - 1);
        this->civilStatus[sizeof(this->civilStatus) - 1] = '\0';
    }

    void ImprimirDatos() const {
        std::cout << "DNI: " << dni << std::endl;
        std::cout << "Nombre: " << name << std::endl;
        std::cout << "Apellido: " << lastname << std::endl;
        std::cout << "Edad: " << edad << std::endl;
        std::cout << "Nacionalidad: " << nationality << std::endl;
        std::cout << "Lugar de Nacimiento: " << placeOfBirth << std::endl;
        std::cout << "Direccion: " << address << std::endl;
        std::cout << "Telefono: " << phone << std::endl;
        std::cout << "Correo: " << email << std::endl;
        std::cout << "Estado Civil: " << civilStatus << std::endl;
        std::cout << "Page ID: " << pageID << std::endl;
    }


    ~Personita() {
    }
};