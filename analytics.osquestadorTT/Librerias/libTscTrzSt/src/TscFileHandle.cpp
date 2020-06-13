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

#include "TscFileHandle.h"
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
// Funcion:       CTscFileHandle::CTscFileHandle
// Descripcion:   Constructor
// Parametros
// Salida:
// Fecha:         27/11/2002
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

CTscFileHandle::CTscFileHandle()
{
	_tcscpy(m_cFileLog, _T(FICHERO_TRAZAS));
	m_TamFile = MAX_TAM_FICHERO;
	m_TipoFile = 1;

	m_bFileOpen = FALSE;
	m_hFileHandle = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::CTscFileHandle
// Descripcion:   Constructor
// Parametros
//    Entrada:    cFileLog: Nombre del fichero de trazas
// Salida:
// Fecha:         27/11/2002
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

CTscFileHandle::CTscFileHandle(TTChar *cFileLog)
{
	_tcscpy(m_cFileLog, cFileLog);

	m_TamFile = MAX_TAM_FICHERO;
	m_TipoFile = 0;
	
	m_bFileOpen = FALSE;
	m_hFileHandle = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::~CTscFileHandle
// Descripcion:   Destructor
// Parametros
// Salida:
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

CTscFileHandle::~CTscFileHandle()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::~CTscFileHandle", TIMEOUT_SINCRO_MUTEX);	
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscFileHandle::~CTscFileHandle: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
	}
	nCloseFile();
}


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nSetFileName
// Descripcion:   Establece el nombre del fichero a manejar
// Parametros
//    Entrada:    cTraceFile: Nombre del fichero
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nSetFileName ( TTChar *cFileName)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::SetFileName", TIMEOUT_SINCRO_MUTEX);	

		nCloseFile();
		_tcscpy ( m_cFileLog, cFileName );
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscFileHandle::SetFileName: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nGetTraceFile
// Descripcion:   Obtiene el nombre del fichero a manejar
// Parametros
//    Salida:     cTraceFile: Nombre del fichero
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nGetFileName ( TTChar *cTraceFile)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::GetFileName", TIMEOUT_SINCRO_MUTEX);	
		_tcscpy (cTraceFile, m_cFileLog);
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscFileHandle::GetFileName: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nOpenFile
// Descripcion:   Abrir el fichero a manejar
// Parametros
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         27/11/2002
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nOpenFile ()
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::nOpenFile", TIMEOUT_SINCRO_MUTEX);	
		if ( !m_bFileOpen )
		{
			m_hFileHandle = CreateFile (m_cFileLog, 
				GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL, 0 );

			if ( m_hFileHandle != INVALID_HANDLE_VALUE )
			{
				SetFilePointer (m_hFileHandle, 0, NULL, FILE_END );
				m_bFileOpen = TRUE;
			}
			else
			{
				m_bFileOpen = FALSE;
				return TRAZA_ERROR_FICHERO;
			}
		}
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscFileHandle::nOpenFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
};

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nCloseFile
// Descripcion:   Cierra el fichero
// Parametros
// Entrada:    
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nCloseFile()
{
  TInt32 bRetorno = 0;
  try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::nCloseFile", TIMEOUT_SINCRO_MUTEX);	
		if ( m_bFileOpen )
		{
			if (CloseHandle ( m_hFileHandle ) != TRUE)
				bRetorno = 1;
			m_bFileOpen = FALSE;
		}
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscFileHandle::nCloseFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}
	return bRetorno;
};


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nWriteLane
// Descripcion:   Introduce una linea en el fichero
// Parametros
//    Entrada:    cFromato es puntero ala cadena a introducir
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         27/11/2002
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nWriteLane (TTChar *cFormato, ...)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::nWriteLane", TIMEOUT_SINCRO_MUTEX);

		TTChar cSalida[TRAZA_MAX_DATA];
		TTChar cBuffer[TRAZA_MAX_DATA];

		va_list args;
		va_start(args, cFormato);

		if (_vsntprintf(cBuffer, sizeof(cBuffer), cFormato, args) == -1)
			return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;
		va_end(args);

		_stprintf_s ( cSalida,TRAZA_MAX_DATA, _T("%s\n"), cBuffer );


      if(_tcslen(cSalida) >= TRAZA_MAX_DATA)
   	{
         return TRAZA_ERROR_MENSAJE_DEMASIADO_GRANDE;
      }

		TInt32 nError = 0;
		// Si el fichero no esta abierto se abre.
		if ( !m_bFileOpen )
		{
			nError = nOpenFile();
		}
		if (!nError)
		{
			DWORD nBytesWritten	= 0;
			DWORD nBytesToWrite	= 0;

			nSetPointerWriteFile();

#ifdef _UNICODE
         nBytesToWrite = _tcslen(cSalida) * 2;
#else
         nBytesToWrite = (int)_tcslen(cSalida);
#endif

			if(WriteFile(m_hFileHandle,
						 cSalida,
						 nBytesToWrite,
						 &nBytesWritten,
						 NULL) == 0)
			{
				nCloseFile();
			}
			else
			{
				// Si el tipo de fichero es cero se comprueba si ha superado el tamaño maximo
				if (m_TipoFile == 0)
				{
					// Se comprueba si se ha sobrepasado el tamaño maximo del fichero de trazas
					LPDWORD pTamFile = 0;
					TInt32 nTamFile;
					nTamFile = GetFileSize(m_hFileHandle, pTamFile);
					if(nTamFile > m_TamFile) nCloseFile();
				}
			}
		}
		else
			cout << _T("CTscFileHandle::nWriteLane. Error abriendo el fichero ") << m_cFileLog << endl;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CTscFileHandle::nWriteLane. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
		return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nRadLane
// Descripcion:   Lee una linea del fichero
// Parametros
//    Entrada:    cFormato es puntero ala cadena a introducir
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         29/11/2002
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nReadLane (TTChar *cSalida)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::nReadLane", TIMEOUT_SINCRO_MUTEX);

		TTChar cBuffer[2];
		TULong nBytesWritten;
		TInt32 nError = 0;
		_tcscpy (cSalida, _T(""));

		// Si el fichero no esta abierto se abre.
		if ( !m_bFileOpen )
		{
			nError = nOpenFile();
		}

    TBool bBusquedaTerminada = FALSE;

		if (!nError)
		{
  		//Nos cargamos los blancos y los saltos de línea
	  	while (!bBusquedaTerminada)
		  {
			  cBuffer[0]=0;
        cBuffer[1]=0;
			  if (ReadFile ( m_hFileHandle, cBuffer, 1, &nBytesWritten, NULL) == 0)
			  {
				  nCloseFile();
          return TRAZA_ERROR_FICHERO;
			  }
        // Mira si estamos en el final del fichero
        if (nBytesWritten == 0)
        {
          return TRAZA_ERROR_FINAL_FICHERO;
        }
       
        if (!(cBuffer[0] == '\n' ))
        {
          _tcscat(cSalida, cBuffer); //Primer byte válido
        }
        else
        {
          bBusquedaTerminada = TRUE;
        }
		  }
		}
		else
			cout << _T("CTscFileHandle::nReadLane. Error abriendo el fichero ") << m_cFileLog << endl;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CTscFileHandle::nWriteLane. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
		return 1;
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nReadWord
// Descripcion:   Lee una palabra del fichero
// Parametros
// Salida:        cSalida es puntero a la palabra leida
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         29/11/2002
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nReadWord (TTChar *cSalida)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::nReadWord", TIMEOUT_SINCRO_MUTEX);

		TTChar cBuffer[2];
		TULong nBytesWritten;
		_tcscpy (cSalida, _T(""));


		TInt32 nError = 0;
		// Si el fichero no esta abierto se abre.
		if ( !m_bFileOpen )
		{
			nError = nOpenFile();
		}

    TBool bBusquedaTerminada = FALSE;
    TBool bCaracterValido = FALSE;

		if (!nError)
		{
  		//Nos cargamos los blancos y los saltos de línea
	  	while (!bBusquedaTerminada)
		  {
			  cBuffer[0]=0;
        cBuffer[1]=0;

			  if (ReadFile ( m_hFileHandle, cBuffer, 1, &nBytesWritten, NULL) == 0)
			  {
				  nCloseFile();
          return TRAZA_ERROR_FICHERO;
			  }
        // Mira si estamos en el final del fichero
        if (nBytesWritten == 0)
        {
          return TRAZA_ERROR_FINAL_FICHERO;
        }
       
        if (!(( cBuffer[0] == ' ' )||( cBuffer[0] == '\n' )))
        {
          _tcscat(cSalida, cBuffer); //Primer byte válido
          bCaracterValido = TRUE;
        }
        else if (bCaracterValido)
        {
           bBusquedaTerminada = TRUE;
        }
		  }
		}
		else
			cout << _T("CTscFileHandle::nReadLane. Error abriendo el fichero ") << m_cFileLog << endl;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CTscFileHandle::nWriteLane. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
		return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nReadWord
// Descripcion:   Lee una palabra del fichero
// Parametros
// Salida:        cSalida es puntero a la palabra leida
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         29/11/2002
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nReadBlock (TTChar *cSalida, TInt32 nBlockLen)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::nReadWord", TIMEOUT_SINCRO_MUTEX);

		_tcscpy (cSalida, _T(""));
		TULong nBytesWritten;

		TInt32 nError = 0;
		// Si el fichero no esta abierto se abre.
		if ( !m_bFileOpen )
		{
			nError = nOpenFile();
		}

    TBool bCaracterValido = FALSE;

		if (!nError)
		{
			if (ReadFile ( m_hFileHandle, cSalida, nBlockLen, &nBytesWritten, NULL) == 0)
			{
				nCloseFile();
        return TRAZA_ERROR_FICHERO;
			}
      // Mira si estamos en el final del fichero
      if (nBytesWritten != nBlockLen)
      {
        return TRAZA_ERROR_FINAL_FICHERO;
      }
		}
		else
			cout << _T("CTscFileHandle::nReadLane. Error abriendo el fichero ") << m_cFileLog << endl;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << _T("CTscFileHandle::nWriteLane. ERROR EN TIMEOUT DE MUTEX: ") << ex.what() <<endl;
		return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nCambiarFichero
// Descripcion:   Renombre el fichero  y crea uno nuevo vacio
// Parametros
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nCambiarFichero()
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
      int parcial = (int)_tcslen(m_cFileLog);
		parcial = parcial - 4;
		_tcsncpy (cFileLog, m_cFileLog, parcial);
		_stprintf_s(sFichero,TRAZA_MAX_DATA,_T("%4d%02d%02d_%02d%02d%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);
		_stprintf_s(cFileOld,TRAZA_MAX_DATA,_T("%s%s.log"), cFileLog, sFichero);

		// Se cierra el fichero de trazas
		if (nCloseFile())
			cout << _T("Error al cerrar el fichero ") << m_cFileLog << endl;
		// Se borra el fichero de trazas antiguo si lo hubiese
		// Se renombre el fichero de trazas a *.old
		if (_trename(m_cFileLog, cFileOld))
			cout << _T("Error al renombrar el fichero ") << cFileOld << endl;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscTrace::nCambiarFicheroTrazas: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}

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

TInt32 CTscFileHandle::nDumpMemory(TInt32 nNivel, TLong nLongitud, TByte *cCadena)
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
	nWriteLane(cSalida);

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

TInt32 CTscFileHandle::nGenDumpMemory(TLong nLongitud, TByte *cCadena, TTChar *cSalida)
{
	_tcscat(cSalida, _T(" "));

	for (TLong i = 0; i < nLongitud; i++) 
		_stprintf(cSalida, _T("%s%02x "), cSalida, cCadena[i]);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nSetTamFile
// Descripcion:   Establece el tamaño del fichero
// Parametros
//    Entrada:    nTam: Nuevo tamaño
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nSetTamFile (TInt32 nTam)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::nSetTamFile", TIMEOUT_SINCRO_MUTEX);	
		m_TamFile = nTam;
	}
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscFileHandle::nSetTamFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}	


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nSetPointerReadFile
// Descripcion:   Posiciona el puntero al fichero apos desde el principio
// Parametros
//    Entrada:    nPos: Nueva posicion
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nSetPointerReadFile (TInt32 nPos)
{
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::nSetPointerReadFile", TIMEOUT_SINCRO_MUTEX);	
		if ( m_hFileHandle != INVALID_HANDLE_VALUE )
		{
			SetFilePointer (m_hFileHandle, nPos, NULL, FILE_BEGIN );
			m_bFileOpen = TRUE;
		}	
  }
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscFileHandle::nSetPointerReadFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Funcion:       CTscFileHandle::nSetPointerWriteFile
// Descripcion:   Posiciona el puntero al fichero escritura a pos desde el principio
// Parametros
//    Entrada:    nPos: Nueva posicion
// Salida:        Devuelve 0 si se realiza correctamente
// Fecha:         1/06/2001
// Observaciones:
/////////////////////////////////////////////////////////////////////////////

TInt32 CTscFileHandle::nSetPointerWriteFile (TInt32 nPos)
{
  TULong puntero;
	try
	{
		TSC_MUTEX_LOCKER lock(m_Mutex, "CTscFileHandle::nSetPointerWtiteFile", TIMEOUT_SINCRO_MUTEX);	
		if ( m_hFileHandle != INVALID_HANDLE_VALUE )
		{
			puntero = SetFilePointer (m_hFileHandle, nPos, NULL, FILE_END );
			m_bFileOpen = TRUE;
		}	
  }
	catch(TSC_MUTEX_EXCEPTION_TIMEOUT &ex)
	{
		cout << "CTscFileHandle::nSetPointerWriteFile: ERROR EN TIMEOUT DE MUTEX " << ex.what() << endl;
		return 1;
	}

	return 0;
}
