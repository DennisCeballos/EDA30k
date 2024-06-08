#include <iostream>
#include <string>

struct Ciudadano
{
public:

    int dni;
    std::string datos;

    Ciudadano() { };

    Ciudadano( 
                int _dni,
                std::string _datos
            )
    {
        //strncpy(this->datos, _datos, sizeof(this->datos));
        datos = _datos;
        dni = _dni;
    };
    
    ~Ciudadano() = default;

    //Es el sizeOf calculado individualmente por cada elemento de la clase
    const static int getSizeofInd = 36; //sizeof(Ciudadano::dni) + sizeof(Ciudadano::datos) ;

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

