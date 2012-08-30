// CFXRegServer.cpp : Defines the entry point for the console application.
//
/*
Version 1.7 by Lewis A. Sellers of Intrafoundaion Software http://www.intrafoundation.com

v1.7b August 22th 2012
copy to cfx folder for .dll

v1.7a August 21th 2012
Fixes.

v1.7 August 19th 2012
Added 64-bit support.
See http://stackoverflow.com/questions/252297/why-is-regopenkeyex-returning-error-code-2-on-vista-64bit
See http://msdn.microsoft.com/en-us/library/ms724072%28VS.85%29.aspx
*/

#include "stdafx.h"

#include <windows.h>
#include "stdio.h" //sprintf

//#include "conio.h"

#include "io.h"
#include <fcntl.h>
#include <sys/stat.h>
#include "windows.h" //getfileattributes

#include "STDLIB.H" //errno

//
#define RETCODE_SUCCESS 		0
#define RETCODE_FAILURE 		1
#define RETCODE_NOTAVAILABLE	2

//#define MAX_PATH 512

enum CFXTYPE
{
	TYPE_UNKNOWN=2,
	TYPE_CPP=0,
	TYPE_JAVA=1
};

///////////////////////////////////////////////////////////////////////////
char *szapp_version="CFXRegServer v1.7c August 22th 2012\0";

///////////////////////////////////////////////////////////////////////////
char szcoldfusion_company[32];
int ncoldfusion_version=0;
int bsupport_java=false;
char szregkey[255+1];
char szCFMXRoot[MAX_PATH+1];

///////////////////////////////////////////////////////////////////////////
//
void errmsg()
{
	LPTSTR lpMsgBuf=NULL;
	int stat=FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
		);
	if(stat)
	{
		printf("error: %s\n",(const char*)lpMsgBuf);
	}

	// Free the buffer.
	LocalFree( lpMsgBuf );
}


//
int getFile(
	const char* szPathFile,
	char* &ptr
	)
{
	//
	int length=0;
	const int stream=_open(szPathFile,_O_RDONLY | _O_BINARY);
	if(stream!=-1)
	{
		length=_filelength(stream);

		ptr = new char[length+1+1];
		if(ptr)
		{
			memset(ptr,0,length+1);

			int const num=_read( stream, ptr, length);
			if(num==-1)
				printf("%d!=%d err:%s\n",num,length,strerror(errno));
			else if(num!=length)
				printf("%d!=%d err:%s\n",num,length,strerror(errno));

			printf("  Load file        \t%s (%d)\n",szPathFile,length);
			//			printf("\n\n****getfile****\n%s\n",ptr);
		}
		//
		_close(stream);
	}

	return length;
}

//
int saveFile(
	const char* szPathFile,
	char* ptr,
	const int length
	)
{
	//
	const int stream=_open(szPathFile,_O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);
	if(stream!=-1)
	{
		int const num=_write( stream, ptr, length);
		if(num==-1)
			printf("%d!=%d err:%s\n",num,length,strerror(errno));
		else if(num!=length)
			printf("%d!=%d err:%s\n",num,length,strerror(errno));

		//
		printf("  Save file        \t%s (%d)\n",szPathFile,length);

		_close(stream);
	}

	return length;
}




///////////////////////////////////////////////////////////////////////////

//
bool detectColdFusionVersion()
{
	szcoldfusion_company[0]='\0';
	szregkey[0]='\0';
	ncoldfusion_version=0;

	HKEY hRegKey;
	char subkey[MAX_PATH+1];
	long ret;

	_snprintf(subkey,MAX_PATH,"SOFTWARE\\Adobe\\Install Data\\Adobe ColdFusion 10");
	ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_READ|KEY_WOW64_64KEY,&hRegKey);
	if(ret==ERROR_SUCCESS)
	{
		RegCloseKey(hRegKey);
		strcpy(szcoldfusion_company,"Adobe\0");
		ncoldfusion_version=10;
		bsupport_java=true;
		strcpy(szregkey,subkey);
		return true;
	}

	_snprintf(subkey,MAX_PATH,"SOFTWARE\\Adobe\\Install Data\\Adobe ColdFusion 9");
	ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_READ|KEY_WOW64_64KEY,&hRegKey);
	if(ret==ERROR_SUCCESS)
	{
		RegCloseKey(hRegKey);
		strcpy(szcoldfusion_company,"Adobe\0");
		ncoldfusion_version=9;
		bsupport_java=true;
		strcpy(szregkey,subkey);
		return true;
	}

	_snprintf(subkey,MAX_PATH,"SOFTWARE\\Adobe\\Install Data\\Adobe ColdFusion 8");
	ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_READ|KEY_WOW64_64KEY,&hRegKey);
	if(ret==ERROR_SUCCESS)
	{
		RegCloseKey(hRegKey);
		strcpy(szcoldfusion_company,"Adobe\0");
		ncoldfusion_version=8;
		bsupport_java=true;
		strcpy(szregkey,subkey);
		return true;
	}

	_snprintf(subkey,MAX_PATH,"SOFTWARE\\Macromedia\\Install Data\\ColdFusion MX 7");
//	_snprintf(subkey,MAX_PATH,"SOFTWARE\\Macromedia\\ColdFusion\\CurrentVersion\\CustomTags");
	ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_READ|KEY_WOW64_64KEY,&hRegKey);
	if(ret==ERROR_SUCCESS)
	{
		RegCloseKey(hRegKey);
		strcpy(szcoldfusion_company,"Macromedia\0");
		ncoldfusion_version=7;
		bsupport_java=true;
		strcpy(szregkey,subkey);
		return true;
	}

		_snprintf(subkey,MAX_PATH,"SOFTWARE\\Macromedia\\Install Data\\ColdFusion MX");
//	_snprintf(subkey,MAX_PATH,"SOFTWARE\\Macromedia\\ColdFusion\\CurrentVersion\\CustomTags");
	ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_READ|KEY_WOW64_64KEY,&hRegKey);
	if(ret==ERROR_SUCCESS)
	{
		RegCloseKey(hRegKey);
		strcpy(szcoldfusion_company,"Macromedia\0");
		ncoldfusion_version=6;
		bsupport_java=true;
		strcpy(szregkey,subkey);
		return true;
	}
	
	_snprintf(subkey,MAX_PATH,"SOFTWARE\\Allaire\\ColdFusion\\CurrentVersion\\CustomTags");
	ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_READ|KEY_WOW64_64KEY,&hRegKey);
	if(ret==ERROR_SUCCESS)
	{
		RegCloseKey(hRegKey);
		strcpy(szcoldfusion_company,"Allaire\0");
		ncoldfusion_version=5;
		bsupport_java=false;
		strcpy(szregkey,subkey);
		return true;
	}
	
	return false;
}


///////////////////////////////////////////////////////////////////////////

//
bool isCFInstalled()
{
	if(ncoldfusion_version>0 && ncoldfusion_version<=5) 
		return true;
	return false;
/*
HKEY hRegKey;
	char subkey[MAX_PATH+1];
	long ret;

	_snprintf(subkey,MAX_PATH,"SOFTWARE\\Allaire\\ColdFusion\\CurrentVersion\\CustomTags");
	ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_READ|KEY_WOW64_64KEY,&hRegKey);
	RegCloseKey(hRegKey);
	if(ret==ERROR_SUCCESS)
		return true;

	return false;
*/
}

//
int unregisterCF(
	const char* szLibrary,
	const char* szDisplayName
	)
{
	printf("::Unregister CF3.x-5.x\n\t%s\n",szLibrary);

	int retcode=RETCODE_SUCCESS;

	if(szLibrary==NULL)
		;
	else
	{
		if(isCFInstalled()==false)
			printf(" CF not found.\n\n");
		else
		{
			//
			HKEY hRegKey;
			DWORD dwDisp;
			char hkey[MAX_PATH+1];

			_snprintf(hkey,MAX_PATH,"SOFTWARE\\Allaire\\ColdFusion\\CurrentVersion\\CustomTags\\%s",szDisplayName);
			if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,hkey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hRegKey,&dwDisp)!=ERROR_SUCCESS)
				retcode=RETCODE_FAILURE;
			else
			{
				RegDeleteKey(HKEY_LOCAL_MACHINE,hkey);
				RegCloseKey(hRegKey);

				printf("  DELETED\n\n");
			}
		}
	}

	return retcode;
}


//
int registerCF(
	const char* szLibrary,
	const int iType,
	const char* szDescription,
	const bool bCache,
	const char* szProcedureName,
	const char* szName,
	const char* szDisplayName
	)
{
	printf("::Register CF3.x-5.x\n\t%s\n",szLibrary);

	int retcode=RETCODE_SUCCESS;

	if(szLibrary==NULL)
		return retcode;

	//
		if(isCFInstalled()==false)
			printf(" CF not found.\n\n");
		else
		{
			if(iType==TYPE_CPP)
			{
				char szNewLibrary[MAX_PATH+1];

				_snprintf(szNewLibrary,MAX_PATH,"%s\\cfx\\%s",szCFMXRoot,szName);
				printf("\n Copying\n  %s\n  to\n  %s\n",szLibrary,szNewLibrary);
				if(CopyFile(szLibrary,szNewLibrary,FALSE)==0)
				{
					printf(" COPY FAILED! ColdFusion server may have jar file locked.\n");
					retcode=RETCODE_FAILURE;
				}
				printf("\n");

				strcpy((char *)szLibrary,szNewLibrary);
				//					strcpy(szFolder,szCFMXRoot);
				printf("  New Library Path    \t%s\n",szLibrary);
			}


			//
			HKEY hRegKey;
			DWORD dwDisp;
			char hkey[MAX_PATH+1];

			_snprintf(hkey,MAX_PATH,"SOFTWARE\\Allaire\\ColdFusion\\CurrentVersion\\CustomTags\\%s",szDisplayName);
			if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,hkey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hRegKey,&dwDisp)!=ERROR_SUCCESS)
				retcode=RETCODE_FAILURE;
			else
			{
				//		printf("dwDisp=%X\n",dwDisp);
				char *szType;
				if(iType==TYPE_JAVA)
					szType="JAVA";
				else
					szType="CPP";

				char *szCache;
				if(bCache==true)
					szCache="1";
				else
					szCache="0";

				RegSetValueEx(hRegKey, "CacheLibrary",	0, REG_SZ, (BYTE*)szCache,			lstrlen(szCache)+1);
				RegSetValueEx(hRegKey, "Description",	0, REG_SZ, (BYTE*)szDescription,	lstrlen(szDescription)+1);
				RegSetValueEx(hRegKey, "LibraryPath",	0, REG_SZ, (BYTE*)szLibrary,		lstrlen(szLibrary)+1);
				RegSetValueEx(hRegKey, "ProcedureName", 0, REG_SZ, (BYTE*)szProcedureName,	lstrlen(szProcedureName)+1);
				RegSetValueEx(hRegKey, "Type",			0, REG_SZ, (BYTE*)szType,			lstrlen(szType)+1);

				RegCloseKey(hRegKey);

				printf("  INSERTED\n\n");
			}
		}

	return retcode;
}



///////////////////////////////////////////////////////////////////////////

//
bool isCFMXInstalled()
{
	if(ncoldfusion_version>=6) 
		return true;
	return false;
/*
	HKEY hRegKey;
	char subkey[MAX_PATH+1];

	_snprintf(subkey,MAX_PATH,"SOFTWARE\\Macromedia\\Install Data\\ColdFusion MX");
	long ret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_READ|KEY_WOW64_64KEY,&hRegKey);
	RegCloseKey(hRegKey);
	if(ret!=ERROR_SUCCESS)
		return false;
	else
		return true;
*/
}

//
bool getCFMXRoot(
	char* szPath
	)
{
	bool ret=true;

	szPath[0]='\0';

	//
	HKEY hKey;
	long lRet = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		szregkey, //"SOFTWARE\\Macromedia\\Install Data\\ColdFusion MX",
		0,
		KEY_QUERY_VALUE|KEY_WOW64_64KEY,
		&hKey);
	if(lRet != ERROR_SUCCESS)
		ret=false;
	else
	{
		DWORD Type=0;
		DWORD cbData=MAX_PATH;
		char szData[MAX_PATH+1];
		long ret=RegQueryValueEx(hKey,"CFMXRoot",NULL,&Type,(LPBYTE)szData,&cbData);

		if(ret!=ERROR_SUCCESS)
			ret=false;
		else
		{
			//
			_snprintf((char *)szPath,MAX_PATH,"%s",szData);
			printf(" CFMX Home         \t%s\n",szPath);
		}

		//
		RegCloseKey( hKey );
	}   

	//
	return ret;
}


//
int unregisterCFMXptr(
	const char* szLibrary,
	const char* szDisplayName,
	char* &ptr,
	int &length
	)
{
	printf("  UnregisterCFMX  \t%p (%d)\n",ptr,length);

	if(ptr==NULL) return 0;

	//
	const char* s1="<struct type='coldfusion.server.ConfigMap'>";
	const char* s12="<struct type='coldfusion.server.ConfigMap' />";
	const char* vs2s="</struct>";
	const char* vs2="</var>";
	char vs1[MAX_PATH+1];
	_snprintf(vs1,MAX_PATH,"<var name='%s'>",szDisplayName);
	char* p1=NULL;
	char* p2=NULL;
	char* p=strstr(ptr,s12);
	if(p==NULL)
	{
		p=strstr(ptr,s1);
		if(p)
		{
			p=strstr(p+strlen(s1),s1);
			if(p)
				p+=strlen(s1);
		}

		p1=strstr(p,vs1);
		if(p1)
		{
			p2=strstr(p1,vs2s);

			if(p1 && p2 && (p1<p2))
			{
				p2+=strlen(vs2s);

				p2=strstr(p2,vs2);
				if(p1 && p2 && (p1<p2))
				{
					p2+=strlen(vs2);
				}
			}

		}
	}
	if(p==NULL)
	{
		printf("**XML Parse failure.**\n");
		return false;
	}

	//
	if(p1 && p2 && (p1<p2))
	{
		const int varlength=(int)(p2-p1);
		int newlength=length-varlength;
		char* newptr=new char[newlength+1+1];
		if(newptr)
		{
			memset(newptr,0,newlength+1);

			const int prelen=(int)(p1-ptr);
			const int postlen=length-(int)(p2-ptr);

			memcpy(newptr,ptr,prelen);
			memcpy(newptr+prelen,ptr+prelen+varlength,postlen);
			*(newptr+newlength)='\0';

			delete [] ptr;
			ptr=newptr;
			length=newlength;

			newptr=0;
			newlength=0;

			printf("  DELETED\n");
		}
	}

	return length;
}

//
int unregisterCFMX(
	const char* szLibrary,
	const char* szDisplayName
	)
{
	printf("::Unregister CFMX\n\t%s\n",szLibrary);

	int retcode=RETCODE_SUCCESS;

	if(szLibrary==NULL)
		return retcode;

	//
	if(!isCFMXInstalled())
			printf(" CFMX not found.\n\n");
		else
		{
			//char szHome[MAX_PATH+1];
			//getCFMXRoot(szHome);

			char szFile[MAX_PATH+1];
			_snprintf((char *)szFile,MAX_PATH,"%s\\lib\\neo-runtime.xml",szCFMXRoot);
			printf("  neo-runtime.xml Path \t%s\n",szFile);

			char szFilebak[MAX_PATH+1];
			_snprintf((char *)szFilebak,MAX_PATH,"%s\\lib\\neo-runtime.xml.bak",szCFMXRoot);

			char* ptr=NULL;
			int length=getFile(szFile,ptr);

			char* newptr=NULL;
			int const newlength=unregisterCFMXptr(szLibrary,szDisplayName,ptr,length);
			if(newptr)
			{
				printf("\n Copying\n  %s\n  to\n  %s\n",szFile,szFilebak);
				if(CopyFile(szFile,szFilebak,FALSE)==0)
				{
					printf(" COPY FAILED! ColdFusion server may have neo-runtime.xml locked.\n");
					retcode=RETCODE_FAILURE;
				}
				printf("\n");

				const int savelength=saveFile(szFile,newptr,newlength);

				delete [] newptr;
				newptr=NULL;

				//
				printf("  DELETED     \t%s\n",szLibrary);
			}

			printf("\n");
		}
	

	return retcode;
}


//
int registerCFMX(
	const char* szLibraryOriginal,
	const int iType,
	const char* szDescription,
	const bool bCache,
	const char* szProcedureName,
	const char* szName,
	const char* szDisplayName,
	const char* szClass
	)
{
	//
	printf("::Register CFMX\n\t%s\n",szLibraryOriginal);
	printf(" type:         \t %d\n",iType);
	printf(" description:  \t %s\n",szDescription);
	printf(" procedure:    \t %s\n",szProcedureName);
	printf(" name:         \t %s\n",szName);
	printf(" display name: \t %s\n",szDisplayName);
	printf(" class:        \t %s\n",szClass);

	int retcode=RETCODE_SUCCESS;

	//
	if(szLibraryOriginal==NULL)
		return retcode;

	//
	if(!isCFMXInstalled())
			printf(" CFMX not found.\n\n");
		else
		{
			//
			char szLibrary[MAX_PATH+1];
			strcpy(szLibrary,szLibraryOriginal);

			//char szHome[MAX_PATH+1];
			//getCFMXRoot(szHome);

			if(iType==TYPE_JAVA)
			{
				char szNewLibrary[MAX_PATH+1];

				_snprintf(szNewLibrary,MAX_PATH,"%s\\cfx\\java\\%s.jar",szCFMXRoot,szName);
				printf("\n Copying\n  %s\n  to\n  %s\n",szLibrary,szNewLibrary);
				if(CopyFile(szLibrary,szNewLibrary,FALSE)==0)
				{
					printf(" COPY FAILED! ColdFusion server may have jar file locked.\n");
					retcode=RETCODE_FAILURE;
				}
				printf("\n");

				strcpy(szLibrary,szNewLibrary);
				//					strcpy(szFolder,szCFMXRoot);
				printf("  New Library Path    \t%s\n",szLibrary);
			}

			if(iType==TYPE_CPP)
			{
				char szNewLibrary[MAX_PATH+1];

				_snprintf(szNewLibrary,MAX_PATH,"%s\\cfx\\%s.dll",szCFMXRoot,szName);
				printf("\n Copying\n  %s\n  to\n  %s\n",szLibrary,szNewLibrary);
				if(CopyFile(szLibrary,szNewLibrary,FALSE)==0)
				{
					printf(" COPY FAILED! ColdFusion server may have dll file locked.\n");
					retcode=RETCODE_FAILURE;
				}
				printf("\n");

				strcpy((char *)szLibrary,szNewLibrary);
				//					strcpy(szFolder,szCFMXRoot);
				printf("  New Library Path    \t%s\n",szLibrary);
			}

			//
			char szFile[MAX_PATH+1];
			_snprintf((char *)szFile,MAX_PATH,"%s\\lib\\neo-runtime.xml",szCFMXRoot);
			printf("  neo-runtime.xml Path \t%s\n",szFile);

			char szFilebak[MAX_PATH+1];
			_snprintf((char *)szFilebak,MAX_PATH,"%s\\lib\\neo-runtime.xml.bak",szCFMXRoot);

			//
			printf("\n Copying\n  %s\n  to\n  %s\n",szFile,szFilebak);
			if(CopyFile(szFile,szFilebak,FALSE)==0)
			{
				printf(" COPY FAILED! Coldfusion server may have neo-untime.xml locked.\n");
				retcode=RETCODE_FAILURE;
			}
			printf("\n");

			//
			char* ptr=NULL;
			int length=getFile(szFile,ptr);

			//
			unregisterCFMXptr(szLibrary,szDisplayName,ptr,length);

			//
			if(ptr!=NULL)
			{
				const char* s1="<struct type='coldfusion.server.ConfigMap'>";
				const char* s12="<struct type='coldfusion.server.ConfigMap' />";
				char* p=strstr(ptr,s12);
				if(p==NULL)
				{
					p=strstr(ptr,s1);
					if(p)
					{
						p=strstr(p+strlen(s1),s1);
						if(p)
							p+=strlen(s1);
					}
				}
				if(p==NULL)
				{
					printf("**XML Parse failure.**\n");
					retcode=RETCODE_FAILURE;
				}
				else
				{
					// <var name='CACHE'><string>true</string></var>
					//
					char var[1024];
					if(iType==TYPE_JAVA)
						_snprintf(var,1023,
						"<var name='%s'><struct type='coldfusion.server.ConfigMap'><var name='CLASSNAME'><string>%s</string></var><var name='TYPE'><string>%s</string></var><var name='NAME'><string>%s</string></var><var name='DESCRIPTION'><string>%s</string></var></struct></var>",
						szDisplayName,szClass,"java",szDisplayName,szDescription);
					else
						_snprintf(var,1023,
						"<var name='%s'><struct type='coldfusion.server.ConfigMap'><var name='PROCEDURE'><string>%s</string></var><var name='CACHE'><string>%s</string></var><var name='TYPE'><string>%s</string></var><var name='NAME'><string>%s</string></var><var name='DESCRIPTION'><string>%s</string></var><var name='LIBRARY'><string>%s</string></var></struct></var>",
						szDisplayName,szProcedureName,(bCache==true)?"true":"false","cpp",szDisplayName,szDescription,szLibrary);

					int const varlength=strlen(var);
					int newlength=length+varlength;
					char* newptr=new char[newlength+1+1];
					if(newptr)
					{
						memset(newptr,0,newlength+1);

						const int prelen=(int)(p-ptr);
						const int postlen=length-(int)(p-ptr);

						memcpy(newptr,ptr,prelen);
						memcpy(newptr+prelen,var,varlength);
						memcpy(newptr+prelen+varlength,p,postlen);
						*(newptr+newlength)='\0';

						//
						printf("  INSERT        \t%s\n",szLibrary);

						//
						const int savelength=saveFile(szFile,newptr,newlength);

						delete [] newptr;
						newptr=NULL;
					}
				}

				delete [] ptr;
				ptr=NULL;

				printf("\n");
			}
		}
	
	return retcode;
}



//
bool addCFMXCFXCLASSPATH(
	const char* szCustomClassPath
	)
{
	printf("::Add CFMX CFX CLASSPATH (%s)\n",szCustomClassPath);

	//
	bool succeed=false;

	//
//	char szHome[MAX_PATH+1];
//	getCFMXRoot(szHome);

	char jvmconfig[MAX_PATH+1];
	_snprintf(jvmconfig,MAX_PATH,"%s\\runtime\\bin\\jvm.config",szCFMXRoot);

	char jvmconfigtmp[MAX_PATH+1];
	_snprintf(jvmconfigtmp,MAX_PATH,"%s\\runtime\\bin\\jvm.config.tmp",szCFMXRoot);

	char jvmconfigbak[MAX_PATH+1];
	_snprintf(jvmconfigbak,MAX_PATH,"%s\\runtime\\bin\\jvm.config.bak",szCFMXRoot);

	printf(" jvm.config     \t%s\n",jvmconfig);

	// c:\\CFusionMX\\cfx\\java,C:\\CFusionMX\\lib\\cfx.jar,
	char tmp[512];
	char szDisplayNamejar[512];
	if(strlen(szCustomClassPath)>0)
		_snprintf(tmp,511,"%s",szCustomClassPath);
	else
		_snprintf(tmp,511,"%s\\cfx\\java,%s\\lib\\cfx.jar,",szCFMXRoot,szCFMXRoot);
	printf(" CFX jar path:\n  %s\n",tmp);
	// double slashes
	int const clen=strlen(tmp)+1;
	for(int i=0,k=0;i<clen;i++,k++)
	{
		szDisplayNamejar[k]=tmp[i];
		if(tmp[i]=='\\')
		{
			szDisplayNamejar[++k]=tmp[i];
		}
	}
	printf(" Double slashed:\n  %s\n\n",szDisplayNamejar);

	//
	const char* sztag="java.class.path=";
	int const len_sztag=strlen(sztag);

	FILE *istream;
	FILE *ostream;
	char line[2048];

	istream = fopen( jvmconfig, "r" );
	ostream = fopen( jvmconfigtmp, "w" );

	if( istream!=NULL)
	{
		if( ostream!=NULL)
		{
			while(feof(istream)==0)
			{
				if( fgets( line, 2047, istream ) == NULL)
					; // printf( "fgets error\n" );
				else
				{
					if(_strnicmp(line,sztag,len_sztag)==0)
					{
						printf("\n>> %s\n\n",line);
						if(strstr(line,szDisplayNamejar)==NULL)
						{
							int len=strlen(line);
							if(line[len-1]=='\r' || line[len-1]=='\n') line[len-1]='\0';
							if(line[len-2]=='\r' || line[len-2]=='\n') line[len-2]='\0';
							len=strlen(line);
							if(line[len-1]!=',')
								strcat(line,",");
							strcat(line,szDisplayNamejar);
							strcat(line,"\r\n");
							printf(" +CFX jar path added to CLASSPATH\n");
							printf("\n<< %s\n\n",line);

							succeed=true;
						}
					}

					fputs(line,ostream);
				}
			}
			fclose( ostream );
		}
		fclose( istream );
	}

	//
	if(succeed)
	{
		if(MoveFileEx(jvmconfig,jvmconfigbak,MOVEFILE_WRITE_THROUGH)==0)
			printf("Backup of %s fails. (%d)\n",jvmconfig,GetLastError());
		if(MoveFileEx(jvmconfigtmp,jvmconfig,MOVEFILE_WRITE_THROUGH)==0)
			printf("Creation of %s fails. (%d)\n",jvmconfigtmp,GetLastError());
	}

	printf("\n");
	return true;
}









///////////////////////////////////////////////////////////////////////////

//
bool isBlueDragonInstalled(
	const char* szKeyName
	)
{
	bool ret=true;

	HKEY hRegKey;
	char subkey[MAX_PATH+1];
	_snprintf(subkey,MAX_PATH,"SOFTWARE\\New Atlanta Communications\\%s",szKeyName);
	long lret=RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_READ|KEY_WOW64_64KEY,&hRegKey);
	if(lret!=ERROR_SUCCESS)
		ret=false;
	else
	{
		DWORD Type=0;
		DWORD cbData=MAX_PATH;
		char szData[MAX_PATH+1];
		long lret2=RegQueryValueEx(hRegKey,"Home",NULL,&Type,(LPBYTE)szData,&cbData);
		if(lret2!=ERROR_SUCCESS)
		{
			ret=false;
		}
	}

	RegCloseKey(hRegKey);

	return ret;
}

//
bool getBlueDragonHome(
	char* szPath,
	const char* szKeyName
	)
{
	bool ret=false;

	szPath[0]='\0';

	//
	HKEY hKey;
	char subkey[MAX_PATH+1];
	_snprintf(subkey,MAX_PATH,"SOFTWARE\\New Atlanta Communications\\%s",szKeyName);
	long lret = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		subkey,
		0, 
		KEY_QUERY_VALUE|KEY_WOW64_64KEY,
		&hKey);
	if(lret != ERROR_SUCCESS)
		ret=false;
	else
	{
		//
		DWORD Type=0;
		DWORD cbData=MAX_PATH;
		char szData[MAX_PATH+1];
		long lret2=RegQueryValueEx(hKey,"Home",NULL,&Type,(LPBYTE)szData,&cbData);
		if(lret2!=ERROR_SUCCESS)
			ret=false;
		else
		{
			// 		
			_snprintf((char *)szPath,MAX_PATH,"%s",szData);
			printf(" BlueDragon Home    \t%s\n",szPath);
		}
	}
	//
	RegCloseKey( hKey );

	//
	return ret;
}


//
int unregisterBlueDragonptr(
	const char* szLibrary,
	const char* szDisplayName,
	char* &ptr,
	int &length
	)
{
	printf("  unregisterBlueDragon \t%p (%d)\n",ptr,length);

	if(ptr==NULL) return 0;

	//
	char lszDisplayName[MAX_PATH];
	strcpy(lszDisplayName,szDisplayName);
	strlwr(lszDisplayName);
	char s1[MAX_PATH+1];
	_snprintf(s1,MAX_PATH,"<mapping name=\"%s\">",lszDisplayName);
	const char* s2="</mapping>";
	char* p1=NULL;
	char* p2=NULL;
	p1=strstr(ptr,s1);
	if(p1!=NULL)
	{
		p2=strstr(p1,s2);
		if(p2==NULL)
		{
			printf("**XML Parse failure (remove).**\n");
			return false;
		}
		else
		{
			p2+=strlen(s2);
			if(*p2=='\r' || *p2=='\n') p2++;
			if(*p2=='\r' || *p2=='\n') p2++;
		}
	}

	//
	if(p1 && p2 && (p1<p2))
	{
		const int varlength=(int)(p2-p1);
		int newlength=length-varlength;
		char* newptr=new char[newlength+1+1];
		if(newptr)
		{
			memset(newptr,0,newlength+1);

			const int prelen=(int)(p1-ptr);
			const int postlen=length-(int)(p2-ptr);

			memcpy(newptr,ptr,prelen);
			memcpy(newptr+prelen,ptr+prelen+varlength,postlen);
			*(newptr+newlength)='\0';
		}

		delete [] ptr;

		ptr=newptr;
		length=newlength;

		newptr=0;
		newlength=0;

		//
		printf("  DELETED\n");
	}

	return length;
}


//
int unregisterBlueDragon(
	const char* szKeyName,
	const char* szLibrary,
	const char* szDisplayName
	)
{
	printf("::Unregister %s\n\t%s\n",szKeyName,szLibrary);

	int retcode=RETCODE_SUCCESS;

	if(szLibrary==NULL)
		;
	else
	{
		if(!isBlueDragonInstalled(szKeyName))
			printf(" %s not found.\n\n",szKeyName);
		else
		{
			char szHome[MAX_PATH+1];
			getBlueDragonHome(szHome,szKeyName);

			char szFile[MAX_PATH+1];
			_snprintf((char *)szFile,MAX_PATH,"%s\\config\\bluedragon.xml",szHome);
			printf(" bluedragon.xml Path \t%s\n",szFile);

			char szFilebak[MAX_PATH+1];
			_snprintf((char *)szFilebak,MAX_PATH,"%s\\config\\bluedragon.xml.bak",szHome);

			char* ptr=NULL;
			int length=getFile(szFile,ptr);

			unregisterBlueDragonptr(szLibrary,szDisplayName,ptr,length);
			if(ptr)
			{
				printf("\n Copying\n  %s\n  to\n  %s\n",szFile,szFilebak);
				if(CopyFile(szFile,szFilebak,FALSE)==0)
				{
					printf(" COPY FAILED! BlueDragon server may have bluedragon.xml locked.\n");
					retcode=RETCODE_FAILURE;
				}
				printf("\n");

				const int savelength=saveFile(szFile,ptr,length);

				delete [] ptr;
				ptr=NULL;

				printf("  DELETED     \t%s\n",szLibrary);
			}

			printf("\n");
		}
	}

	return retcode;
}


//
int registerBlueDragon(
	const char* szKeyName,
	const char* szLibraryOriginal,
	const int iType,
	const char* szDescription,
	const bool bCache,
	const char* szProcedureName,
	const char* szName,
	const char* szDisplayName,
	const char* szClass
	)
{
	//
	printf("::Register %s\n\t%s\n",szKeyName,szLibraryOriginal);

	int retcode=RETCODE_SUCCESS;

	//
	if(szLibraryOriginal==NULL)
		return retcode;

	//
	if(!isBlueDragonInstalled(szKeyName))
			printf(" %s not found.\n\n",szKeyName);
		else
		{
			//
			char szLibrary[MAX_PATH+1];
			strcpy(szLibrary,szLibraryOriginal);

			char szHome[MAX_PATH+1];
			getBlueDragonHome(szHome,szKeyName);

			if(iType==TYPE_JAVA)
			{
				char szNewLibrary[MAX_PATH+1];

				_snprintf(szNewLibrary,MAX_PATH,"%s\\classes\\%s",szHome,szName);
				printf("\n Copying\n  %s\n  to\n  %s\n",szLibrary,szNewLibrary);
				if(CopyFile(szLibrary,szNewLibrary,FALSE)==0)
				{
					printf(" COPY FAILED! BlueDragon server may have file locked.\n");
					retcode=RETCODE_FAILURE;
				}
				printf("\n");

				strcpy(szLibrary,szNewLibrary);
				// 				strcpy(szFolder,szHome);
				printf("  New Library Path    \t%s\n",szLibrary);
			}

			//
			char szFile[MAX_PATH+1];
			_snprintf((char *)szFile,MAX_PATH,"%s\\config\\bluedragon.xml",szHome);
			printf("  bluedragon.xml Path \t%s\n",szFile);

			char szFilebak[MAX_PATH+1];
			_snprintf((char *)szFilebak,MAX_PATH,"%s\\config\\bluedragon.xml.bak",szHome);

			//
			printf("\n Copying\n  %s\n  to\n  %s\n",szFile,szFilebak);
			if(CopyFile(szFile,szFilebak,FALSE)==0)
			{
				printf(" COPY FAILED!\n");
				retcode=RETCODE_FAILURE;
			}
			printf("\n");

			//
			char* ptr=NULL;
			int length=getFile(szFile,ptr);
			unregisterBlueDragonptr(szLibrary,szDisplayName,ptr,length);

			// <classes>classes/</classes>
			if(ptr!=NULL)
			{
				char* p=NULL;
				if(iType==TYPE_JAVA)
				{
					const char *sb="<javacustomtags>";
					p=strstr(ptr,sb);
					if(p==NULL)
					{
						printf("**XML Parse failure. (add)**\n");
						return RETCODE_FAILURE;
					}
					else
					{
						p+=strlen(sb);
						const char* sc="<classes>classes/</classes>";
						p=strstr(p,sc);
						if(p)
							p+=strlen(sc);
						else
						{
							printf("**XML Parse failure. (add 2)**\n");
							return RETCODE_FAILURE;
						}
					}
				}
				else
				{
					const char *sb="<nativecustomtags>";
					p=strstr(ptr,sb);
					if(p==NULL)
					{
						printf("**XML Parse failure. (add)**\n");
						return RETCODE_FAILURE;
					}
					else
						p+=strlen(sb);
				}

				//
				if(p==NULL)
					retcode=RETCODE_FAILURE;
				else
				{
					char lszDisplayName[MAX_PATH];
					strcpy(lszDisplayName,szDisplayName);
					strlwr(lszDisplayName);

					//
					char var[1024];
					if(iType==TYPE_JAVA)
						_snprintf(var,1023,
						"\r\n    <mapping name=\"%s\">\r\n      <name>%s</name>\r\n      <displayname>%s</displayname>\r\n      <description>%s</description>\r\n      <class>%s</class>\r\n    </mapping>",
						lszDisplayName,szDisplayName,szDisplayName,szDescription,szClass);
					else
						_snprintf(var,1023,
						"\r\n    <mapping name=\"%s\">\r\n      <function>%s</function>\r\n      <keeploaded>%s</keeploaded>\r\n      <name>%s</name>\r\n      <module>%s</module>\r\n      <displayname>%s</displayname>\r\n      <description>%s</description>\r\n    </mapping>",
						lszDisplayName,szProcedureName,(bCache==true)?"true":"false",szName,szLibrary,szDisplayName,szDescription);

					//
					int const varlength=strlen(var);
					int newlength=length+varlength;
					char* newptr=new char[newlength+1+1];
					if(newptr)
					{
						memset(newptr,0,newlength+1);

						const int prelen=(int)(p-ptr);
						const int postlen=length-(int)(p-ptr);

						memcpy(newptr,ptr,prelen);
						memcpy(newptr+prelen,var,varlength);
						memcpy(newptr+prelen+varlength,p,postlen);
						*(newptr+newlength)='\0';

						//
						printf("  INSERT        \t%s\n",szLibrary);

						const int savelength=saveFile(szFile,newptr,newlength);

						delete [] newptr;
						newptr=NULL;
					}
				}

				delete [] ptr;
				ptr=NULL;

				printf("\n");
			}
		}
	

	return retcode;
}


//
bool getBlueDragonHomes(
	char* szHome,
	const int ccbHome,
	const int dwIndex
	)
{
	szHome[0]='\0';

	//
	HKEY hKey;
	char subkey[MAX_PATH+1];
	_snprintf(subkey,MAX_PATH,"SOFTWARE\\New Atlanta Communications");
	long lret= RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		subkey,
		0,
		KEY_READ|KEY_WOW64_64KEY, //KEY_QUERY_VALUE,
		&hKey);
	if(lret != ERROR_SUCCESS)
		return false;

	//

	FILETIME ftLastWriteTime;
	long lret2=0;

	DWORD cbHome=ccbHome;
	lret2=RegEnumKeyEx(hKey,dwIndex,szHome,&cbHome,NULL,NULL,NULL,&ftLastWriteTime);

	if(lret2!=ERROR_SUCCESS) return false;
	//		if(lret2==ERROR_NO_MORE_ITEMS) return false;

	printf(" BlueDragon #%d: %s\n",dwIndex+1,szHome);

	//
	RegCloseKey( hKey );

	//
	return true;
}







///////////////////////////////////////////////////////////////////////////
//
int unregisterIgniteFusion(
	const char* szLibrary,
	const char* szDisplayName
	)
{
	printf("::Unregister IgniteFusion \t%s\n",szLibrary);

	return RETCODE_FAILURE;
}


//
int registerIgniteFusion(
	const char* szLibrary,
	const char* szType,
	const char* szDescription,
	const char* szCache,
	const char* szProcedureName,
	const char* szName,
	const char* szDisplayName
	)
{
	printf("::Register IgniteFusion   \t%s\n",szLibrary);

	return RETCODE_FAILURE;
}







///////////////////////////////////////////////////////////////////////////
//
int unregisterRailo(
	const char* szLibrary,
	const char* szDisplayName
	)
{
	printf("::Unregister Railo \t%s\n",szLibrary);

	return RETCODE_FAILURE;
}


//
int registerRailo(
	const char* szLibrary,
	const char* szType,
	const char* szDescription,
	const char* szCache,
	const char* szProcedureName,
	const char* szName,
	const char* szDisplayName
	)
{
	printf("::Register Railo   \t%s\n",szLibrary);

	return RETCODE_FAILURE;
}






///////////////////////////////////////////////////////////////////////////
//
int main(int argc, char* argv[])
{
	//
	printf("\n");
	printf("%s. FREEWARE.\n",szapp_version);
	printf("By Lewis A. Sellers (AKA min), http://www.intrafoundation.com. ? for help.\n");
	printf("\n");

	//
	if(
		argc>=2 &&
		( (stricmp(argv[1],"?")==0) || (stricmp(argv[1],"/?")==0) || (stricmp(argv[1],"/help")==0) )
		)
	{
		//
		printf("See index.html (included with software) for detailed instructions.\n");
		printf("(Or http://www.intrafoundation.com/software/CFXRegServer.htm if you've lost your copy.)\n\n");
		return RETCODE_SUCCESS;
	}

	//
	int i=0;

	//
	detectColdFusionVersion();

	//
	printf("Detected ColdFusion Version: %s %d\n",szcoldfusion_company,ncoldfusion_version);
	printf(" Java CFX supported? %s\n",(bsupport_java?"Yes":"No"));	
	printf(" Regkey: %s\n",(szregkey));	

	//
	szCFMXRoot[0]='\0';
	if(isCFMXInstalled())
		getCFMXRoot(szCFMXRoot);
	
	//
	if(argc>=2 && (stricmp(argv[1],"/cp")==0) )
	{
		if(argc>=3)
			addCFMXCFXCLASSPATH(argv[2]);
		else
			addCFMXCFXCLASSPATH("");
		return RETCODE_SUCCESS;
	}

	// unregister
	if(argc>=3 && (stricmp(argv[1],"/u")==0))
	{
		//
		int returncode=RETCODE_SUCCESS;

		//
		char szLibrary[MAX_PATH+1]; szLibrary[0]='\0';
		CFXTYPE iType=TYPE_CPP; //0=CPP, 1=Java
		char szFolder[MAX_PATH+1]; szFolder[0]='\0';
		char szFile[MAX_PATH+1]; szFile[0]='\0';
		char szName[MAX_PATH+1]; szName[0]='\0';

		//
		strcpy(szLibrary,argv[2]);

		if(strrchr(szLibrary,'\\')==NULL)
		{
			char szCurDir[MAX_PATH+1];
			unsigned int dw=GetCurrentDirectory(MAX_PATH,szCurDir);
			char tmp[MAX_PATH+1];
			_snprintf(tmp,MAX_PATH,"%s\\%s",szCurDir,szLibrary);
			strcpy(szLibrary,tmp);

			printf(" Current directory  \t%s\n",szCurDir);
			printf(" Actual library     \t%s\n\n",szLibrary);
		}

		char *p1=strrchr(szLibrary,'\\');
		char* p2=p2=strrchr(szLibrary,'.');
		if(p1)
		{
			const int flen=(int)(p1-szLibrary);
			strncpy(szFolder,szLibrary,flen);
			*(szFolder+flen)='\0';
		}
		else
			strcpy(szFolder,szLibrary);

		if(p1 && p2 && p1<=p2)
		{
			const int nlen=(int)(p2-p1)-1;
			strcpy(szFile,p1+1);
			strncpy(szName,p1+1,nlen);
			*(szName+nlen)='\0';
		}
		else if(p2)
		{
			const int nlen=(int)(p2-p1)-1;
			strcpy(szFile,szLibrary);
			strncpy(szName,szLibrary,nlen);
			*(szName+nlen)='\0';
		}

		//
		const int len=strlen(szLibrary);
		if(len>4)
		{
			char* p=szLibrary+len-4;
			if(stricmp(p,".dll")==0)
				iType=TYPE_CPP;
			else if(stricmp(p,".jar")==0)
				iType=TYPE_JAVA;
			else
			{
				printf("**Can not register %s. Unknown extension type.**\n",szLibrary);
				return RETCODE_FAILURE;
			}
		}

		//
		char szDisplayName[MAX_PATH+1];
		if(strnicmp(szName,"CFX_",4)==0)
			strcpy(szDisplayName,szName);
		else
			_snprintf(szDisplayName,MAX_PATH,"CFX_%s",szName);

		//
		printf(" Library       \t%s\n",szLibrary);
		printf(" Folder        \t%s\n",szFolder);
		printf(" File          \t%s\n",szFile);
		printf(" Name          \t%s\n",szName);
		printf(" DisplayName   \t%s\n",szDisplayName);
		printf(" Type          \t%s\n",iType==TYPE_JAVA?"Java":"CPP");
		printf(" MAX_PATH      \t%d\n",MAX_PATH);
		printf("\n");

		//
		printf("Unregistering %s\n\n",szDisplayName);

		if(isCFInstalled()) returncode|=unregisterCF(szLibrary,szDisplayName);
		if(isCFMXInstalled()) returncode|=unregisterCFMX(szLibrary,szDisplayName);

		char szHome[MAX_PATH+1];
		i=0;
		while(getBlueDragonHomes(szHome,MAX_PATH,i))
		{
			returncode|=unregisterBlueDragon(szHome,szLibrary,szDisplayName);
			i++;
		}

		//
		printf("\nFini. RETCODE=%d\n",returncode);
		return returncode;
	}

	// register
	if(argc>=2)
	{
		//
		int returncode=RETCODE_SUCCESS;

		//
		char szLibrary[MAX_PATH+1]; szLibrary[0]='\0';
		CFXTYPE iType=TYPE_CPP; //0=CPP, 1=Java
		char szFolder[MAX_PATH+1]; szFolder[0]='\0';
		char szFile[MAX_PATH+1]; szFile[0]='\0';
		char szName[MAX_PATH+1]; szName[0]='\0';

		//
		strcpy(szLibrary,argv[1]);

		if(strrchr(szLibrary,'\\')==NULL)
		{
			char szCurDir[MAX_PATH+1];
			unsigned int dw=GetCurrentDirectory(MAX_PATH,szCurDir);
			char tmp[MAX_PATH+1];
			_snprintf(tmp,MAX_PATH,"%s\\%s",szCurDir,szLibrary);
			strcpy(szLibrary,tmp);

			printf(" Current directory  \t%s\n",szCurDir);
			printf(" Actual library     \t%s\n\n",szLibrary);
		}

		char *p1=strrchr(szLibrary,'\\');
		char* p2=p2=strrchr(szLibrary,'.');
		if(p1)
		{
			const int flen=(int)(p1-szLibrary);
			strncpy(szFolder,szLibrary,flen);
			*(szFolder+flen)='\0';
		}
		else
			strcpy(szFolder,szLibrary);

		if(p1 && p2 && p1<=p2)
		{
			const int nlen=(int)(p2-p1)-1;
			strcpy(szFile,p1+1);
			strncpy(szName,p1+1,nlen);
			*(szName+nlen)='\0';
		}
		else if(p2)
		{
			const int nlen=(int)(p2-p1)-1;
			strcpy(szFile,szLibrary);
			strncpy(szName,szLibrary,nlen);
			*(szName+nlen)='\0';
		}

		//
		const int len=strlen(szLibrary);
		if(len>4)
		{
			char* p=szLibrary+len-4;
			if(stricmp(p,".dll")==0)
				iType=TYPE_CPP;
			else if(stricmp(p,".jar")==0)
				iType=TYPE_JAVA;
			else
			{
				printf("**Can not register %s. Unknown extension type.**\n",szLibrary);
				return RETCODE_FAILURE;
			}
		}

		//
		char szDisplayName[MAX_PATH+1];
		char szProcedureName[256];
		bool bCache;
		char szDescription[MAX_PATH+1];
		char szClass[MAX_PATH+1];

		switch(iType)
		{
		case TYPE_CPP:
			strcpy(szProcedureName,"ProcessTagRequest");
			if(strnicmp(szName,"CFX_",4)==0)
				strcpy(szDisplayName,szName);
			else
				_snprintf(szDisplayName,MAX_PATH,"CFX_%s",szName);
			_snprintf(szDescription,MAX_PATH-1,"%s - Registered by %s http://intrafoundation.com",szDisplayName,szapp_version); //make a default
			szClass[0]='\0';
			bCache=true;
			break;

		case TYPE_JAVA:
			strcpy(szProcedureName,"");
			if(strnicmp(szName,"CFX_",4)==0)
				strcpy(szDisplayName,szName);
			else
				_snprintf(szDisplayName,MAX_PATH,"CFX_%s",szName);
			_snprintf(szDescription,MAX_PATH-1,"%s - Registered by %s http://intrafoundation.com",szDisplayName,szapp_version); //make a default
			szClass[0]='\0';
			bCache=true;
			break;
		}

		/*
		c++: dllpathfilename displayname procedure cache description

		CFX_test.dll
		CFX_test1.dll CFX_TestOne.dll true "my desc" ProcessRequest

		java: jarclasspathfilename displayname class description

		CFX_test3.jar CFX_TestThree.jar "my desc3" com.CFX_TestThree
		CFX_test4.jar CFX_Test4Four.jar "my desc4" com.CFX_TestFour 
		*/

		//
		if(iType==TYPE_CPP)
		{
			if(argc>=3 && stricmp(argv[2],"false")==0)
				bCache=false;
			else
				bCache=true;
			if(argc>=4)
				strcpy(szDisplayName,	argv[3]);
			if(argc>=5)
				strcpy(szDescription,	argv[4]);
			if(argc>=6)
				strcpy(szProcedureName,	argv[5]);
		}

		if(iType==TYPE_JAVA)
		{
			if(argc>=3)
				strcpy(szDisplayName,	argv[2]);

			if(argc>=4)
				strcpy(szDescription,	argv[3]);

			if(argc>=5)
				strcpy(szClass,			argv[4]);
			else
				strcpy(szClass,			argv[1]);
		}

		//
		char *pch=strchr(szClass,'.');
		if(pch) *pch='\0';

		//
		printf(" Library       \t%s\n",szLibrary);
		printf(" Folder        \t%s\n",szFolder);
		printf(" File          \t%s\n",szFile);
		printf(" Name          \t%s\n",szName);
		printf(" DisplayName   \t%s\n",szDisplayName);
		printf(" Type          \t%s\n",iType==TYPE_JAVA?"Java":"CPP");
		printf(" ProcedureName \t%s\n",szProcedureName);
		printf(" Cache         \t%s\n",bCache?"true":"false");
		printf(" Class         \t%s\n",szClass);
		printf(" Description   \t%s\n",szDescription);
		printf(" MAX_PATH      \t%d\n",MAX_PATH);
		printf("\n");

		//
		printf("Registering %s as TYPE=%s\n\n",argv[1],iType==TYPE_JAVA?"Java":"CPP");

		if(isCFInstalled())
			returncode|=registerCF(szLibrary,iType,szDescription,bCache,szProcedureName,szName,szDisplayName);

		if(isCFMXInstalled())
			returncode|=registerCFMX(szLibrary,iType,szDescription,bCache,szProcedureName,szName,szDisplayName,szClass);

		/*
		if(isJRun4CFMXInstalled())
		{
		returncode|=registerJRun4CFMX(szLibrary,iType,szDescription,bCache,szProcedureName,szName,szDisplayName,szClass);
		}
		*/

		char szHome[MAX_PATH+1];
		i=0;
		while(getBlueDragonHomes(szHome,MAX_PATH,i))
		{
			returncode|=registerBlueDragon(szHome,szLibrary,iType,szDescription,bCache,szProcedureName,szName,szDisplayName,szClass);
			i++;
		}

		//
		printf("\nFini. RETCODE=%d\n",returncode);
		return returncode;
	}

	//
	return RETCODE_FAILURE;
}
