#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <tuple>
#include <ctime>
#include <vector>

#include "Ciudadano.h"

using namespace std;

//*SERIA BUENO QUE ESTO ESTE EN OTRA CLASE
//*SERIA BUENO QUE ESTO ESTE EN OTRA CLASE
//*SERIA BUENO QUE ESTO ESTE EN OTRA CLASE

class CtrlCiudadano
{
    private:
        int nroElementos; 
        string nombreArchivo;
    public:
        CtrlCiudadano(const char* _filaName)
        {
            this->nroElementos = 0;
            this->nombreArchivo = _filaName;
        }

        string getNombreArchivo(){
            return nombreArchivo;
        }

        // Fucion para guardar un ciudadano en el archivo binario
        int guardarCiudadano(Ciudadano* ciudadano, bool debug = false)
        {
            ofstream myfile;

            //Se abre el archivo en binario | salida de datos
            myfile.open(nombreArchivo, ios::binary | ios::out | ios::app);

            //Moverse hasta el final del archivo
            myfile.seekp(sizeof(Ciudadano)*this->nroElementos, ios::end);
            
            //Guardar la posicion actual del documento (o sea justo donde se va guardar al ciudadano)
            int pos = myfile.tellp();

            //Almacenar el ciudadano en binario
            myfile.write(reinterpret_cast<char*>(&ciudadano->dni), sizeof(Ciudadano::dni));
            myfile.write(reinterpret_cast<char*>(&ciudadano->datos), sizeof(Ciudadano::datos));

            //Cerrar el archivo por seguridad
            myfile.close();

            if (debug) { cout<<"Guardado correctamente al ciudadano "<<ciudadano->datos.substr(0,10)<<" en pos "<<pos<<endl; }
            this->nroElementos += 1;
            return pos;
        }

        // Fucion para guardar VARIOS ciudadanos en el archivo binario
        vector<int> guardarCiudadano_MASIVO(vector<Ciudadano*> vectorCiudadanos, bool debug = false)
        {
            vector<int> vectorIDs;
            int pos;

            ofstream myfile;

            //Se abre el archivo en binario | salida de datos
            myfile.open(nombreArchivo, ios::binary | ios::out | ios::app);
            
            try
            {
                for (Ciudadano* ciudadano : vectorCiudadanos)
                {
                    //Moverse hasta el final del archivo
                    myfile.seekp(0, ios::end);

                    //Guardar la posicion actual del documento (o sea justo donde se va guardar al ciudadano)
                    pos = myfile.tellp();
                    vectorIDs.push_back( pos );

                    //Almacenar el ciudadano en binario
                    myfile.write(reinterpret_cast<char*>(ciudadano), sizeof(Ciudadano));                
                    
                    if (debug) { cout<<"Guardado correctamente al ciudadano "<<ciudadano->datos<<" en pos "<<pos<<endl; }

                    this->nroElementos += 1;
                }
            }
            catch(const std::exception& e)
            {
                cout<<"Hubo un error en la insercion masiva"<<endl;
                std::cerr << e.what() << std::endl;
            }
            
            //Cerrar el archivo por seguridad
            myfile.close();

            return vectorIDs;
        }

        //Funcion para retornar un ciudadano
        //segun su id en el archivo
        Ciudadano* obtenerCiudadanoEnPos(int id, bool debug=false)
        {
            try
            {
                Ciudadano* ciudadano = new Ciudadano();
                //Abrir el archivo
                ifstream otroFile;
                otroFile.open(nombreArchivo, ios::binary | ios::in);
                if (!otroFile)
                {
                    throw runtime_error("No se encontro el archivo con datos ciudadanos, se busca " + nombreArchivo);
                }

                // Mueve el puntero la cantidad de espacios necesarios
                otroFile.seekg(sizeof(Ciudadano)*id, ios::beg);
                
                // Lee los datos desde el binario y carga los datos al objeto
                otroFile.read(reinterpret_cast<char*>(&ciudadano->dni), sizeof(Ciudadano::dni));
                otroFile.read(reinterpret_cast<char*>(&ciudadano->datos), sizeof(Ciudadano::datos));
                
                if (!otroFile)
                {
                    throw runtime_error("Error al leer de archivo");
                }
                
                //Cerrar el documento
                otroFile.close();

                if (debug) { cout<<"Se obtuvo al ciudadano "<<ciudadano->getDatos().substr(0,10)<<" en pos "<<id<<endl; }
                return ciudadano;
            }
            catch(const runtime_error& e)
            {
                std::cerr << "Error: "<< e.what() << endl;
                return new Ciudadano();
            }

            //return Ciudadano(); //El compilador por algun motivo quiere que esto este aqui
        }

};


//*SERIA BUENO QUE ESTO ESTE EN OTRA CLASE
//*SERIA BUENO QUE ESTO ESTE EN OTRA CLASE
//*SERIA BUENO QUE ESTO ESTE EN OTRA CLASE

int main()
{
    const char* archivo = "datos.bin";
    CtrlCiudadano ctrlCiudadano(archivo);

    Ciudadano* pepe = new Ciudadano(
        12345678,
        "Pepe, Peruano, Ica, Su casa, 123456789, unGmail, soltero si eres mujer"
    );

    Ciudadano* juan = new Ciudadano(
        87654321, "Juan, Chino, Callao, Casa de su tia, 987654321, nica, Viudo"
    );

    Ciudadano* promedio = new Ciudadano(
        66677700, "promedio, peruana, Callao, Casa cualquiera, 111555999, nica, Viudo"
    );

    cout<<"tamano"<<sizeof(Ciudadano)<<endl;
    cout<<"tamanoIndi"<<sizeof(Ciudadano::getSizeofInd())<<endl;
    cout<<"tamano indio"<<sizeof(Ciudadano::datos) + sizeof(Ciudadano::dni)<<endl;
    
    int sitio = -1;
    sitio = ctrlCiudadano.guardarCiudadano(pepe, true);
    cout<<">>"<<sitio<<endl;
    
    sitio = ctrlCiudadano.guardarCiudadano(juan, true);
    cout<<">>"<<sitio<<endl;

    sitio = ctrlCiudadano.guardarCiudadano(promedio, true);
    cout<<">>"<<sitio<<endl;

    //clock_t start = clock();
    //
    ////! OBJETIVO 33 000 000
    ////! Actual   1   00 000
    //for (int i = 0; i < 100000; i++)
    //{
    //    promedio->dni = promedio->dni+1;
    //    sitio = ctrlCiudadano.guardarCiudadano(promedio);
    //    cout<<i<<" - DNI:"<<promedio->dni<<endl;
    //}

    //clock_t end = clock();
    //double tiempo_transcurrido = (double)(end - start) / CLOCKS_PER_SEC;
    //cout<<"Tiempo de ejecucion: "<<tiempo_transcurrido<<" segundos"<<endl;

    cout<<"Datos extraidos:"<<endl;
    Ciudadano* fin = ctrlCiudadano.obtenerCiudadanoEnPos(0, true);
    cout<<fin->getDni()<<endl;
    cout<<fin->getDatos()<<endl;



    return 0;
}