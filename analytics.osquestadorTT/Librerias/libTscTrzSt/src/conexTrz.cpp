/////////////////////////////////////////////////////////////////////////////
// Fichero:           conexTrz.cpp
// Descripcion:       Clase para la conexion con el servidor de trazas
// Autor:             Miguel Romero
// Auditor:
// Fecha:             1/06/2001
// Modificaciones:
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

#include "iostream"

#include "conexTrz.h"
using namespace std;


#if defined _DEBUG
#ifdef DEBUG_NEW
#undef DEBUG_NEW
#endif
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CConexionTrazas::nThConexProc
// Descripcion:   Funcion para realizar la conexion con el servidor de trazas
//                en un thread diferente
// Parametros
//    Entrada:    pvdTrz: Puntero al propio objeto de conexion de trazas
// Salida:        Devuelve cero en cualquier caso
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TULong WINAPI CConexionTrazas::nThConexProc(void *pvdTrz)
{
	CConexionTrazas *pTrazas = (CConexionTrazas *)pvdTrz;

	HANDLE hSalir;

	// Bucle para el intento de conexion con el servidor de trazas
	// Si la conexion no se consigue, se reintenta cada 10 segundos
	// salvo que se termine el proceso
	while(1)
	{
		try
		{
			if (!pTrazas->nConexionOrb())
				break;
			if ((hSalir = pTrazas->GetHandleSalir()) != NULL)
				if (WaitForSingleObject (hSalir, 10000) == WAIT_OBJECT_0)
					break;
		}
		catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
		{
			cout << _T("CConexionTrazas::nThConexProc. ERROR EN TIMEOUT DE MUTEX ") << ex.what() << endl;
		}
		catch(TSC_MUTEX_EXCEPTION_CREATE &)
		{
			cout << _T("CConexionTrazas::nThConexProc: ERROR EN CREACION DE MUTEX") << endl;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CConexionTrazas::CConexionTrazas
// Descripcion:   Constructor para el propio servidor de trazas
// Parametros
//    Entrada:    cProcess: Nombre del servidor de trazas
// Salida:        
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

CConexionTrazas::CConexionTrazas(TTChar *cProcess)
{
	//m_TscTrzPtr = TscTraceORB::_nil(); 

	m_nFd             = 0;
	m_bORBConnected   = FALSE;
	m_hConexProc = NULL;

	_tcscpy ( m_cConexion, _T(":") );
	_tcscat ( m_cConexion, cProcess );

	m_hSalir = CreateEvent ( NULL, TRUE, FALSE, NULL );

	m_bRegistrarCallback = TRUE;

	// Se realiza la conexion con el servidor de trazas
	// Si la conexion no se consigue se lanza un thread que los intentara periodicamente
	m_bEsSrvTrz = FALSE;
	if (nConexionOrb())
		m_hConexProc = CreateThread(NULL, 0, nThConexProc, (void *)this, 0, &m_ulIdTh);

}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CConexionTrazas::CConexionTrazas
// Descripcion:   Contructor 2. Cuando es el propio servidor de trazas
// Parametros
//    Entrada:    pSrvTrz: Proxy
// Salida:        
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

CConexionTrazas::CConexionTrazas(TscTraceORB_i *pSrvTrz)
{
	//m_TscTrzPtr = pSrvTrz; 

	m_nFd             = 0;
	m_bORBConnected   = TRUE;
	m_hConexProc = NULL;

	m_hSalir = CreateEvent ( NULL, TRUE, FALSE, NULL );

	m_bRegistrarCallback = TRUE;

	m_bEsSrvTrz = TRUE;

}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CConexionTrazas::~CConexionTrazas
// Descripcion:   Destructor
// Parametros
// Salida:        
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

CConexionTrazas::~CConexionTrazas()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CConexionTrazas::~CConexionTrazas", TIMEOUT_SINCRO_MUTEX);	
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CConexionTrazas::~CConexionTrazas: ERROR EN TIMEOUT DE MUTEX. ") << ex.what() << endl;
	}

	SetEvent(m_hSalir);

	// Se espera un maximo de 500 milisegundos que termine el thread de conexion
	WaitForSingleObject(m_hConexProc, 500);

	CloseHandle(m_hSalir);
	if (m_hConexProc)
		CloseHandle(m_hConexProc);
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CConexionTrazas::nConexionOrb
// Descripcion:   Establece la conexion con el servidor de trazas
// Parametros
// Salida:        Devuelve cero si la conexion se realiza correctamente.
//                En caso contrario devuelve 1.
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CConexionTrazas::nConexionOrb ( )
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CConexionTrazas::OrbixFDClose
// Descripcion:   Funcion que se activa cuando se detecta una perdida de comunicacion
//                con un servidor orbix
// Parametros
//    Entrada:    nfd: FD de la conexion con el servidor
// Salida:        
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

void CConexionTrazas::OrbixFDClose(TInt32 nfd)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CConexionTrazas::OrbixFDClose", TIMEOUT_SINCRO_MUTEX);	
		if (nfd != m_nFd)
			return;

		m_nFd = 0;
		m_bORBConnected = FALSE;

		cout << _T("CConexionTrazas::OrbixFDClose PERDIDA DE COMUNICACION FD = ") << nfd << endl;

		// Se lanza el thread de conexion para establecer de nuevo la conexion con el servidor de trazas
		if (WaitForSingleObject(m_hConexProc, 1500) != WAIT_TIMEOUT)
			m_hConexProc = CreateThread(NULL, 0, nThConexProc, (void *)this, 0, &m_ulIdTh);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CConexionTrazas::nConexionOrb: ERROR EN TIMEOUT DE MUTEX. ") << ex.what() << endl;
		return;
	}

}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CConexionTrazas::GetHandleSalir
// Descripcion:   Devuelve el handle de salida
// Parametros
//    Entrada:    
// Salida:        Devuelve el handle
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

const HANDLE CConexionTrazas::GetHandleSalir()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CConexionTrazas::GetHandleSalir", TIMEOUT_SINCRO_MUTEX);	
		return m_hSalir;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CConexionTrazas::nConexionOrb: ERROR EN TIMEOUT DE MUTEX. ") << ex.what() << endl;
		return NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CConexionTrazas::vdTraceORB
// Descripcion:   Envia al servidor de trazas la traza correspondiente
// Parametros
//    Entrada:    cTexto: Mensaje a tracear
//                bToScreen: Indica si el mensaje se debe mostrar en pantalla
//                bToFile: Indica si el mensaje se debe mostrar en fichero
// Salida:
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

void CConexionTrazas::vdTraceORB(TTChar *cTexto, TBool bToScreen, TBool bToFile)
{
	try
	{
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CConexionTrazas::nConexionOrb: ERROR EN TIMEOUT DE MUTEX. ") << ex.what() << endl;
	}
}

