#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <string>
#include <queue>
#include <cmath>

#include "ArbolRB.h"
// En este archivo se definen las clases importantes del proyecto
//  - Elemento de Nodos
//  - Nodos
//  - ArbolRB

enum EditarNodo
{
    idIzquierda,
    idDerecha,
    idPadre,
};

class ArbolEnDisco
{
private:
    int nroNodos{0};
    int idRaiz{-1};
    std::string nombreArchivo;
    ArbolRB *arbolMemoria;

public:
    // Constructor de la clase
    ArbolEnDisco(const char *filename);

    // Este es el tamano de la cabecera
    const static int inicioDataNodos =
        {
            sizeof(nroNodos) + sizeof(idRaiz)};

    // Decontructor para asegurar el almacenamiento de datos
    ~ArbolEnDisco();

    //
    // Son las mismas operaciones del ArbolRojo pero aplicadas sobre el disco
    //
    void Insertar_Disco(std::fstream &data_stream, Elemento elemento);
    void InsertarRecursivamente_Disco(std::fstream &data_stream, int idNodoActual, int idNuevoNodo);
    void AjustarInsercion_Disco(std::fstream &data_stream, int idNodo);

    Nodo *Buscar_Disco(std::fstream &data_stream, int valor);

    // Nodo* Eliminar_Disco(std::fstream data_stream, int valor);
    // Nodo* ObtenerSiguiente_Disco
    // void AjustarEliminacion_Disco
    // bool ColorDeNodo

    Nodo *CrearNodo_Disco(std::fstream &data_stream, Elemento elemento);

    void RotarIzquierda_Disco(std::fstream &data_stream, int idNodo);
    void RotarDerecha_Disco(std::fstream &data_stream, int idNodo);

    void CambiarColor_Disco(std::fstream &data_stream, int idNodo, bool color);

    int buscarPorDNI_Disco(std::fstream &data_stream, int ciudadanoDni);

    //
    // Control de nodos en Disco
    //
    Nodo *obtenerNodo_Disco(std::fstream &data_stream, int nodoId);
    void actualizarDatosNodo_Disco(std::fstream &data_stream, int nodoId, EditarNodo variableEditar, int nuevoValor);

    void guardarNodosEnDisco_MASIVO(ArbolRB *arbol);
};

// Constructor de la clase
ArbolEnDisco::ArbolEnDisco(const char *filename)
{
    this->nombreArchivo = filename;
    this->idRaiz = -1;
    this->nroNodos = -1;
    this->arbolMemoria = new ArbolRB; // Se genera un arbol con raiz nula

    std::fstream fileIN(filename, std::ios::in | std::ios::binary);

    // Revisa que el archivo exista
    if (fileIN.is_open())
    {
        // Si EXISTE
        //  *Obtener los datos desde la cabecera
        fileIN.seekg(0, std::ios::beg);
        fileIN.read(reinterpret_cast<char *>(&this->nroNodos), sizeof(nroNodos));
        fileIN.read(reinterpret_cast<char *>(&this->idRaiz), sizeof(idRaiz));

        // Agregar la raiz al arbol
        //! ACA NO ESTA OBTENIENDO CORRECTAMENTE EL NODO 0
        Nodo *raiz = obtenerNodo_Disco(fileIN, this->idRaiz);
        arbolMemoria->raiz = raiz;

        // Cerrar el archivo
        fileIN.close();
    }
    else
    {
        // SI EL ARCHIVO NO EXISTE
        //* Crea un nuevo archivo
        std::fstream fileOUT(filename, std::ios::out | std::ios::binary);

        // *Grabar la cabecera de archivo
        fileOUT.seekp(0, std::ios::beg);
        fileOUT.write(reinterpret_cast<char *>(&nroNodos), sizeof(nroNodos));
        fileOUT.write(reinterpret_cast<char *>(&idRaiz), sizeof(idRaiz));

        fileOUT.flush();
        // Cerrar el archivo
        fileOUT.close();
    }
}

ArbolEnDisco::~ArbolEnDisco()
{
    // SI EL ARCHIVO NO EXISTE
    //* Crea un nuevo archivo
    std::fstream fileOUT(this->nombreArchivo, std::ios::in | std::ios::out | std::ios::binary);

    // *Grabar la cabecera de archivo
    fileOUT.seekp(0, std::ios::beg);
    fileOUT.write(reinterpret_cast<char *>(&this->nroNodos), sizeof(nroNodos));
    fileOUT.write(reinterpret_cast<char *>(&this->idRaiz), sizeof(idRaiz));

    fileOUT.flush();
    // Cerrar el archivo
    fileOUT.close();
    std::cout << "Se decontruyo un Arbol" << std::endl;
};

void ArbolEnDisco::Insertar_Disco(std::fstream &data_stream, Elemento elemento)
{
    Nodo *nuevoNodo = CrearNodo_Disco(data_stream, elemento);
    if (this->idRaiz == -1)
    {
        this->idRaiz = nuevoNodo->id;
        CambiarColor_Disco(data_stream, nuevoNodo->id, ROJO);
    }
    else
    {
        InsertarRecursivamente_Disco(data_stream, this->idRaiz, nuevoNodo->id);
        CambiarColor_Disco(data_stream, nuevoNodo->id, ROJO); //? Si por defecto se genera un nuevoNodo rojo, esta linea estaria de mas
    }
    std::cout << "Se inserta el nodo con dni: " << nuevoNodo->elemento.dni << std::endl;
    AjustarInsercion_Disco(data_stream, nuevoNodo->id);
}

void ArbolEnDisco::InsertarRecursivamente_Disco(std::fstream &data_stream, int idNodoActual, int idNuevoNodo)
{

    Nodo *nodoActual = obtenerNodo_Disco(data_stream, idNodoActual);
    Nodo *nuevoNodo = obtenerNodo_Disco(data_stream, idNuevoNodo);

    if (nuevoNodo->elemento.dni < nodoActual->elemento.dni)
    {
        if (nodoActual->idIzquierda == -1)
        {
            // Actualizar datos en el disco
            actualizarDatosNodo_Disco(data_stream, nodoActual->id, EditarNodo::idIzquierda, idNuevoNodo);
            actualizarDatosNodo_Disco(data_stream, nuevoNodo->id, EditarNodo::idPadre, idNodoActual);
        }
        else
        {
            InsertarRecursivamente_Disco(data_stream, nodoActual->idIzquierda, idNuevoNodo);
        }
    }
    else
    {
        if (nodoActual->idDerecha == -1)
        {
            actualizarDatosNodo_Disco(data_stream, nodoActual->id, EditarNodo::idDerecha, idNuevoNodo);
            actualizarDatosNodo_Disco(data_stream, nuevoNodo->id, EditarNodo::idPadre, idNodoActual);
        }
        else
        {
            InsertarRecursivamente_Disco(data_stream, nodoActual->idDerecha, idNuevoNodo);
        }
    }

    // Eliminar de memoria las variables
    delete nodoActual, nuevoNodo;
}

void ArbolEnDisco::AjustarInsercion_Disco(std::fstream &data_stream, int idNodo)
{
    bool continuarFlag = true;
    while (continuarFlag)
    {
        Nodo *nodo = obtenerNodo_Disco(data_stream, idNodo);
        Nodo *nodoPadre = obtenerNodo_Disco(data_stream, nodo->idPadre);
        Nodo *nodoPadrePadre = obtenerNodo_Disco(data_stream, nodoPadre->idPadre);

        // if que reemplaza al while
        if (nodo->idPadre != -1 && nodoPadre->color == ROJO) // reemplazo del While
        {
            if (nodoPadre->id == nodoPadrePadre->idIzquierda)
            {
                Nodo *tio = obtenerNodo_Disco(data_stream, nodoPadrePadre->idDerecha);
                if (tio->id != -1 && tio->color == ROJO)
                {
                    CambiarColor_Disco(data_stream, tio->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadre->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadrePadre->id, ROJO);
                    idNodo = nodoPadrePadre->id;
                }
                else
                {
                    if (idNodo == nodoPadre->idDerecha)
                    {
                        idNodo = nodoPadre->id;
                        RotarIzquierda_Disco(data_stream, idNodo);

                        // Ya que se cambia el valor del idNodo (Y posteriormente son necesarios esos datos)
                        // es necesario re-actualizar los valores de los nodoPadre y nodoPadrePadre
                        // TODO: MEJORABLE PARA EVITAR TRABAJO SOBRE DISCO
                        nodo = obtenerNodo_Disco(data_stream, idNodo);
                        nodoPadre = obtenerNodo_Disco(data_stream, nodo->idPadre);
                        nodoPadrePadre = obtenerNodo_Disco(data_stream, nodoPadre->idPadre);
                    }

                    CambiarColor_Disco(data_stream, nodoPadre->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadrePadre->id, ROJO);
                    RotarDerecha_Disco(data_stream, nodoPadrePadre->id);
                }
            }
            else
            {
                Nodo *tio = obtenerNodo_Disco(data_stream, nodoPadrePadre->idIzquierda);
                if (tio->id != -1 && tio->color == ROJO)
                {
                    CambiarColor_Disco(data_stream, tio->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadre->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadrePadre->id, ROJO);
                    idNodo = nodoPadrePadre->id;
                }
                else
                {
                    if (idNodo == nodoPadre->idIzquierda)
                    {
                        idNodo = nodoPadre->id;
                        RotarDerecha_Disco(data_stream, idNodo);
                        // Ya que se cambia el valor del idNodo (Y posteriormente son necesarios esos datos)
                        // es necesario re-actualizar los valores de los nodoPadre y nodoPadrePadre
                        // TODO: MEJORABLE PARA EVITAR TRABAJO SOBRE DISCO
                        nodo = obtenerNodo_Disco(data_stream, idNodo);
                        nodoPadre = obtenerNodo_Disco(data_stream, nodo->idPadre);
                        nodoPadrePadre = obtenerNodo_Disco(data_stream, nodoPadre->idPadre);
                    }
                    CambiarColor_Disco(data_stream, nodoPadre->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadrePadre->id, ROJO);
                    RotarIzquierda_Disco(data_stream, nodoPadrePadre->id);
                }
            }
            if (idNodo == this->idRaiz)
            {
                continuarFlag = false; // Esto deberia romper el while
            }
        }
        else // Parte de la conversion del raro while hacia un if
        {
            continuarFlag = false;
        }
    }
    CambiarColor_Disco(data_stream, this->idRaiz, NEGRO);
}

//Busca un elemento en disco de acuerdo al DNI portado por el nodo
Nodo *ArbolEnDisco::Buscar_Disco(std::fstream &data_stream, int valorDni)
{
    Nodo *nodoActual = obtenerNodo_Disco(data_stream, this->idRaiz);
    while (nodoActual->id != -1 && nodoActual->elemento.dni != valorDni)
    {
        if (valorDni < nodoActual->elemento.dni)
        {
            nodoActual = obtenerNodo_Disco(data_stream, nodoActual->idIzquierda);
        }
        else
        {
            nodoActual = obtenerNodo_Disco(data_stream, nodoActual->idDerecha);
        }
    }
    return nodoActual;
}

/*
SECCION DE ELIMINACION
*/

// Guarda un nodo en Disco y retorna su ID(ubicacion relativa)
Nodo *ArbolEnDisco::CrearNodo_Disco(std::fstream &data_stream, Elemento elemento)
{
    if (!data_stream.is_open())
    {
        return nullptr;
    }

    // Crear el nodo que se va almacenar
    Nodo *nodo = new Nodo(elemento);

    // Moverse al final del archivo
    data_stream.seekp(0, std::ios::end);

    // Guardar posicion en el archivo
    int pos = data_stream.tellp();

    //! IMPORTANTE EL ORDEN EN EL QUE SE GUARDAN AL ARCHIVO
    // ESTA *ES* LA ESTRUCTURA DE UN NODO EN BINARIO
    data_stream.write(reinterpret_cast<char *>(&nodo->idIzquierda), sizeof(int));
    data_stream.write(reinterpret_cast<char *>(&nodo->idDerecha), sizeof(int));
    data_stream.write(reinterpret_cast<char *>(&nodo->idPadre), sizeof(int));
    data_stream.write(reinterpret_cast<char *>(&nodo->elemento.dni), sizeof(int));
    data_stream.write(reinterpret_cast<char *>(&nodo->elemento.id), sizeof(int));
    data_stream.write(reinterpret_cast<char *>(&nodo->color), sizeof(bool));
    data_stream.flush();

    this->nroNodos += 1;

    // Guarda la posicion (id) del nodo en el mismo nodo
    //                           !Este id de nodo solo se encuentra en memoria Ram
    nodo->id = (pos - ArbolEnDisco::inicioDataNodos) / Nodo::sizeBinario;
    // TODO Hace falta poner una validacion? en TEORIA no debe fallar este calculo
    //  pero seria bueno agregar una validacion de que el id es un valor seguro

    return nodo;
}

void ArbolEnDisco::RotarIzquierda_Disco(std::fstream &data_stream, int idNodo)
{
    // Obtener el nodo por defecto del entero
    Nodo *nodo = obtenerNodo_Disco(data_stream, idNodo);
    Nodo *nodoDerecha = obtenerNodo_Disco(data_stream, nodo->idDerecha);
    actualizarDatosNodo_Disco(data_stream, nodo->id, EditarNodo::idDerecha, nodoDerecha->idIzquierda);

    if (nodoDerecha->idIzquierda != -1)
    {
        actualizarDatosNodo_Disco(data_stream, nodoDerecha->idIzquierda, EditarNodo::idPadre, idNodo);
    }
    actualizarDatosNodo_Disco(data_stream, nodoDerecha->id, EditarNodo::idPadre, nodo->idPadre);
    Nodo *nodoPadre = obtenerNodo_Disco(data_stream, nodo->idPadre); // Paso que no existe en la funcion original pero es necesario porque se va usar muchas veces al padre
    if (nodo->idPadre == -1)
    {
        this->idRaiz = nodoDerecha->id;
    }
    else if (nodo->id == nodoPadre->idIzquierda)
    {
        actualizarDatosNodo_Disco(data_stream, nodoPadre->id, EditarNodo::idIzquierda, nodoDerecha->id);
    }
    else
    {
        actualizarDatosNodo_Disco(data_stream, nodoPadre->id, EditarNodo::idDerecha, nodoDerecha->id);
    }
    actualizarDatosNodo_Disco(data_stream, nodoDerecha->id, EditarNodo::idIzquierda, nodo->id);
    actualizarDatosNodo_Disco(data_stream, nodo->id, EditarNodo::idPadre, nodoDerecha->id);
}

void ArbolEnDisco::RotarDerecha_Disco(std::fstream &data_stream, int idNodo)
{
    Nodo *nodo = obtenerNodo_Disco(data_stream, idNodo);
    Nodo *nodoIzquierda = obtenerNodo_Disco(data_stream, nodo->idIzquierda);
    actualizarDatosNodo_Disco(data_stream, nodo->id, EditarNodo::idIzquierda, nodoIzquierda->idDerecha);

    if (nodoIzquierda->idDerecha != -1)
    {
        actualizarDatosNodo_Disco(data_stream, nodoIzquierda->idDerecha, EditarNodo::idPadre, idNodo);
    }
    actualizarDatosNodo_Disco(data_stream, nodoIzquierda->id, EditarNodo::idPadre, nodo->idPadre);
    Nodo *nodoPadre = obtenerNodo_Disco(data_stream, nodo->idPadre);
    if (nodo->idPadre == -1)
    {
        this->idRaiz = nodoIzquierda->id;
    }
    else if (nodo->id == nodoPadre->idDerecha)
    {
        actualizarDatosNodo_Disco(data_stream, nodoPadre->id, EditarNodo::idDerecha, nodoIzquierda->id);
    }
    else
    {
        actualizarDatosNodo_Disco(data_stream, nodoPadre->id, EditarNodo::idIzquierda, nodoIzquierda->id);
    }
    actualizarDatosNodo_Disco(data_stream, nodoIzquierda->id, EditarNodo::idDerecha, nodo->id);
    actualizarDatosNodo_Disco(data_stream, nodo->id, EditarNodo::idPadre, nodoIzquierda->id);
}

void ArbolEnDisco::CambiarColor_Disco(std::fstream &data_stream, int idNodo, bool color)
{
    Nodo *nodo = obtenerNodo_Disco(data_stream, idNodo);
    // Verificar que el nodo que se va eliminar existe //?SE PUEDE HACER MAS EFICIENTE
    if (nodo->id != -1)
    {
        //  METERSE AL DISCO
        // Moverse hasta la posicion de IDNodo + iniciando en el inicio de datos
        data_stream.seekp(ArbolEnDisco::inicioDataNodos + Nodo::sizeBinario * idNodo, std::ios::beg);

        // Moverse todas las posiciones hasta llegar al dato color
        data_stream.seekp(sizeof(int) * 5, std::ios::cur);
        // Cambiar el valor de color
        data_stream.write(reinterpret_cast<char *>(&color), sizeof(bool));
        data_stream.flush();
    }
}

//Retorna un nodo buscando por DNI
int ArbolEnDisco::buscarPorDNI_Disco(std::fstream &data_stream, int ciudadanoDni)
{
    Nodo *nodoActual = obtenerNodo_Disco(data_stream, this->idRaiz);
    //
    // Algo me dice que esta funcion esta de mas.....
    //
    return 0;
}

// Funcion para obtener un nodo (desde el stream dado) segun su id en el archivo
// de ser un nodo nulo (o sea el dni del elemento es -1), se retorna nullptr
Nodo *ArbolEnDisco::obtenerNodo_Disco(std::fstream &data_stream, int nodoId)
{
    // Nodo que va retornarse
    Nodo *encontrado = new Nodo();

    // En caso se envie un entero imposible
    if (nodoId < 0)
    {
        encontrado->id = -1;
        return encontrado;
    }

    // Moverse hasta la posicion de IDNodo + iniciando en el inicio de datos
    data_stream.seekg(ArbolEnDisco::inicioDataNodos + Nodo::sizeBinario * nodoId, std::ios::beg);

    int posDebug = data_stream.tellg();

    //! IMPORTANTE EL ORDEN EN EL QUE SE GUARDAN AL ARCHIVO
    // ESTA *ES* LA ESTRUCTURA DE UN NODO EN BINARIO
    data_stream.read(reinterpret_cast<char *>(&encontrado->idIzquierda), sizeof(int));
    data_stream.read(reinterpret_cast<char *>(&encontrado->idDerecha), sizeof(int));
    data_stream.read(reinterpret_cast<char *>(&encontrado->idPadre), sizeof(int));
    data_stream.read(reinterpret_cast<char *>(&encontrado->elemento.dni), sizeof(int));
    data_stream.read(reinterpret_cast<char *>(&encontrado->elemento.id), sizeof(int));
    data_stream.read(reinterpret_cast<char *>(&encontrado->color), sizeof(bool));

    encontrado->id = nodoId;

    if (encontrado->elemento.dni == -1) // Significa que en realidad es un elemento nulo
    {
        // Y por ende, este nodo "no existe"
        encontrado->id = -1;
    }

    return encontrado;
}

void ArbolEnDisco::actualizarDatosNodo_Disco(std::fstream &data_stream, int nodoIdModificar, EditarNodo variableEditar, int nuevoValor)
{
    // Moverse hasta la posicion de IDNodo (que es inicio de los datos + posicion de nodoId)
    data_stream.seekg(ArbolEnDisco::inicioDataNodos + Nodo::sizeBinario * nodoIdModificar, std::ios::beg);

    switch (variableEditar)
    {
    /*
    ! IMPORTANTE EL ORDEN EN EL QUE SE GUARDAN AL ARCHIVO
    ESTA *ES* LA ESTRUCTURA DE UN NODO EN BINARIO
    file.write(reinterpret_cast<char*>(nodo->idIzquierda), sizeof(int));
    file.write(reinterpret_cast<char*>(nodo->idDerecha), sizeof(int));
    file.write(reinterpret_cast<char*>(nodo->idPadre), sizeof(int));
    file.write(reinterpret_cast<char*>(nodo->elemento.dni), sizeof(int));
    file.write(reinterpret_cast<char*>(nodo->elemento.id), sizeof(int));
    file.write(reinterpret_cast<char*>(nodo->color), sizeof(bool));
    */
    case EditarNodo::idDerecha:
        // Moverse todas las posiciones hasta llegar al dato IdDERECHA
        data_stream.seekp(sizeof(int) * 1, std::ios::cur);

        // Cambiar el valor de color
        data_stream.write(reinterpret_cast<char *>(&nuevoValor), sizeof(int));
        break;

    case EditarNodo::idIzquierda:
        // Moverse todas las posiciones hasta llegar al dato IdIZQUIERDA
        // no es necesario moverse nada en este caso

        // Cambiar el valor de color
        data_stream.write(reinterpret_cast<char *>(&nuevoValor), sizeof(int));
        break;

    case EditarNodo::idPadre:
        // Moverse todas las posiciones hasta llegar al dato IdDERECHA
        data_stream.seekp(sizeof(int) * 2, std::ios::cur);

        // Cambiar el valor de color
        data_stream.write(reinterpret_cast<char *>(&nuevoValor), sizeof(int));
        break;

    default:
        std::cout << "Se ha solicitado actualizar un dato irreconocible sobre un nodo" << std::endl;
        break;
    }
    data_stream.flush();
}

// Recibe un arbol binario y lo almacena en disco, por defecto dejara a la raiz en la primera posicion del archivo
void ArbolEnDisco::guardarNodosEnDisco_MASIVO(ArbolRB *arbol)
{
    // TODO REVISAR, ESTO PUEDE FALLAR POR ESTO DEL NULLPTR
    if (arbol->raiz == nullptr)
    {
        std::cout << "Se intento guardar un arbol vacio";
        return;
    }
    std::vector<Nodo *> concatenado;
    std::queue<Nodo *> fila;

    fila.push(arbol->raiz);

    // Genera un "concatenado" con todos los nodos del arbol
    while (!fila.empty())
    {
        Nodo *actual = fila.front();
        fila.pop();

        // Guarda el elemento
        concatenado.push_back(actual);

        if (actual->izquierda != nullptr)
            fila.push(actual->izquierda);
        if (actual->derecha != nullptr)
            fila.push(actual->derecha);
    }

    // Guarda masivamente a los nodos ubicados en el arbol
    std::cout << "A punto de guardar un arbol con elementos " << concatenado.size() << std::endl;

    // Abrir el archivo
    std::fstream file;
    file.open(this->nombreArchivo, std::ios::binary | std::ios::out);

    // Moverse hasta la posicion de los datos de los nodos
    file.seekp(ArbolEnDisco::inicioDataNodos, std::ios::beg);

    int aux;
    //! Ya que es subida masiva, se deben generar los valores de los ID's de los nodos
    // cada valor se genera a partir de una operacion matematica especifica
    for (int i = 0; i < concatenado.size(); i++)
    {
        //! IMPORTANTE EL ORDEN EN EL QUE SE GUARDAN AL ARCHIVO
        // ESTA *ES* LA ESTRUCTURA DE UN NODO EN BINARIO

        aux = i * 2 + 1; // Calculo del idIzquierda
        file.write(reinterpret_cast<char *>(&aux), sizeof(int));

        aux = i * 2 + 2; // Calculo del idDerecha
        file.write(reinterpret_cast<char *>(&aux), sizeof(int));

        aux = std::floor(i / 2 - 0.5); // Calculo del idPadre
        file.write(reinterpret_cast<char *>(&aux), sizeof(int));

        file.write(reinterpret_cast<char *>(&concatenado.at(i)->elemento.dni), sizeof(int));
        file.write(reinterpret_cast<char *>(&concatenado.at(i)->elemento.id), sizeof(int));
        file.write(reinterpret_cast<char *>(&concatenado.at(i)->color), sizeof(bool));
    }

    // Actualizar el numero de nodos antes de cerrar el documento
    this->nroNodos = concatenado.size();
    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<char *>(&nroNodos), sizeof(nroNodos));

    // Cerrar el archivo
    file.close();
}

// ---- Fin de clase ----

void imprimirNodo(Nodo *nodo)
{
    std::cout << "Datos del elemento contenido " << std::endl;
    std::cout << "> " << nodo->elemento.id << std::endl;
    std::cout << "> " << nodo->elemento.dni << std::endl;

    std::cout << "Datos sobre el nodo" << std::endl;
    std::cout << "Id: \t" << nodo->id << std::endl;

    std::cout << "IdPadre:\t" << nodo->idPadre << std::endl;

    std::cout << "IdIzquierda:\t" << nodo->idIzquierda << std::endl;

    std::cout << "IdDerecha:\t" << nodo->idDerecha << std::endl;

    if (nodo->color)
    {
        std::cout << "Color ROJO" << std::endl;
    }
    else
    {
        std::cout << "Color NEGRO" << std::endl;
    }
}

int mainMemoria() // main memoria
{
    ArbolRB arbol;
    // Insercion del id y dni
    Insertar(&arbol, Elemento(1, 12345678));
    Insertar(&arbol, Elemento(2, 23456789));
    Insertar(&arbol, Elemento(3, 34567890));

    int id = BuscarPorDni(&arbol, 34567890);
    if (id != -1)
    {
        std::cout << "El ID del DNI " << 2365 << " esta en " << id << std::endl;
    }
    else
    {
        std::cout << "No se encontro un ID del DNI " << 2365 << std::endl;
    }

    // ArbolEnDisco guardado("Arbol.bin");
    // guardado.guardarNodosEnDisco_MASIVO(&arbol);
    //
    //
    // Nodo* avr = guardado.buscarNodoEnArchivo(0);
    // std::cout<<avr->elemento.dni<<std::endl;
    // std::cout<<avr->elemento.id<<std::endl;
    // std::cout<<avr->idDerecha<<std::endl;
    // std::cout<<avr->idIzquierda<<std::endl;
    // std::cout<<avr->idPadre<<std::endl;

    return 0;
}

int main() // main disco
{
    ArbolEnDisco arbol("Arbol.bin");

    std::fstream archivo("Arbol.bin", std::ios::in | std::ios::out | std::ios::binary);
    arbol.Insertar_Disco(archivo, Elemento(99, 9));  // 0
    arbol.Insertar_Disco(archivo, Elemento(99, 33)); // 1
    arbol.Insertar_Disco(archivo, Elemento(99, 11)); // 2
    arbol.Insertar_Disco(archivo, Elemento(99, 54)); // 3
    arbol.Insertar_Disco(archivo, Elemento(99, 72)); // 4
    arbol.Insertar_Disco(archivo, Elemento(99, 85)); // 5
    arbol.Insertar_Disco(archivo, Elemento(99, 18)); // 6
    arbol.Insertar_Disco(archivo, Elemento(99, 29)); // 7

    arbol.Insertar_Disco(archivo, Elemento(99, 43)); // 8

    arbol.Insertar_Disco( archivo, Elemento( 99, 51) ); //9
    arbol.Insertar_Disco( archivo, Elemento( 99,  2) ); //10

    Nodo *nodo;
    for (int i = 0; i <= 10; i++)
    {
        nodo = arbol.obtenerNodo_Disco(archivo, i);
        if (nodo->id == -1)
        {
            std::cout << "No existe un nodo en esa posicion " << i << std::endl;
        }
        else
        {
            imprimirNodo(nodo);
        }
        std::cout << "--" << std::endl;
    }

    return 0;
}
