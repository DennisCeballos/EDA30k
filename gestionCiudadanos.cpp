#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <tuple>
#include <ctime>
#include <vector>

#include "gestionCiudadanos.h"

using namespace std;

CtrlCiudadano::CtrlCiudadano(const char *_filaName)
{
    this->nroElementos = 0;
    this->nombreArchivo = _filaName;
    
    std::fstream fileIN(nombreArchivo, std::ios::in | std::ios::binary);

    // Revisa que el archivo exista
    if (fileIN.is_open())
    {
        // Si es que EXISTE
        //  *Obtener los datos desde la cabecera
        fileIN.seekg(0, std::ios::beg);
        fileIN.read(reinterpret_cast<char *>(&this->nroElementos), sizeof(nroElementos));

        // Cerrar el archivo
        fileIN.close();
    }
    else
    {
        // SI EL ARCHIVO NO EXISTE
        //* Crea un nuevo archivo
        std::fstream fileOUT(nombreArchivo, std::ios::out | std::ios::binary);

        // *Grabar la cabecera de archivo
        fileOUT.seekp(0, std::ios::beg);
        fileOUT.write(reinterpret_cast<char *>(&nroElementos), sizeof(nroElementos));

        fileOUT.flush();
        // Cerrar el archivo
        fileOUT.close();
    }
}

CtrlCiudadano::~CtrlCiudadano()
{
    
}

const int CtrlCiudadano::sizeCabecera = sizeof(int); //Se almacena un int al principio para la variable nroElementos

string CtrlCiudadano::getNombreArchivo()
{
    return nombreArchivo;
}

// Fucion para guardar un ciudadano en el archivo binario
int CtrlCiudadano::guardarCiudadano(Ciudadano *ciudadano, bool debug = false)
{
    ofstream myfile;

    // Se abre el archivo en binario | salida de datos | final del archivo
    myfile.open(nombreArchivo, ios::binary | ios::out | ios::app);

    // Moverse hasta el final del archivo
    myfile.seekp(sizeCabecera + nroElementos*Ciudadano::getSizeofInd, ios::beg);

    // Guardar la posicion actual del documento (o sea justo donde se va guardar al ciudadano)
    int pos = myfile.tellp();

    // Almacenar el ciudadano en binario
    //   primero guardar el dni
    myfile.write(reinterpret_cast<char *>(&ciudadano->dni), sizeof(Ciudadano::dni));

    // escribir la cadena de datos
    myfile.write(reinterpret_cast<char *>(&ciudadano->datos), Ciudadano::sizeDatos);

    // Cerrar el archivo por seguridad
    myfile.close();

    // Calcula la posicion (id) 
    pos = (pos - CtrlCiudadano::sizeCabecera) / Ciudadano::getSizeofInd;
    // TODO Hace falta poner una validacion? en TEORIA no debe fallar este calculo
    //  pero seria bueno agregar una validacion de que el id es un valor seguro

    if (debug)
    {
        cout << "Guardado correctamente al ciudadano " << ciudadano->datos << " en pos " << pos << endl;
    }
    this->nroElementos += 1;
    return pos;
}

// Fucion para guardar VARIOS ciudadanos en el archivo binario
vector<int> CtrlCiudadano::guardarCiudadano_MASIVO(vector<Ciudadano *> vectorCiudadanos, bool debug = false)
{
    vector<int> vectorIDs;
    int pos;

    ofstream myfile;

    // Se abre el archivo en binario | salida de datos
    myfile.open(nombreArchivo, ios::binary | ios::out | ios::app);

    try
    {
        for (Ciudadano *ciudadano : vectorCiudadanos)
        {
            // Moverse hasta el final del archivo
            myfile.seekp(0, ios::end);

            // Guardar la posicion actual del documento (o sea justo donde se va guardar al ciudadano)
            pos = myfile.tellp();
            // Calcula la posicion (id) 
            pos = (pos - CtrlCiudadano::sizeCabecera) / Ciudadano::getSizeofInd;
            vectorIDs.push_back(pos);

            // Almacenar el ciudadano en binario
            myfile.write(reinterpret_cast<char *>(ciudadano), sizeof(Ciudadano));

            if (debug)
            {
                cout << "Guardado correctamente al ciudadano " << ciudadano->datos << " en pos " << pos << endl;
            }

            this->nroElementos += 1;
        }
    }
    catch (const std::exception &e)
    {
        cout << "Hubo un error en la insercion masiva" << endl;
        std::cerr << e.what() << std::endl;
    }

    // Cerrar el archivo por seguridad
    myfile.close();

    return vectorIDs;
}

// Funcion para retornar un ciudadano
// segun su id en el archivo
Ciudadano *CtrlCiudadano::obtenerCiudadanoEnPos(int id, bool debug = false)
{
    try
    {
        Ciudadano *ciudadano = new Ciudadano();
        // Abrir el archivo
        ifstream otroFile;
        otroFile.open(nombreArchivo, ios::binary | ios::in);
        if (!otroFile)
        {
            throw runtime_error("No se encontro el archivo con datos ciudadanos, se busca " + nombreArchivo);
        }

        // Mueve el puntero la cantidad de espacios necesarios
        otroFile.seekg(CtrlCiudadano::sizeCabecera + Ciudadano::getSizeofInd * id, ios::beg);

        // Lee los datos desde el binario y carga los datos al objeto
        otroFile.read(reinterpret_cast<char *>(&ciudadano->dni), sizeof(Ciudadano::dni));

        //  lee el string con datos
        otroFile.read(reinterpret_cast<char *>(&ciudadano->datos), Ciudadano::sizeDatos);

        if (!otroFile)
        {
            throw runtime_error("Error al leer de archivo");
        }

        // Cerrar el documento
        otroFile.close();

        if (debug)
        {
            cout << "Se obtuvo al ciudadano " << ciudadano->getDatos().substr(0, 10) << " en pos " << id << endl;
        }
        return ciudadano;
    }
    catch (const runtime_error &e)
    {
        std::cerr << "Error: " << e.what() << endl;
        return new Ciudadano();
    }

    // return Ciudadano(); //El compilador por algun motivo quiere que esto este aqui
}



/*
int mainCiudadano()
{

    const char *archivo = "datosCiudadanos.bin";
    CtrlCiudadano ctrlCiudadano(archivo);

    Ciudadano *pepe = new Ciudadano(
        12345678,
        "Pepe, Peruano, Ica, Su casa, 123456789, unGmail, soltero");

    Ciudadano *juan = new Ciudadano(
        87654321, "Juan, Chino, Callao, Casa de su tia, 987654321, nica, Viudo");

    Ciudadano *promedio = new Ciudadano(
        66677700, "promedio, peruana, Callao, Casa cualquiera, 111555999, unHotmail, Viudo");

    int sitio = -1;
    // sitio = ctrlCiudadano.guardarCiudadano(pepe, true);
    // cout<<">>"<<sitio<<endl;
    //
    // sitio = ctrlCiudadano.guardarCiudadano(juan, true);
    // cout<<">>"<<sitio<<endl;
    //
    // sitio = ctrlCiudadano.guardarCiudadano(promedio, true);
    // cout<<">>"<<sitio<<endl;

    // clock_t start = clock();
    //
    ////! OBJETIVO 33 000 000
    ////! Actual   1   00 000
    // for (int i = 0; i < 100000; i++)
    //{
    //     promedio->dni = promedio->dni+1;
    //     sitio = ctrlCiudadano.guardarCiudadano(promedio);
    //     cout<<i<<" - DNI:"<<promedio->dni<<endl;
    // }

    // clock_t end = clock();
    // double tiempo_transcurrido = (double)(end - start) / CLOCKS_PER_SEC;
    // cout<<"Tiempo de ejecucion: "<<tiempo_transcurrido<<" segundos"<<endl;

    // CtrlCiudadano ctrlCiudadano("datos.bin");
    cout << "Datos extraidos:" << endl;
    Ciudadano *fin = ctrlCiudadano.obtenerCiudadanoEnPos(2, true);
    cout << fin->getDni() << endl;
    cout << fin->getDatos() << endl;

    int a;
    std::cin >> a;

    return 0;
}
*/