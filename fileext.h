#ifndef FILEEXT_H
#define FILEEXT_H

class FileExtension;

class FileExtHeader
{
	public:
	FileExtHeader();
	~FileExtHeader();
	void AddExtension(const char *ext);
	bool MatchExtension(const char *filename);
	FileExtension *FirstExt();
	protected:
	FileExtension *firstfileext;
	friend class FileExtension;
};


class FileExtension
{
	public:
	FileExtension(FileExtHeader *header,const char *ext);
	~FileExtension();
	bool MatchExtension(const char *ext);
	FileExtension *NextExt();
	FileExtension *PrevExt();
	protected:
	FileExtension *next,*prev;
	FileExtHeader *header;
	char *ext;
	friend class FileExtHeader;
};

#endif
