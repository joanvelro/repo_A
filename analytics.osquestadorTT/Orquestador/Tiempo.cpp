#include "Tiempo.h"



Tiempo::Tiempo()
{
	this->count = 0;
}

void Tiempo::fechaHora()
{
	this->t = std::time(nullptr);
	this->tm = *std::localtime(&(this->t));
	this->oss.str("");
	this->oss << std::put_time(&(this->tm), "%d-%m-%Y %H-%M-%S");
	this->str = this->oss.str();
	this->fecha = static_cast<string>(this->str);
	this->fecha[10] = '_';
	this->fecha = this->fecha + "_";
	this->min = stoi(this->fecha.substr(14, 2));
	this->hora = stoi(this->fecha.substr(11, 2));
	this->seg = stoi(this->fecha.substr(17, 2));
	this->dia = stoi(this->fecha.substr(0, 2));
	this->mes = stoi(this->fecha.substr(3, 2));
	this->año = stoi(this->fecha.substr(6, 4));
	this->oss.str("");
}
/*
void Tiempo::fechaHoraIni()
{
	this->t = std::time(nullptr);
	this->tm = *std::localtime(&(this->t));
	this->oss.str("");
	this->oss << std::put_time(&(this->tm), "%d-%m-%Y %H-%M-%S");
	this->str = this->oss.str();
	this->fechaIni = (string)this->str;
	this->fechaIni[10] = '_';
	this->fechaIni = this->fechaIni + "_";
	this->minIni = stoi(this->fechaIni.substr(14, 2));
	this->horaIni = stoi(this->fechaIni.substr(11, 2));
	this->segIni = stoi(this->fechaIni.substr(17, 2));
	this->diaIni = stoi(this->fechaIni.substr(0, 2));
	this->mesIni = stoi(this->fechaIni.substr(3, 2));
	this->añoIni = stoi(this->fechaIni.substr(6, 4));
	this->oss.str("");
}
*/
// Temporizador. Termina de ejecutarse en un minuto multiplo de @minuto y 00 segundos
void Tiempo::ComienzoEnMinuto(int minuto)
{
	this->fechaHora();
	int seconds = (minuto - (this->min % minuto) - 1) * 60 * 1000 + (60 - this->seg - 1) * 1000;
	cout << seconds << endl;
	Sleep(seconds);
	this->fechaHora();
	while (((this->min % minuto) != 0) || (this->seg != 0))
	{
		Sleep(30);
		this->fechaHora();
	}
}

void Tiempo::ComienzoEnSegundo(int segundo)
{
	this->fechaHora();
	while (this->seg != segundo)
	{
		Sleep(30);
		this->fechaHora();
	}
}

int Tiempo::getHora()
{
	return this->hora;
}

int Tiempo::getMin()
{
	return this->min;
}

int Tiempo::getSeg()
{
	return this->seg;
}

int Tiempo::getDia()
{
	return this->dia;
}

int Tiempo::getMes()
{
	return this->mes;
}

int Tiempo::getAño()
{
	return this->año;
}

void Tiempo::temporizador(string minBucle)
{
	cout << "Iter siguiente: " << minBucle << endl;
	this->fechaHora();
	while (!((this->esMayorIgual(this->fecha, minBucle) != 0) && (this->seg == 0)))
	{
		Sleep(10);
		this->fechaHora();
	}
	this->contador();
}

bool Tiempo::haPasadoTiempo(int dias, Fecha* fechaIni)
{
	bool resultado;
	if (this->esMayorIgual(this->fecha, fechaIni->sumaDias(fechaIni->getFecha(), dias)) != 0)
		resultado = true;
	else
		resultado = false;
	/*
	if ((this->dia) <= dias)
	{
		if ((this->mesIni == 2))
		{
			int diasFeb;
			if (this->añoIni % 4 == 0)
				diasFeb = 29;
			else
				diasFeb = 28;
			resultado = ((diasFeb - (dias - this->dia)) == this->diaIni);
		}
		if ((this->mesIni == 4) || (this->mesIni == 6) || (this->mesIni == 9) || (this->mesIni == 11))/* 30 dias */
			/*resultado = ((30 - (dias - this->dia)) == this->diaIni);
		else /* 31 dias */
			/*resultado = ((31 - (dias - this->dia)) == this->diaIni);
	}
	else
		resultado = ((this->dia) - dias == this->diaIni);
	*/
	return resultado;
}

bool Tiempo::haPasadoTiempoTest(int mins, Fecha* fechaIni)
{
	bool resultado;
	cout << fechaIni->sumaMinutos(mins) << endl;
	if (this->esMayorIgual(this->fecha, fechaIni->sumaMinutos(mins)) != 0)
		resultado = true;
	else
		resultado = false;
	return resultado;
}


void Tiempo::inicioReloj()
{
	this->time_counter = 0;
	this->last_time = clock();
}

double Tiempo::getTimeCounter()
{
	this->time_counter = static_cast<double>(clock() - this->last_time);
	return this->time_counter;
}

void Tiempo::contador()
{
	this->count++;
}

string Tiempo::getFechaHora()
{
	return this->fecha;
}

/* Esta funcion devuelve 0 si fecha1<fecha2, 1 si fecha1>fecha2, 2 si fecha1==fecha2 */
int Tiempo::esMayorIgual(string &fecha1, string &fecha2)
{
	bool result;
	Fecha* Fecha1 = new Fecha(fecha1);
	Fecha* Fecha2 = new Fecha(fecha2);
	if (Fecha1->getAño() == Fecha2->getAño())
	{
		if (Fecha1->getMes() == Fecha2->getMes())
		{
			if (Fecha1->getDia() == Fecha2->getDia())
			{
				if (Fecha1->getHora() == Fecha2->getHora())
				{
					if (Fecha1->getMin() == Fecha2->getMin())
					{
						if (Fecha1->getSeg() == Fecha2->getSeg())
						{
							result = 2;
						}
						else
							result = (Fecha1->getSeg() > Fecha2->getSeg());
					}
					else
						result = (Fecha1->getMin() > Fecha2->getMin());
				}
				else
					result = (Fecha1->getHora() > Fecha2->getHora());
			}
			else
				result = (Fecha1->getDia() > Fecha2->getDia());
		}
		else
			result = (Fecha1->getMes() > Fecha2->getMes());
	}
	else
		result = (Fecha1->getAño() > Fecha2->getAño());
	delete Fecha1;
	delete Fecha2;
	return result;
}

Tiempo::~Tiempo()
{
}
