#ifndef Tiempo_h
#define Tiempo_h

#pragma warning(disable : 4996)

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <string.h>
#include "Windows.h"
#include "Fecha.h"
//#include <stdio.h>

using namespace std;

class Tiempo
{
private:
	// Para la fecha y Hora
	time_t t;
	tm tm;
	std::ostringstream oss;
	string str;
	string fecha;
	int dia;
	int mes;
	int año;
	int hora;
	int min;
	int seg;
	/*
	string fechaIni;
	int diaIni;
	int mesIni;
	int añoIni;
	int horaIni;
	int minIni;
	int segIni;
	*/
	// Para el temporizador
	int count;
	double time_counter;
	clock_t last_time;
public:
	bool haPasadoTiempo(int dias, Fecha* fechaIni);
	bool haPasadoTiempoTest(int mins, Fecha* fechaIni);
	int getDia();
	int getMes();
	int getAño();
	void temporizador(string minBucle);
	int getMin();
	int getSeg();
	int getHora();
	void ComienzoEnMinuto(int minuto);
	void ComienzoEnSegundo(int segundo);
	void inicioReloj();
	void contador();
	double getTimeCounter();
	string getFechaHora();
	string getFechaHoraIni();
	void fechaHora();
	void fechaHoraIni();
	Tiempo();
	int esMayorIgual(string & fecha1, string & fecha2);
	~Tiempo();
};

#endif