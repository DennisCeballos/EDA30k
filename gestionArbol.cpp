#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <cstring>
#include <string>
#include <queue>
#include <cmath>

#include "ArbolRB.h"
#include "gestionArbol.h"
// En este archivo se definen las clases importantes del proyecto
//  - Elemento de Nodos
//  - Nodos
//  - ArbolRB

// Constructor de la clase
ArbolEnDisco::ArbolEnDisco(const char *filename)
{
    this->nombreArchivo = filename;
    this->idRaiz = -1;
    this->nroNodos = 0;
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

        // *Obtener los valores de los espacios eliminados
        // Moverse hasta la ultima posicion del ultimo nodo
        fileIN.seekg(ArbolEnDisco::inicioDataNodos + Nodo::sizeBinario * this->nroNodos, std::ios::beg);

        //Leer el numero de espacios libres
        int numEspacios = 0;
        fileIN.read(reinterpret_cast<char *>(&numEspacios), sizeof(int));

        //Leer el "numEspacios" e insertarlos en el minHeap
        int valor = 0;
        for (int i = 0; i < numEspacios; i++)
        {
            fileIN.read(reinterpret_cast<char *>(&valor), sizeof(int));
            this->espaciosLibre.push(valor);
        }

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

const int ArbolEnDisco::inicioDataNodos = { sizeof(nroNodos) + sizeof(idRaiz) };

ArbolEnDisco::~ArbolEnDisco()
{
    //* Crea un nuevo archivo
    std::fstream fileOUT(this->nombreArchivo, std::ios::in | std::ios::out | std::ios::binary);

    // *Grabar la cabecera de archivo
    fileOUT.seekp(0, std::ios::beg);
    fileOUT.write(reinterpret_cast<char *>(&this->nroNodos), sizeof(nroNodos));
    fileOUT.write(reinterpret_cast<char *>(&this->idRaiz), sizeof(idRaiz));

    fileOUT.flush();

    // *Grabar el pie de pagina del archivo
    //Moverse hasta el final de la posicion del ultimo nodo
    fileOUT.seekp(ArbolEnDisco::inicioDataNodos + Nodo::sizeBinario * (this->nroNodos+1), std::ios::beg); //? Podria reemplazarse por seekp(0, std::ios::end)?
    int numEspacios = this->espaciosLibre.size();
    fileOUT.write(reinterpret_cast<char *>(&numEspacios), sizeof(int));

    //Si solo existe mas de un valor
    if (numEspacios > 0)
    {
        //Almacenar cada valor interior en disco
    }
    
    //Almacenar cada valor interior en disco
    int valorGuardar = 0;
    while (!espaciosLibre.empty())
    {
        //Obtener el valor mas alto
        valorGuardar = this->espaciosLibre.top();
        //Eliminar el valor mas alto
        espaciosLibre.pop();

        //Almacenar el valor en disco
        fileOUT.write(reinterpret_cast<char *>(&valorGuardar), sizeof(int));
    }

    // Cerrar el archivo
    fileOUT.close();
    std::cout << "- Se decontruyo un Arbol" << std::endl;
};

void ArbolEnDisco::Insertar_Disco(std::fstream &data_stream, Elemento elemento)
{
    Nodo *nodo = CrearNodo_Disco(data_stream, elemento);
    
    Nodo* y = new Nodo();
    Nodo* x = obtenerNodo_Disco(data_stream, this->idRaiz);

    while ( x->id != -1 )
    {
        y = x;
        if( nodo->elemento.dni < x->elemento.dni )
        {
            x = obtenerNodo_Disco(data_stream, x->idIzquierda);
        }
        else
        {
            x = obtenerNodo_Disco(data_stream, x->idDerecha);
        }
    }

    actualizarDatosNodo_Disco(data_stream, nodo->id, EditarNodo::idPadre, y->id);
    nodo->idPadre = y->id; //Se actualiza el dato en disco y en memoria porque sera util mas adelante
    
    if (y->id == -1)
    {
        this->idRaiz = nodo->id;
    }
    else if ( nodo->elemento.dni < y->elemento.dni )
    {
        actualizarDatosNodo_Disco(data_stream, y->id, EditarNodo::idIzquierda, nodo->id);
    }
    else
    {
        actualizarDatosNodo_Disco(data_stream, y->id, EditarNodo::idDerecha, nodo->id);
    }

    if (nodo->idPadre == -1)
    {
        CambiarColor_Disco(data_stream, nodo->id, NEGRO);
        return;
    }

    Nodo* nodoPadre = obtenerNodo_Disco(data_stream, nodo->idPadre);
    if (nodoPadre->idPadre == -1)
    {
        return;
    }

    AjustarInsercion_Disco(data_stream, nodo->id);
    //TODO
    //ELIMINAR LOS NODOS PARALIBERAR ESPACIO DE MEMORIA
}

void ArbolEnDisco::AjustarInsercion_Disco(std::fstream &data_stream, int idNodo)
{
    // Inicializacion de variables necesarias
    Nodo *nodo;
    Nodo *nodoPadre;
    Nodo *nodoPadrePadre;

    bool continuarFlag = true;
    while (continuarFlag)
    {
        nodo = obtenerNodo_Disco(data_stream, idNodo);
        nodoPadre = obtenerNodo_Disco(data_stream, nodo->idPadre);
        nodoPadrePadre = obtenerNodo_Disco(data_stream, nodoPadre->idPadre);

        // if que reemplaza al while
        if (nodoPadre->color == ROJO) // reemplazo del While
        {
            if (nodoPadre->id == nodoPadrePadre->idDerecha)
            {
                Nodo *tio = obtenerNodo_Disco(data_stream, nodoPadrePadre->idIzquierda);
                if (tio->color == ROJO) //if tio.id != -1
                {
                    //Caso 3.1
                    CambiarColor_Disco(data_stream, tio->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadre->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadrePadre->id, ROJO);
                    idNodo = nodoPadrePadre->id;
                }
                else
                {
                    if (idNodo == nodoPadre->idIzquierda)
                    {
                        // caso 3.2
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
            else
            {
                Nodo *tio = obtenerNodo_Disco(data_stream, nodoPadrePadre->idDerecha);
                if (tio->color == ROJO) //? verificar si tio->id !=-1
                {
                    // caso 3.1 reflejo
                    CambiarColor_Disco(data_stream, tio->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadre->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadrePadre->id, ROJO);
                    idNodo = nodoPadrePadre->id;
                }
                else
                {
                    if (idNodo == nodoPadre->idDerecha)
                    {
                        // caso 3.2.2 (espejo)
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

// Busca un elemento en disco de acuerdo al DNI portado por el nodo
Nodo *ArbolEnDisco::Buscar_Disco(std::fstream &data_stream, int valorDni)
{
    Nodo *nodoActual = obtenerNodo_Disco(data_stream, this->idRaiz);
    while (nodoActual->id != -1)
    {
        if (nodoActual->elemento.dni == valorDni)
        {
            return nodoActual;
        }

        if( nodoActual->elemento.dni <= valorDni)
        {
            nodoActual = obtenerNodo_Disco(data_stream, nodoActual->idDerecha);
        }
        else
        {
            nodoActual = obtenerNodo_Disco(data_stream, nodoActual->idDerecha);
        }        
    }
    return nodoActual;
}

// Funcion llamada para eliminar un nodo del arbol
void ArbolEnDisco::Eliminar_Disco(std::fstream &data_stream, int dni)
{

    Nodo *z = Buscar_Disco(data_stream, dni);
    Nodo *x = new Nodo();
    Nodo *y = new Nodo();

    if ( z->id == -1)
    {
        return;
    }

    y = z;
    bool y_original_color = y->color;
    
    if (z->idIzquierda == -1)
    {
        x = obtenerNodo_Disco(data_stream, z->id);
        x->idPadre = z->id; //Reafirmar el origen del padre solo por si acaso

        rbTransplant_Disco(data_stream, z->id, z->idDerecha);
        z = obtenerNodo_Disco(data_stream, z->id);
    }
    else if (z->idDerecha == -1)
    {
        x = obtenerNodo_Disco(data_stream, z->idIzquierda);
        x->idPadre = z->idIzquierda;
        rbTransplant_Disco(data_stream, z->id, z->idIzquierda);
        z = obtenerNodo_Disco(data_stream, z->id);
    }
    else
    {
        y = minimum_Disco(data_stream, z->idDerecha);
        y_original_color = y->color;
        x = obtenerNodo_Disco(data_stream, y->idDerecha);
        x->idPadre = y->idDerecha;
        if (y->idPadre == z->id)
        {
            actualizarDatosNodo_Disco(data_stream, x->id, EditarNodo::idPadre, y->id);
        }
        else
        {
            rbTransplant_Disco(data_stream, y->id, y->idDerecha);
            
            y = obtenerNodo_Disco(data_stream, y->id); //Necesario tras el anterior funcion de REEMPLAZO
            actualizarDatosNodo_Disco(data_stream, y->id, EditarNodo::idDerecha, z->idDerecha);
            y->idDerecha = z->idDerecha;

            Nodo* yDerecha = obtenerNodo_Disco(data_stream, y->idDerecha);
            actualizarDatosNodo_Disco(data_stream, yDerecha->id, EditarNodo::idPadre, y->id);
        }

        rbTransplant_Disco(data_stream, z->id, y->id);
        z = obtenerNodo_Disco(data_stream, z->id);
        actualizarDatosNodo_Disco(data_stream, y->id, EditarNodo::idIzquierda, z->idIzquierda);
        y->idIzquierda = z->idIzquierda;

        // puedo utilizar a z->idIzquierda para no volver a obtener a y desde disco
        Nodo* yIzquierda = obtenerNodo_Disco(data_stream, y->idIzquierda);
        actualizarDatosNodo_Disco(data_stream, yIzquierda->id, EditarNodo::idPadre, y->id);
        CambiarColor_Disco(data_stream, y->id, z->color);
    }

    EliminarNodo_Disco(data_stream, z->id);
    delete z; 
    if (y_original_color == NEGRO)
    {
        AjustarEliminacion_Disco(data_stream, x->id, x->idPadre);
    }
}

void ArbolEnDisco::AjustarEliminacion_Disco(std::fstream &data_stream, int idNodo, int idNodoPadre)
{
    // Generar los nodos necesarios para el calculo con el que se va trabajar
    Nodo *nodo;
    Nodo *nodoPadre;

    Nodo *hermano;
    Nodo *hermanoIzquierda;
    Nodo *hermanoDerecha;

    bool continuarFlag = true;
    while (continuarFlag)
    {
        nodo = obtenerNodo_Disco(data_stream, idNodo);
        if (idNodo == -1)
        {
            nodo->idPadre = idNodoPadre;            
        }
        nodoPadre = obtenerNodo_Disco(data_stream, nodo->idPadre);

        // If enorme que reemplaza al while clasico
        if (nodo->id != this->idRaiz && nodo->color == NEGRO) // No es necesaria la funcion "ColorDeNodo"
        {
            if (nodo->id == nodoPadre->idIzquierda)
            {
                hermano = obtenerNodo_Disco(data_stream, nodoPadre->idDerecha);
                hermanoIzquierda = obtenerNodo_Disco(data_stream, hermano->idIzquierda);
                hermanoDerecha = obtenerNodo_Disco(data_stream, hermano->idDerecha);
                if (hermano->color == ROJO)
                {
                    //Caso 3.1
                    CambiarColor_Disco(data_stream, hermano->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadre->id, ROJO);
                    RotarIzquierda_Disco(data_stream, nodoPadre->id);
                    hermano = obtenerNodo_Disco(data_stream, nodoPadre->idDerecha);
                    
                    // TODO BUSCAR UNA MANERA DE OPTIMIZAR ESTO (se actualiza varias veces el valor de los hermanos)
                    hermanoIzquierda = obtenerNodo_Disco(data_stream, hermano->idIzquierda);
                    hermanoDerecha = obtenerNodo_Disco(data_stream, hermano->idDerecha);
                }

                if (hermanoIzquierda->color == NEGRO && hermanoDerecha->color == NEGRO)
                {
                    //Caso 3.2
                    CambiarColor_Disco(data_stream, hermano->id, ROJO);
                    idNodo = nodoPadre->id;
                    //Aca se hace imposible que idNodo sea de un TNULL
                    // por ende innecesario actualizar el valor del idNodoPadre "correctamente"
                }
                else
                {
                    if (hermanoDerecha->color == NEGRO)
                    {
                        //Caso 3.3
                        CambiarColor_Disco(data_stream, hermanoIzquierda->id, NEGRO);
                        CambiarColor_Disco(data_stream, hermano->id, ROJO);
                        RotarDerecha_Disco(data_stream, hermano->id);
                        hermano = obtenerNodo_Disco(data_stream, nodoPadre->idDerecha);

                        // TODO BUSCAR UNA MANERA DE OPTIMIZAR ESTO (se actualiza varias veces el valor de los hermanos)
                        hermanoIzquierda = obtenerNodo_Disco(data_stream, hermano->idIzquierda);
                        hermanoDerecha = obtenerNodo_Disco(data_stream, hermano->idDerecha);
                    }
                    //Caso 3.4
                    CambiarColor_Disco(data_stream, hermano->id, nodoPadre->color);
                    CambiarColor_Disco(data_stream, nodoPadre->id, NEGRO);
                    CambiarColor_Disco(data_stream, hermanoDerecha->id, NEGRO);
                    RotarIzquierda_Disco(data_stream, nodoPadre->id);
                    idNodo = this->idRaiz;
                    //Aca se hace imposible que idNodo sea de un TNULL
                    // por ende innecesario actualizar el valor del idNodoPadre "correctamente"
                }
            }
            else //Caso del otro lado
            {
                hermano = obtenerNodo_Disco(data_stream, nodoPadre->idIzquierda);
                hermanoIzquierda = obtenerNodo_Disco(data_stream, hermano->idIzquierda);
                hermanoDerecha = obtenerNodo_Disco(data_stream, hermano->idDerecha);
                
                if (hermano->color == ROJO)
                {
                    //Caso 3.1
                    CambiarColor_Disco(data_stream, hermano->id, NEGRO);
                    CambiarColor_Disco(data_stream, nodoPadre->id, ROJO);
                    RotarDerecha_Disco(data_stream, nodoPadre->id);
                    hermano = obtenerNodo_Disco(data_stream, nodoPadre->idIzquierda);
                    
                    // TODO BUSCAR UNA MANERA DE OPTIMIZAR ESTO (se actualiza varias veces el valor de los hermanos)
                    hermanoIzquierda = obtenerNodo_Disco(data_stream, hermano->idIzquierda);
                    hermanoDerecha = obtenerNodo_Disco(data_stream, hermano->idDerecha);
                }

                if (hermanoDerecha->color == NEGRO && hermanoIzquierda->color == NEGRO)
                {
                    //Caso 3.2
                    CambiarColor_Disco(data_stream, hermano->id, ROJO);
                    idNodo = nodoPadre->id;
                    //Aca se hace imposible que idNodo sea de un TNULL
                    // por ende innecesario actualizar el valor del idNodoPadre "correctamente"
                }
                else
                {
                    if (hermanoIzquierda->color == NEGRO)
                    {
                        //Caso 3.3
                        CambiarColor_Disco(data_stream, hermanoDerecha->id, NEGRO);
                        CambiarColor_Disco(data_stream, hermano->id, ROJO);
                        RotarDerecha_Disco(data_stream, hermano->id);
                        hermano = obtenerNodo_Disco(data_stream, nodoPadre->idIzquierda);

                        // TODO BUSCAR UNA MANERA DE OPTIMIZAR ESTO (se actualiza varias veces el valor de los hermanos)
                        hermanoIzquierda = obtenerNodo_Disco(data_stream, hermano->idIzquierda);
                        hermanoDerecha = obtenerNodo_Disco(data_stream, hermano->idDerecha);
                    }
                    //Caso 3.4
                    CambiarColor_Disco(data_stream, hermano->id, nodoPadre->color);
                    CambiarColor_Disco(data_stream, nodoPadre->id, NEGRO);
                    CambiarColor_Disco(data_stream, hermanoIzquierda->id, NEGRO);
                    RotarIzquierda_Disco(data_stream, nodoPadre->id);
                    idNodo = this->idRaiz;
                    //Aca se hace imposible que idNodo sea de un TNULL
                    // por ende innecesario actualizar el valor del idNodoPadre "correctamente"
                }
            }
        }
        else
        {
            continuarFlag = false;
        }
    }
    CambiarColor_Disco(data_stream, idNodo, NEGRO);
}

//Se reemplaza el NodoU con el NodoV
void ArbolEnDisco::rbTransplant_Disco(std::fstream &data_stream, int idNodoU, int idNodoV)
{
    Nodo *nodoU = obtenerNodo_Disco(data_stream, idNodoU);
    Nodo *nodoUPadre = obtenerNodo_Disco(data_stream, nodoU->idPadre);
    Nodo *nodoV = obtenerNodo_Disco(data_stream, idNodoV);
    if ( nodoUPadre->id == -1)
    {
        this->idRaiz = nodoV->id;
    }
    else if ( nodoU->id == nodoUPadre->idIzquierda )
    {
        actualizarDatosNodo_Disco(data_stream, nodoUPadre->id, EditarNodo::idIzquierda, nodoV->id);
    }
    else
    {
        actualizarDatosNodo_Disco(data_stream, nodoUPadre->id, EditarNodo::idDerecha, nodoV->id);
    }
    actualizarDatosNodo_Disco(data_stream, nodoV->id, EditarNodo::idPadre, nodoUPadre->id);
}

Nodo *ArbolEnDisco::maximum_Disco(std::fstream &data_stream, int idNodo)
{
    Nodo* nodo = obtenerNodo_Disco(data_stream, idNodo);
    while (nodo->idDerecha != -1)
    {
        nodo = obtenerNodo_Disco(data_stream, nodo->idDerecha);
    }
    return nodo;
    
}

Nodo *ArbolEnDisco::minimum_Disco(std::fstream &data_stream, int idNodo)
{
    Nodo *nodo = obtenerNodo_Disco(data_stream, idNodo);
    while (nodo->idIzquierda != -1)
    {
        nodo = obtenerNodo_Disco(data_stream, nodo->idIzquierda);
    }
    return nodo;
}

// Guarda un nodo en Disco y retorna su ID(ubicacion relativa)
Nodo *ArbolEnDisco::CrearNodo_Disco(std::fstream &data_stream, Elemento elemento)
{
    if (!data_stream.is_open())
    {
        return nullptr;
    }

    // Crear el nodo que se va almacenar
    Nodo *nodo = new Nodo(elemento);
    //Replicando el comportamiento de un "TNULL"
    nodo->idPadre = -1;
    nodo->idIzquierda = -1;
    nodo->idDerecha = -1;
    nodo->color = ROJO;

    // Checar si hay un espacio disponible en el vector
    if (this->espaciosLibre.size() > 0)
    {
        //Asignar la posicion segun el id disponible (el minHeap contiene los id's Disponibles)
        data_stream.seekp(ArbolEnDisco::inicioDataNodos + Nodo::sizeBinario * this->espaciosLibre.top(), std::ios::beg);
        
        //Eliminar la primera posicion disponible
        this->espaciosLibre.pop();
    }
    else
    {
        // Moverse al final de la posicion de nodos en el archivo 
        data_stream.seekp(ArbolEnDisco::inicioDataNodos + Nodo::sizeBinario * this->nroNodos , std::ios::beg);
    }

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

    //Aumentar el nro de Nodos del arbol
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
    data_stream.clear();
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

//Funcion para "Eliminar" un nodo del disco
// se refiere a habilitar el espacio dentro del minHeap o liberar la memoria de ser necesario
void ArbolEnDisco::EliminarNodo_Disco(std::fstream &data_stream, int idNodo)
{
    
    //Revisar si el nodo eliminado es el ultimo
    if(idNodo == this->nroNodos-1)
    {
        //De ser el ultimo
        //Moverse hasta la posicion del ultimoNodo
        // Moverse hasta la posicion de IDNodo + iniciando en el inicio de datos
        data_stream.seekg(ArbolEnDisco::inicioDataNodos + Nodo::sizeBinario * idNodo, std::ios::beg);
    
        //Escribir cadena de 0's (equivalente a eliminar al escribir un arcivo)
        char zeros[Nodo::sizeBinario] = {0};
        data_stream.write(zeros, Nodo::sizeBinario);
        data_stream.flush();

        //Disminuir el nro de nodos del arbol
        this->nroNodos = this->nroNodos-1;

    }
    else
    {
        //De no ser el ultimo que se elimina

        //(SOLO POR RAZONES LOGICAS Y DE SAFE-GUARDADO)
        //Eliminar la relacion con el padre del nodo a eliminar
        actualizarDatosNodo_Disco(data_stream, idNodo, EditarNodo::idPadre, -2);

        //Luego agregar esta posicion al minHeap
        this->espaciosLibre.push(idNodo);

        //Y disminuir el numero de nodos del arbol
        this->nroNodos = this->nroNodos-1;
    }

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

void ArbolEnDisco::recorridoInOrder(std::fstream &data_stream, int idNodo)
{
    if (idNodo < 0) {
        return;
    }
    
    Nodo* nodoActual = obtenerNodo_Disco(data_stream, idNodo);

    // Traverse left subtree
    this->recorridoInOrder(data_stream, nodoActual->idIzquierda);

    // Print current node value
    char clr = (nodoActual->color)? 'R' : 'N';
    std::cout << clr << nodoActual->elemento.dni << " ";

    // Traverse right subtree
    this->recorridoInOrder(data_stream, nodoActual->idDerecha);
}

// ---- Fin de clase ----
/*

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

//Funcion para verificar rapidamente el funcionamiento del arbol 
void inOrderTraversal(Nodo* nodo)
{
    if (nodo == nullptr) {
        return;
    }

    // Traverse left subtree
    inOrderTraversal(nodo->izquierda);

    // Print current node value
    if (nodo->elemento.dni != -1)
    {
        std::string color = (nodo->color)? "R" : "N";
        std::cout << color <<nodo->elemento.dni << " ";
    }

    // Traverse right subtree
    inOrderTraversal(nodo->derecha);
}

int mainMemoria() // main memoria
{
    ArbolRB arbol;
    // Insercion del id y dni
    Insertar(&arbol, Elemento(99, 9));
    Insertar(&arbol, Elemento(99, 33));
    Insertar(&arbol, Elemento(99, 11));
    Insertar(&arbol, Elemento(99, 54));
    Insertar(&arbol, Elemento(99, 72));
    Insertar(&arbol, Elemento(99, 85));

    Eliminar(&arbol, 9);
    Eliminar(&arbol, 72);

    //Insertar(&arbol, Elemento(99, 20));
    //Insertar(&arbol, Elemento(99, 59));
    //Eliminar(&arbol, 11);
    
    inOrderTraversal(arbol.raiz);
    std::cout<<std::endl;

    Nodo *encontrado = Buscar(&arbol, 1);
    if (encontrado != nullptr)
    {
        std::cout << "El ID del DNI " << 1 << " esta en " << encontrado->elemento.dni << std::endl;
    }
    else
    {
        std::cout << "No se encontro un ID del DNI " << 85 << std::endl;
    }
    return 0;
}

int mainDisco() // main disco
{
    ArbolEnDisco arbol("Arbol.bin");

    std::fstream archivo("Arbol.bin", std::ios::in | std::ios::out | std::ios::binary);
    arbol.Insertar_Disco(archivo, Elemento(99, 9));  // 0
    arbol.Insertar_Disco(archivo, Elemento(99, 33)); // 1
    arbol.Insertar_Disco(archivo, Elemento(99, 11)); // 2
    arbol.Insertar_Disco(archivo, Elemento(99, 54)); // 3
    arbol.Insertar_Disco(archivo, Elemento(99, 72)); // 4
    arbol.Insertar_Disco(archivo, Elemento(99, 85)); // 5
    arbol.Insertar_Disco(archivo, Elemento(99, 5)); // 6
    arbol.Insertar_Disco(archivo, Elemento(99, 35)); // 7
    arbol.Insertar_Disco(archivo, Elemento(99, 4)); // 8

   
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
    if (archivo.fail())
    {
        archivo.clear();
        archivo.seekg(0, std::ios::beg);
        archivo.seekp(0, std::ios::beg);
    }

    arbol.recorridoInOrder(archivo, arbol.idRaiz);
    std::cout<<std::endl;

    Nodo *pru = arbol.Buscar_Disco(archivo, 72);
    std::cout << "Se busco el nodo con DNI 72: " << std::endl;
    std::cout << pru->elemento.id << std::endl;
    std::cout << "Hay " << arbol.nroNodos << " nodos" << std::endl;

    return 0;
}
*/
