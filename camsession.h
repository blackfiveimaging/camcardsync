#ifndef CAMSESSION_H
#define CAMSESSION_H

#include "imagefile.h"
#include "daystamp.h"
#include "fileext.h"
#include "exclusions.h"
#include "progress.h"
#include "configdb.h"

class CamSession;

class CamSessionList : public FileExtHeader, public ExclusionHeader, public ConfigDB
{
	public:
	CamSessionList(ConfigFile *myfile);
	~CamSessionList();
	bool AddPath(const char *srcdir,Progress *p=NULL);
	void AddFile(const char *filename);
	void SetIncludedDefaults(const char *destdir);
	enum ImageFileStatus Copy(const char *dstdir,ImageCopyStats *stats=NULL,Progress *p=NULL);
	CamSession *FindDay(DayStamp &ds);
	CamSession *FirstDay();
	long GetFileSize();
	int GetFileCount();
	void SetExtensions();
	void SetExclusions();
	protected:
	CamSession *firstday;
	friend class CamSession;
	static ConfigTemplate Template[];
};

class CamSession : public ImageFileHeader, public DayStamp
{
	public:
	CamSession(CamSessionList *header,DayStamp &stamp);
	virtual ~CamSession();
	bool GetIncluded();
	void SetIncluded(bool included);
	bool SetIncludedDefault(const char *destdir);
	virtual enum ImageFileStatus Copy(const char *dstdir,ImageCopyStats *stats=NULL,Progress *p=NULL);
	CamSession *NextDay();
	CamSession *PrevDay();
	protected:
	bool included;
	ImageFile *first;
	CamSessionList *header;
	CamSession *next,*prev;
	friend class CamSessionList;
};

#endif
