#ifndef BorraMueve_h
#define BorraMueve_h

#include "Existencia.h"

#include <stdio.h>
#include <sstream>
#include <string.h>
#include "Windows.h"

using namespace std;

class BorraMueve
{
private:
	string ruta;
	Existencia* existencia;
public:
	bool mueve(string &nuevaRuta);
	bool borra();
	BorraMueve(string &rutaArchivo);
	~BorraMueve();
};

#endif