#ifndef EXCLUSIONS_H
#define EXCLUSIONS_H

class Exclusion;

class ExclusionHeader
{
	public:
	ExclusionHeader();
	~ExclusionHeader();
	void AddExclusion(const char *ext);
	bool MatchExclusion(const char *filename);
	Exclusion *FirstExcl();
	protected:
	Exclusion *firstexcl;
	friend class Exclusion;
};


class Exclusion
{
	public:
	Exclusion(ExclusionHeader *header,const char *excl);
	~Exclusion();
	bool MatchExclusion(const char *ext);
	Exclusion *NextExcl();
	Exclusion *PrevExcl();
	protected:
	Exclusion *nextexcl,*prevexcl;
	ExclusionHeader *header;
	char *excl;
	friend class ExclusionHeader;
};

#endif
