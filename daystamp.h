#ifndef DAYSTAMP_H
#define DAYSTAMP_H

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

class DayStamp
{
	public:
	DayStamp(const char *filename)
	{
		struct stat statbuf;
		stat(filename,&statbuf);
		tm *mytm=gmtime(&statbuf.st_mtime);
		year=mytm->tm_year+1900;
		month=mytm->tm_mon+1;
		day=mytm->tm_mday;
		dirname=(char *)malloc(12);
		sprintf(dirname,"%04d-%02d-%02d",year,month,day);
	}
	DayStamp(int year,int month,int day)
		: year(year),month(month),day(day), dirname(NULL)
	{
		dirname=(char *)malloc(12);
		sprintf(dirname,"%04d-%02d-%02d",year,month,day);
	}
	DayStamp(const DayStamp &d)
		: year(d.year),month(d.month),day(d.day)
	{
		dirname=strdup(d.dirname);
	}
	DayStamp &operator=(const DayStamp &d)
	{
		year=d.year;
		month=d.month;
		day=d.day;
		if(dirname)
			free(dirname);
		dirname=strdup(d.dirname);
		return(*this);
	}
	bool operator==(const DayStamp &d)
	{
		return((d.year==year)&&(d.month==month)&&(d.day==day));
	}
	~DayStamp()
	{
		if(dirname)
			free(dirname);
	}
	const char *GetDirName()
	{
		return(dirname);
	}
	char *GetDestDirName(const char *destdir)
	{
		char *dir=(char *)malloc(strlen(destdir)+strlen(dirname)+3);
		sprintf(dir,"%s%c%s",destdir,PATH_SEPARATOR,dirname);
		return(dir);
	}
	char *GetDisplayName()
	{
		return(NULL);
	}
	protected:
	int year;
	int month;
	int day;
	char *dirname;
};

#endif
