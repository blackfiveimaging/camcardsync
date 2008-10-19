#include <iostream>
#include <string>
#include "fileext.h"

using namespace std;


FileExtHeader::FileExtHeader()
	: firstfileext(NULL)
{
}


FileExtHeader::~FileExtHeader()
{
	FileExtension *ext;
	while(firstfileext)
		delete firstfileext;
}


FileExtension *FileExtHeader::FirstExt()
{
	return(firstfileext);
}


void FileExtHeader::AddExtension(const char *ext)
{
	new FileExtension(this,ext);
}


bool FileExtHeader::MatchExtension(const char *filename)
{
	FileExtension *fe=FirstExt();
	while(fe)
	{
		if(fe->MatchExtension(filename))
			return(true);
		fe=fe->NextExt();
	}	
	return(false);
}


FileExtension::FileExtension(FileExtHeader *header,const char *extension)
	:	next(NULL), prev(NULL), header(header), ext(NULL)
{
	while(extension[0]=='.')
		++extension;
	ext=strdup(extension);

	if((next=header->firstfileext))
		next->prev=this;
	header->firstfileext=this;
}


FileExtension::~FileExtension()
{
	if(ext)
		free(ext);
	if(prev)
		prev->next=next;
	else
		header->firstfileext=next;
	if(next)
		next->prev=prev;
}


bool FileExtension::MatchExtension(const char *filename)
{
	if(!filename)
		return(false);
	int n=strlen(filename);
	while(n)
	{
		if(filename[n]=='.')
		{
			if((strcasecmp(&filename[n+1],ext)==0))
				return(true);
			else
				return(false);
		}
		--n;		
	}
	return(false);
}


FileExtension *FileExtension::NextExt()
{
	return(next);
}


FileExtension *FileExtension::PrevExt()
{
	return(prev);
}
