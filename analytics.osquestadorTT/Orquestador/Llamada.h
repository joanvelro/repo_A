#ifndef Llamada_h
#define Llamada_h

#include "Windows.h"
#include <string.h>
#include <stdio.h>
#include <tchar.h>
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

class Llamada
{
private:
	string ruta;
	void DisplayError(LPCTSTR errorDesc, DWORD errorCode);
public:
	Llamada(string rutaEjecutable);
	void ejecuta();
	void dejaEnPie();
	~Llamada();
};

#endif

