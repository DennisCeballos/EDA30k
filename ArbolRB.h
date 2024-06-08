#ifndef ARBOLRB_H
#endif // ARBOLRB_H
#define ARBOLRB_H

#define ROJO true
#define NEGRO false

struct Elemento;
struct Nodo;
struct ArbolRB;



void Insertar               (ArbolRB *arbol, Elemento elem);
void AjustarInsercion       (ArbolRB *arbol, Nodo *nodo);
void InsertarRecursivamente (Nodo *nodoActual, Nodo *nuevoNodo);

Nodo *Buscar                (ArbolRB *arbol, int valor);
Nodo *Eliminar              (ArbolRB *arbol, int valor);
Nodo *ObtenerSiguiente      (Nodo *nodo);
void AjustarEliminacion     (ArbolRB *arbol, Nodo *nodo);
Nodo *CrearNodo             (Elemento elem);
void RotarIzquierda         (ArbolRB *arbol, Nodo *nodo);
void RotarDerecha           (ArbolRB *arbol, Nodo *nodo);
void CambiarColor           (Nodo *nodo, bool color);
bool ColorDeNodo            (Nodo *nodo);
void PrintColor             (Nodo *nodo);

int BuscarPorDni            (ArbolRB *arbol, int dni);

