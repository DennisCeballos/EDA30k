#include <iostream>
#include <windows.h>

#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <iomanip>

#include "gestionArbol.h"
#include "gestionCiudadanos.h"

#define ARRIBA 'w' // Código de flecha arriba
#define ABAJO 's'  // Código de flecha abajo
#define ENTER 13   // Código de Enter

using namespace std;

char getch2()
{
    char c = 0;
    DWORD modo, contador;
    HANDLE ih = GetStdHandle(STD_INPUT_HANDLE);
    // Obtener el modo actual de la consola
    GetConsoleMode(ih, &modo);

    // Desactivamos escritura en terminal
    SetConsoleMode(ih, modo & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

    ReadConsoleA(ih, &c, 1, &contador, NULL);

    if (c == 0)
    {
        ReadConsoleA(ih, &c, 1, &contador, NULL);
    }

    SetConsoleMode(ih, modo); // Devolvemos control normal

    return c;
}

void gotoxy(int x, int y)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    SetConsoleCursorPosition(hConsole, pos);
}

void setColor(int textColor, int backgroundColor)
{
    int color = (backgroundColor << 4) | textColor;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

bool esNumero(const string &str)
{
    for (char c : str)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }
    return true;
}

bool esSoloLetras(const string &str)
{
    for (char c : str)
    {
        if (!isalpha(c) && c != ' ')
        {
            return false;
        }
    }
    return true;
}

bool esCorreoValido(const string &correo)
{
    const regex pattern("[\\w-]+@([\\w-]+\\.)+[\\w-]+");
    return regex_match(correo, pattern);
}

bool esTelefonoValido(const string &telefono)
{
    for (char c : telefono)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }
    return telefono.length() >= 7 && telefono.length() <= 15;
}

bool esEstadoCivilValido(const string &estadoCivil)
{
    return esSoloLetras(estadoCivil);
}

void imprimirDNI(int dni)
{
    cout << std::setw(8) << std::setfill('0') << dni << std::endl;
}

int menu(const char titulo[], const char *opciones[], int n);
void menu_Insertar();
void menu_Eliminar();
void menu_Buscar();
void menu_Editar();
void mostrarCiudadanos();

/*
Variables globales importantes
*/
const char *archivoCiudadano = "datosCiudadanos.bin";
const char *archivoArbol = "datosArbol.bin";

// Crear la clase Gestionador del Arbol
ArbolEnDisco gestionArbol(archivoArbol);
fstream arbolData_stream(archivoArbol, std::ios::in | std::ios::out | std::ios::binary);

// Crear la clase Gestionador de Ciudadanos
CtrlCiudadano gestionCiudadanos(archivoCiudadano);
/*
Variables globales importantes
*/

int main()
{
    // Menu principal
    bool repite = true;
    int opcion;

    // Titulo del menú y nombres de las opciones
    const char *titulo = "MENU DE OPCIONES (usar w y s para mover la flecha y enter para seleccionar)";
    const char *opciones[] = {"Ingresar datos de ciudadano", "Eliminar datos de ciudadano", "Buscar ciudadano", "Editar datos de ciudadano", "Mostrar ciudadanos", "Salir"};

    int n = 6; // Numero de opciones

    do
    {
        opcion = menu(titulo, opciones, n);

        switch (opcion)
        {
        case 1:
            menu_Insertar();
            break;

        case 2:
            menu_Eliminar();
            break;

        case 3:
            menu_Buscar();
            break;

        case 4:
            menu_Editar();
            break;

        case 5:
            mostrarCiudadanos();
            break;

        case 6:
            cout << "\nEl programa se cerrara!! ADIOS" << endl;
            repite = false;
            system("pause>nul");
            break;
        }

    } while (repite);

    // Cerrar ambos archivos
    //gestionArbol.~ArbolEnDisco();
    //gestionCiudadanos.~CtrlCiudadano();
    return 0;
}

void menu_Insertar()
{
    bool repite = true;
    int opcion;

    // Titulo del menú y nombres de las opciones
    const char *titulo = "MENU PARA INGRESAR DATOS DE CIUDADANO";
    const char *opciones[] = {"Ingresar Datos", "Regresar"};
    int n = 2; // Numero de opciones

    string DNI, nombres, apellidos, nacionalidad, lugarNacimiento, direccion, telefono, correo, estadoCivil;
    string datos;

    do
    {
        opcion = menu(titulo, opciones, n);

        system("cls");
        switch (opcion)
        {
        case 1:
            do
            {
                cout << "DNI (8 caracteres numericos): ";
                cin >> DNI;

                if (DNI.length() != 8 || !esNumero(DNI))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: El DNI debe tener exactamente 8 caracteres numericos." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (DNI.length() != 8 || !esNumero(DNI));

            do
            {
                cout << "Nombres: ";
                cin >> nombres;

                if (!esSoloLetras(nombres))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: Los nombres solo deben contener letras." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esSoloLetras(nombres));

            do
            {
                cout << "Apellidos: ";
                cin >> apellidos;

                if (!esSoloLetras(apellidos))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: Los apellidos solo deben contener letras." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esSoloLetras(apellidos));

            do
            {
                cout << "Nacionalidad: ";
                cin >> nacionalidad;

                if (!esSoloLetras(nacionalidad))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: La nacionalidad solo debe contener letras." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esSoloLetras(nacionalidad));

            do
            {
                cout << "Lugar de Nacimiento: ";
                cin >> lugarNacimiento;

                if (!esSoloLetras(lugarNacimiento))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: El lugar de nacimiento solo debe contener letras." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esSoloLetras(lugarNacimiento));

            cout << "Direccion: ";
            cin >> direccion;

            do
            {
                cout << "Telefono: ";
                cin >> telefono;

                if (!esTelefonoValido(telefono))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: El telefono debe tener entre 7 y 15 caracteres numericos." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esTelefonoValido(telefono));

            do
            {
                cout << "Correo: ";
                cin >> correo;

                if (!esCorreoValido(correo))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: El correo no es valido." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esCorreoValido(correo));

            do
            {
                cout << "Estado Civil ( solter@ | casad@ | viud@): ";
                cin >> estadoCivil;

                if (!esEstadoCivilValido(estadoCivil))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: Estado civil no valido." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esEstadoCivilValido(estadoCivil));

            // Proceso de guardado en disco, tanto arbol como ciudadano
            {
                Nodo* buscado = gestionArbol.Buscar_Disco(arbolData_stream, stoi(DNI));
                if (buscado->id != -1)
                {
                    setColor(0xE, 0xA);
                    cout << "Ya existe un ciudadano con ese DNI" << endl;
                    setColor(0xE, 0x7);
                    system("pause>nul");
                    break;
                }
                

                // string DNI, nombres, apellidos, nacionalidad, lugarNacimiento, direccion, telefono, correo, estadoCivil;
                datos = nombres + "," + apellidos + "," + nacionalidad + "," +
                        lugarNacimiento + "," + direccion + "," + telefono + "," +
                        correo + "," + estadoCivil;

                Ciudadano *ciudadano = new Ciudadano(stoi(DNI), datos);

                // LLAMADA A CLASE GESTIONCIUDADANOS
                int idCiudadano = gestionCiudadanos.guardarCiudadano(ciudadano, true);

                // LLAMADA A CLASE GESTIONARBOL
                gestionArbol.Insertar_Disco(arbolData_stream, Elemento(idCiudadano, stoi(DNI)));
            }

            setColor(0xE, 0xA);
            cout << "Datos registrados" << endl;
            setColor(0xE, 0x7);

            system("pause>nul");
            break;

        case 2:
            repite = false;
            break;
        }

    } while (repite);
}

void menu_Eliminar()
{
    bool repite = true;
    int opcion;

    // Titulo del menú y nombres de las opciones
    const char *titulo = "MENU PARA ELIMINAR DATOS DE CIUDADANO";
    const char *opciones[] = {"Ingresar DNI de persona ", "Regresar"};
    int n = 2; // Numero de opciones

    string DNI;

    do
    {
        opcion = menu(titulo, opciones, n);

        system("cls");
        switch (opcion)
        {
        case 1:
            // Mensaje de error de datos del DNI
            do
            {
                cout << "DNI (8 caracteres numericos): ";
                cin >> DNI;

                if (DNI.length() != 8 || !esNumero(DNI))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: El DNI debe tener exactamente 8 caracteres numericos." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (DNI.length() != 8 || !esNumero(DNI));

            //Proceso de eliminacion en disco
            {
                Nodo *nodo = gestionArbol.Buscar_Disco(arbolData_stream, stoi(DNI));
                if (nodo->id == -1)
                {
                    setColor(0xE, 0xA);
                    cout << "No existe un ciudadano registrado con ese DNI" << endl;
                    setColor(0xE, 0x7);
                }
                else
                {
                    gestionArbol.Eliminar_Disco(arbolData_stream, stoi(DNI));
                    setColor(0xE, 0xA);
                    cout << "Datos Encontrados y Eliminados" << endl;
                    setColor(0xE, 0x7);
                }
            }
            system("pause>nul");
            break;

        case 2:
            repite = false;
            break;
        }

    } while (repite);
}

void menu_Buscar()
{
    bool repite = true;
    int opcion;

    // Titulo del menú y nombres de las opciones
    const char *titulo = "MENU PARA BUSCAR DATOS DE CIUDADANO";
    const char *opciones[] = {"Ingresar DNI de persona", "Regresar"};
    int n = 2; // Numero de opciones

    string DNI;

    do
    {
        opcion = menu(titulo, opciones, n);

        system("cls");
        switch (opcion)
        {
        case 1:
            do
            {
                cout << "Nro. Ciudadanos registrados: " << gestionCiudadanos.getNroCiudadanos() << endl;

                cout << "DNI (8 caracteres numericos): ";
                cin >> DNI;

                if (DNI.length() != 8 || !esNumero(DNI))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: El DNI debe tener exactamente 8 caracteres numericos." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (DNI.length() != 8 || !esNumero(DNI));

            // Proceso de buscar datos desde archivos
            {
                int dniInt = stoi(DNI);
                Nodo *nodo = gestionArbol.Buscar_Disco(arbolData_stream, dniInt);
                gestionArbol.recorridoInOrder(arbolData_stream, gestionArbol.idRaiz);
                if (nodo->id == -1)
                {
                    cout << "No se encontro un usuario con DNI ";
                    imprimirDNI(dniInt);
                }
                else
                {
                    Ciudadano *encontrado = gestionCiudadanos.obtenerCiudadanoEnPos(nodo->elemento.id, false);
                    setColor(0xE, 0xA);
                    cout << "Persona encontrada\n";
                    setColor(0x7, 0xE);

                    cout << "--------------DATOS----------------------\n";
                    imprimirDNI(dniInt);
                    cout << encontrado->datos;
                }
            }

            system("pause>nul");
            break;

        case 2:
            repite = false;
            break;
        }

    } while (repite);
}

void menu_Editar()
{
    bool repite = true;
    int opcion;

    // Titulo del menú y nombres de las opciones
    const char *titulo = "MENU PARA EDITAR DATOS DE CIUDADANO";
    const char *opciones[] = {"Ingresar DNI de persona", "Regresar"};
    int n = 2; // Numero de opciones

    string DNI, nombres, apellidos, nacionalidad, lugarNacimiento, direccion, telefono, correo, estadoCivil;
    string datos;

    do
    {
        opcion = menu(titulo, opciones, n);

        system("cls");
        switch (opcion)
        {
        case 1:
            do
            {
                cout << "DNI (8 caracteres numericos): ";
                cin >> DNI;

                if (DNI.length() != 8 || !esNumero(DNI))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: El DNI debe tener exactamente 8 caracteres numericos." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (DNI.length() != 8 || !esNumero(DNI));

            do
            {
                cout << "Nombres: ";
                cin >> nombres;

                if (!esSoloLetras(nombres))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: Los nombres solo deben contener letras." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esSoloLetras(nombres));

            do
            {
                cout << "Apellidos: ";
                cin >> apellidos;

                if (!esSoloLetras(apellidos))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: Los apellidos solo deben contener letras." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esSoloLetras(apellidos));

            do
            {
                cout << "Nacionalidad: ";
                cin >> nacionalidad;

                if (!esSoloLetras(nacionalidad))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: La nacionalidad solo debe contener letras." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esSoloLetras(nacionalidad));

            do
            {
                cout << "Lugar de Nacimiento: ";
                cin >> lugarNacimiento;

                if (!esSoloLetras(lugarNacimiento))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: El lugar de nacimiento solo debe contener letras." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esSoloLetras(lugarNacimiento));

            cout << "Direccion: ";
            cin >> direccion;

            do
            {
                cout << "Telefono: ";
                cin >> telefono;

                if (!esTelefonoValido(telefono))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: El telefono debe tener entre 7 y 15 caracteres numericos." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esTelefonoValido(telefono));

            do
            {
                cout << "Correo: ";
                cin >> correo;

                if (!esCorreoValido(correo))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: El correo no es valido." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esCorreoValido(correo));

            do
            {
                cout << "Estado Civil (solter@ | casad@ | viud@): ";
                cin >> estadoCivil;

                if (!esEstadoCivilValido(estadoCivil))
                {
                    setColor(0xE, 0xC); // resaltar de rojo
                    cout << "Error: Estado civil no valido." << endl;
                    setColor(0xE, 0x7); // regresar al color del fondo
                }
            } while (!esEstadoCivilValido(estadoCivil));


            // Proceso de guardado en disco, tanto arbol como ciudadano
            {
                // string DNI, nombres, apellidos, nacionalidad, lugarNacimiento, direccion, telefono, correo, estadoCivil;
                datos = nombres + ";" + apellidos + ";" + nacionalidad + ";" +
                        lugarNacimiento + ";" + direccion + ";" + telefono + ";" +
                        correo + ";" + estadoCivil;

                Ciudadano *ciudadano = new Ciudadano(stoi(DNI), datos);

                // LLAMADA A CLASE GESTIONCIUDADANOS
                int idCiudadano = gestionCiudadanos.guardarCiudadano(ciudadano, true);

                // LLAMADA A CLASE GESTIONARBOL
                gestionArbol.Insertar_Disco(arbolData_stream, Elemento(idCiudadano, stoi(DNI)));
            }

            setColor(0xE, 0xA);
            cout << "Datos actualizados" << endl;
            setColor(0x7, 0xE);

            system("pause>nul");
            break;

        case 2:
            repite = false;
            break;
        }

    } while (repite);
}

void mostrarCiudadanos()
{
    
    system("cls");
    int nro;

    cout<<"Cuantos ciudadanos registrados deseas ver (maximo de lista "<<gestionCiudadanos.getNroCiudadanos()<<")"<<endl;
    cin>>nro;

    setColor(0xE, 0xA);
    cout << "Listado de Ciudadanos:" << endl;
    setColor(0x7, 0xE);

    if (nro > gestionCiudadanos.getNroCiudadanos())
    {
        nro = gestionCiudadanos.getNroCiudadanos();
    }
    
    std::vector<std::string> elements;
    std::string element;

    Ciudadano *ciudadano;
    for (int i = 0; i < nro; i++)
    {
        ciudadano = gestionCiudadanos.obtenerCiudadanoEnPos(i, false);
        std::istringstream iss(ciudadano->datos);
        elements.clear();

        while (std::getline(iss, element, ',')) {
            elements.push_back(element);
        }

        cout << "--------------------------------" << endl;
        cout << "DNI: "                 << ciudadano->dni << endl;
        cout << "Nombres: "             << elements.at(0) << endl;
        cout << "Apellidos: "           << elements.at(1) << endl;
        cout << "Nacionalidad: "        << elements.at(2) << endl;
        cout << "Lugar de Nacimiento: " << elements.at(3) << endl;
        cout << "Direccion: "           << elements.at(4) << endl;
        cout << "Telefono: "            << elements.at(5) << endl;
        cout << "Correo: "              << elements.at(6) << endl;
        cout << "Estado Civil: "        << elements.at(7) << endl;
        cout << "--------------------------------" << endl;

    }

    system("pause>nul");
    
}

int menu(const char titulo[], const char *opciones[], int n)
{
    int opcionSeleccionada = 1; // Indica la opcionSeleccionada
    int tecla;
    bool repite = true; // controla el bucle para regresar a la rutina que lo llamo, al presionar ENTER

    do
    {
        system("cls");
        system("color 7e");
        gotoxy(5, 3 + opcionSeleccionada);
        cout << "==>" << endl;

        // Imprime el título del menú
        gotoxy(15, 2);
        cout << titulo;

        // Imprime las opciones del menú
        for (int i = 0; i < n; ++i)
        {
            gotoxy(10, 4 + i);
            cout << i + 1 << ") " << opciones[i];
        }

        // Solo permite que se ingrese ARRIBA, ABAJO o ENTER
        do
        {
            tecla = getch2();
        } while (tecla != ARRIBA && tecla != ABAJO && tecla != ENTER);

        switch (tecla)
        {
        case ARRIBA: // En caso que se presione ARRIBA
            opcionSeleccionada--;
            if (opcionSeleccionada < 1)
            {
                opcionSeleccionada = n;
            }
            break;

        case ABAJO:
            opcionSeleccionada++;
            if (opcionSeleccionada > n)
            {
                opcionSeleccionada = 1;
            }
            break;

        case ENTER:
            repite = false;
            break;
        }

    } while (repite);

    return opcionSeleccionada;
}

int mainCarga() // main cargaMasiva
{
    
    std::ifstream file("TF_datosDNI.csv"); // nombre del archivo CSV
    std::vector<Ciudadano *> ciudadanos;
    std::vector<int> dnis;

    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(iss, token, ','))
            {
                tokens.push_back(token);
            }

            if (tokens.size() == 13)
            { // assuming 13 columns in the CSV file
                int dni = std::stoi(tokens[0]);
                std::string datos = tokens[1] + ";" + tokens[2] + ";" + tokens[3] + ";" + tokens[4] + ";" +
                                    tokens[5] + ";" + tokens[6] + ";" + tokens[7] + ";" + tokens[8] + ";" +
                                    tokens[9] + ";" + tokens[10] + ";" + tokens[11] + ";" + tokens[12];

                Ciudadano *ciudadano = new Ciudadano(dni, datos);
                ciudadanos.push_back(ciudadano);
                dnis.push_back(dni);
            }
        }

        file.close();
    }
    else
    {
        std::cerr << "Unable to open file" << std::endl;
        return 1;
    }
    cout << "Se termino de obtener Ciudadanos" << endl;
    // Guardar masivamente en un binario los ciudadanos
    vector<int> ids = gestionCiudadanos.guardarCiudadano_MASIVO(ciudadanos, false);
    // vector<int> dnis generado anteriormente

    /*
    //Almacenar los ids en un archivo (temporalmente)
    // Abrir el archivo
    std::ofstream outFile("vector_data.txt");

    if (!outFile) {
        std::cerr << "Error al crear txtfile vector." << std::endl;
        return 1;
    }

    // Write the vector values to the file
    for (const int& value : dnis) {
        outFile << value << "\n";
    }

    outFile.close();
    std::cout << "Vector data escrito en el archivo vector_data.txt" << std::endl;
    
    
    
    //Rearmar el archivo con dnis
    // Abrir el archivo en modo lectura
    vector<int> dnisArchivo;
    int value;
    std::ifstream inFile("vector_data.txt");

    if (!inFile) {
        std::cerr << "Error opening file for reading." << std::endl;
        return 1;
    }

    // Read the values from the file into the vector
    while (inFile >> value) {
        dnisArchivo.push_back(value);
    }

    inFile.close();
    std::cout << "Se recontruyo el vector de dni's desde vector_data.txt" << std::endl;

    // Generar un arbol con todos los dnis y ids
    ArbolRB arbol;
    for (size_t i = 0; i < dnisArchivo.size(); i++)
    {
        // Insertando al arbol en RAM individualmente cada dni y id
        Insertar(&arbol, Elemento(i, dnisArchivo.at(i)));
    }
    cout << "Se termino de construir el arbol" << endl;

    // Guarda masivamente el arbol generado en un archivo binario en disco
    gestionArbol.guardarNodosEnDisco_MASIVO(&arbol);
    */
    return 0;
}