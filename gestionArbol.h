#ifndef GESTION_ARBOL_H
#define GESTION_ARBOL_H

#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <cstring>
#include <cmath>
#include "ArbolRB.h"

enum EditarNodo
{
    idIzquierda,
    idDerecha,
    idPadre,
};

class ArbolEnDisco
{
public:
    int nroNodos;
    int idRaiz;
    std::string nombreArchivo;
    ArbolRB *arbolMemoria;
    std::priority_queue<int, std::vector<int>, std::greater<int>> espaciosLibre; //Un minheap para almacenar las eliminaciones

    // Constructor de la clase
    ArbolEnDisco(const char *filename);

    // Este es el tamano de la cabecera
    const static int inicioDataNodos;

    // Decontructor para asegurar el almacenamiento de datos
    ~ArbolEnDisco();

    // Son las mismas operaciones del ArbolRojo pero aplicadas sobre el disco
    void Insertar_Disco(std::fstream &data_stream, Elemento elemento);
    void AjustarInsercion_Disco(std::fstream &data_stream, int idNodo);

    Nodo *Buscar_Disco(std::fstream &data_stream, int valor);

    void Eliminar_Disco(std::fstream &data_stream, int valor);
    void AjustarEliminacion_Disco(std::fstream &data_stream, int idNodo, int idNodoPadre);
    
    //Nuevos
    void rbTransplant_Disco(std::fstream &data_stream, int idNodoU, int idNodoV);
    Nodo* minimum_Disco(std::fstream &data_stream, int idNodo);
    Nodo* maximum_Disco(std::fstream &data_stream, int idNodo);

    Nodo *CrearNodo_Disco(std::fstream &data_stream, Elemento elemento);

    void RotarIzquierda_Disco(std::fstream &data_stream, int idNodo);
    void RotarDerecha_Disco(std::fstream &data_stream, int idNodo);

    void CambiarColor_Disco(std::fstream &data_stream, int idNodo, bool color);

    // Control de nodos en Disco
    Nodo *obtenerNodo_Disco(std::fstream &data_stream, int nodoId);
    void actualizarDatosNodo_Disco(std::fstream &data_stream, int nodoId, EditarNodo variableEditar, int nuevoValor);
    void EliminarNodo_Disco(std::fstream &data_stream, int nodoId);

    // Funciones adicionales
    void guardarNodosEnDisco_MASIVO(ArbolRB *arbol);

    void recorridoInOrder(std::fstream &data_stream, int nodoRaiz);
};

#endif // GESTION_ARBOL_H
