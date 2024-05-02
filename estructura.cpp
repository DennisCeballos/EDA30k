#include<iostream>
#include"estructura.h"

using namespace std;

Estructura::Estructura(/* args */)
{
}

bool Estructura::CargarDatos(){
    cout<<"Cargando datos..."<<endl;
    return true;   
}

int Estructura::Buscar(int id){
    cout<<"Buscando datos"<<endl;
    return 0;
}

void Estructura::Insertar(int data){
    cout<<"Insertando dato"<<data<<endl;
}

Estructura::~Estructura()
{
}