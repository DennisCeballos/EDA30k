#include <string>

using namespace std;

struct Ciudadano
{
public:

    int dni;
    string datos;

    Ciudadano() { };

    Ciudadano( 
                int _dni,
                string _datos
            )
    {
        //strncpy(this->datos, _datos, sizeof(this->datos));
        datos = _datos;
        dni = _dni;
    };
    
    ~Ciudadano() = default;

    //Es el sizeOf calculado individualmente por cada elemento de la clase
    static int getSizeofInd()
    {
        return
        {
            sizeof(Ciudadano::dni) +
            sizeof(Ciudadano::datos)
        };
        /*
        return sizeof(Ciudadano);
        */
    }

    static void printOffset()
    {
        std::cout << "Size of Ciudadano: " << sizeof(Ciudadano) << " bytes" << std::endl;
        std::cout << "Offset of dni: " << offsetof(Ciudadano, Ciudadano::dni) << std::endl;
        std::cout << "Offset of telef: " << offsetof(Ciudadano, Ciudadano::datos) << std::endl;
    }

    //Getters
    int getDni()        { return dni; }
    string getDatos()   { return datos; }

};

