#include "Registro.h"



Registro::Registro(string &rutaKeyRegistro):rutaRegistro(rutaKeyRegistro)
{
	this->subclave = "";
}

void Registro::setSubclave(string &subClave)
{
	this->subclave = subClave;
}

bool Registro::existeClave()
{
	std::wstring widestr = std::wstring(this->rutaRegistro.begin(), this->rutaRegistro.end());
	const wchar_t* szName = (widestr).c_str();
	LPCTSTR lpSubKey = szName;
	HKEY hkey;
	LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hkey);
	bool existe;
	if (result == ERROR_SUCCESS)
		existe = true;
	else
		existe = false;
	if (result == ERROR_FILE_NOT_FOUND)
		cout << "No existe fichero" << endl;
	if (result == ERROR_ACCESS_DENIED)
		cout << "No hay acceso" << endl;
	RegCloseKey(hkey);
	return existe;
}

string Registro::leeValor(string &valueName)
{
	//Abrimos la clave
	string ruta = this->rutaRegistro;
	if (this->subclave != "")
		ruta = this->rutaRegistro + "\\" + this->subclave;
	std::wstring widestr = std::wstring(ruta.begin(), ruta.end());
	const wchar_t* szName = (widestr).c_str();
	LPCTSTR lpSubKey = szName;
	HKEY hkey;
	LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hkey);
	//Leemos el valor llamado valueName
	std::wstring widestrValue = std::wstring(valueName.begin(), valueName.end());
	const wchar_t* szNameValue = (widestrValue).c_str();
	LPCTSTR value = szNameValue;
	DWORD BufferSize = 800;
	DWORD tipoDato = REG_SZ;
	string valor;
	char* dato = new char[BufferSize];
	LONG queri = RegQueryValueEx(hkey, value, NULL, &tipoDato, (BYTE *)dato, &BufferSize);
	if (queri == ERROR_FILE_NOT_FOUND)
		cout << "No existe el valor" << endl;
	if (queri == ERROR_ACCESS_DENIED)
		cout << "Acceso denegado" << endl;
	if (queri == ERROR_MORE_DATA)
		cout << "Necesita mas espacio de datos" << endl;
	if (queri == ERROR_SUCCESS)
	{
		cout << "Se ha leido el valor correctamente" << endl;
		int tope;
		if (BufferSize % 2 == 0)
			tope = BufferSize / 2;
		else
			tope = (BufferSize - 1) / 2;
		for (int i = 0; i < tope; i++)
			valor = valor + dato[2 * i];
	}
	RegCloseKey(hkey);
	return valor;
}

Registro::~Registro()
{
}
