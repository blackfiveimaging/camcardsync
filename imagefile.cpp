#include <iostream>

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <sys/stat.h>
#include <sys/types.h>

#include "imagefile.h"

using namespace std;


#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif


ImageFileHeader::ImageFileHeader()
	:	firstimage(NULL)
{
}


ImageFileHeader::~ImageFileHeader()
{
	while(firstimage)
		delete firstimage;
}


ImageFile *ImageFileHeader::FirstImage()
{
	return(firstimage);
}


void ImageFileHeader::AddFile(const char *filename)
{
	new ImageFile(this,filename);
}


enum ImageFileStatus ImageFileHeader::Copy(const char *destdir,ImageCopyStats *stats,Progress *p)
{
	bool skipped=true;
	bool error=false;
	ImageFile *im=FirstImage();
	countcopied=countskipped=counterrored=0;
	while(im)
	{
		switch(im->Copy(destdir,stats))
		{
			case IFS_COPIED:
				++countcopied;
				skipped=false;
				break;
			case IFS_SKIPPED:
				++countskipped;
				break;
			case IFS_ERROR:
				++counterrored;
				error=true;
				break;
			case IFS_CANCELLED:
				return(IFS_CANCELLED);
		}
		if(p)
		{
			if(!p->DoProgress())
				return(IFS_CANCELLED);
		}
		im=im->NextImage();
	}
	if(error)
		return(IFS_ERROR);
	if(skipped)
		return(IFS_SKIPPED);
	return(IFS_COPIED);
}


// Returns IFE_EXISTENT if all the files under this ImageHeader already exist in destdir,
// IFE_NONEXISTENT if none of them do, and
// IFE_PARTIAL if some of them do.
enum ImageFileExistence ImageFileHeader::TestExistence(const char *destdir)
{
	enum ImageFileExistence state=IFE_ERROR;

	ImageFile *im=FirstImage();
	while(im)
	{
		if(im->DestExists(destdir))
		{
			if(state==IFE_ERROR)
				state=IFE_EXISTENT;
			else
				if(state==IFE_NONEXISTENT)
					state=IFE_PARTIAL;
		}
		else
		{
			if(state==IFE_ERROR)
				state=IFE_NONEXISTENT;
			else
				if(state==IFE_EXISTENT)
					state=IFE_PARTIAL;
		}
		im=im->NextImage();
	}
	switch(state)
	{
		case IFE_ERROR:
			cerr << "Scan of " << destdir << " failed" << endl;
			break;
		case IFE_EXISTENT:
			cerr << "Files in " << destdir << " already exist" << endl;
			break;
		case IFE_NONEXISTENT:
			cerr << "No files in " << destdir << " already exist" << endl;
			break;
		case IFE_PARTIAL:
			cerr << "Some files in " << destdir << " already exist" << endl;
			break;
	}
	return(state);
}


long ImageFileHeader::GetFileSize()
{
	// Return total file sizes in kb
	long size=0;
	ImageFile *im=FirstImage();
	while(im)
	{
		size+=im->GetFileSize();
		im=im->NextImage();
	}
	return(size);
}


int ImageFileHeader::GetFileCount()
{
	int count=0;
	ImageFile *im=FirstImage();
	while(im)
	{
		++count;
		im=im->NextImage();
	}
	return(count);
}


ImageFile::ImageFile(ImageFileHeader *header,const char *filename)
	: header(header),included(true),filename(NULL),filesize(0),next(NULL),prev(NULL)
{
	if((next=header->firstimage))
		next->prev=this;
	header->firstimage=this;

	this->filename=strdup(filename);
	struct stat statbuf;
	stat(filename,&statbuf);
	filesize=(statbuf.st_size+1023)/1024;  // Round up to the nearest KB
}


ImageFile::~ImageFile()
{
	if(prev)
		prev->next=next;
	else
		header->firstimage=next;
	if(next)
		next->prev=prev;
	if(filename)
		free(filename);
}


void ImageFile::SetIncluded(bool inc)
{
	included=inc;
}


bool ImageFile::GetIncluded()
{
	return(included);
}


ImageFile *ImageFile::NextImage()
{
	return(next);
}


ImageFile *ImageFile::PrevImage()
{
	return(prev);
}


char *ImageFile::DestFilename(const char *destdir)
{
	char *destfilename=NULL;
	const char *fn=this->filename;
	int n=strlen(filename)-1;
	while(n>0)
	{
		if(fn[n]==PATH_SEPARATOR)
			break;
		--n;
	}
	if(n>0)
		fn+=n+1;

	destfilename=(char *)malloc(strlen(fn)+strlen(destdir)+3);
	sprintf(destfilename,"%s%c%s",destdir,PATH_SEPARATOR,fn);
	return(destfilename);
}


bool ImageFile::DestExists(const char *destdir)
{
	bool result=false;
	struct stat statbuf;
	char *dfn=DestFilename(destdir);
	result=(stat(dfn,&statbuf)==0);
	free(dfn);
	return(result);
}


enum ImageFileStatus ImageFile::Copy(const char *destdir,ImageCopyStats *stats)
{
	enum ImageFileStatus result=IFS_COPIED;
	char *dfn=DestFilename(destdir);

	if(DestExists(destdir))
	{
		result=IFS_SKIPPED;
		if(stats)
			++stats->skipped;
	}
	else
	{
		#define BUFSIZE 16384
		char *buf=(char *)malloc(BUFSIZE);
		FILE *sf=fopen(filename,"rb");
		FILE *df=fopen(dfn,"wb");
		size_t btc;
		do
		{
			btc=fread(buf,1,BUFSIZE,sf);
			fwrite(buf,1,btc,df);		
		} while(btc==BUFSIZE);
		
		fclose(df);
		fclose(sf);
		free(buf);
		if(stats)
		{
			++stats->copied;
			stats->copiedsize+=filesize;
		}
	}
	free(dfn);
	return(result);
}


long ImageFile::GetFileSize()
{
	return(filesize);
}
