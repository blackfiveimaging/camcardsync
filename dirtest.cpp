#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include "fileext.h"

using namespace std;

#ifdef WIN32
#define PATH_SEPARATOR '\'
#else
#define PATH_SEPARATOR '/'
#endif

void recursivedir(FileExtHeader &feheader,const char *toplevel)
{	
	DIR *tld=opendir(toplevel);
	if(tld)
	{
		struct dirent *de=NULL;
		do
		{
			de=NULL;
			while(!de)
			{
				de=readdir(tld);
				if(!de)
				{
					closedir(tld);
					return;
				}
				if(de)
				{
					if(strcmp(".",de->d_name)==0)
						de=NULL;
					else if(strcmp("..",de->d_name)==0)
						de=NULL;
				}
			}

			string tl(toplevel);
			string dn(de->d_name);
			string fullpath=tl+PATH_SEPARATOR+dn;

			struct stat statbuf;
			stat(fullpath.c_str(),&statbuf);
			if(S_ISDIR(statbuf.st_mode))
			{
//				cerr << "Entering directory: " << fullpath << endl;
				recursivedir(feheader,fullpath.c_str());
			}
			else
			{
				tm *mytm=gmtime(&statbuf.st_mtime);
				if(feheader.Match(fullpath.c_str()))
				{
					cerr << "File: " << fullpath << endl;
					cerr << "Modified on: " << mytm->tm_year+1900 << "-" << mytm->tm_mon+1 << "-" << mytm->tm_mday << endl;
					cerr << "File size: " << statbuf.st_size << endl;
				}
			}

		} while(de);
	}
}


int main(int argc,char **argv)
{
	if(argc==2)
	{
		FileExtHeader feheader;
		feheader.Add("BMP");
		feheader.Add("JPG");
		char *tld=argv[1];
		recursivedir(feheader,tld);
	}
}
