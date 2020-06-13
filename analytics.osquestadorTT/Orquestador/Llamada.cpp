#include "Llamada.h"



Llamada::Llamada(string rutaEjecutable)
{
	this->ruta = rutaEjecutable;
}

void Llamada::dejaEnPie()
{

	// Generamos un fichero .log del proceso
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	string nombreOut = this->ruta + "_out.log";
	std::wstring nombreOut2 = std::wstring(nombreOut.begin(), nombreOut.end());
	LPCWSTR nameOut = nombreOut2.c_str();

	//HANDLE h = CreateFile(_T("out.log"),
	HANDLE h = CreateFile(nameOut,
		FILE_APPEND_DATA,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		&sa,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// Levantamos el proceso con CreateProcess
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL ret = FALSE;
	//DWORD flags = CREATE_NO_WINDOW;

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.hStdInput = NULL;
	si.hStdError = h;
	si.hStdOutput = h;

	cout << this->ruta << endl;
	std::wstring rutaCompletatemp = std::wstring(this->ruta.begin(), this->ruta.end());
	LPCWSTR rutaCompletaSw = rutaCompletatemp.c_str();
	if (CreateProcess(NULL,
		const_cast<LPWSTR>(rutaCompletaSw),
		NULL,
		NULL,
		TRUE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi))
	{
		//WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		DWORD errorCode = GetLastError();
		DisplayError(TEXT("No se pudo ejecutar."), errorCode);
	}

	//cout << this->ruta << endl;
	////WinExec(rutaCompleta.c_str(), SW_HIDE);
	//PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter
	//STARTUPINFO StartupInfo; //This is an [in] parameter
	//ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	//StartupInfo.cb = sizeof StartupInfo; //Only compulsory field
	//std::wstring rutaCompletatemp = std::wstring(this->ruta.begin(), this->ruta.end());
	//LPCWSTR rutaCompletaSw = rutaCompletatemp.c_str();
	//if (!CreateProcess(NULL,
	//	const_cast<LPWSTR>(rutaCompletaSw),
	//	NULL,
	//	NULL,
	//	TRUE,
	//	CREATE_NEW_CONSOLE,
	//	NULL,
	//	NULL,
	//	&StartupInfo,
	//	&ProcessInfo))
	//{
	//	DWORD errorCode = GetLastError();
	//	DisplayError(TEXT("No se pudo ejecutar."), errorCode);
	//}
	//WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
}

void Llamada::ejecuta()
{

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	
	string nombreOut = this->ruta + "_out.log";
	std::wstring nombreOut2 = std::wstring(nombreOut.begin(), nombreOut.end());
	LPCWSTR nameOut = nombreOut2.c_str();

	//HANDLE h = CreateFile(_T("out.log"),
	HANDLE h = CreateFile(nameOut,
		FILE_APPEND_DATA,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		&sa,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL ret = FALSE;
	//DWORD flags = CREATE_NO_WINDOW;

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.hStdInput = NULL;
	si.hStdError = h;
	si.hStdOutput = h;

	cout << this->ruta << endl;
	std::wstring rutaCompletatemp = std::wstring(this->ruta.begin(), this->ruta.end());
	LPCWSTR rutaCompletaSw = rutaCompletatemp.c_str();
	if (CreateProcess(NULL,
		const_cast<LPWSTR>(rutaCompletaSw),
		NULL,
		NULL,
		TRUE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi))
	{
		WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else 
	{
		DWORD errorCode = GetLastError();
		DisplayError(TEXT("No se pudo ejecutar."), errorCode);
	}
	
	//cout << this->ruta << endl;
	////WinExec(rutaCompleta.c_str(), SW_HIDE);
	//PROCESS_INFORMATION ProcessInfo; //This is what we get as an [out] parameter
	//STARTUPINFO StartupInfo; //This is an [in] parameter
	//ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	//StartupInfo.cb = sizeof StartupInfo; //Only compulsory field
	//std::wstring rutaCompletatemp = std::wstring(this->ruta.begin(), this->ruta.end());
	//LPCWSTR rutaCompletaSw = rutaCompletatemp.c_str();
	//if (!CreateProcess(NULL,
	//	const_cast<LPWSTR>(rutaCompletaSw),
	//	NULL,
	//	NULL,
	//	TRUE,
	//	CREATE_NEW_CONSOLE,
	//	NULL,
	//	NULL,
	//	&StartupInfo,
	//	&ProcessInfo))
	//{
	//	DWORD errorCode = GetLastError();
	//	DisplayError(TEXT("No se pudo ejecutar."), errorCode);
	//}
	//WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
}

void Llamada::DisplayError(LPCTSTR errorDesc, DWORD errorCode)
{
	TCHAR errorMessage[1024] = TEXT("");

	DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS
		| FORMAT_MESSAGE_MAX_WIDTH_MASK;

	FormatMessage(flags,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		errorMessage,
		sizeof(errorMessage) / sizeof(TCHAR),
		NULL);

#ifdef _UNICODE
	wcerr << L"Error : " << errorDesc << endl;
	wcerr << L"Code    = " << errorCode << endl;
	wcerr << L"Message = " << errorMessage << endl;
#else
	cerr << "Error : " << errorDesc << endl;
	cerr << "Code    = " << errorCode << endl;
	cerr << "Message = " << errorMessage << endl;
#endif
}

Llamada::~Llamada()
{
}
