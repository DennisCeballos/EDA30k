#include <string>

using namespace std;

struct Ciudadano
{
public:

    char nombresyApellidos[64] = {"null"};
    char nacionalidad[32]  = {"null"};
    char lugarNacimiento[64]  = {"null"};
    char direccion[64]  = {"null"};
    char correo[32]  = {"null"};
    char estadoCivil[16]  = {"null"};
    char padding[2]; //NO HACE NADA, PERO ES PARA EL PADDING DEL COMPILADOR C++
    int dni;
    int telef;

    Ciudadano() { };

    Ciudadano(  int _dni,
                const char* _nombresyApellidos,
                const char* _nacionalidad,
                const char* _lugarNacimiento,
                const char* _direccion,
                int _telef,
                const char* _correo,
                const char* _estadoCivil
            )
    {
        this->dni = _dni;

        strncpy(this->nombresyApellidos, _nombresyApellidos, sizeof(this->nombresyApellidos));
        this->nombresyApellidos[sizeof(this->nombresyApellidos)] = '\0';

        strncpy(this->nacionalidad, _nacionalidad, sizeof(this->nacionalidad));
        this->nacionalidad[sizeof(this->nacionalidad)] = '\0';

        strncpy(this->lugarNacimiento, _lugarNacimiento, sizeof(this->lugarNacimiento));
        this->lugarNacimiento[sizeof(this->lugarNacimiento)] = '\0';
        
        strncpy(this->direccion, _direccion, sizeof(this->direccion));
        this->direccion[sizeof(this->direccion)] = '\0';

        this->telef = _telef;

        strncpy(this->correo, _correo, sizeof(this->correo));
        this->correo[sizeof(this->correo)] = '\0';
        
        strncpy(this->estadoCivil, _estadoCivil, sizeof(this->estadoCivil));
        this->estadoCivil[sizeof(this->estadoCivil)] = '\0';
    };
    
    ~Ciudadano() = default;

    //Es el sizeOf calculado individualmente por cada elemento de la clase
    static int getSizeofInd()
    {
        return
        {
            sizeof(Ciudadano::dni) +
            sizeof(Ciudadano::telef) +
            sizeof(Ciudadano::nombresyApellidos) +
            sizeof(Ciudadano::nacionalidad) +
            sizeof(Ciudadano::lugarNacimiento) +
            sizeof(Ciudadano::direccion) +
            sizeof(Ciudadano::correo) +
            sizeof(Ciudadano::estadoCivil) +
            sizeof(Ciudadano::padding)
        };
        /*
        return sizeof(Ciudadano);
        */
    }

    static void printOffset()
    {
        std::cout << "Size of Ciudadano: " << sizeof(Ciudadano) << " bytes" << std::endl;
        std::cout << "Offset of dni: " << offsetof(Ciudadano, Ciudadano::dni) << std::endl;
        std::cout << "Offset of telef: " << offsetof(Ciudadano, Ciudadano::telef) << std::endl;
        std::cout << "Offset of nombresyApellidos: " << offsetof(Ciudadano, Ciudadano::nombresyApellidos) << std::endl;
        std::cout << "Offset of nacionalidad: " << offsetof(Ciudadano, Ciudadano::nacionalidad) << std::endl;
        std::cout << "Offset of lugarNacimiento: " << offsetof(Ciudadano, Ciudadano::lugarNacimiento) << std::endl;
        std::cout << "Offset of direccion: " << offsetof(Ciudadano, Ciudadano::direccion) << std::endl;
        std::cout << "Offset of correo: " << offsetof(Ciudadano, Ciudadano::correo) << std::endl;
        std::cout << "Offset of estadoCivil: " << offsetof(Ciudadano, Ciudadano::estadoCivil) << std::endl;
        std::cout << "Offset of 'padding': " << offsetof(Ciudadano, Ciudadano::padding) << std::endl;
    }

    //Getters
    int getDni()                    { return dni; }
    string getNombresyApellidos()   { return nombresyApellidos; }
    string getNacionalidad()        { return nacionalidad; }
    string getLugarNacimiento()     { return lugarNacimiento; }
    string getDireccion()           { return direccion; }
    int getTelef()                  { return telef; }
    string getCorreo()              { return correo; }
    string getEstadoCivil()         { return estadoCivil; }

    //Setters
    /*
    void setDni(int value)                  { dni = value; }
    void setNombresyApellidos(string value) { nombresyApellidos = value; }
    void setNacionalidad(string value)      { nacionalidad = value; }
    void setLugarNacimiento(string value)   { lugarNacimiento = value; }
    void setDireccion(string value)         { direccion = value; }
    void setTelef(int value)                { telef = value; }
    void setCorreo(string value)            { correo = value; }
    void setEstadoCivil(string value)       { estadoCivil = value; }
    */
};

