#include <string>
#include <Windows.h>
#include <winhttp.h>

// Funzione che esegue una chiamata HTTP
std::string request(std::string dataToSend){

    // Eseguo la richiesta POST
    const char* data = dataToSend.c_str();

    // Funzione che crea un HINTERNET handle
    HINTERNET sessionHandle = NULL;
    sessionHandle = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0); // Spiegazione a seguire
    /* Parametri in ordine:
        1. Nome user agent: opzionale, quindi metto NULL
        2. Tipo di accesso: non usiamo proxy, quindi metto WINHTTP_ACCESS_TYPE_NO_PROXY
        3. Nome del proxy: non lo usiamo, quindi metto WINHTTP_NO_PROXY_NAME
        4. Ip che il proxy non deve considerare: non lo usiamo, quindi metto WINHTTP_NO_PROXY_BYPASS
        5. Flag addizionali per la funzione: non ci servono, quindi metto 0
    */
   
    // Funzione che specifica il target della connessione
    HINTERNET connectionHandle = NULL;
    LPCWSTR hostname;
    INTERNET_PORT port;
    connectionHandle = WinHttpConnect(sessionHandle, hostname, port, 0); // Spiegazione a seguire
    /* Parametri in ordine:
        1. Handle ricavato dalla funzione WinHttpOpen
        2. Hostname
        3. Porta alla quale connettersi
        4. Questo valore deve essere tassativamente 0, come suggerisce Microsoft
    */

    // Funzione che crea un handle per la richiesta
    HINTERNET requestHandle = NULL;
    LPCWSTR requestVerb;
    LPCWSTR targetResource;
    requestHandle = WinHttpOpenRequest(connectionHandle, requestVerb, targetResource, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0); // Spiegazione a seguire
    /* Parametri in ordine:
        1. Handle ricavato dalla funzione WinHttpConnect
        2. Verbo della chiamata HTTP
        3. Stringa che contiene il nome della risorsa targettata dalla connessione.
        4. Stringa che contiene la versione di HTTP da usare: uso NULL, quindi HTTP/1.1
        5. Stringa che specifica l'URL del documento da dove prendo il parametro 3: uso WINHTTP_NO_REFERER
        6. Array di stringhe, NULL-terminato, che contengono i tipi di documenti accettati dal client
        7. Flag addizionali: non ci servono, quindi metto 0
    */

    // Funzione che manda la richiesta specificata dalle tre funzioni sopra
    BOOL requestStatus;
    requestStatus = WinHttpSendRequest(requestHandle, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0); // Spiegazione a seguire
    /* Parametri in ordine
        1. Handle ricavato dalla funzione WinHttpOpenRequest
        2. Puntatore a stringa che contiene header addizionali: non ne usiamo, metto WINHTTP_NO_ADDITIONAL_HEADERS
        3. Lunghezza del parametro 2: è 0, perché non usiamo header addizionali
        4. Puntatore a un buffer che contiene i dati mandati dopo l'header
        5. Dimensione del parametro 4
        6. Dimensione totale dei dati addizionali mandati
        7. Puntatore che contiene un valore che viene passato ad una funzione di callback, che non usiamo, quindi metto 0
    */

   DWORD dwBytesWritten = 0;

   requestStatus = WinHttpWriteData(requestHandle, data, (DWORD)strlen(data), &dwBytesWritten);

   // Funzione che riceve la risposta dal server riguardo alla richiesta effettuata
   BOOL responseStatus;
   responseStatus = WinHttpReceiveResponse(requestHandle, NULL); // Spiegazione a seguire
   /* Parametri in ordine:
        1. Handle ottenuto con la WinHttpOpenRequest e usato dalla WinHttpSendRequest
        2. Parametro che deve essere per forza NULL, grazie Microsoft
   */

    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer;
    std::string result;

    if (!responseStatus){
        //Error in receiving servers response
    } else {
            do {
                // Check for available data.
                dwSize = 0;
                if (!WinHttpQueryDataAvailable(requestHandle, &dwSize)) 
                {
                    //Errore in WinHttpQueryDataAvailable
                    break;
                }
                
                // No more available data.
                if (!dwSize)
                    break;

                // Allocate space for the buffer.
                pszOutBuffer = new char[dwSize+1];
                if (!pszOutBuffer)
                {
                    //Memoria esaurita
                    break;
                }
                
                // Read the Data.
                ZeroMemory(pszOutBuffer, dwSize+1);

                if (!WinHttpReadData(requestHandle, (LPVOID)pszOutBuffer, 
                                    dwSize, &dwDownloaded))
                {                                  
                    //Errore in WinHttpReadData
                }
                else
                {
                    //La risposta del server è...
                }
            
                // Free the memory allocated to the buffer.
                result = pszOutBuffer;
                delete [] pszOutBuffer;

                // This condition should never be reached since WinHttpQueryDataAvailable
                // reported that there are bits to read.
                if (!dwDownloaded)
                    break;
                    
            } while (dwSize > 0);
    }

    // Close open handles.
    if (requestHandle) WinHttpCloseHandle(requestHandle);
    if (connectionHandle) WinHttpCloseHandle(connectionHandle);
    if (sessionHandle) WinHttpCloseHandle(sessionHandle);

    return result;
}
