#include <cstdlib>
#include <string>

// may be changed to a class instead of a struct ?
struct Peruvian {
    uint64_t DNI; // 8-digit ID
    // tip: may be update the variable type for the fields below
    std::string firstName;
    std::string lastName;
    std::string nationality;
    std::string placeOfBirth;
    std::string address;
    std::string phoneNumber;
    std::string email;
    std::string maritalStatus;
};
