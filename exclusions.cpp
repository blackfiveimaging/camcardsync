#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "exclusions.h"

using namespace std;


ExclusionHeader::ExclusionHeader()
	: firstexcl(NULL)
{
}


ExclusionHeader::~ExclusionHeader()
{
	while(firstexcl)
		delete firstexcl;
}


Exclusion *ExclusionHeader::FirstExcl()
{
	return(firstexcl);
}


void ExclusionHeader::AddExclusion(const char *excl)
{
	new Exclusion(this,excl);
}


bool ExclusionHeader::MatchExclusion(const char *filename)
{
	Exclusion *fe=FirstExcl();
	while(fe)
	{
		if(fe->MatchExclusion(filename))
			return(true);
		fe=fe->NextExcl();
	}	
	return(false);
}


Exclusion::Exclusion(ExclusionHeader *header,const char *exclusion)
	:	nextexcl(NULL), prevexcl(NULL), header(header), excl(NULL)
{
	excl=strdup(exclusion);

	if((nextexcl=header->firstexcl))
		nextexcl->prevexcl=this;
	header->firstexcl=this;
}


Exclusion::~Exclusion()
{
	if(excl)
		free(excl);
	if(prevexcl)
		prevexcl->nextexcl=nextexcl;
	else
		header->firstexcl=nextexcl;
	if(nextexcl)
		nextexcl->prevexcl=prevexcl;
}


bool Exclusion::MatchExclusion(const char *filename)
{
	return(strcmp(filename,excl)==0);
}


Exclusion *Exclusion::NextExcl()
{
	return(nextexcl);
}


Exclusion *Exclusion::PrevExcl()
{
	return(prevexcl);
}
