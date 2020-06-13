#include "Fecha.h"

Fecha::Fecha(string &fecha)
{
	this->fecha = fecha;
	this->minuto = stoi(this->fecha.substr(14, 2));
	this->hora = stoi(this->fecha.substr(11, 2));
	this->segundo = stoi(this->fecha.substr(17, 2));
	this->dia = stoi(this->fecha.substr(0, 2));
	this->mes = stoi(this->fecha.substr(3, 2));
	this->a�o = stoi(this->fecha.substr(6, 4));
}

void Fecha::setFecha(string &fecha)
{
	this->fecha = fecha;
	this->minuto = stoi(this->fecha.substr(14, 2));
	this->hora = stoi(this->fecha.substr(11, 2));
	this->segundo = stoi(this->fecha.substr(17, 2));
	this->dia = stoi(this->fecha.substr(0, 2));
	this->mes = stoi(this->fecha.substr(3, 2));
	this->a�o = stoi(this->fecha.substr(6, 4));
}

string Fecha::construyeFecha(int dia, int mes, int a�o, int hora, int minuto, int segundo)
{
	return (this->intToString(dia) + "-" + this->intToString(mes) + "-" + this->intToString(a�o) + "_" +
		this->intToString(hora) + "-" + this->intToString(minuto) + "-" + this->intToString(segundo) + "_");
}

string Fecha::intToString(int entero)
{
	if (entero < 10)
		return ("0" + to_string(entero));
	else
		return to_string(entero);
}

string Fecha::sumaMinutos(int minSum)
{
	string fechaResul;
	int nuevoMin = minSum + this->minuto;
	if (nuevoMin >= 60)
	{
		int horaSum = (int)(nuevoMin / 60);
		nuevoMin = (nuevoMin % 60);
		int nuevaHora = this->hora + horaSum;
		if (nuevaHora >= 24)
		{
			int diaSum = (int)(nuevaHora / 24);
			nuevaHora = (nuevaHora % 24);
			string fechaAux = sumaDias(this->fecha, diaSum);
			int nuevoDia = stoi(fechaAux.substr(0, 2));
			int nuevoMes = stoi(fechaAux.substr(3, 2));
			int nuevoA�o = stoi(fechaAux.substr(6, 4));
			fechaResul = this->construyeFecha(nuevoDia, nuevoMes, nuevoA�o, nuevaHora, nuevoMin, this->segundo);
		}
		else
			fechaResul = this->construyeFecha(this->dia, this->mes, this->a�o, nuevaHora, nuevoMin, this->segundo);
	}
	else
	{
		fechaResul = this->construyeFecha(this->dia, this->mes, this->a�o, this->hora, nuevoMin, this->segundo);
	}
	return fechaResul;
}

string Fecha::sumaDias(string &fechaIni, int dias)
{
	int dia = stoi(fechaIni.substr(0, 2)) + dias;
	int mes = stoi(fechaIni.substr(3, 2));
	int a�o = stoi(fechaIni.substr(6, 4));
	if ((mes == 2) && (dia > 28))
	{
		if (((a�o % 4 == 0) && (a�o % 100 != 0)) || (a�o % 400 == 0))
		{
			if (dia > 29)
			{
				dia = dia - 29;
				mes++;
			}
		}
		else
		{
			dia = dia - 28;
			mes++;
		}
	}
	if (((mes == 4) || (mes == 6) || (mes == 9) || (mes == 11)) && (dia > 30))
	{
		dia = dia - 30;
		mes++;
	}
	if (((mes == 1) || (mes == 3) || (mes == 5) || (mes == 7) || (mes == 8) || (mes == 10) || (mes == 12)) && (dia > 31))
	{
		dia = dia - 31;
		if (mes == 12)
		{
			mes = 1;
			a�o++;
		}
		else
			mes++;
	}
	string cadDia = this->intToString(dia);
	string cadMes = this->intToString(mes);
	string cadA�o = to_string(a�o);
	string diaComp = fechaIni;
	diaComp[0] = cadDia[0];
	diaComp[1] = cadDia[1];
	diaComp[3] = cadMes[0];
	diaComp[4] = cadMes[1];
	diaComp[6] = cadA�o[0];
	diaComp[7] = cadA�o[1];
	diaComp[8] = cadA�o[2];
	diaComp[9] = cadA�o[3];
	return diaComp;
}


int Fecha::getMin()
{
	return this->minuto;
}
int Fecha::getHora()
{
	return this->hora;
}
int Fecha::getDia()
{
	return this->dia;
}
int Fecha::getSeg()
{
	return this->segundo;
}
int Fecha::getA�o()
{
	return this->a�o;
}
int Fecha::getMes()
{
	return this->mes;
}
string Fecha::getFecha()
{
	return this->fecha;
}

Fecha::~Fecha()
{
}
