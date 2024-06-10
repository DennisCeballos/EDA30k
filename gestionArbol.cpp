#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <string>
#include <queue>
#include <cmath>

#include "ArbolRB.h"
//En este archivo se definen las clases importantes del proyecto
// - Elemento de Nodos
// - Nodos
// - ArbolRB

class ArbolEnDisco
{
private:
    int nroNodos { 0 };
    int idRaiz { -1 };
    std::string nombreArchivo;
    ArbolRB* arbolMemoria;

public:
    ArbolEnDisco(const char* filename);

    //Este es el tamano puesto que es el tamano de la cabecera
    const static int inicioDataNodos =
    {
        sizeof(nroNodos) + sizeof(idRaiz)
    };



    //Copias de las operaciones del ArbolRojo pero aplicadas sobre el disco
    void Insertar_Disco(std::fstream& data_stream, Elemento elemento);
    void InsertarRecursivamente_Disco(std::fstream& data_stream, Nodo* nuevoNodo);



    void CambiarColor_Disco(std::fstream& data_stream, int idNodo, bool color);

    int buscarPorDNI_Disco(int ciudadanoId);
    
    //
    //Control de nodos
    //
    //
    int guardarNodo_Disco(std::fstream& data_stream, Nodo* nodo);
    void actualizarNodoArchivo(int nodoId);
    void guardarNodosEnDisco_MASIVO(ArbolRB* arbol);
    Nodo* buscarNodoEnArchivo(int nodoId);

};

void ArbolEnDisco::Insertar_Disco(std::fstream& data_stream, Elemento elemento)
{

    Nodo *nuevoNodo = CrearNodo(elemento);
    if (this->idRaiz==-1)
    {
        this->idRaiz = guardarNodo_Disco(data_stream, nuevoNodo);
        CambiarColor_Disco(data_stream, ROJO)
    }
    else
    {
        InsertarRecursivamente_Disco(data_stream, nuevoNodo);
        CambiarColor_Disco(data_stream, nuevoNodo, ROJO);
    }

}

// Constructor de la clase
ArbolEnDisco::ArbolEnDisco(const char* filename)
{
    this->nombreArchivo = filename;
    this->arbolMemoria = new ArbolRB; //Se genera un arbol con raiz nula
    std::ifstream fileIN(filename, std::ios::in | std::ios::binary);

    // Revisa que el archivo exista
    if (fileIN.is_open())
    {
        //Si EXISTE
        // *Obtener los datos desde la cabecera
        fileIN.seekg(0, std::ios::beg);
        fileIN.read(reinterpret_cast<char*>(&nroNodos),sizeof(nroNodos));
        fileIN.read(reinterpret_cast<char*>(&idRaiz),sizeof(idRaiz));

        // Agregar la raiz al arbol
        Nodo* raiz = this->buscarNodoEnArchivo(this->idRaiz);
        arbolMemoria->raiz = raiz;

        //Cerrar el archivo
        fileIN.close();
    }
    else
    {
        //SI EL ARCHIVO NO EXISTE
        //* Crea un nuevo archivo 
        std::ofstream fileOUT(filename, std::ios::binary | std::ios::out);
        
        // *Grabar la cabecera de archivo
        fileOUT.seekp(0, std::ios::beg);
        fileOUT.write(reinterpret_cast<char*>(&nroNodos), sizeof(nroNodos));
        fileOUT.write(reinterpret_cast<char*>(&idRaiz), sizeof(idRaiz));
        
        //Cerrar el archivo
        fileOUT.close();
    }

}

// Funcion para obtener el id de un ciudadano de acuerdo a su dni
int ArbolEnDisco::buscarPorDNI_Disco(int ciudadanoDNI)
{
    //Buscar desde la raiz
    Nodo* nodoActual = arbolMemoria->raiz;
    
    //Bucle para buscar un elemento
    while (nodoActual != nullptr)
    {
        if (nodoActual->elemento.dni == ciudadanoDNI)
        {
            //Encontro una coincidencia con el DNI, por lo cual retorna el id
            return nodoActual->elemento.id;
        }
        if ( ciudadanoDNI < nodoActual->elemento.dni)
        {
            //Si el nodo actual no apunta a nada, que obtenga el nodo desde archivo
            if (nodoActual->izquierda == nullptr)
            {
                nodoActual->izquierda = buscarNodoEnArchivo(nodoActual->idIzquierda);
            }
            nodoActual = nodoActual->izquierda;
        }
        else
        {
            //Si el nodo actual no apunta a nada, que obtenga el nodo desde archivo
            if (nodoActual->derecha == nullptr)
            {
                nodoActual->derecha = buscarNodoEnArchivo(nodoActual->idDerecha);
            }
            nodoActual = nodoActual->derecha;
        }
    }
    return -1; //Indica que no se encontro un DNI
}

//Funcion para obtener un nodo desde archivo segun su id en el archivo
Nodo* ArbolEnDisco::buscarNodoEnArchivo(int nodoId)
{
    Nodo* encontrado = new Nodo();

    //Abre el archivo en binario
    std::ifstream file;
    file.open(this->nombreArchivo, std::ios::in | std::ios::binary );
    
    if (!file) //Revisar errores
    {
        throw std::runtime_error("No se encontro el archivo con datos de nodos, se busca " + nombreArchivo);
    }

    //Moverse hasta la posicion de IDNodo + iniciando en el inicio de datos 
    file.seekg( ArbolEnDisco::inicioDataNodos + Nodo::sizeBinario * nodoId, std::ios::beg);

    //! IMPORTANTE EL ORDEN EN EL QUE SE GUARDAN AL ARCHIVO
    //ESTA *ES* LA ESTRUCTURA DE UN NODO EN BINARIO
    file.read(reinterpret_cast<char*>(&encontrado->idIzquierda), sizeof(int));
    file.read(reinterpret_cast<char*>(&encontrado->idDerecha), sizeof(int));
    file.read(reinterpret_cast<char*>(&encontrado->idPadre), sizeof(int));
    file.read(reinterpret_cast<char*>(&encontrado->elemento.dni), sizeof(int));
    file.read(reinterpret_cast<char*>(&encontrado->elemento.id), sizeof(int));
    file.read(reinterpret_cast<char*>(&encontrado->color), sizeof(bool));

    file.close();

    if (encontrado->elemento.dni == -1) //Significa que en realidad es un elemento nulo
    {
        return nullptr;
    }

    return encontrado;
}

//Guarda un nodo en Disco y retorna su ID(ubicacion relativa)
int ArbolEnDisco::guardarNodo_Disco(std::fstream& data_stream, Nodo* nodo)
{

    //Moverse al final del archivo
    data_stream.seekp(0, std::ios::end);
    
    //Guardar posicion en el archivo
    int pos = data_stream.tellp();

    //! IMPORTANTE EL ORDEN EN EL QUE SE GUARDAN AL ARCHIVO
    //ESTA *ES* LA ESTRUCTURA DE UN NODO EN BINARIO
    data_stream.write(reinterpret_cast<char*>(nodo->idIzquierda), sizeof(int));
    data_stream.write(reinterpret_cast<char*>(nodo->idDerecha), sizeof(int));
    data_stream.write(reinterpret_cast<char*>(nodo->idPadre), sizeof(int));
    data_stream.write(reinterpret_cast<char*>(nodo->elemento.dni), sizeof(int));
    data_stream.write(reinterpret_cast<char*>(nodo->elemento.id), sizeof(int));
    data_stream.write(reinterpret_cast<char*>(nodo->color), sizeof(bool));
    
    this->nroNodos += 1;

    return pos;
}

//Recibe un arbol binario y lo almacena en disco, por defecto dejara a la raiz en la primera posicion del archivo
void ArbolEnDisco::guardarNodosEnDisco_MASIVO(ArbolRB* arbol)
{
    if (arbol->raiz == nullptr) 
    {
        std::cout<<"Se intento guardar un arbol vacio";
        return;
    }
    std::vector<Nodo*> concatenado;
    std::queue<Nodo*> fila;

    fila.push(arbol->raiz);

    //Genera un "concatenado" con todos los nodos del arbol
    while (!fila.empty())
    {
        Nodo* actual = fila.front();
        fila.pop();

        //Guarda el elemento
        concatenado.push_back(actual);

        if (actual->izquierda!=nullptr) fila.push(actual->izquierda);
        if (actual->derecha!=nullptr)   fila.push(actual->derecha);
    }

    //Guarda masivamente a los nodos ubicados en el arbol
    std::cout<<"A punto de guardar un arbol con elementos "<<concatenado.size()<<std::endl;

    //Abrir el archivo
    std::ofstream file;
    file.open(this->nombreArchivo, std::ios::binary | std::ios::out );

    //Moverse hasta la posicion de los datos de los nodos
    file.seekp(ArbolEnDisco::inicioDataNodos, std::ios::beg);

    int aux;
    //! Ya que es subida masiva, se deben generar los valores de los ID's de los nodos
    // cada valor se genera a partir de una operacion matematica especifica
    for (int i = 0; i < concatenado.size(); i++)
    {
        //! IMPORTANTE EL ORDEN EN EL QUE SE GUARDAN AL ARCHIVO
        //ESTA *ES* LA ESTRUCTURA DE UN NODO EN BINARIO
        
        aux = i*2+1; //Calculo del idIzquierda 
        file.write(reinterpret_cast<char*>( &aux ), sizeof(int));

        aux = i*2+2; //Calculo del idDerecha
        file.write(reinterpret_cast<char*>( &aux ), sizeof(int));

        aux = std::floor(i/2 -0.5); //Calculo del idPadre
        file.write(reinterpret_cast<char*>( &aux ), sizeof(int));

        file.write(reinterpret_cast<char*>(&concatenado.at(i)->elemento.dni), sizeof(int));
        file.write(reinterpret_cast<char*>(&concatenado.at(i)->elemento.id),  sizeof(int));
        file.write(reinterpret_cast<char*>(&concatenado.at(i)->color),        sizeof(bool));
    }
    
    //Actualizar el numero de nodos antes de cerrar el documento
    this->nroNodos = concatenado.size();
    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<char*>(&nroNodos),sizeof(nroNodos));

    //Cerrar el archivo
    file.close();
    
}

int main()
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

    //ArbolEnDisco guardado("Arbol.bin");
    //guardado.guardarNodosEnDisco_MASIVO(&arbol);
    //
    //
    //Nodo* avr = guardado.buscarNodoEnArchivo(0);
    //std::cout<<avr->elemento.dni<<std::endl;
    //std::cout<<avr->elemento.id<<std::endl;
    //std::cout<<avr->idDerecha<<std::endl;
    //std::cout<<avr->idIzquierda<<std::endl;
    //std::cout<<avr->idPadre<<std::endl;
    
    return 0;
}
