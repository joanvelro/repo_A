// Orquestador.cpp: define el punto de entrada de la aplicación de consola.
//

#include "Orquestador.h"
#pragma warning(disable : 4996)

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "Error: Tiene que insertar un argumento: AUSOL ó NORTE" << endl;
		cout << "Ejemplo:" << endl;
		cout << "Orquestador.exe NORTE" << endl;
		return 1;
	}
	//Recogemos la salida en estos ficheros
	freopen("stdout.txt", "w", stdout);
	freopen("stderr.txt", "w", stderr);
	// Abrimos la clave correspondiente del registro
	string ciudad = argv[1];
	string rutaRegistro = "SOFTWARE\\Wow6432Node\\Indra\\OrquestadorTTPAM\\" + ciudad;
	cout << "Leemos en la clave del registro: " << rutaRegistro << endl;
	Registro* registro = new Registro(rutaRegistro);
	if (!registro->existeClave()) // si no existe el programa se cierra
	{
		cout << "Error: No existe dicha clave" << endl;
		return 1;
	}
	Tiempo* tiempo = new Tiempo();
	registro->setSubclave(static_cast<string>("Periodos"));
	int periodo = stoi((registro->leeValor(static_cast<string>("GeneraDatos"))).c_str());
	tiempo->ComienzoEnMinuto(periodo/60);
	ProcesoBucle* bucle = new ProcesoBucle(registro, tiempo);

	bucle->bucle();

	delete bucle;
	delete tiempo;
	delete registro;
	fclose(stdout);
	fclose(stderr);
	return 0;
}

