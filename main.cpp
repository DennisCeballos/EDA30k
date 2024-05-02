#include <iostream>
#include "estructura.h"
using namespace std;

int main() {

    //Crear Estructura de datos
    Estructura* data = new Estructura();

    //Cargar datos a la estructura
    if ( !data->CargarDatos() ){
        cout<<"> Hubo un error al cargar los datos"<<endl;
        cout<<"> Cerrando programa . . ."<<endl;
        return 0;
    }

    //Iniciar menu
    int rpta = -1;
    while (rpta != 0)
    {
        rpta = -1;
        cout<<"> Que operacion deseas realizar?"<<endl;
        
        while ( !(0<=rpta && rpta<=5) )
        {
            cin>>rpta;
        }

        switch (rpta)
        {
        case 0:
            break;

        case 1:
            cout<<"> Elegiste la 1"<<endl;
            data->Insertar(1);
            break;

        case 2:
            cout<<"> Elegiste la 2"<<endl;
            data->Buscar(1);
            break;
        
        case 3:
            cout<<"> Elegiste la 3"<<endl;
            break;
        
        default:
            cout<<"> Debes elegir una de las opciones validas"<<endl;
            break;
        }

    }
    
}