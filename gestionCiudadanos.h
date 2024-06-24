#ifndef GESTION_CIUDADANOS_H
#define GESTION_CIUDADANOS_H

#include <iostream>
#include <fstream>
#include <vector>
#include "Ciudadano.h"

class CtrlCiudadano
{
private:
    int nroElementos;
    std::string nombreArchivo;

public:
    CtrlCiudadano(const char* _filaName);
    ~CtrlCiudadano();

    const static int sizeCabecera;

    std::string getNombreArchivo();
    int getNroCiudadanos();

    // Función para guardar un ciudadano en el archivo binario
    int guardarCiudadano(Ciudadano* ciudadano, bool debug);

    // Función para guardar VARIOS ciudadanos en el archivo binario
    std::vector<int> guardarCiudadano_MASIVO(std::vector<Ciudadano*> vectorCiudadanos, bool debug);

    // Función para retornar un ciudadano según su id en el archivo
    Ciudadano* obtenerCiudadanoEnPos(int id, bool debug);
};

#endif // GESTION_CIUDADANOS_H
