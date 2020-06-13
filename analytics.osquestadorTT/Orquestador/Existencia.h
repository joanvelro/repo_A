#ifndef Existencia_h
#define Existencia_h

#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

class Existencia
{
private:
	string ruta;
public:
	Existencia(string &rutaArchivo);
	~Existencia();
	bool existe();
	//bool buenFormato();
};
#endif

