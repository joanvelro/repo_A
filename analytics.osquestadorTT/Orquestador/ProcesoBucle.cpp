#include "ProcesoBucle.h"



ProcesoBucle::ProcesoBucle(Registro* reg, Tiempo* time) :registro(reg), tiempo(time)
{
	this->comprime = true; 
	this->hayHilo = false; 
	this->minBucle = new Fecha(this->tiempo->getFechaHora()); 
	this->fechaComp = new Fecha(this->tiempo->getFechaHora()); 
	this->firstIter = true;
}

void ProcesoBucle::bucle()
{
	while (true)
	{
		if (this->comprime == true)
		{
			this->lanzaHilo = clock();
		}

		// Tomamos la fecha y hora de la iteración
		this->fecha = this->tiempo->getFechaHora();

		cout << "Toma de datos: " << this->fecha << endl;
		cout << "Fecha Iteracion: " << this->minBucle->getFecha() << endl;

		// Tomamos los datos del registro
		this->leeDatos();

		if ((this->hayHilo == true) && ((clock() - this->lanzaHilo) >= (500 * this->comprimeCada * CLOCKS_PER_SEC)))
		{
			this->hiloDat.join();
			this->hiloPred.join();
			this->hayHilo = false;
		}

		// Llamamos al proceso 1: Toma de datos
		this->proceso(this->rutaP1, this->DatosP1);
		BorraMueve* datosP2 = new BorraMueve(this->DatosP2);
		datosP2->borra();
		delete datosP2;
		BorraMueve* datosP1P2 = new BorraMueve(this->DatosP1);
		bool existe = datosP1P2->mueve(this->DatosP2);
		delete datosP1P2;
		if (existe)
		{
			// Llamamos al proceso multialgoritmo
			if (firstIter) {
				//this->multAlg = thread(&ProcesoBucle::multialgoritmo,this);
				multialgoritmo();
			}
			// Llamamos al proceso 2: Algoritmo de Predicciones
			/*
			this->proceso(this->rutaP2, this->PrediccionP2);
			BorraMueve* prediccionP3 = new BorraMueve(this->PrediccionP3);
			prediccionP3->borra();
			delete prediccionP3;
			BorraMueve* prediccionP2P3 = new BorraMueve(this->PrediccionP2);
			existe = prediccionP2P3->mueve(this->PrediccionP3);
			delete prediccionP2P3;
			*/
			// Comprobamos que se genere fichero de predicciones
			bool file_exist = false;
			Existencia* se_ha_generado = new Existencia(this->PrediccionP2);
			while (!file_exist) {
				file_exist = se_ha_generado->existe();
				Sleep(1000);
			}
			delete se_ha_generado;
			BorraMueve* p2_p3 = new BorraMueve(this->PrediccionP2);
			existe = p2_p3->mueve(this->PrediccionP3);

			if (existe){
				// Llamamos al proceso 3: Escritura de prediccion en base de datos
				this->proceso(this->rutaP3, this->PrediccionP3);
				Sleep(15000);
				BorraMueve* archivoPredic = new BorraMueve(this->PrediccionP3);
				//Aqui depende de la excepcion
				archivoPredic->mueve(this->rutaConFecha(this->PrediccionProc));
				delete archivoPredic;
			}
			//Movemos el archivo de datos a procesados
			BorraMueve* archivoDatos = new BorraMueve(this->DatosP2);
			archivoDatos->mueve(this->rutaConFecha(this->DatosProc));
			delete archivoDatos;
		}
		this->comprime = this->tiempo->haPasadoTiempo(2 * this->comprimeCada, this->fechaComp);
		if (this->comprime)
		{
			this->hiloDat = thread(&ProcesoBucle::comprimir,
				this,
				this->DatosProc,
				this->comprimeCada,
				this->fechaComp->getFecha());
			this->hiloPred = thread(&ProcesoBucle::comprimir,
				this,
				this->PrediccionProc,
				this->comprimeCada,
				this->fechaComp->getFecha());

			this->hayHilo = true;
			this->fechaComp->setFecha(this->fechaComp->sumaDias(this->fechaComp->getFecha(), this->comprimeCada));
		}
		this->limpiaDatos();
		int periodoMin = this->periodo / 60;
		this->minBucle->setFecha(this->minBucle->sumaMinutos(periodoMin));
		//this->fechaComp->setFecha(this->fechaComp->sumaMinutos(this->comprimeCada));
		this->tiempo->temporizador(this->minBucle->getFecha());
		this->tiempo->fechaHora();
		firstIter = false;
	}
}
/*
void ProcesoBucle::comprimirTest(string & ruta, int minEs, string & fechaIni)
{
	Fecha* test = new Fecha(fechaIni);
	clock_t inicio = clock();
	string diaComp;
	for (int i = 0; i < minEs; i++)
	{
		size_t pos = ruta.find_last_of("\\");
		diaComp = test->sumaMinutos(i);
		string directorio = ruta.substr(0, pos + 1) + diaComp.substr(0, 16) + "*";
		string rutaArchivoComp = ruta.substr(0, pos + 1) + "Minuto_" + fechaIni.substr(0, 16) + ".zip";
		string lineaComandos = "7z " + string("a ") + rutaArchivoComp + " " + directorio + " -sdel";
		Llamada * compZip = new Llamada(lineaComandos);
		compZip->ejecuta();
		delete compZip;
	}
	cout << "La compresion ha tardado = " << (clock() - inicio) << endl;
	delete test;
}
*/
void ProcesoBucle::comprimir(string &ruta, int diasEs, string &fechaIni)
{
	string diaComp;
	Fecha* fechaInicio = new Fecha(fechaIni);
	clock_t inicio = clock();
	for (int i = 0; i < diasEs; i++)
	{
		size_t pos = ruta.find_last_of("\\");
		diaComp = fechaInicio->sumaDias(fechaIni, i);
		string directorio = ruta.substr(0, pos + 1) + diaComp.substr(0, 10) + "*";
		string rutaArchivoComp = ruta.substr(0, pos + 1) + "Semana_" + fechaIni.substr(0, 10) + ".zip";
		string lineaComandos = "7z " + string("a ") + rutaArchivoComp + " " + directorio + " -sdel";
		Llamada * compZip = new Llamada(lineaComandos);
		compZip->ejecuta();
		delete compZip;
	}
	cout << "La compresion ha tardado = " << (clock() - inicio) << endl;
}

void ProcesoBucle::multialgoritmo() {
	//string rutaMultialg = "main.py";
	string rutaMultialg = this->rutaP2;
	string configuracion = this->confP2;
	string ruta = "python " + rutaMultialg + " -c " + configuracion;
	Llamada * multialg = new Llamada(ruta);
	multialg->dejaEnPie();
	delete multialg;
}

string ProcesoBucle::rutaConFecha(string &ruta)
{
	size_t pos = ruta.find_last_of("\\");
	string nombre = ruta.substr(pos + 1);
	string directorio = ruta.substr(0, pos);
	return (directorio + this->fecha + nombre);
}

bool ProcesoBucle::proceso(string &rutaExe, string &rutaFich)
{
	Llamada* call = new Llamada(rutaExe);
	call->ejecuta();
	delete call;
	Existencia* existencia = new Existencia(rutaFich);
	bool existe = existencia->existe();
	delete existencia;
	return existe;
}

void ProcesoBucle::leeDatos()
{
	//Periodos
	this->registro->setSubclave(static_cast<string>("Periodos"));
	this->comprimeCada = stoi((this->registro->leeValor(static_cast<string>("Comprimir"))).c_str());
	this->periodo = stoi((this->registro->leeValor(static_cast<string>("GeneraDatos"))).c_str());
	//Proceso1
	this->registro->setSubclave(static_cast<string>("P1"));
	this->rutaP1 = this->leeValor(static_cast<string>("Ejecutable"));
	this->DatosP1 = this->leeValor(static_cast<string>("Fichero"));
	//Proceso2
	this->registro->setSubclave(static_cast<string>("P2"));
	this->rutaP2 = this->leeValor(static_cast<string>("Ejecutable"));
	this->confP2 = this->leeValor(static_cast<string>("Configuracion"));
	this->DatosP2 = this->leeValor(static_cast<string>("Entrada"));
	this->PrediccionP2 = this->leeValor(static_cast<string>("Salida"));
	//Proceso3
	this->registro->setSubclave(static_cast<string>("P3"));
	this->rutaP3 = this->leeValor(static_cast<string>("Ejecutable"));
	this->PrediccionP3 = this->leeValor(static_cast<string>("Fichero"));
	//Procesamiento
	this->registro->setSubclave(static_cast<string>("Procesados"));
	this->DatosProc = this->leeValor(static_cast<string>("Datos"));
	this->PrediccionProc = this->leeValor(static_cast<string>("Predicciones"));
	this->registro->setSubclave(static_cast<string>(""));
}

void ProcesoBucle::limpiaDatos()
{
	this->rutaP1.clear();
	this->DatosP1.clear();
	this->rutaP2.clear();
	this->DatosP2.clear();
	this->PrediccionP2.clear();
	this->rutaP3.clear();
	this->PrediccionP3.clear();
	this->DatosProc.clear();
	this->PrediccionProc.clear();
}

string ProcesoBucle::leeValor(string &valor)
{
	return this->editorValor(this->registro->leeValor(valor));
}

string ProcesoBucle::editorValor(string &valor)
{
	return valor.erase(valor.length() - 1);
}

ProcesoBucle::~ProcesoBucle()
{
	//this->multAlg.join();
	delete minBucle;
}