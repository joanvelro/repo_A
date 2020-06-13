/////////////////////////////////////////////////////////////////////////////
// Fichero:           TscTraceUtil.cpp
// Descripcion:       Clase estatica para facilitar el uso de la libreria.
//                    Simplemente es una capa que utiliza un objeto trazas
//                    y su mision es hacer accesible a todo el ambito de la
//                    aplicacion la funcionalidad de trazas.
// Autor:             Miguel Romero
// Auditor:
// Fecha:             1/06/2001
// Modificaciones:
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

#include <io.h>
#include <stdio.h>
#include <iostream>

#include "TscTraceUtil.h"
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

// Objetos estaticos
CTscTrace CTscTraceUtil::m_trzUtil;
TSC_MUTEX CTscTraceUtil::m_Mutex;

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nInitSrv
// Descripcion:   Establece la conexion con el servidor de trazas
// Parametros
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nInitSrv()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nInitSrv", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nInitSrv();
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nInitSrv(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nInitSrv
// Descripcion:   Establece la conexion con el propio servidor de trazas
// Parametros
//    Entrada:    pSrvTrz: Objeto de trazas
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nInitSrv(TscTraceORB_i *pSrvTrz)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nInitSrv (TscTraceORB_i *)", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nInitSrv(pSrvTrz);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nInitSrv(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nEndSrv
// Descripcion:   Finaliza la conexion con el servidor de trazas
// Parametros
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nEndSrv()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::EndSrv", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nEndSrv();
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::EndSrv(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nSetTraceFile
// Descripcion:   Establece el nombre del fichero de trazas
// Parametros
//    Entrada:    cTraceFile: Nombre del fichero de trazas
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nSetTraceFile ( TTChar *cTraceFile)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nSetTraceFile", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nSetTraceFile(cTraceFile);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nSetTraceFile(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nGetTraceFile
// Descripcion:   Obtiene el nombre del fichero de trazas
// Parametros
//    Salida:     cTraceFile: Nombre del fichero de trazas
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nGetTraceFile ( TTChar *cTraceFile)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nGetTraceFile", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nGetTraceFile(cTraceFile);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nGetTraceFile(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nSetProcessName
// Descripcion:   Establece el nombre del proceso que genera las trazas
// Parametros
//    Entrada:    cNombreProceso: Nombre del proceso
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nSetProcessName(TTChar *cNombreProceso)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nSetProcessName", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nSetProcessName(cNombreProceso);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nSetProcessName(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nGetProcessName
// Descripcion:   Obtiene el nombre del proceso que genera las trazas
// Parametros
//    Salida:     cNombreProceso: Nombre del proceso
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nGetProcessName(TTChar *cNombreProceso)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nGetProcessName", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nGetProcessName(cNombreProceso);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nGetProcessName(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nSetTraceToFile
// Descripcion:   Establece si se envian o no las trazas a fichero
// Parametros
//    Entrada:    bTraceToFile: Condicion de envio o no envio
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nSetTraceToFile (TBool bTraceToFile)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nSetTraceToFile", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nSetTraceToFile(bTraceToFile);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nSetTraceToFile(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nGetTraceToFile
// Descripcion:   Devuelve si se envian o no las trazas a fichero
// Parametros
//    Salida:     bEnable: Condicion
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nGetTraceToFile(TBool& bEnable)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nGetTraceToFile", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nGetTraceToFile(bEnable);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nGetTraceToFile(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return FALSE;
	}

	return 0;
};


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nSetTraceToScreen
// Descripcion:   Establece si se envian las trazas a pantalla
//    Entrada:    bTraceTScreen: Condicion
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nSetTraceToScreen(TBool bTraceTScreen)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nSetTraceToScreen", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nSetTraceToScreen(bTraceTScreen);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nSetTraceToScreen(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nGetTraceToScreen
// Descripcion:   Devuelve si se envian o no las trazas a pantalla
// Parametros
//    Salida:     bEnableTrace: CondicionMensaje a tracear
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nGetTraceToScreen(TBool& bEnableTrace)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nGetTraceToScreen", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nGetTraceToScreen(bEnableTrace);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nGetTraceToScreen(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nSetLevelTraceFile
// Descripcion:   Establece el nivel de trazas a mostrar en fichero
// Parametros
//    Entrada:    nNivel: Nivel minimo cuyas trazas se mostraran
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nSetLevelTraceFile(TInt32 nNivel)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nSetLevelTraceFile", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nSetLevelTraceFile(nNivel);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nSetLevelTraceFile(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nSetLevelTraceScreen
// Descripcion:   Establece el nivel de trazas a mostrar en pantalla
// Parametros
//    Entrada:    nNivel: Nivel minimo cuyas trazas se mostrarán
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nSetLevelTraceScreen(TInt32 nNivel)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nSetLevelTraceScreen", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nSetLevelTraceScreen(nNivel);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nSetLevelTraceScreen(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nGetLevelTraceScreen
// Descripcion:   Devuelve el nivel de trazas a mostrar en pantalla
// Parametros
//    Salida:     nNivel: Nivel minimo que se mostrara
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nGetLevelTraceScreen(TInt32& nNivel)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nGetLevelTraceScreen", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nGetLevelTraceScreen(nNivel);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nGetLevelTraceScreen(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
};
	
/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nGetLevelTraceFile
// Descripcion:   Devuelve el nivel de trazas a mostrar en fichero
// Parametros
//    Entrada:    nNivel: Nivel minimo que se mostrara
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nGetLevelTraceFile(TInt32& nNivel)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nGetLevelTraceFile", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nGetLevelTraceFile(nNivel);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nGetLevelTraceFile(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nTrace
// Descripcion:   Muestra una traza si aplica
// Parametros
//    Entrada:    nNivel: Nivel de la traza
//                cFormato: Texto y formato de la traza
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nTrace(TInt32 nNivel, TTChar *i_cFormato, ...)
{
	TInt32 nEstado = 0;
	try
	{
		if (m_trzUtil.nComprobarNivel(nNivel))
			return nEstado;

		int iLen = TRAZA_MAX_DATA_SEC;

		if (i_cFormato) iLen = (int)_tcslen(i_cFormato);

		TTChar cBuffer[TRAZA_MAX_DATA];
		va_list args;
		va_start(args, i_cFormato);
		_vsntprintf_s(cBuffer, TRAZA_MAX_DATA_SEC, _TRUNCATE, i_cFormato, args);
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nTrace", TIMEOUT_SINCRO_MUTEX);



		nEstado = m_trzUtil.nTraceEx("", -1, nNivel, cBuffer);
	}
	catch (TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		TTChar *sVal;
		char *cWhat;
		cWhat = (char *)ex.what();

		int iLen;
		iLen = (int)strlen(cWhat);
		sVal = new TTChar[iLen + 100];
		_stprintf_s(sVal, iLen + 100, _T("\nCTscTraceUtil::nTrace. ERROR EN TIMEOUT DE MUTEX [%s]\n"), ex.what());

		//TRACE(sVal);

		cout << sVal << endl;

		nEstado = 1;

		delete[]sVal;

	}
	catch (...)
	{
		TTChar *sVal;
		int iLen = 0;
		sVal = new TTChar[iLen + 100];
		_stprintf_s(sVal, iLen + 100, _T("\nCTscTraceUtil::nTrace. Exception\n"));

		cout << sVal << endl;

		nEstado = 1;

		delete[]sVal;
	}

	return nEstado;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nTraceExt
// Descripcion:   Muestra una traza si aplica
// Parametros
//    Entrada:    nNivel: Nivel de la traza
//                cFormato: Texto y formato de la traza
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nTraceExt(char * file, int line, TInt32 nNivel, TTChar *i_cFormato, ...)
{
	TInt32 nEstado = 0;
	try
	{
		if (m_trzUtil.nComprobarNivel(nNivel))
			return nEstado;

		int iLen = TRAZA_MAX_DATA_SEC;

		if (i_cFormato) iLen = (int)_tcslen(i_cFormato);

		TTChar cBuffer[TRAZA_MAX_DATA];
		va_list args;
		va_start(args, i_cFormato);
		_vsntprintf_s(cBuffer, TRAZA_MAX_DATA_SEC, _TRUNCATE, i_cFormato, args);
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nTrace", 5000);



		nEstado = m_trzUtil.nTraceEx(file,line, nNivel, cBuffer);
	}
	catch (TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		TTChar *sVal;
		char *cWhat;
		cWhat = (char *)ex.what();

		int iLen;
		iLen = (int)strlen(cWhat);
		sVal = new TTChar[iLen + 100];
		_stprintf_s(sVal, iLen + 100, _T("\nCTscTraceUtil::nTrace. ERROR EN TIMEOUT DE MUTEX [%s]\n"), ex.what());

		cout << sVal << endl;

		nEstado = 1;

		delete[]sVal;

	}
	catch (...)
	{
		TTChar *sVal;
		int iLen = 0;
		sVal = new TTChar[iLen + 100];
		_stprintf_s(sVal, iLen + 100, _T("\nCTscTraceUtil::nTrace. Exception\n"));

		cout << sVal << endl;

		nEstado = 1;

		delete[]sVal;
	}

	return nEstado;
}


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nDumpMemory
// Descripcion:   Genera una traza con el volcado de memoria de un puntero
// Parametros
//    Entrada:    nNivel: Nivel de la traza
//                nLongitud: Longitud del puntero
//                cCadena: Puntero a la cadena a tracear
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nDumpMemory(TInt32 nNivel, TLong nLongitud, TByte *cCadena)
{
	TInt32 nEstado = 0;
	try
	{
        if (m_trzUtil.nComprobarNivel(nNivel))
		  return nEstado;

		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::CTscTraceUtil", TIMEOUT_SINCRO_MUTEX);	
		nEstado = m_trzUtil.nDumpMemory("", 0,nNivel, nLongitud, cCadena);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CTscTraceUtil::CTscTraceUtil. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
		nEstado = 1;
	}

	return nEstado;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nDumpMemory
// Descripcion:   Genera una traza con el volcado de memoria de un puntero
// Parametros
//    Entrada:    nNivel: Nivel de la traza
//                nLongitud: Longitud del puntero
//                cCadena: Puntero a la cadena a tracear
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nDumpMemoryExt(char * file, int line, TInt32 nNivel, TLong nLongitud, TByte *cCadena)
{
	TInt32 nEstado = 0;
	try
	{
        if (m_trzUtil.nComprobarNivel(nNivel))
		  return nEstado;

		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::CTscTraceUtil", TIMEOUT_SINCRO_MUTEX);	
		nEstado = m_trzUtil.nDumpMemory(file,line,nNivel, nLongitud, cCadena);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CTscTraceUtil::CTscTraceUtil. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
		nEstado = 1;
	}

	return nEstado;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nTraceDumpMemory
// Descripcion:   Genera una traza con un texto formateado y el volcado de memoria
//                de un puntero
// Parametros
//    Entrada:    nNivel: Nivel de la traza
//                nLongitud: Longitud del puntero
//                cCadena: Puntero a la cadena a tracear
//                cFormat: Cadena formateada
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nTraceDumpMemory(TInt32 nNivel , TLong nLongitud, TByte *cCadena, TTChar *cFormat, ... )
{
	TInt32 nEstado = 0;
	try
	{
        if (m_trzUtil.nComprobarNivel(nNivel))
		  return nEstado;

		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nTraceDumpMemory", TIMEOUT_SINCRO_MUTEX);	

		TTChar cBuffer[TRAZA_MAX_DATA];
		va_list args;
		va_start(args, cFormat);
		_vsntprintf_s(cBuffer, TRAZA_MAX_DATA_SEC,_TRUNCATE, cFormat, args);

		nEstado = m_trzUtil.nTraceDumpMemory("", 0, nNivel, nLongitud, cCadena, cBuffer);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CTscTraceUtil::nTraceDumpMemory. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
		nEstado = 1;
	}

	return nEstado;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nTraceDumpMemory
// Descripcion:   Genera una traza con un texto formateado y el volcado de memoria
//                de un puntero
// Parametros
//    Entrada:    nNivel: Nivel de la traza
//                nLongitud: Longitud del puntero
//                cCadena: Puntero a la cadena a tracear
//                cFormat: Cadena formateada
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nTraceDumpMemoryExt(char * file, int line, TInt32 nNivel , TLong nLongitud, TByte *cCadena, TTChar *cFormat, ... )
{
	TInt32 nEstado = 0;
	try
	{
        if (m_trzUtil.nComprobarNivel(nNivel))
		  return nEstado;

		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nTraceDumpMemory", TIMEOUT_SINCRO_MUTEX);	

		TTChar cBuffer[TRAZA_MAX_DATA];
		va_list args;
		va_start(args, cFormat);
		_vsntprintf_s(cBuffer, TRAZA_MAX_DATA_SEC,_TRUNCATE, cFormat, args);

		nEstado = m_trzUtil.nTraceDumpMemory(file, line, nNivel, nLongitud, cCadena, cBuffer);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CTscTraceUtil::nTraceDumpMemory. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
		nEstado = 1;
	}

	return nEstado;
}


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nGetTraceSrv
// Descripcion:   Obtiene el nombre del servidor de trazas
// Parametros
//    Salida:     cSrv: Nombre del servidor
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nGetTraceSrv (TTChar *cSrv)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nGetTraceSrv", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nGetTraceSrv(cSrv);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nGetTraceSrv(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}	

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nSetTraceSrv
// Descripcion:   Establece el nombre del servidor de trazas
// Parametros
//    Entrada:    cSrv: Mensaje a tracear
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nSetTraceSrv (TTChar *cSrv)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nSetTraceSrv", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nSetTraceSrv(cSrv);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nSetTraceSrv(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}	

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nSetTamFile
// Descripcion:   Establece el tamaño maximo del fichero de trazas
// Parametros
//    Entrada:    nTam: Mensaje a tracear
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nSetTamFile (TInt32 nTam)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nSetTamFile", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nSetTamFile(nTam);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nSetTamFile(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}	

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nSetLineInfo
// Descripcion:   Establece si se debe trazar o no la información de ficehro y linea dónde se trazo
// Parametros
//    Entrada:    bsetLineInfo: Si true traza
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         21/05/2017
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nSetLineInfo(TBool bsetLineInfo)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nSetLineInfo", TIMEOUT_SINCRO_MUTEX);
		m_trzUtil.nSetLineInfo(bsetLineInfo);
	}
	catch (TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nSetLineInfo(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTraceUtil::nSetTipoFile
// Descripcion:   Establece el tipo del fichero de trazas
// Parametros
//    Entrada:    nTipo: Tipo de fichero (0 se renombre old, 1 no se renombra)
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTraceUtil::nSetTipoFile (TInt32 nTipo)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nSetTipoFile", TIMEOUT_SINCRO_MUTEX);	
		m_trzUtil.nSetTipoFile(nTipo);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTraceUtil::nSetTipoFile(): ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}	


TInt32 CTscTraceUtil::nTraceExecInfo(TInt32 nNivel, TTChar *cFormato)
{
	TInt32 nEstado = 0;
	try
	{
        if (m_trzUtil.nComprobarNivel(nNivel))
		  return nEstado;

		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nTrace", TIMEOUT_SINCRO_MUTEX);	
		if (_tcslen(cFormato) > TRAZA_MAX_DATA)
			return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;

		nEstado = m_trzUtil.nTraceExecInfo("", 0, nNivel, cFormato);

	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CTscTraceUtil::nTraceExecInfo. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
		nEstado = 1;
	}

	return nEstado;
}


TInt32 CTscTraceUtil::nTraceExecInfoExt(char * file, int line, TInt32 nNivel, TTChar *cFormato)
{
	TInt32 nEstado = 0;
	try
	{
        if (m_trzUtil.nComprobarNivel(nNivel))
		  return nEstado;

		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nTrace", TIMEOUT_SINCRO_MUTEX);	
		if (_tcslen(cFormato) > TRAZA_MAX_DATA)
			return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;

		nEstado = m_trzUtil.nTraceExecInfo(file, line, nNivel, cFormato);

	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CTscTraceUtil::nTraceExecInfo. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
		nEstado = 1;
	}

	return nEstado;
}
