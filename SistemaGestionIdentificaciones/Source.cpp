#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <set>
#include <chrono>

using namespace std::chrono;
using namespace std;

struct Registro {
    string datos;
};

// Función para generar un número de teléfono aleatorio de 9 dígitos que comience con "9"
string generarNumeroTelefonico() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(100000000, 999999999); // Rango de números de teléfono posibles
    return "9" + to_string(dis(gen));
}

// Función para combinar datos de varios archivos en un solo archivo CSV
void crearCSVTelefonos(const string& nombreArchivoSalida, int cantidadRegistros) {
    ofstream archivoSalida(nombreArchivoSalida);
    if (!archivoSalida.is_open()) {
        throw runtime_error("No se pudo abrir el archivo CSV para escribir.");
    }

    // Generar y escribir números de teléfono aleatorios
    archivoSalida << "telefono\n"; // Encabezado de columna
    for (int i = 0; i < cantidadRegistros; ++i) {
        archivoSalida << generarNumeroTelefonico() << "\n";
    }
}

// Función para generar un correo electrónico basado en un nombre completo y alternar dominios
string generarCorreoElectronico(const string& nombreCompleto) {
    static const vector<string> dominios = { "@gmail.com", "@outlook.com" };
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dis(0, dominios.size() - 1);

    string correoElectronico;

    // Convertir nombre completo a minúsculas
    string nombreCompletoEnMinusculas = nombreCompleto;
    transform(nombreCompletoEnMinusculas.begin(), nombreCompletoEnMinusculas.end(), nombreCompletoEnMinusculas.begin(), ::tolower);

    // Reemplazar espacios y comas con puntos
    for (char& c : nombreCompletoEnMinusculas) {
        if (c == ' ' || c == ',') {
            c = '.';
        }
    }

    // Eliminar cualquier punto adicional al final del nombre completo
    if (!nombreCompletoEnMinusculas.empty() && nombreCompletoEnMinusculas.back() == '.') {
        nombreCompletoEnMinusculas.pop_back();
    }

    // Seleccionar aleatoriamente un dominio
    string dominio = dominios[dis(gen)];

    // Generar dirección de correo electrónico
    correoElectronico = nombreCompletoEnMinusculas + dominio;

    return correoElectronico;
}


void crearCSVCorreos(const string& nombreArchivoNombres, const string& nombreArchivoSalida, int cantidadRegistros) {
    ifstream archivoNombres(nombreArchivoNombres);
    if (!archivoNombres.is_open()) {
        throw runtime_error("No se pudo abrir el archivo CSV de nombres para leer.");
    }

    ofstream archivoSalida(nombreArchivoSalida);
    if (!archivoSalida.is_open()) {
        throw runtime_error("No se pudo abrir el archivo CSV de correos para escribir.");
    }

    // Leer nombres completos del archivo CSV
    string linea;
    while (getline(archivoNombres, linea)) {
        // Generar correo electrónico para cada nombre completo
        string correoElectronico = generarCorreoElectronico(linea);
        // Escribir nombre completo y correo electrónico en el archivo de salida
        archivoSalida << linea << correoElectronico << "\n";
    }
}



vector<Registro> leerDesdeArchivos(const vector<string>& nombresArchivos, int cantidadRegistros) {
    vector<ifstream> archivos;
    for (const auto& nombreArchivo : nombresArchivos) {
        ifstream archivo(nombreArchivo);
        if (!archivo.is_open()) {
            throw runtime_error("No se pudo abrir el archivo: " + nombreArchivo);
        }
        archivos.push_back(move(archivo));
    }

    vector<vector<string>> datos;
    datos.resize(nombresArchivos.size());

    for (int i = 0; i < nombresArchivos.size(); ++i) {
        string linea;
        while (getline(archivos[i], linea) && datos[i].size() < cantidadRegistros) {
            datos[i].push_back(linea);
        }
    }

    // Combinar los datos de todos los archivos
    vector<Registro> registros;
    random_device rd;
    mt19937 gen(rd());

    for (int i = 0; i < cantidadRegistros; ++i) {
        Registro registro;
        string datosCombinados;

        // Concatenar los datos de los archivo
        for (int j = 0; j < nombresArchivos.size(); ++j) {
            uniform_int_distribution<> dis(0, datos[j].size() - 1);
            datosCombinados += datos[j][dis(gen)]+ ",";
        }
        registro.datos = datosCombinados;
        registros.push_back(registro);
    }

    return registros;
}

void escribirACSV(const string& nombreArchivoSalida, const vector<Registro>& registros) {
    ofstream archivoSalida(nombreArchivoSalida);
    if (!archivoSalida.is_open()) {
        throw runtime_error("No se pudo abrir el archivo CSV para escribir.");
    }

    for (const auto& registro : registros) {
        archivoSalida << registro.datos << endl;
    }
}

// Función para leer data desde un archivo CSV
vector<string> leerDesdeCSV(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    vector<string> data;
    string linea;

    while (getline(archivo, linea)) {
        // Eliminar la coma al final de la línea, si existe
        if (!linea.empty() && linea.back() == ',') {
            linea.pop_back();
        }

        data.push_back(linea);
    }

    return data;
}

// Función para crear pares de ID-data y escribirlos en un archivo TXT con formato JSON
void crearParesIDData(const string& nombreArchivoSalida, const vector<string>& data, int totalPares) {
    ofstream archivoSalida(nombreArchivoSalida);
    if (!archivoSalida.is_open()) {
        cerr << "Error al abrir el archivo de salida: " << nombreArchivoSalida << endl;
        return;
    }

    archivoSalida << "{\n";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(100000000, 199999999); // Rango de DNI posibles
    
    set<int> dnisValidos; // Conjunto para mantener DNIs únicos

    for (int i = 0; i < totalPares; ++i) {
        int dni;
        do {
            dni = dis(gen); // Genera un DNI aleatorio
        } while (dnisValidos.count(dni) > 0); // Verifica si el DNI ya ha sido generado

        dnisValidos.insert(dni); // Agrega el DNI al conjunto de DNIs válidos

        // Elimina todas las comillas dobles de los datos
        string datosSinComillas = data[i];
        datosSinComillas.erase(remove(datosSinComillas.begin(), datosSinComillas.end(), '\"'), datosSinComillas.end());

        archivoSalida << "  \"" << dni << "\": \"" << datosSinComillas << "\"";
        if (i < totalPares - 1) {
            archivoSalida << ",";
        }
        archivoSalida << "\n";
    }
    archivoSalida << "}\n";

    cout << "Pares de ID-data generados correctamente en el archivo: " << nombreArchivoSalida << endl;
}

int main() {
    int numeroDeIDs = 1000; // Numero de IDs a generar

    // PRIMERA PARTE

    vector<string> nombresCompletos = { "nombres.csv", "apellidos.csv" };
    string archivoNombresCompletos = "TF_nombres_completos.csv";

    auto start = high_resolution_clock::now();

    // Leer registros desde los archivos de datos
    vector<Registro> registrosNombres = leerDesdeArchivos(nombresCompletos, numeroDeIDs);
    
    // Escribir registros en un archivo CSV
    escribirACSV(archivoNombresCompletos, registrosNombres);

    cout << "Nombres Completos creados para " << numeroDeIDs << " IDs en el archivo " << archivoNombresCompletos << '\n';
    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<minutes>(stop - start);
    cout << "Tiempo de ejecucion: " << duration.count() << "minutos.\n";

    // SEGUNDA PARTE

    start = high_resolution_clock::now();
    string nombreArchivoNombresCorreos = "TF_nombres_correos.csv";
    // Crear archivo CSV con direcciones de correo electrónico aleatorias
    crearCSVCorreos(archivoNombresCompletos, nombreArchivoNombresCorreos, numeroDeIDs);

    cout << "Correos creados para " << numeroDeIDs << " IDs en el archivo " << nombreArchivoNombresCorreos << '\n';
    stop = high_resolution_clock::now();
    duration = duration_cast<minutes>(stop - start);
    cout << "Tiempo de ejecucion: " << duration.count() << "minutos.\n";

    // TERCERA PARTE

    string nombreArchivoTelefonos = "TF_telefonos.csv";
    // Crear archivo CSV con números de teléfono aleatorios
    crearCSVTelefonos(nombreArchivoTelefonos, numeroDeIDs);

    // CUARTA PARTE

    start = high_resolution_clock::now();
    vector<string> nombresArchivos = { nombreArchivoNombresCorreos, nombreArchivoTelefonos,"estadoCivil.csv", "nacionalidades.csv","ubigeos_direccion.csv","ubicaciones.csv"}; // Lista de nombres de archivos de datos
    
    string nombreArchivoSalidaFINAL = "TF_datosDNI.csv"; // ARCHIVO FINAL

    // Leer registros desde los archivos de datos
    vector<Registro> registros = leerDesdeArchivos(nombresArchivos, numeroDeIDs);

    // Escribir registros en un archivo CSV
    escribirACSV(nombreArchivoSalidaFINAL, registros);

    cout << "Datos creados para " << numeroDeIDs << " IDs en el archivo " << nombreArchivoSalidaFINAL << '\n';
    stop = high_resolution_clock::now();
    duration = duration_cast<minutes>(stop - start);
    cout << "Tiempo de ejecucion: " << duration.count() << "minutos.\n";

    // PARTE FINAL

    start = high_resolution_clock::now();
    std::string jsonFinal = "TF_jsonDataDNI.json";

    // Leer data desde el archivo CSV
    vector<string> data = leerDesdeCSV(nombreArchivoSalidaFINAL);
    // Crear archivo JSON con pares de ID-nombre
    crearParesIDData(jsonFinal, data, numeroDeIDs);
    stop = high_resolution_clock::now();
    duration = duration_cast<minutes>(stop - start);
    cout << "Tiempo de ejecucion: " << duration.count() << "minutos.\n";

    return 0;
}
