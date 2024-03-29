/*
 * progresstext.h - progress displays.
 * By default, provides "twirling baton" display on stderr.
 *
 * Copyright (c) 2007 by Alastair M. Robinson
 * Distributed under the terms of the GNU General Public License -
 * see the file named "COPYING" for more details.
 *
 */

#ifndef PROGRESSTEXT_H
#define PROGRESSTEXT_H

#include <iostream>

#include "progress.h"

using namespace std;

class ProgressText : public Progress
{
	public:
	ProgressText() : counter(0), counter2(0), message(NULL)
	{
	}
	virtual ~ProgressText()
	{
		if(message)
			free(message);
	}
	virtual bool DoProgress(int i, int maxi)
	{
		Progress::DoProgress(i,maxi);
		char *baton="/-\\|";
		if((counter==0) || (i==maxi))
		{
			if(i>maxi)
				i=maxi;
			if(message)
				cerr << message << " ";
			cerr << baton[counter2] << " " << ((i+1)*100)/maxi << "%\r";
			if(i==maxi)
				cerr << endl;
			cerr.flush();
			++counter2;
			counter2&=3;
		}
		++counter;
		int mod=maxi/500;
		if(!mod)
			mod=1;
		counter%=mod;
		return(true);
	}
	virtual void SetMessage(const char *msg)
	{
		if(message)
			free(message);
		message=NULL;
		if(msg)
			message=strdup(msg);
	}
	private:
	int counter;
	int counter2;
	int limit;
	char *message;
};

#endif
