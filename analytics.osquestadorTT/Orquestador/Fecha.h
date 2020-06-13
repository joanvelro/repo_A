#ifndef Fecha_h
#define Fecha_h

#include <sstream>
using namespace std;

class Fecha
{
private:
	int dia, mes, a�o, hora, minuto, segundo;
	string fecha;
public:
	Fecha(string &fecha);
	void setFecha(string & fecha);
	string construyeFecha(int dia, int mes, int a�o, int hora, int minuto, int segundo);
	string intToString(int entero);
	string sumaMinutos(int minSum);
	string sumaDias(string & fechaIni, int dias);
	int getMin();
	int getHora();
	int getDia();
	int getSeg();
	int getA�o();
	int getMes();
	string getFecha();
	~Fecha();
};
#endif


