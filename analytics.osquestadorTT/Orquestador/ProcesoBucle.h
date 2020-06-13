#ifndef ProcesoBucle_h
#define ProcesoBucle_h

#include "Registro.h"
#include "Tiempo.h"
#include "Llamada.h"
#include "Existencia.h"
#include "BorraMueve.h"

#include <thread>

class ProcesoBucle
{
private:
	bool firstIter;
	//thread multAlg;

	Registro* registro;
	Tiempo* tiempo;
	Fecha* minBucle; // Fecha de la iteración teórica actual
	Fecha* fechaComp; // Fecha del inicio de la siguiente compresión
	bool hayHilo; // Valdrá true cuando haya un hilo en ejecución
	bool comprime; // Valdrá true cuando haya que comprimir

	thread hiloDat; // Hilo compresor datos
	thread hiloPred; // Hilo compresor predicciones
	clock_t lanzaHilo; // Contador de tiempo que hay que esperar para terminar el hilo

	string fecha; // Fecha real de cada iteración actual
	double periodo; // Periodo (en segundos) de ejecución del bucle
	int comprimeCada; // Periodo (en dias) de compresión de archivo

	string rutaP1; // Ruta abs de Ejecutable1
	string DatosP1; // Ruta abs de archivo de datos generado por P1

	string rutaP2; // Ruta abs de Ejecutable2
	string confP2; // Ruta abs del archivo de configuraciones de P2
	string DatosP2; // Ruta abs de archivo de datos procesado por P2
	string PrediccionP2; // Ruta abs de archivo de prediccion generado por P2

	string rutaP3; // Ruta abs de Ejecutable3
	string PrediccionP3; // Ruta abs de archivo de prediccion procesado por P3
	
	string DatosProc; // Ruta abs de archivo de predicciones procesados
	string PrediccionProc; // Ruta abs de archivo de datos procesados

	// Metodos
	void leeDatos();
	bool proceso(string &rutaP1, string &rutaFich);
	string leeValor(string &valor);
	string editorValor(string &valor);
	void limpiaDatos();
public:
	string rutaConFecha(string &ruta);
	//thread* bucle();
	void bucle();
	//void comprimirTest(string & ruta, int minEs, string & fechaIni);
	void comprimir(string & ruta, int diasEs, string & fechaIni);
	//string sumaDiaFecha(string & fechaIni, int i);
	void multialgoritmo();
	ProcesoBucle(Registro* registro, Tiempo* tiempo);
	~ProcesoBucle();
};

#endif