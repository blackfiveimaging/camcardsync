#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include "camsession.h"
#include "imagefile.h"
#include "pathsupport.h"
#include "configdb.h"

using namespace std;


CamSession::CamSession(CamSessionList *header,DayStamp &stamp)
	: ImageFileHeader(), DayStamp(stamp), included(true),
	header(header),next(NULL),prev(NULL)
{
	if((next=header->firstday))
		next->prev=this;
	header->firstday=this;
}


CamSession::~CamSession()
{
	if(prev)
		prev->next=next;
	else
		header->firstday=next;
	if(next)
		next->prev=prev;
}


CamSession *CamSession::NextDay()
{
	return(next);
}


CamSession *CamSession::PrevDay()
{
	return(prev);
}


enum ImageFileStatus CamSession::Copy(const char *destdir,ImageCopyStats *stats,Progress *p)
{
	if(!included)
		return(IFS_SKIPPED);
	char *dir=GetDestDirName(destdir);

	if(p)
	{
		const char *dn=GetDirName();
		char *message=(char *)malloc(strlen("Copying files from ...")+strlen(dn)+3);
		sprintf(message,"Copying files from %s...",dn);
		p->SetMessage(message);
		free(message);
	}

	struct stat statbuf;
	if(stat(dir,&statbuf)!=0)
	{
		cerr << "Can't stat directory - attempting to create: " << dir << endl;
#ifdef WIN32
		mkdir(dir);
#else
		mkdir(dir,0750);
#endif
	}

	enum ImageFileStatus result=ImageFileHeader::Copy(dir,stats,p);

	free(dir);
	
	return(result);
}


void CamSession::SetIncluded(bool inc)
{
	included=inc;
	// FIXME - loop through the files in the list, disabling them individually.
}


bool CamSession::SetIncludedDefault(const char *destdir)
{
	// We use the following heuristic for deciding whether a camsession should be included by default
	// * If a destination directory for this session doesn't exist, we include the session.
    // * If the directory does exist, but none of the files in this session exist,
	//   we include it.  This catches the case where another cardfull of images from the same date
	//   has already been synced.
	// * If the directory does exist, but some or all of the files in this session exist,
	//   we don't include it.  This catches the case where this session has been synced before but
	//   the user has already gone through and deleted or moved some of the images.  This fails if the
	//   user syncs a card then takes more images on the same day on the same card - but this is only
	//   a default, which the user can over-ride.

	char *dir=GetDestDirName(destdir);
	struct stat statbuf;
	if(stat(dir,&statbuf)!=0)
	{
		cerr << "Directory " << dir << " not found - including in sync" << endl;
		included=true;
	}
	else
	{
		cerr << "Directory " << dir << " exists - checking contents..." << endl;
		included=false;
		// check to see if none of the files in this session already exist.
		switch(TestExistence(dir))
		{
			case IFE_NONEXISTENT:
				included=true;
				break;
			case IFE_EXISTENT:
			case IFE_PARTIAL:
			default:
				included=false;
				break;
		}
	}
	free(dir);
	return(included);
}


bool CamSession::GetIncluded()
{
	return(included);
}

 
CamSessionList::CamSessionList(ConfigFile *myfile)
	: FileExtHeader(), ExclusionHeader(), ConfigDB(Template), firstday(NULL)
{
	new ConfigDBHandler(myfile,"[CamCardSync]",this);
}


CamSessionList::~CamSessionList()
{
	while(firstday)
		delete firstday;
}


void CamSessionList::SetIncludedDefaults(const char *destdir)
{
	CamSession *s=FirstDay();
	while(s)
	{
		s->SetIncludedDefault(destdir);
		s=s->NextDay();
	}
}


bool CamSessionList::AddPath(const char *srcdir,Progress *p)
{
	cerr << "Processing directory " << srcdir << endl;
	DIR *tld=opendir(srcdir);
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
					return(true);
				}
				if(de)
				{
					if(strcmp(".",de->d_name)==0)
						de=NULL;
					else if(strcmp("..",de->d_name)==0)
						de=NULL;
				}
			}

			string fullpath=string(srcdir)+PATH_SEPARATOR+string(de->d_name);

			struct stat statbuf;
			stat(fullpath.c_str(),&statbuf);
			if(S_ISDIR(statbuf.st_mode))
			{
				// Check against exclusion list
				if(!MatchExclusion(de->d_name))
				{
					if(!AddPath(fullpath.c_str(),p))
					{
						closedir(tld);
						return(false);
					}
				}
				else
					cerr << "Excluding: " << de->d_name << endl;
			}
			else
			{
				if(MatchExtension(fullpath.c_str()))
				{
					AddFile(fullpath.c_str());
					if(!p->DoProgress(0,0))
					{
						closedir(tld);
						return(false);
					}
				}
			}
			if(p)
			{
				static int c=0;
				++c;
				if((c&255)==0)
				{
					if(!p->DoProgress(0,0))
					{
						closedir(tld);
						return(false);
					}
				}
			}

		} while(de);
	}
	return(true);
}


void CamSessionList::AddFile(const char *filename)
{
	DayStamp ds(filename);
	CamSession *sd;
	sd=FindDay(ds);
	if(sd)
	{
		sd->AddFile(filename);
	}
	else
	{
		sd=new CamSession(this,ds);
		sd->AddFile(filename);
	}
}


enum ImageFileStatus CamSessionList::Copy(const char *dstdir,ImageCopyStats *stats,Progress *p)
{
	bool error=false;
	bool skipped=true;

	char *dst=substitute_homedir(dstdir);

	struct stat statbuf;
	if(stat(dst,&statbuf)!=0)
	{
		cerr << "Can't stat directory - attempting to create: " << dst << endl;
#ifdef WIN32
		mkdir(dst);
#else
		mkdir(dst,0750);
#endif
	}

	int filecount=GetFileCount();
	if(p)
		p->DoProgress(0,filecount);

	CamSession *sd=FirstDay();
	while(sd)
	{
		switch(sd->Copy(dst,stats,p))
		{
			case IFS_COPIED:
				skipped=false;
				break;
			case IFS_SKIPPED:
				break;
			case IFS_ERROR:
				error=true;
				break;
			case IFS_CANCELLED:
				return(IFS_CANCELLED);
				break;
		}
		sd=sd->NextDay();
	}
	free(dst);
	if(error)
		return(IFS_ERROR);
	if(skipped)
		return(IFS_SKIPPED);
	return(IFS_COPIED);
}


CamSession *CamSessionList::FindDay(DayStamp &ds)
{
	CamSession *sd=FirstDay();
	while(sd)
	{
		if(*sd==ds)
			return(sd);
		sd=sd->NextDay();
	}
	return(NULL);
}


CamSession *CamSessionList::FirstDay()
{
	return(firstday);
}


long CamSessionList::GetFileSize()
{
	long size=0;
	CamSession *sd=FirstDay();
	while(sd)
	{
		if(sd->GetIncluded())
		{
			size+=sd->GetFileSize();
			sd=sd->NextDay();
		}
	}
	return(size);
}


int CamSessionList::GetFileCount()
{
	int count=0;
	CamSession *sd=FirstDay();
	while(sd)
	{
		if(sd->GetIncluded())
		{
			count+=sd->GetFileCount();
		}
		sd=sd->NextDay();
	}
	return(count);
}


void CamSessionList::SetExtensions()
{
	FileExtension *ext;
	while((ext=FirstExt()))
		delete ext;

	const char *es=FindString("Extensions");
	char *e=strdup(es);
	char *ep=e;
	char *e2=e;
	while(*e2)
	{
		while((*e2) && (*e2!=':'))
			++e2;
		if(*e2)
		{
			*e2=0;
			AddExtension(ep);
			cerr << "Adding extension: " << ep << endl;
			ep=e2=e2+1;
		}
		else
		{
			AddExtension(ep);
			cerr << "Adding extension: " << ep << endl;
		}
	}
	free(e);
}


void CamSessionList::SetExclusions()
{
	Exclusion *excl;
	while((excl=FirstExcl()))
		delete excl;

	const char *es=FindString("Exclusions");
	char *e=strdup(es);
	char *ep=e;
	char *e2=e;
	while(*e2)
	{
		while((*e2) && (*e2!=':'))
			++e2;
		if(*e2)
		{
			*e2=0;
			AddExclusion(ep);
			cerr << "Adding exclusion: " << ep << endl;
			ep=e2=e2+1;
		}
		else
		{
			AddExclusion(ep);
			cerr << "Adding exclusion: " << ep << endl;
		}
	}
	free(e);
}


ConfigTemplate CamSessionList::Template[]=
{
        ConfigTemplate("DefaultSourcePath","/media/camera"),
        ConfigTemplate("DestPath","$HOME/Photos"),
        ConfigTemplate("Extensions","jpg:tif:tiff:raw:nef"),
        ConfigTemplate("Exclusions",""),
        ConfigTemplate() // NULL terminated...
};
