#include <iostream>

#include "ArbolRB.h"

// Funcion para insertar un nodo al arbol
void Insertar(ArbolRB *arbol, Elemento elem)
{
    // Crear un nodo
    Nodo *nodo = CrearNodo(arbol, elem);

    Nodo *y = nullptr;
    Nodo *x = arbol->raiz;

    // Buscar al elemento en el arbol
    while (x != arbol->TNULL)
    {
        y = x;
        if (nodo->elemento.dni < x->elemento.dni)
        {
            x = x->izquierda;
        }
        else
        {
            x = x->derecha;
        }
    }

    // y is parent of x
    nodo->padre = y;
    if (y == nullptr)
    {
        arbol->raiz = nodo;
    }
    else if (nodo->elemento.dni < y->elemento.dni)
    {
        y->izquierda = nodo;
    }
    else
    {
        y->derecha = nodo;
    }

    // if new node is a root node, simply return
    if (nodo->padre == nullptr)
    {
        CambiarColor(nodo, NEGRO);
        return;
    }

    // if the grandparent is null, simply return
    if (nodo->padre->padre == nullptr)
    {
        return;
    }

    // Fix the tree
    AjustarInsercion(arbol, nodo);
}

// Funcion para ajustar el arbol luego de una insercion
void AjustarInsercion(ArbolRB *arbol, Nodo *nodo)
{
    Nodo *u;
    while (nodo->padre->color == ROJO)
    {
        if (nodo->padre == nodo->padre->padre->derecha)
        {
            u = nodo->padre->padre->izquierda; // uncle
            if (u->color == ROJO)
            {
                // case 3.1
                CambiarColor(u, NEGRO);
                CambiarColor(nodo->padre, NEGRO);
                CambiarColor(nodo->padre->padre, ROJO);
                nodo = nodo->padre->padre;
            }
            else
            {
                if (nodo == nodo->padre->izquierda)
                {
                    // case 3.2.2
                    nodo = nodo->padre;
                    RotarDerecha(arbol, nodo);
                }
                // case 3.2.1
                CambiarColor(nodo->padre, NEGRO);
                CambiarColor(nodo->padre->padre, ROJO);
                RotarIzquierda(arbol, nodo->padre->padre);
            }
        }
        else
        {
            u = nodo->padre->padre->derecha; // uncle

            if (u->color == 1)
            {
                // mirror case 3.1
                CambiarColor(u, NEGRO);
                CambiarColor(nodo->padre, NEGRO);
                CambiarColor(nodo->padre->padre, ROJO);
                nodo = nodo->padre->padre;
            }
            else
            {
                if (nodo == nodo->padre->derecha)
                {
                    // mirror case 3.2.2
                    nodo = nodo->padre;
                    RotarIzquierda(arbol, nodo);
                }
                // mirror case 3.2.1
                CambiarColor(nodo->padre, NEGRO);
                CambiarColor(nodo->padre->padre, ROJO);
                RotarDerecha(arbol, nodo->padre->padre);
            }
        }
        if (nodo == arbol->raiz)
        {
            break;
        }
    }
    CambiarColor(arbol->raiz, NEGRO);
}

// Funcion para buscar un nodo por DNI, de no encontrar nada retorna un TNULL
Nodo *Buscar(ArbolRB *arbol, int dni)
{
    Nodo *nodoActual = arbol->raiz;
    while (nodoActual != arbol->TNULL)
    {
        if (nodoActual->elemento.dni == dni)
        {
            return nodoActual;
        }

        if (nodoActual->elemento.dni <= dni)
        {
            nodoActual = nodoActual->derecha;
        }
        else
        {
            nodoActual = nodoActual->izquierda;
        }
    }
    return nodoActual;
}

void Eliminar(ArbolRB *arbol, int dni)
{

    Nodo *z = Buscar(arbol, dni); // z debe representar un TNULL
    Nodo *x, *y;

    if (z == arbol->TNULL)
    {
        return;
    }

    y = z;
    bool y_original_color = y->color;

    if (z->izquierda == arbol->TNULL)
    {
        x = z->derecha;
        rbTransplant(arbol, z, z->derecha);
    }
    else if (z->derecha == arbol->TNULL)
    {
        x = z->izquierda;
        rbTransplant(arbol, z, z->izquierda);
    }
    else
    {
        y = minimum(z->derecha);
        y_original_color = y->color;
        x = y->derecha;
        if (y->padre == z)
        {
            x->padre = y;
        }
        else
        {
            rbTransplant(arbol, y, y->derecha);
            y->derecha = z->derecha;
            y->derecha->padre = y;
        }

        rbTransplant(arbol, z, y);
        y->izquierda = z->izquierda;
        y->izquierda->padre = y;
        CambiarColor(y, z->color);
    }
    delete z;
    if (y_original_color == NEGRO)
    {
        AjustarEliminacion(arbol, x);
    }
}

// Retorna el nodo que tiene el segundo valor mas alto del arbol para mantenerlo ordenado -> "sucesor in-order"
Nodo *ObtenerSiguiente(Nodo *nodo)
{
    while (nodo->izquierda != nullptr)
    {
        nodo = nodo->izquierda;
    }
    return nodo;
}

// Funcion para ~cambiar los valores~ de dos nodos
void rbTransplant(ArbolRB *arbol, Nodo *nodoU, Nodo *nodoV)
{
    if (nodoU == nullptr)
    {
        nodoU = new Nodo();
    }
    if (nodoV == nullptr)
    {
        nodoV = new Nodo();
    }

    if (nodoU->padre == nullptr)
    {
        arbol->raiz = nodoV;
    }
    else if (nodoU == nodoU->padre->izquierda)
    {
        nodoU->padre->izquierda = nodoV;
    }
    else
    {
        nodoU->padre->derecha = nodoV;
    }
    nodoV->padre = nodoU->padre;
}

Nodo *maximum(Nodo *nodo)
{
    while (nodo->derecha != nullptr)
    {
        nodo = nodo->derecha;
    }
    return nodo;
}

Nodo *minimum(Nodo *nodo)
{
    while (nodo->izquierda != nullptr)
    {
        nodo = nodo->izquierda;
    }
    return nodo;
}

// Funcion para ajustar los colores tras una eliminacion
void AjustarEliminacion(ArbolRB *arbol, Nodo *x)
{
    Nodo *s;
    while (x != arbol->raiz && x->color == NEGRO)
    {
        if (x == x->padre->izquierda)
        {
            s = x->padre->derecha;
            if (s->color == ROJO)
            {
                // caso 3.1
                CambiarColor(s, NEGRO);
                CambiarColor(x->padre, ROJO);
                RotarIzquierda(arbol, x->padre);
                s = x->padre->derecha;
            }

            if (s->izquierda->color == NEGRO && s->derecha->color == NEGRO)
            {
                // case 3.2
                CambiarColor(s, ROJO);
                x = x->padre;
            }
            else
            {
                if (s->derecha->color == 0)
                {
                    // case 3.3
                    CambiarColor(s->izquierda, NEGRO);
                    CambiarColor(s, ROJO);
                    RotarDerecha(arbol, s);
                    s = x->padre->derecha;
                }
                // case 3.4
                CambiarColor(s, x->padre->color);
                CambiarColor(x->padre, NEGRO);
                CambiarColor(s->derecha, NEGRO);
                RotarIzquierda(arbol, x->padre);
                x = arbol->raiz;
            }
        }
        else
        {
            s = x->padre->izquierda;
            if (s->color == ROJO)
            {
                // case 3.1
                CambiarColor(s, NEGRO);
                CambiarColor(x->padre, ROJO);
                RotarDerecha(arbol, x->padre);
                s = x->padre->izquierda;
            }
            if (s->derecha->color == NEGRO && s->derecha->color == NEGRO)
            {
                // case 3.2
                CambiarColor(s, ROJO);
                x = x->padre;
            }
            else
            {
                if (s->izquierda->color == NEGRO)
                {
                    // case 3.3
                    CambiarColor(s->derecha, NEGRO);
                    CambiarColor(s, ROJO);
                    RotarIzquierda(arbol, s);
                    s = x->padre->izquierda;
                }
                // case 3.4
                CambiarColor(s, x->padre);
                CambiarColor(x->padre, NEGRO);
                CambiarColor(s->izquierda, NEGRO);
                RotarDerecha(arbol, x->padre);
                x = arbol->raiz;
            }
        }
    }
    CambiarColor(x, NEGRO);
}


// Funcion para crear un nodo
Nodo *CrearNodo(ArbolRB *arbol, Elemento elem)
{
    Nodo* nodo = new Nodo(elem);
    nodo->padre = nullptr;
    nodo->izquierda = arbol->TNULL;
    nodo->derecha = arbol->TNULL;
    CambiarColor(nodo, ROJO);
    return nodo;
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
    nodo->color = color;
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