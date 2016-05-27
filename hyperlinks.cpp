#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"


bool striplink(char *link){
	return false;
}

bool downURL(char *NDomain, char *NAddress, char *Filename){
	    HINTERNET Initialize,Connection,File;
        DWORD dwBytes;
        BYTE ch[300000];
		HANDLE f = NULL;
		BOOL fSuccess;
		char *fullFilename;
		char ndom[500];
		
		strcpy(ndom,NDomain);

		// Open / Create the file first
		fullFilename = (char*)malloc(sizeof(char)*257);
		sprintf(fullFilename, "%s",Filename);
		f = CreateFile(fullFilename,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (f == INVALID_HANDLE_VALUE) 
		{
			#ifdef VERBOSITY
				printf("Could not open file (error %d)\n", GetLastError());
			#endif
			LocalFree(fullFilename);
			return false;
		}
        Initialize = InternetOpen("html2pdf/0.1",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
        Connection = InternetConnect(Initialize,NDomain,INTERNET_DEFAULT_HTTP_PORT,NULL,"HTTP/1.0",INTERNET_SERVICE_HTTP,0,0);
        File = HttpOpenRequest(Connection,"GET",NAddress,NULL,NULL,NULL,INTERNET_FLAG_RELOAD,0);

        if(HttpSendRequest(File, NULL, 0, NULL, 0))
        {
			#ifdef VERBOSITY
				printf("Downloading... http://%s%s\n",NDomain,NAddress);
			#endif
			
				
			do{	
				if(!InternetReadFile(File,&ch,200000,&dwBytes)){
					printf("Download Error.\n");
					CloseHandle(f);
					return false;
				}

				#ifdef VERBOSITY
					printf("Bytes Read: %i\n", dwBytes);
				#endif
				if (!dwBytes){
					break; //EOF
				}else{
					fSuccess = WriteFile(f,ch,200000,&dwBytes,NULL);
				}
				if (!fSuccess) 
				{
					#ifdef VERBOSITY
						printf ("WriteFile Error\n");
					#endif
					LocalFree(fullFilename);
					return false;
				}
			}while(1);
        }
		#ifdef VERBOSITY
			printf("Saved file: %s\n", fullFilename);
		#endif
        InternetCloseHandle(File);
        InternetCloseHandle(Connection);
        InternetCloseHandle(Initialize);
		CloseHandle(f);
		free(fullFilename);
		//DeleteFile("");
        return true;
}