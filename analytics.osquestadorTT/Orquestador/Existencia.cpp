#include "Existencia.h"



Existencia::Existencia(string &rutaArchivo):ruta(rutaArchivo)
{
}

bool Existencia::existe()
{
	std::ifstream infile(this->ruta);
	return infile.good();
}

Existencia::~Existencia()
{
}
