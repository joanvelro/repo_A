/////////////////////////////////////////////////////////////////////////////
// Fichero:           TscTraceIm.cpp
// Descripcion:       Clase que implementa las funcionalidades de las trazas
// Autor:             Miguel Romero
// Auditor:
// Fecha:             1/06/2001
// Modificaciones:
// Observaciones:
/////////////////////////////////////////////////////////////////////////////


#include <io.h>
#include <stdio.h>
#include <iostream>

/*#ifndef VS_EXPRESS_TEST
   //#include <afxwin.h>
#else
   #include "mfc_adapter.h"
#endif*/

#include <sys/timeb.h>
#include "TscTraceIm.h"


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


#define VERINFO_comments "comments"
#define VERINFO_CompanyName "CompanyName"
#define VERINFO_FileDescrib "FileDescription"
#define VERINFO_FileVersion "FileVersion"
#define VERINFO_InternalName "InternalName"
#define VERINFO_LegalCopyright "LegalCopyright"
#define VERINFO_LegalTrademarks "LegalTrademarks"
#define VERINFO_OriginalFilename "OriginalFilename"
#define VERINFO_ProductName "ProductName"
#define VERINFO_ProductVersion "ProductVersion"
#define VERINFO_PrivateBuild "PrivateBuild"
#define VERINFO_SpecialBuild "SpecialBuild"


using namespace std;

struct FileInfo
{
string fileName;
string version;
string date;
};
struct LANGANDCODEPAGE {
WORD wLanguage;
WORD wCodePage;
};


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::CTscTrace
// Descripcion:   Constructor
// Parametros
// Salida:
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

CTscTrace::CTscTrace()
{
	m_bTraceToScreen = TRUE;
	m_bTraceToFile = TRUE;
	m_nNivelTrazaScreenAMostrar = TRAZA_AVISO;
	m_nNivelTrazaFileAMostrar = TRAZA_AVISO;
	_tcscpy(m_cProcessName, _T("NOPROCESS"));
	_tcscpy(m_cFileLog, _T(FICHERO_TRAZAS));
	m_TamFile = MAX_TAM_FICHERO;
	m_TipoFile = 0;

	m_bSrvTrazas = FALSE;
	_tcscpy(m_cSrvTrazas, _T(""));

	m_bTraceFileOpen = FALSE;
	m_hTraceFile = NULL;

	m_bLineInfo = false;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::CTscTrace
// Descripcion:   Constructor
// Parametros
//    Entrada:    bTraceToScreen: Mostrar o no las trazas en pantalla
//                bTraceToFile: Mostrar o no las trazas en pantalla
//                nNivelTrazaScreenAMostrar: Nivel de trazas a mostrar en pantalla
//                nNivelTrazaFileAMostrar: Nivel de trazas a mostrar en pantalla
//                cProcessName: Nombre del proceso que genera las trazas
//                cFileLog: Nombre del fichero de trazas
// Salida:
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

CTscTrace::CTscTrace(TBool bTraceToScreen,
					TBool bTraceToFile,
					TInt32 nNivelTrazaScreenAMostrar,
					TInt32 nNivelTrazaFileAMostrar,
					TTChar *cProcessName,
					TTChar *cFileLog)
{
	m_bTraceToScreen = bTraceToScreen;
	m_bTraceToFile = bTraceToFile;
	m_nNivelTrazaScreenAMostrar = nNivelTrazaScreenAMostrar;
	m_nNivelTrazaFileAMostrar = nNivelTrazaFileAMostrar;
	_tcscpy(m_cProcessName, cProcessName);
	_tcscpy(m_cFileLog, cFileLog);

	m_TamFile = MAX_TAM_FICHERO;
	m_TipoFile = 0;

	m_bSrvTrazas = FALSE;
	_tcscpy(m_cSrvTrazas, _T(""));

	m_bTraceFileOpen = FALSE;
	m_hTraceFile = NULL;

	m_bLineInfo = false;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::CTscTrace
// Descripcion:   Destructor
// Parametros
// Salida:
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

CTscTrace::~CTscTrace()
{
      nCloseTraceFile();
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nInitSrv
// Descripcion:   Establece la conexion con el servidor de trazas
// Parametros
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nInitSrv()
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nInitSrv
// Descripcion:   Establece la conexion con el servidor de trazas teniendo en 
//                que es el propio servidor de trazas
// Parametros
//    Entrada:    pSrvTrz: Objeto de trazas
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nInitSrv(TscTraceORB_i *pSrvTrz)
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nEndSrv
// Descripcion:   Finaliza la conexion con el servidor de trazas
// Parametros
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nEndSrv()
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nSetTraceFile
// Descripcion:   Establece el nombre del fichero de trazas
// Parametros
//    Entrada:    cTraceFile: Nombre del fichero de trazas
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nSetTraceFile ( LPCTSTR cTraceFile)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::SetTraceFile", TIMEOUT_SINCRO_MUTEX);	

		nCloseTraceFile();
		_tcscpy ( m_cFileLog, cTraceFile );
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::SetTraceFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nGetTraceFile
// Descripcion:   Obtiene el nombre del fichero de trazas
// Parametros
//    Salida:     cTraceFile: Nombre del fichero de trazas
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nGetTraceFile ( TTChar *cTraceFile)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::GetTraceFile", TIMEOUT_SINCRO_MUTEX);	
		_tcscpy (cTraceFile, m_cFileLog);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "GetTraceFile::nInitSrv: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nSetProcessName
// Descripcion:   Establece el nombre del proceso que genera las trazas
// Parametros
//    Entrada:    cNombreProceso: Nombre del proceso
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nSetProcessName(TTChar *cNombreProceso)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nSetProcessName", TIMEOUT_SINCRO_MUTEX);	
		_tcscpy(m_cProcessName, cNombreProceso);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nSetProcessName: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nGetProcessName
// Descripcion:   Obtiene el nombre del proceso que genera las trazas
// Parametros
//    Salida:     cNombreProceso: Nombre del proceso
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nGetProcessName(TTChar *cNombreProceso)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nGetProcessName", TIMEOUT_SINCRO_MUTEX);	
		_tcscpy(cNombreProceso, m_cProcessName);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nGetProcessName: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nSetTraceToFile
// Descripcion:   Establece si se envian o no las trazas a fichero
// Parametros
//    Entrada:    bTraceToFile: Condicion de envio o no envio
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nSetTraceToFile (TBool bTraceToFile)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nSetTraceToFile", TIMEOUT_SINCRO_MUTEX);	
		m_bTraceToFile = bTraceToFile;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nSetTraceToFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nGetTraceToFile
// Descripcion:   Devuelve si se envian o no las trazas a fichero
// Parametros
//    Salida:     bEnable: Condicion
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nGetTraceToFile(TBool& bEnable)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nGetTraceToFile", TIMEOUT_SINCRO_MUTEX);	
		bEnable = m_bTraceToFile;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nGetTraceToFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return FALSE;
	}
	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nOpenTraceFile
// Descripcion:   Abrir el fichero de trazas
// Parametros
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nOpenTraceFile ()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nOpenTraceFile", TIMEOUT_SINCRO_MUTEX);	
		if ( !m_bTraceFileOpen )
		{
         m_hTraceFile = CreateFile( m_cFileLog, 
                                    GENERIC_WRITE | GENERIC_READ,
                                    FILE_SHARE_READ,
                                    NULL,
                                    OPEN_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL );

			if ( m_hTraceFile != INVALID_HANDLE_VALUE )
			{
				SetFilePointer (m_hTraceFile, 0, NULL, FILE_END );
				m_bTraceFileOpen = TRUE;
			}
			else
			{
				m_bTraceFileOpen = FALSE;
				return TRAZA_ERROR_FICHERO;
			}
		}
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nOpenTraceFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nCloseTraceFile
// Descripcion:   Cierra el fichero de trazas
// Parametros
//    Entrada:    cTexto: Mensaje a tracear
//                bToScreen: Indica si el mensaje se debe mostrar en pantalla
//                bToFile: Indica si el mensaje se debe mostrar en fichero
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nCloseTraceFile()
{
	try
	{
		if ( m_bTraceFileOpen )
		{
			TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nCloseTraceFile", TIMEOUT_SINCRO_MUTEX);
			if (CloseHandle ( m_hTraceFile ) != TRUE) return 1;
			m_bTraceFileOpen = FALSE;
		} //endif
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nCloseTraceFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nSetTraceToScreen
// Descripcion:   Establece si se envian las trazas a pantalla
//    Entrada:    bTraceTScreen: Condicion
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nSetTraceToScreen(TBool bTraceTScreen)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nSetTraceToScreen", TIMEOUT_SINCRO_MUTEX);	
		m_bTraceToScreen = bTraceTScreen;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nSetTraceToScreen: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nGetTraceToScreen
// Descripcion:   Devuelve si se envian o no las trazas a pantalla
// Parametros
//    Salida:     bEnableTrace: CondicionMensaje a tracear
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nGetTraceToScreen(TBool& bEnableTrace)
{
	try
	{
		//TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nGetTraceToScreen", TIMEOUT_SINCRO_MUTEX);	
		bEnableTrace = m_bTraceToScreen;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nGetTraceToScreen: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nSetLevelTraceFile
// Descripcion:   Establece el nivel de trazas a mostrar en fichero
// Parametros
//    Entrada:    nNivel: Nivel minimo cuyas trazas se mostraran
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nSetLevelTraceFile(TInt32 nNivel)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nSetLevelTraceFile", TIMEOUT_SINCRO_MUTEX);	
		m_nNivelTrazaFileAMostrar = nNivel;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nSetLevelTraceFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nSetLevelTraceScreen
// Descripcion:   Establece el nivel de trazas a mostrar en pantalla
// Parametros
//    Entrada:    nNivel: Nivel minimo cuyas trazas se mostrarán
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nSetLevelTraceScreen(TInt32 nNivel)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nSetLevelTraceScreen", TIMEOUT_SINCRO_MUTEX);	
		m_nNivelTrazaScreenAMostrar = nNivel;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nSetLevelTraceScreen: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nGetLevelTraceScreen
// Descripcion:   Devuelve el nivel de trazas a mostrar en pantalla
// Parametros
//    Salida:     nNivel: Nivel minimo que se mostrara
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nGetLevelTraceScreen(TInt32& nNivel)
{
	try
	{
		//TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nGetLevelTraceScreen", TIMEOUT_SINCRO_MUTEX);	
		nNivel = m_nNivelTrazaScreenAMostrar;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nGetLevelTraceScreen: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
};
	
/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nGetLevelTraceFile
// Descripcion:   Devuelve el nivel de trazas a mostrar en fichero
// Parametros
//    Entrada:    nNivel: Nivel minimo que se mostrara
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nGetLevelTraceFile(TInt32& nNivel)
{
	try
	{
		//TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nGetLevelTraceFile", TIMEOUT_SINCRO_MUTEX);	
		nNivel = m_nNivelTrazaFileAMostrar;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nGetLevelTraceFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
}


TInt32 CTscTrace::nTrace ( TInt32 nNivel, TTChar *i_cFormato, ... )
{
   TInt32 nEstado = 0;
   try
   {
      int iLen = TRAZA_MAX_DATA_SEC;

      if(i_cFormato) iLen = (int)_tcslen(i_cFormato);

      TTChar cBuffer[TRAZA_MAX_DATA];
      va_list args;
      va_start(args, i_cFormato);
      _vsntprintf_s (cBuffer, TRAZA_MAX_DATA_SEC,_TRUNCATE, i_cFormato, args);

      TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nTrace", TIMEOUT_SINCRO_MUTEX);



		nEstado = nTraceEx("", -1, nNivel, cBuffer);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
      TTChar *sVal;
      char *cWhat;
      cWhat = (char *)ex.what();

      int iLen;
      iLen = (int)strlen(cWhat);
      sVal = new TTChar[iLen+100];
      _stprintf_s(sVal,iLen+100,_T("\nCTscTraceUtil::nTrace. ERROR EN TIMEOUT DE MUTEX [%s]\n"), ex.what());

		cout << sVal << endl;

		nEstado = 1;

      delete []sVal;



	}
   catch(...)
   {
      TTChar *sVal;
      int iLen = 0;
      sVal = new TTChar[iLen+100];
      _stprintf_s(sVal,iLen+100,_T("\nCTscTraceUtil::nTrace. Exception\n"));

		cout << sVal << endl;

		nEstado = 1;

      delete []sVal;
   }

	return nEstado;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nTrace
// Descripcion:   Muestra una traza si aplica
// Parametros
//    Entrada:    nNivel: Nivel de la traza
//                cFormato: Texto y formato de la traza
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nTraceEx (char * file, int line, TInt32 nNivel, TTChar *cFormato )
{
   try
   {
      int iLen = TRAZA_MAX_DATA_SEC;
      if(cFormato) iLen = (int)_tcslen(cFormato);
      if(iLen >= TRAZA_MAX_DATA_SEC) return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;

      TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nTraceEx", TIMEOUT_SINCRO_MUTEX);	

      TTChar cSalida[TRAZA_MAX_DATA];

      // Se genera la fecha de la traza
      struct tm *newtime;
	  TTChar cTime[30] =  {0};

      struct _timeb timebFecha;
      _ftime ( &timebFecha );

      newtime = localtime( &timebFecha.time );

      _stprintf_s(cTime,30,_T("%04d-%02d-%02d %02d:%02d:%02d,%03d"),
				  newtime->tm_year + 1900, newtime->tm_mon+1,newtime->tm_mday,
                  newtime->tm_hour,newtime->tm_min,newtime->tm_sec,timebFecha.millitm);

      // Se obtiene el nivel de la traza
      TTChar cNivel[16];
      switch(nNivel)
      {
      case TRAZA_DEPURA:
         _tcscpy(cNivel, _T("DEBUG"));
         break;
      case TRAZA_INFO:
         _tcscpy(cNivel, _T("INFO"));
         break;
      case TRAZA_AVISO:
         _tcscpy(cNivel, _T("WARN"));
         break;
      case TRAZA_ERROR:
         _tcscpy(cNivel, _T("ERROR"));
         break;
      default:
         return 1;
      } //endswitch


	  if (m_bLineInfo)
	  {
		  // Se genera el texto de la traza
		  _stprintf_s(cSalida,
			  TRAZA_MAX_DATA,
			  _T("%s %s [%lX] (%s:%d) %s\r\n"),
			  cTime,
			  cNivel,
			  GetCurrentThreadId(),
			  file,
			  line,
			  cFormato);
	  }
	  else
	  {
		  // Se genera el texto de la traza
		  _stprintf_s(cSalida,
			  TRAZA_MAX_DATA,
			  _T("%s %s [%lX] %s\r\n"),
			  cTime,
			  cNivel,
			  GetCurrentThreadId(),
			  cFormato);
	  }

      if(_tcslen(cSalida) >= TRAZA_MAX_DATA)
      {
         return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;
      } //endif

      // Si no existe conexion con el servidor de trazas se genera la traza en local
      if(!m_bSrvTrazas)
      {
         // Se comprueba el nivel de la traza para mostrar en fichero
         if(!nComprobarNivelFile(nNivel))
         {
            TInt32 nError = 0;
            TBool bTraceFile = FALSE;
            // Se comprueba si el la traza se mostrara en fichero
            nGetTraceToFile(bTraceFile);
            if (bTraceFile)
            {
               // Si el fichero no esta abierto se abre renombrando el fichero si existiese si el tipo es 0
               if(!m_bTraceFileOpen)
               {
                  if (m_TipoFile == 0) nCambiarFicheroTrazas();
                  nError = nOpenTraceFile();
               }
               if(!nError)
               {
                  DWORD nBytesWritten = 0;
                  DWORD nBytesToWrite = 0;

#ifdef _UNICODE
                  nBytesToWrite = (DWORD)_tcslen(cSalida) * 2;
#else
                  nBytesToWrite = (DWORD)_tcslen(cSalida);
#endif

                  BOOL bRet = FALSE;
                  bRet = WriteFile( m_hTraceFile,
                                    cSalida,
                                    nBytesToWrite,
                                    &nBytesWritten,
                                    NULL );

                  if( bRet == FALSE )
                  {
                     nCloseTraceFile();
                  }
                  else
                  {
                     // Si el tipo de fichero es cero se comprueba si ha superado el tamaño maximo
                     if (m_TipoFile == 0)
                     {
                        LPDWORD pTamFile=0;
                        TInt32 nTamFile;
                        nTamFile = GetFileSize(m_hTraceFile,pTamFile);
                        if(nTamFile > m_TamFile) nCloseTraceFile();
                     } //endif
                  }
               }
               else cout << _T("CTscTrace::nTraceEx. Error abriendo el fichero ") << m_cFileLog << endl;
            } //endif
         } //endif

         // Se comprueba el nivel de la traza para mostrar en pantalla
         if (!nComprobarNivelScreen(nNivel))
         {
            TBool bTraceScreeen = FALSE;
            // Se comprueba si el la traza se mostrara en pantalla
            nGetTraceToScreen(bTraceScreeen);
            if (bTraceScreeen)
            {
               BOOL bRet;
               //LC: modificación para ver caracteres OEM
               TCHAR cSalidaConsola[TRAZA_MAX_DATA];
               CHAR scSalidaConsola[TRAZA_MAX_DATA];
               bRet = ::CharToOem(cSalida,scSalidaConsola);

#ifdef _UNICODE
               MultiByteToWideChar(CP_ACP, 0, scSalidaConsola, -1, cSalidaConsola, TRAZA_MAX_DATA );
#else
               sprintf_s(cSalidaConsola,TRAZA_MAX_DATA,"%s",scSalidaConsola);
#endif

#ifdef _DEBUG
               if(bRet == FALSE)
               {
                  DWORD dwRet = 0;
                  dwRet = GetLastError();
                  
                  _tprintf(_T("__tscTraceIm__ CharToOem error :%d - %ld"),
                     bRet,
                     dwRet );
               } //endif
#endif
               _tprintf(_T("%s"),cSalidaConsola);
            } //endif
         } //endif
      }
   }
   catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
   {
      cout << _T("CTscTrace::nTraceEx. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
      return 1;
   }

   return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nCambiarFicheroTrazas
// Descripcion:   Renombre el fichero de trazas y crea uno nuevo vacio
// Parametros
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nCambiarFicheroTrazas()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nCambiarFicheroTrazas", TIMEOUT_SINCRO_MUTEX);	
		TTChar cFileOld[TRAZA_MAX_DATA];

		struct tm* atm;
		time_t mtm = time (NULL);
		atm = localtime(&mtm);	
		int nSec = atm->tm_sec;
		int nMin = atm->tm_min;
		int nHour = atm->tm_hour;
		int nDay = atm->tm_mday;
		int nMonth = atm->tm_mon + 1;        // tm_mon is 0 based
		int nYear = atm->tm_year + 1900;     // tm_year is 1900 based

		TTChar sFichero[TRAZA_MAX_DATA];
		TTChar cFileLog[TRAZA_MAX_DATA];
		int parcial = (int)_tcslen (m_cFileLog);
		parcial = parcial - 4;
		_tcsncpy (cFileLog, m_cFileLog, parcial);
		cFileLog[parcial] = '\0';
		_stprintf_s(sFichero,TRAZA_MAX_DATA,_T("%4d%02d%02d_%02d%02d%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);
		_stprintf_s(cFileOld,TRAZA_MAX_DATA, _T("%s%s.log"), cFileLog, sFichero);

		// Se cierra el fichero de trazas
		if (nCloseTraceFile()) cout << _T("Error al cerrar el fichero ") << m_cFileLog << endl;

      BOOL bRet = FALSE;
      DWORD luRet = 0;
#ifdef _UNICODE
      WIN32_FIND_DATAW ffd;
#else
      WIN32_FIND_DATA ffd;
#endif
      HANDLE hFind;
      hFind = FindFirstFile(m_cFileLog, &ffd);
      if(hFind != INVALID_HANDLE_VALUE)
      {
		   // Se renombre el fichero de trazas a *.old
		   if (_trename(m_cFileLog, cFileOld))
			   cout << _T("Error al renombrar el fichero ") << cFileOld << endl;

         bRet = FindClose(hFind);
         if(bRet == FALSE)
         {
            luRet = GetLastError();
            cout << _T("ERROR:: CTscTrace::nCambiarFicheroTrazas. FindClose (retEx:") << luRet << _T(")") << endl;
         } //endif
      } //endif
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nCambiarFicheroTrazas: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nComprobarNivelScreen
// Descripcion:   Comprueba si el nivel que se pasa es menor que el nivel de trazas
//                a pantalla establecido
// Parametros
//    Entrada:    nNivel: Nivel a testear
// Salida:        Devuelve 0 si nNivel es mayor o igual que el nivel establecido.
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nComprobarNivelScreen (TInt32 nNivel)
{
	try
	{
      int iRet = 0;
		//TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nComprobarNivelScreen", TIMEOUT_SINCRO_MUTEX);	
		if (nNivel < m_nNivelTrazaScreenAMostrar) iRet = 1;
		
      return iRet;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nComprobarNivelScreen: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nComprobarNivelFile
// Descripcion:   Comprueba si el nivel que se pasa es menor que el nivel de trazas
//                a fichero establecido
// Parametros
//    Entrada:    nNivel: Nivel a testear
// Salida:        Devuelve 0 si nNivel es mayor o igual que el nivel establecido.
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nComprobarNivelFile (TInt32 nivel)
{
	try
	{
      int iRet = 0;
		//TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nComprobarNivelFile", TIMEOUT_SINCRO_MUTEX);	
		if (nivel < m_nNivelTrazaFileAMostrar) iRet = 1;

      return iRet;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nComprobarNivelFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
}

TInt32 CTscTrace::nComprobarNivel (TInt32 nivel)
{
	try
	{
      int iRet = 0;
		//TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nComprobarNivel", TIMEOUT_SINCRO_MUTEX);	
		if (((nivel < m_nNivelTrazaFileAMostrar)|| !m_bTraceToFile)&& ((nivel < m_nNivelTrazaScreenAMostrar)||!m_bTraceToScreen))
			iRet = 1;

      return iRet;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nComprobarNivel: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nDumpMemory
// Descripcion:   Genera una traza con el volcado de memoria de un puntero
// Parametros
//    Entrada:    nNivel: Nivel de la traza
//                nLongitud: Longitud del puntero
//                cCadena: Puntero a la cadena a tracear
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nDumpMemory(char * file, int line, TInt32 nNivel, TLong nLongitud, TByte *cCadena)
{
   TTChar cSalida[TRAZA_MAX_DATA];

   // Se calcula la longitud de la traza
   TInt32 nLongitudSalida = 3*nLongitud + (nLongitud / LONGITUD_LINEA + 1) + 2;

   if (nLongitudSalida >= TRAZA_MAX_DATA)
   return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;

   _stprintf_s(cSalida,TRAZA_MAX_DATA, _T("%d Bytes"), nLongitud);

   // Se genera la cadena de salida 
   nGenDumpMemory(nLongitud, cCadena, cSalida);

   // Se muestra la traza
   nTraceEx(file, line, nNivel, cSalida);

   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nTraceDumpMemory
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

TInt32 CTscTrace::nTraceDumpMemory(char * file, int line, TInt32 nNivel , TLong nLongitud, TByte *cCadena, TTChar *cFormat, ... )
{
   try
   {
      int iLen = TRAZA_MAX_DATA_SEC;
      if(cFormat) iLen = (int)_tcslen(cFormat);
	   if(iLen >= TRAZA_MAX_DATA_SEC) return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;

	   TTChar cSalida[TRAZA_MAX_DATA];

	   // Se calcula la longitud minima de la cadena a tracear
	   TInt32 nLongitudSalida = 3*nLongitud + (nLongitud / LONGITUD_LINEA + 1) + 2 + (int)_tcslen(cFormat);

	   if(nLongitudSalida >= TRAZA_MAX_DATA) return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;

	   _stprintf_s(cSalida,TRAZA_MAX_DATA, _T(",%d Bytes"), nLongitud);

	   // Se genera la traza
	   nGenDumpMemory(nLongitud, cCadena, cSalida);

	   TTChar cBuffer[TRAZA_MAX_DATA];
	   va_list args;
	   va_start(args, cFormat);
	   if (_vsntprintf(cBuffer, sizeof(cBuffer), cFormat, args) == -1)
		   return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;
	   if (_tcslen(cBuffer) + _tcslen(cSalida) >= TRAZA_MAX_DATA)
		   return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;
	   _stprintf_s(cBuffer,TRAZA_MAX_DATA, _T("%s%s"), cBuffer, cSalida);

	   // Se muestra la traza
	   nTraceEx(file, line, nNivel, cBuffer);
   }
   catch(...)
   {
   }

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nGenDumpMemory
// Descripcion:   Genera una cadena de texto a partir de un puntero
// Parametros
//    Entrada:    nLongitud: Longitud del puntero
//                cCadena: Puntero
//    Salida:     cSalida: Cadena de texto generada
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nGenDumpMemory(TLong nLongitud, TByte *cCadena, TTChar *cSalida)
{
	_tcscat(cSalida, _T(" "));

	for (TLong i = 0; i < nLongitud; i++) 
		_stprintf(cSalida, _T("%s%02x "), cSalida, cCadena[i]);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nGetTraceSrv
// Descripcion:   Obtiene el nombre del servidor de trazas
// Parametros
//    Salida:    cSrv: Nombre del servidor
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nGetTraceSrv (TTChar *cSrv)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nGetTraceSrv", TIMEOUT_SINCRO_MUTEX);	
		_tcscpy(cSrv, m_cSrvTrazas);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nGetTraceSrv: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}	

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nSetTraceSrv
// Descripcion:   Establece el nombre del servidor de trazas
// Parametros
//    Entrada:    cSrv: Mensaje a tracear
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nSetTraceSrv (TTChar *cSrv)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nSetTraceSrv", TIMEOUT_SINCRO_MUTEX);	
		_tcscpy(m_cSrvTrazas, cSrv);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nSetTraceSrv: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}	

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nSetTamFile
// Descripcion:   Establece el tamaño del fichero de trazas
// Parametros
//    Entrada:    nTam: Nuevo tamaño
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nSetTamFile (TInt32 nTam)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nSetTamFile", TIMEOUT_SINCRO_MUTEX);	
		m_TamFile = nTam;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nSetTamFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}	


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nSetLineInfo
// Descripcion:   Establece si se debe trazar o no la información de ficehro y linea dónde se trazo
// Parametros
//    Entrada:    bsetLineInfo: Si true traza
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         23/05/2017
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nSetLineInfo(TBool bsetLineInfo)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nSetLineInfo", TIMEOUT_SINCRO_MUTEX);
		m_bLineInfo = bsetLineInfo;
	}
	catch (TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nSetLineInfo: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscTrace::nSetTipoFile
// Descripcion:   Establece el tipo de fichero de trazas
// Parametros
//    Entrada:    nTipo: Tipo del fichero (0 se renombra old, 1 no se renombra)
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscTrace::nSetTipoFile (TInt32 nTipo)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTrace::nSetTipoFile", TIMEOUT_SINCRO_MUTEX);	
		m_TipoFile = nTipo;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nSetTipoFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}	

TInt32 CTscTrace::nTraceExecInfo(char * file, int line, TInt32 nNivel, TTChar *cFormato)
{
	TInt32 nEstado = 0;
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscTraceUtil::nTraceExecInfo", TIMEOUT_SINCRO_MUTEX);	
		if(_tcslen(cFormato) >= TRAZA_MAX_DATA_SEC)
			return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;

		TTChar cBuffer[TRAZA_MAX_DATA];
		std::string cProductVerInfo = GetFileVersionInfoString(VERINFO_ProductVersion);
		std::string cProductCompInfo = GetFileVersionInfoString(VERINFO_FileVersion);
		std::string cPath = GetPathExec();
		std::string cCreationDateExec = GetCreationDateExec(cPath);
		std::string cLastDateExec = GetLastDateExec(cPath);

		_stprintf_s ( cBuffer,TRAZA_MAX_DATA, _T("%s\r\n\tExecutable: %s\r\n\tProduct Version  : %s\r\n\tComponent Version: %s\r\n\t%s\r\n\t%s"),cFormato, cPath.c_str(),cProductVerInfo.c_str(), cProductCompInfo.c_str(),cCreationDateExec.c_str(),cLastDateExec.c_str());

		nEstado = nTraceEx(file, line, nNivel, cBuffer);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CTscTrace::nTraceExecInfo. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
		nEstado = 1;
	}

	return nEstado;
}

std::string CTscTrace::GetPathExec()
{
	TTChar sDirectorio[500];
	std::string sPath;
	GetModuleFileName(GetModuleHandle(NULL),sDirectorio,500);

   char ssDirectorio[500];
#ifdef _UNICODE
   int iLen = 0;
   iLen = (int)(wcslen(sDirectorio) + 1);
   WideCharToMultiByte(CP_ACP,0,sDirectorio, iLen, ssDirectorio, 500, NULL, NULL);
#else
   sprintf_s(ssDirectorio,"%s", sDirectorio);
#endif

	sPath = ssDirectorio;
	return sPath;
}

std::string CTscTrace::GetFileVersionInfoString(LPCSTR verInfoString)
{
	char *rcData = 0;
	DWORD dwSize;
	UINT wSize;
	LANGANDCODEPAGE *lpTranslate;	
	//LPTSTR fileName = (LPTSTR)fileFullName;
	TTChar sDirectorio[500];
	GetModuleFileName(GetModuleHandle(NULL),sDirectorio,500);
	LPTSTR fileName = (LPTSTR)sDirectorio;

   string verInfo = "Unavailable";
#ifndef VS_EXPRESS_TEST
	//file name and path
	CFileStatus status;
	if( CFile::GetStatus(fileName, status ) )
#endif
	{
		//Version
		LPDWORD handle1 = 0;
		dwSize = ::GetFileVersionInfoSize(fileName, handle1);
		if(!dwSize) return verInfo;

		rcData = new char[dwSize];
		// get valid resource data using path, handle, and size
		int result = ::GetFileVersionInfo(fileName, 0, dwSize, (LPVOID)rcData);
		if (result)
		{
			result = ::VerQueryValue(rcData,TEXT("\\VarFileInfo\\Translation" ),
			(LPVOID*)&lpTranslate, &wSize);
			if (result == 0 || wSize == 0)
			{
				delete []rcData;
				return verInfo;
			}
		}
		// Read the file description for each language and code page.
		TCHAR SubBlock[100];
		LPVOID lpBuffer;
		for( unsigned i=0; i < (wSize/sizeof(struct LANGANDCODEPAGE)); i++ )
		{
			wsprintf( SubBlock, TEXT("\\StringFileInfo\\%04x%04x\\%s"),
			lpTranslate[i].wLanguage,lpTranslate[i].wCodePage,verInfoString);
			result = VerQueryValue(rcData, SubBlock, (LPVOID*)&lpBuffer, &wSize);
			if (!result) break;
			verInfo = (LPCSTR)lpBuffer;
		} //endfor
		delete []rcData;
	}
	return verInfo;
}


std::string CTscTrace::GetCreationDateExec(std::string i_sPath)
{
	std::string sBuffer= "";

   TCHAR sPath[512];
#ifdef _UNICODE
   MultiByteToWideChar(CP_ACP, 0, i_sPath.c_str(), -1, sPath, 512 );
#else
   sprintf_s(sPath,512,"%s", i_sPath.c_str());
#endif

#ifndef VS_EXPRESS_TEST
   CFileFind fBusca;
   CTime tFechaEjecutable;

   BOOLEAN bEncuentra = fBusca.FindFile(sPath);
   if(bEncuentra)
   {
      bEncuentra = fBusca.FindNextFile();
      fBusca.GetCreationTime( tFechaEjecutable );
      TCHAR sFechaEjecutable[1024];
      _stprintf_s(sFechaEjecutable,1024,_T("%s"), 
         tFechaEjecutable.Format(_T("Date Creation Exec   : %d/%m/%Y %H:%M:%S") ) ); 
#else
   WIN32_FIND_DATA ffd;
   HANDLE hFind = FindFirstFile(sPath, &ffd);
   if(hFind != INVALID_HANDLE_VALUE)
   {
      SYSTEMTIME stUTC, stLocal;
      // Convert the last-write time to local time.
      FileTimeToSystemTime(&ffd.ftCreationTime, &stUTC);
      SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

      TCHAR sFechaEjecutable[1024] = "unknown";
      // Build a string showing the date and time.
      sprintf_s(sFechaEjecutable, 1024, _T("Date Creation Exec   : %d/%d/%d %d:%d:%d"),
         stLocal.wDay, stLocal.wMonth, stLocal.wYear,
         stLocal.wHour, stLocal.wMinute, stLocal.wSecond );

      BOOL bRet = FALSE;
      bRet = FindClose(hFind);
      if(bRet == FALSE)
      {
         DWORD luRet = 0;
         luRet = GetLastError();
      } //endif
#endif

      char ssFechaEjecutable[1024];
#ifdef _UNICODE
      int iLen = 0;
      iLen = (int)(wcslen(sFechaEjecutable) + 1);
      WideCharToMultiByte(CP_ACP,0,sFechaEjecutable, iLen, ssFechaEjecutable, 1024, NULL, NULL);
#else
      sprintf_s(ssFechaEjecutable,"%s", sFechaEjecutable);
#endif
      sBuffer = ssFechaEjecutable;
   } //endif
   return sBuffer;
}

std::string CTscTrace::GetLastDateExec(std::string i_sPath)
{
	std::string sBuffer= "";

   TCHAR sPath[512];
#ifdef _UNICODE
   MultiByteToWideChar(CP_ACP, 0, i_sPath.c_str(), -1, sPath, 512 );
#else
   sprintf_s(sPath,512,"%s", i_sPath.c_str());
#endif
#ifndef VS_EXPRESS_TEST
   CFileFind fBusca;
   CTime tFechaEjecutable;
	BOOLEAN bEncuentra = fBusca.FindFile(sPath);
	if(bEncuentra)
	{
		bEncuentra = fBusca.FindNextFile();
		fBusca.GetLastWriteTime( tFechaEjecutable );

      TCHAR sFechaEjecutable[1024];
      _stprintf_s(sFechaEjecutable,1024,_T("%s"), 
         tFechaEjecutable.Format(_T("Date Last Modify Exec   : %d/%m/%Y %H:%M:%S") ) ); 
#else
   WIN32_FIND_DATA ffd;
   HANDLE hFind = FindFirstFile(sPath, &ffd);
   if (hFind != INVALID_HANDLE_VALUE)
   {
      SYSTEMTIME stUTC, stLocal;
      // Convert the last-write time to local time.
      FileTimeToSystemTime(&ffd.ftLastWriteTime, &stUTC);
      SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

      TCHAR sFechaEjecutable[1024] = "unknown";
      // Build a string showing the date and time.
      sprintf_s(sFechaEjecutable, 1024, _T("Date Last Modify Exec   : %d/%d/%d %d:%d:%d"),
         stLocal.wDay, stLocal.wMonth, stLocal.wYear,
         stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

      BOOL bRet = FALSE;
      bRet = FindClose(hFind);
      if (bRet == FALSE)
      {
         DWORD luRet = 0;
         luRet = GetLastError();
         
      } //endif
#endif

      char ssFechaEjecutable[1024];
#ifdef _UNICODE
      int iLen = 0;
      iLen = (int)(wcslen(sFechaEjecutable) + 1);
      WideCharToMultiByte(CP_ACP,0,sFechaEjecutable, iLen, ssFechaEjecutable, 1024, NULL, NULL);
#else
      sprintf_s(ssFechaEjecutable,"%s", sFechaEjecutable);
#endif
      sBuffer = ssFechaEjecutable;
	}              
	return sBuffer;
}
