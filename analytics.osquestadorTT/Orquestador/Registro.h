#ifndef Registro_h
#define Registro_h

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Windows.h"
//#include "Winreg.h"

using namespace std;

class Registro
{
private:
	string rutaRegistro;
	string subclave;
public:
	void setSubclave(string &subClave);
	string leeValor(string &valueName);
	Registro(string &rutaKeyRegistro);
	bool existeClave();
	~Registro();
};

#endif