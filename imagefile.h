#ifndef IMAGEFILE_H
#define IMAGEFILE_H

#include "progress.h"

class ImageFile;

enum ImageFileStatus {IFS_COPIED,IFS_SKIPPED,IFS_CANCELLED,IFS_ERROR};
enum ImageFileExistence {IFE_EXISTENT,IFE_PARTIAL,IFE_NONEXISTENT,IFE_ERROR};

class ImageCopyStats
{
	public:
	ImageCopyStats() : copied(0), copiedsize(0), skipped(0)
	{}
	~ImageCopyStats()
	{}
	int copied;
	long copiedsize;
	int skipped;
};


class ImageFileHeader
{
	public:
	ImageFileHeader();
	virtual ~ImageFileHeader();
	ImageFile *FirstImage();
	virtual void AddFile(const char *filename);
	virtual enum ImageFileExistence TestExistence(const char *destdir);
	virtual enum ImageFileStatus Copy(const char *destdir,ImageCopyStats *stats=NULL, Progress *p=NULL);
	long GetFileSize();
	int GetFileCount();
	protected:
	int countcopied;
	int countskipped;
	int counterrored;
	ImageFile *firstimage;
	friend class ImageFile;
};


class ImageFile
{
	ImageFile(ImageFileHeader *header,const char *filename);
	~ImageFile();
	void SetIncluded(bool included);
	bool GetIncluded();
	ImageFile *NextImage();
	ImageFile *PrevImage();
	char *DestFilename(const char *destdir);
	bool DestExists(const char *destdir);
	enum ImageFileStatus Copy(const char *destdir,ImageCopyStats *stats=NULL);
	long GetFileSize();
	protected:
	ImageFileHeader *header;
	bool included;
	char *filename;
	long filesize;
	ImageFile *next,*prev;
	friend class ImageFileHeader;
};

#endif
