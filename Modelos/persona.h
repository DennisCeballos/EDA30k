#include <string>
using namespace std;

class Persona
{
private:
    char dni[8];
    string nombresyApellidos;
    string nacionalidad;
    string lugarNacimiento;
    string direccion;
    string telef;
    string correo;
    string estadoCivil;

public:
    Persona(/* args */);
    ~Persona();
};

Persona::Persona(/* args */)
{
}

Persona::~Persona()
{
}
