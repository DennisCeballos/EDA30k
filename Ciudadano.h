#include <iostream>
#include <string>

#ifndef CIUDADANOS_H
#define CIUDADANOS_H

#define SIZE_DATOS 120

struct Ciudadano
{
public:

    int dni;
    char datos[SIZE_DATOS];

    Ciudadano() { }; //datos.resize(this->sizeString); };

    Ciudadano( 
                int _dni,
                const std::string& _datos
            )
    {
        strncpy(this->datos, _datos.c_str(), 120); //datos = _datos.substr(0,120);
        dni = _dni;
    };
    
    ~Ciudadano() = default;

    //Es el sizeOf calculado individualmente por cada elemento de la clase
    const static int sizeDatos = SIZE_DATOS;
    const static int getSizeofInd = sizeof(Ciudadano::dni) + SIZE_DATOS;

    static void printOffset()
    {
        std::cout << "Size of Ciudadano: " << sizeof(Ciudadano) << " bytes" << std::endl;
        std::cout << "Offset of dni: " << offsetof(Ciudadano, Ciudadano::dni) << std::endl;
        std::cout << "Offset of telef: " << offsetof(Ciudadano, Ciudadano::datos) << std::endl;
    }

    //Getters
    int getDni()        { return dni; }
    std::string getDatos()   { return datos; }

};

#endif //CIUDADANOS_H