#include <iostream>
#include "ArbolRB.h"

//Funcion para insertar un nodo al arbol
void Insertar(ArbolRB *arbol, Elemento elem)
{
    Nodo *nuevoNodo = CrearNodo(elem);
    if (arbol->raiz == nullptr)
    {
        arbol->raiz = nuevoNodo;
        CambiarColor(arbol->raiz, NEGRO);
    }
    else
    {
        InsertarRecursivamente(arbol->raiz, nuevoNodo);
        CambiarColor(nuevoNodo, ROJO);
    }
    AjustarInsercion(arbol, nuevoNodo);
}

void InsertarRecursivamente(Nodo *nodoActual, Nodo *nuevoNodo)
{
    if (nuevoNodo->elemento.dni < nodoActual->elemento.dni)
    {
        if (nodoActual->izquierda == nullptr)
        {
            nodoActual->izquierda = nuevoNodo;
            nuevoNodo->padre = nodoActual;
        }
        else
        {
            InsertarRecursivamente(nodoActual->izquierda, nuevoNodo);
        }
    }
    else
    {
        if (nodoActual->derecha == nullptr)
        {
            nodoActual->derecha = nuevoNodo;
            nuevoNodo->padre = nodoActual;
        }
        else
        {
            InsertarRecursivamente(nodoActual->derecha, nuevoNodo);
        }
    }
}

//Funcion para ajustar el arbol luego de una insercion
void AjustarInsercion(ArbolRB *arbol, Nodo *nodo)
{
    while (nodo->padre != nullptr && nodo->padre->color == ROJO)
    {
        if (nodo->padre == nodo->padre->padre->izquierda)
        {
            Nodo *tio = nodo->padre->padre->derecha;
            if (tio != nullptr && tio->color == ROJO)
            {
                CambiarColor(tio, NEGRO);
                CambiarColor(nodo->padre, NEGRO);
                CambiarColor(nodo->padre->padre, ROJO);
                nodo = nodo->padre->padre;
            }
            else
            {
                if (nodo == nodo->padre->derecha)
                {
                    nodo = nodo->padre;
                    RotarIzquierda(arbol, nodo);
                }
                CambiarColor(nodo->padre, NEGRO);
                CambiarColor(nodo->padre->padre, ROJO);
                RotarDerecha(arbol, nodo->padre->padre);
            }
        }
        else
        {
            Nodo *tio = nodo->padre->padre->izquierda;
            if (tio != nullptr && tio->color == ROJO)
            {
                CambiarColor(tio, NEGRO);
                CambiarColor(nodo->padre, NEGRO);
                CambiarColor(nodo->padre->padre, ROJO);
                nodo = nodo->padre->padre;
            }
            else
            {
                if (nodo == nodo->padre->izquierda)
                {
                    nodo = nodo->padre;
                    RotarDerecha(arbol, nodo);
                }
                CambiarColor(nodo->padre, NEGRO);
                CambiarColor(nodo->padre->padre, ROJO);
                RotarIzquierda(arbol, nodo->padre->padre);
            }
        }
        if (nodo == arbol->raiz)
        {
            break;
        }
    }
    CambiarColor(arbol->raiz, NEGRO);
}

//Funcion para buscar un nodo por DNI
Nodo *Buscar(ArbolRB *arbol, int dni)
{
    Nodo *nodoActual = arbol->raiz;
    while (nodoActual != nullptr && nodoActual->elemento.dni != dni)
    {
        if (dni < nodoActual->elemento.dni)
        {
            nodoActual = nodoActual->izquierda;
        }
        else
        {
            nodoActual = nodoActual->derecha;
        }
    }
    return nodoActual;
}

//Función para eliminar un nodo por DNI
Nodo *Eliminar(ArbolRB *arbol, int dni)
{
    Nodo *nodoReemplazo = nullptr;
    Nodo *hijoReemplazo = nullptr;
    Nodo *nodoAEliminar = Buscar(arbol, dni);
    if (nodoAEliminar == nullptr)
    {
        return nullptr;
    }

    if (nodoAEliminar->izquierda == nullptr || nodoAEliminar->derecha == nullptr)
    {
        nodoReemplazo = nodoAEliminar;
    }
    else
    {
        nodoReemplazo = ObtenerSiguiente(nodoAEliminar);
    }

    if (nodoReemplazo->izquierda != nullptr)
    {
        hijoReemplazo = nodoReemplazo->izquierda;
    }
    else
    {
        hijoReemplazo = nodoReemplazo->derecha;
    }

    if (hijoReemplazo != nullptr)
    {
        hijoReemplazo->padre = nodoReemplazo->padre;
    }

    if (nodoReemplazo->padre == nullptr)
    {
        arbol->raiz = hijoReemplazo;
    }
    else
    {
        if (nodoReemplazo == nodoReemplazo->padre->izquierda)
        {
            nodoReemplazo->padre->izquierda = hijoReemplazo;
        }
        else
        {
            nodoReemplazo->padre->derecha = hijoReemplazo;
        }
    }

    if (nodoReemplazo != nodoAEliminar)
    {
        nodoAEliminar->elemento = nodoReemplazo->elemento;
    }

    if (nodoReemplazo->color == NEGRO)
    {
        AjustarEliminacion(arbol, hijoReemplazo);
    }

    delete nodoReemplazo;
    return nodoAEliminar;
}

//Retorna el nodo que tiene el segundo valor mas alto del arbol para mantenerlo ordenado -> "sucesor in-order"
Nodo *ObtenerSiguiente(Nodo *nodo)
{
    Nodo *actual = nodo->derecha;
    while (actual->izquierda != nullptr)
    {
        actual = actual->izquierda;
    }
    return actual;
}

//Funcion para ajustar los colores tras una eliminacion
void AjustarEliminacion(ArbolRB *arbol, Nodo *nodo)
{
    Nodo *hermano;
    while (nodo != arbol->raiz && ColorDeNodo(nodo) == NEGRO)
    {
        if (nodo == nodo->padre->izquierda)
        {
            hermano = nodo->padre->derecha;
            if (ColorDeNodo(hermano) == ROJO)
            {
                CambiarColor(hermano, NEGRO);
                CambiarColor(nodo->padre, ROJO);
                RotarIzquierda(arbol, nodo->padre);
                hermano = nodo->padre->derecha;
            }
            if (ColorDeNodo(hermano->izquierda) == NEGRO && ColorDeNodo(hermano->derecha) == NEGRO)
            {
                CambiarColor(hermano, ROJO);
                nodo = nodo->padre;
            }
            else
            {
                if (ColorDeNodo(hermano->derecha) == NEGRO)
                {
                    CambiarColor(hermano->izquierda, NEGRO);
                    CambiarColor(hermano, ROJO);
                    RotarDerecha(arbol, hermano);
                    hermano = nodo->padre->derecha;
                }
                CambiarColor(hermano, ColorDeNodo(nodo->padre));
                CambiarColor(nodo->padre, NEGRO);
                CambiarColor(hermano->derecha, NEGRO);
                RotarIzquierda(arbol, nodo->padre);
                nodo = arbol->raiz;
            }
        }
        else
        {
            hermano = nodo->padre->izquierda;
            if (ColorDeNodo(hermano) == ROJO)
            {
                CambiarColor(hermano, NEGRO);
                CambiarColor(nodo->padre, ROJO);
                RotarDerecha(arbol, nodo->padre);
                hermano = nodo->padre->izquierda;
            }
            if (ColorDeNodo(hermano->izquierda) == NEGRO && ColorDeNodo(hermano->derecha) == NEGRO)
            {
                CambiarColor(hermano, ROJO);
                nodo = nodo->padre;
            }
            else
            {
                if (ColorDeNodo(hermano->izquierda) == NEGRO)
                {
                    CambiarColor(hermano->derecha, NEGRO);
                    CambiarColor(hermano, ROJO);
                    RotarIzquierda(arbol, hermano);
                    hermano = nodo->padre->izquierda;
                }
                CambiarColor(hermano, ColorDeNodo(nodo->padre));
                CambiarColor(nodo->padre, NEGRO);
                CambiarColor(hermano->izquierda, NEGRO);
                RotarDerecha(arbol, nodo->padre);
                nodo = arbol->raiz;
            }
        }
    }
    CambiarColor(nodo, NEGRO);
}

bool ColorDeNodo(Nodo *nodo)
{
    if (nodo == nullptr)
    {
        return NEGRO;
    }
    return nodo->color;
}

//Funcion para crear un nodo
Nodo* CrearNodo(Elemento elem)
{
    return new Nodo(elem);
}

void RotarIzquierda(ArbolRB *arbol, Nodo *nodo)
{
    Nodo *nodoDerecha = nodo->derecha;
    nodo->derecha = nodoDerecha->izquierda;
    if (nodoDerecha->izquierda != nullptr)
    {
        nodoDerecha->izquierda->padre = nodo;
    }
    nodoDerecha->padre = nodo->padre;
    if (nodo->padre == nullptr)
    {
        arbol->raiz = nodoDerecha;
    }
    else if (nodo == nodo->padre->izquierda)
    {
        nodo->padre->izquierda = nodoDerecha;
    }
    else
    {
        nodo->padre->derecha = nodoDerecha;
    }
    nodoDerecha->izquierda = nodo;
    nodo->padre = nodoDerecha;
}

void RotarDerecha(ArbolRB *arbol, Nodo *nodo)
{
    Nodo *nodoIzquierda = nodo->izquierda;
    nodo->izquierda = nodoIzquierda->derecha;
    if (nodoIzquierda->derecha != nullptr)
    {
        nodoIzquierda->derecha->padre = nodo;
    }
    nodoIzquierda->padre = nodo->padre;
    if (nodo->padre == nullptr)
    {
        arbol->raiz = nodoIzquierda;
    }
    else if (nodo == nodo->padre->derecha)
    {
        nodo->padre->derecha = nodoIzquierda;
    }
    else
    {
        nodo->padre->izquierda = nodoIzquierda;
    }
    nodoIzquierda->derecha = nodo;
    nodo->padre = nodoIzquierda;
}

void CambiarColor(Nodo *nodo, bool color)
{
    if (nodo != nullptr)
    {
        nodo->color = color;
    }
}

void PrintColor(Nodo *nodo)
{
    if (nodo != nullptr)
    {
        if (nodo->color == ROJO)
        {
            std::cout << "Rojo";
        }
        else
        {
            std::cout << "Negro";
        }
    }
}
