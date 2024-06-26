#pragma once
#ifndef ARBOLRB_H
#define ARBOLRB_H

#define ROJO true
#define NEGRO false

struct Elemento
{
public:
    int dni; //DNI del ciudadano
    int id;  //"Posicion" del ciudadano en el archivo de datos

    Elemento(int id = 0, int dni = 0) : id(id), dni(dni) {}
};

struct Nodo
{
    Elemento elemento {-1, -1};
    bool color {NEGRO};
    int id {-1};

    Nodo *izquierda { nullptr };
    int idIzquierda {-1};

    Nodo *derecha {nullptr};
    int idDerecha {-1};

    Nodo *padre {nullptr};
    int idPadre {-1};

    Nodo(Elemento elem) : elemento(elem) {}
    Nodo() {}

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
    const static int sizeBinario =
    {
        sizeof(idIzquierda)+
        sizeof(idDerecha)+
        sizeof(idPadre)+
        sizeof(elemento.dni)+
        sizeof(elemento.id)+
        sizeof(color) 
    };
};

struct ArbolRB
{
    Nodo *raiz;
    Nodo *TNULL;

    ArbolRB()
    {
        TNULL = new Nodo();
        TNULL->color = NEGRO;
        TNULL->izquierda = nullptr;
        TNULL->derecha = nullptr;

        raiz = TNULL;
    }
};

void Insertar               (ArbolRB *arbol, Elemento elem);
void AjustarInsercion       (ArbolRB *arbol, Nodo *nodo);

Nodo *Buscar                (ArbolRB *arbol, int valor);

void Eliminar               (ArbolRB *arbol, int valor);
void AjustarEliminacion     (ArbolRB *arbol, Nodo *nodo);
//Nuevos
void rbTransplant           (ArbolRB *arbol, Nodo *nodoU, Nodo *nodoV);
Nodo *minimum               (Nodo *nodo);
Nodo *maximum               (Nodo *nodo);
//Nuevos


Nodo *CrearNodo             (ArbolRB *arbol, Elemento elem);

void RotarIzquierda         (ArbolRB *arbol, Nodo *nodo);
void RotarDerecha           (ArbolRB *arbol, Nodo *nodo);

void CambiarColor           (Nodo *nodo, bool color);

void PrintColor             (Nodo *nodo);


#endif //ARBOLRB_H
