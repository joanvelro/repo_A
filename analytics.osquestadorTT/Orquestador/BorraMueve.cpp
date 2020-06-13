#include "BorraMueve.h"



BorraMueve::BorraMueve(string &rutaArchivo):ruta(rutaArchivo)
{
	this->existencia = new Existencia(rutaArchivo);
}

bool BorraMueve::borra()
{
	cout << "Se borra: " << this->ruta << endl;
	remove(this->ruta.c_str());
	bool exito = !this->existencia->existe();
	return exito;
}

bool BorraMueve::mueve(string &nuevaRuta)
{
	cout << "Se mueve: " << this->ruta << endl;
	std::wstring stemp = std::wstring(this->ruta.begin(), this->ruta.end());
	LPCWSTR origen = stemp.c_str();
	std::wstring stemp2 = std::wstring(nuevaRuta.begin(), nuevaRuta.end());
	LPCWSTR destino = stemp2.c_str();
	MoveFile(origen, destino);
	Existencia* seHaMovido = new Existencia(nuevaRuta);
	bool exito = (!this->existencia->existe()) && (seHaMovido->existe());
	delete seHaMovido;
	return exito;
}

BorraMueve::~BorraMueve()
{
	delete existencia;
}
