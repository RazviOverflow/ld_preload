#ifndef FILEOBJECTINFOS
#define FILEOBJECTINFOS


#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct{
  char *path;
  int inode;
} FileObjectInfo;

typedef struct{
	FileObjectInfo * list;
	size_t used;
	size_t size;
} FileObjectInfos;

FileObjectInfos Initialize(size_t);
void Insert(FileObjectInfos*, char *, ino_t);
void Free(FileObjectInfos *);
int Find(FileObjectInfos *, char* );
//void CreateFileObjectInfo(char*, ino_t); //private

#endif //FILEOBJECTINFOS