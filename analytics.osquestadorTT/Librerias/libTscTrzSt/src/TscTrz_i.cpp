/////////////////////////////////////////////////////////////////////////////
// Fichero:           TscTraceIm.cpp
// Descripcion:       Clase que implementa el servidor de trazas
// Autor:             Grupo desarrollo trafico
// Fecha de creación: 1/06/2001
// 
// Revisiones:
// Fecha		Nombre              Revision
/////////////////////////////////////////////////////////////////////////////

#include "iostream"
#include "TscTrace.h"
#include "TscTrz_i.h"
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


TscTraceORB_i::TscTraceORB_i(TCHAR *cFichero)
{
	_tcscpy(m_cFichero, cFichero);

	m_bTraceFileOpen = FALSE;
	m_hTraceFile = NULL;
	m_TipoFile = 0;
}

TscTraceORB_i::~TscTraceORB_i()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "TscTraceORB_i::~TscTraceORB_i", TIMEOUT_SINCRO_MUTEX);	
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &)
	{
		cout << "TscTraceORB_i::~TscTraceORB_i: ERROR EN TIMEOUT DE MUTEX." << endl;
	}
	CloseTraceFile();
}

int TscTraceORB_i::OpenTraceFile ()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "TscTraceORB_i::OpenTraceFile", TIMEOUT_SINCRO_MUTEX);	
		if ( !m_bTraceFileOpen )
		{
			m_hTraceFile = CreateFile (m_cFichero, 
				GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL, 0 );

			if ( m_hTraceFile != INVALID_HANDLE_VALUE )
			{
				SetFilePointer (m_hTraceFile, 0, NULL, FILE_END );
				m_bTraceFileOpen = TRUE;
			}
			else
			{
				m_bTraceFileOpen = FALSE;
				return 1;
			}
		}
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("TscTraceORB_i::OpenTraceFile: ERROR EN TIMEOUT DE MUTEX ") << ex.what() << endl;
		return 1;
	}

	return 0;
};

int TscTraceORB_i::CloseTraceFile()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "TscTraceORB_i::CloseTraceFile", TIMEOUT_SINCRO_MUTEX);	
		if ( m_bTraceFileOpen )
		{
			if (CloseHandle ( m_hTraceFile ) != TRUE)
				return 1;
			m_bTraceFileOpen = FALSE;
		}
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("TscTraceORB_i::CloseTraceFile: ERROR EN TIMEOUT DE MUTEX ") << ex.what() << endl ;
		return 1;
	}

	return 0;
};

int TscTraceORB_i::CambiarFicheroTrazas()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "TscTraceORB_i::CambiarFicheroTrazas", TIMEOUT_SINCRO_MUTEX);	
		TCHAR cFileOld[TRAZA_MAX_DATA];
		_stprintf_s(cFileOld,TRAZA_MAX_DATA, _T("%s.old"), m_cFichero);
		if (CloseTraceFile())
			cout << _T("Error al cerrar el fichero ") << m_cFichero << endl;
		if (_tremove(cFileOld))
			cout << _T("Error al borrar el fichero ") << cFileOld << endl;
		if (_trename(m_cFichero, cFileOld))
			cout << _T("Error al renombrar el fichero ") << cFileOld << endl;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("TscTraceORB_i::CambiarFicheroTrazas: ERROR EN TIMEOUT DE MUTEX ") << ex.what() << endl;
		return 1;
	}

	return 0;
};

TInt32 TscTraceORB_i::nSetTipoFile (TInt32 nTipo)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "TscTraceORB_i::nSetTipoFile", TIMEOUT_SINCRO_MUTEX);	
		m_TipoFile = nTipo;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "TscTraceORB_i::nSetTipoFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}	
