#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
using namespace std;

class Student
{
private:
  int* number = nullptr;
  char name[100] = {"null"};
  float gpa = 0.1;
  int hola = 0;

public:
  Student() {}
  Student(int n, const char *s, float g) : number(&n), gpa(g) {
    strncpy(name, s, sizeof(name));
  }

  size_t size() const {
    return sizeof(Student);
    //return offsetof(Student, gpa) + sizeof(gpa);
  }

  void save(ofstream &of)
  {
    of.write(reinterpret_cast<const char*>(this), this->size());
    /*
    of.write((char *)&number, sizeof(number));
    of.write(name, sizeof(name));
    of.write((char *)&gpa, sizeof(gpa));
    */
  }

  static Student load(ifstream &inf)
  {
    Student s;
    inf.read(reinterpret_cast<char*>(&s), s.size());
    return s;
    /*
    inf.read((char *)&number, sizeof(number));
    inf.read(name, sizeof(name));
    inf.read((char *)&gpa, sizeof(gpa));
    */
  }

  int getNumber()
  {
    return *number;
  }

  char* getName(){
    return name;
  }

  float getGpa()
  {
    return gpa;
  }

  void cambiar(int n, const char *s, float g)
  {
    *number = n;
    strncpy(name, s, 49); // leave room for null terminator
    name[49] = '\0';      // ensure null termination
    gpa = g;
  }
};

//Funcion para almacenar datos al binario
void guardarDatos()
{
  cout << "Guardando Data" << endl;
  ofstream myfile;
  myfile.open("silly.dat", ios::binary | ios::out);

  // Se almacena este estudiante en binario
  Student me(111, "Myself", 10.1);
  me.save(myfile);

  // Se almacena este estudiante en binario
  Student me2(222, "Yo212345678965432165498765431023156489", 20.3);
  me2.save(myfile);

  // Se almacena este estudiante en binario
  Student me3(333, "Yooo3", 35.4);
  me3.save(myfile);

  // Cerrar el documento
  myfile.close();
}

//Funcion para extraer e imprimir datos
void imprimirData(int elemento)
{
  cout << "EXTRAACCION" << endl;


  // Abrir el archivo binario
  ifstream otroFile;
  otroFile.open("silly.dat", ios::binary | ios::out);

  // Mueve el puntero la cantidad de espacios necesarios
  otroFile.seekg(sizeof(Student) * elemento, ios::beg);

  // Carga los datos a un objeto
  Student yo = Student::load(otroFile);

  cout << yo.getNumber() << endl;
  cout << yo.getName() << endl;
  cout << yo.getGpa() << endl;

  otroFile.close();
}

int otro()
{
  Student pr (90768320, "HolaoasFANSfolA falKSFnlakfsn *(213 Asf$12)", 32325.5652032);
  
  cout<<sizeof(Student)<<endl;
  cout<<pr.size()<<endl;
  
  guardarDatos();
  imprimirData(0);

  return (0);
}