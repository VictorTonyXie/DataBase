/*
 * rm.h
 *
 *  Created on: 2015年10月26日
 *      Author: QianBenY
 */

#ifndef RM_H_
#define RM_H_

#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include "../common/common.h"
#include <cstring>
#include <iostream>

//
// RM_Record: RM Record interface
//
class RM_Record {
public:
    RM_Record ();
    ~RM_Record();

    // Return the data corresponding to the record.  Sets *pData to the
    // record contents.
    int GetData(char *&pData) const;

    int Set(char *pData, int size, RID rid_);

    // Return the RID associated with the record
    int GetRid (RID &rid) const;
	
//private:
    int recordSize;
    char *data;
    RID rid;
};

class FileHead{			//可以继续加
public:
	FileHead(int recordSize){
		int pageHead = 96;
		int emptyHead = 12;
		int byteNumber = pageHead - emptyHead;
		int bitPerByte = 8;
		int bitPerPage = 8196;
		this->recordSize = recordSize;
		this->pageNumber = 0;
		if (recordSize * byteNumber * bitPerByte < (bitPerPage - pageHead))
			this->recordPerPage = byteNumber * bitPerByte;
		else
			this->recordPerPage = (bitPerPage - pageHead) / recordSize;
		this->recordNumber = 0;
	}
	int recordSize;		//记录长度
	int pageNumber;		//页的个数
	int recordPerPage;	//每页记录个数
	int	recordNumber;	//记录的总数
};

//
// RM_FileHandle: RM File interface
//
class RM_FileHandle {
public:
    RM_FileHandle() : bpm(NULL), fileID(-1) {}
    RM_FileHandle(BufPageManager *bpm_, int fileID_)
    {
    	Open(bpm_, fileID_);
    }
    ~RM_FileHandle() {}

    void Open(BufPageManager *bpm_, int fileID_)
    {
    	bpm = bpm_;
    	fileID = fileID_;
    	int index;
    	fileHead = (FileHead*)(bpm->getPage(fileID, 0, index));
    	recordSize = fileHead->recordSize;
    	recordPerPage = fileHead->recordPerPage;
    	bpm->access(index);
    }

    int getFileID() const
    {
    	return fileID;
    }

    // Given a RID, return the record
    int GetRec     (const RID &rid, RM_Record &rec) const;

    int InsertRec  (const char *pData, RID &rid);       // Insert a new record

    int DeleteRec  (const RID &rid);                    // Delete a record
    int UpdateRec  (const RM_Record &rec);              // Update a record

private:
    BufPageManager *bpm;
    int fileID;
    FileHead *fileHead;
    int recordSize;
    int recordPerPage;
};

class PFS{
public:
	PFS(){
		pageNumber = 1;
		for (int i = 0; i < 8088; i++)
			used[i] = 0;
	}
	~PFS();
	int pageNumber;		//使用的页数
	char head[100];		//随意吧
	char used[8088];	//0--unused, 1--used
};

class RM_Manager {
	FileManager* myFileManager;
	BufPageManager* bpm;
public:
    RM_Manager    (FileManager *pfm, BufPageManager* bpm){
    	myFileManager = pfm;
    	this->bpm = bpm;
    }
    ~RM_Manager   ();

	FileManager* getFileManager(){
		return myFileManager;
	}
	
	BufPageManager* getBufPageManager(){
		return bpm;
	}
    int CreateFile (const char *fileName, int recordSize){
    	int index;
    	int fileID;
    	int pageID = 0;
    	if (myFileManager == NULL)
    		cout << "file not found" << endl;
    	myFileManager->createFile(fileName);
    	myFileManager->openFile(fileName, fileID);
		cout << "log: fileID " <<fileID << endl;
		BufType b = bpm->allocPage(fileID, pageID, index, false);
		memset(b, 0, 8 * 1024);
//		BufType b = bpm->getPage(fileID, pageID, index);
    	bpm->markDirty(index);
    	cout << "log: recordSize " << recordSize <<endl;

    	FileHead *newPage = new FileHead(recordSize);
    	memcpy(b, newPage, sizeof(FileHead));
    	cout << "log: after memcpy" << b[0] << endl;

    	//PFS *pfs = new PFS();
    	//BufType b1 = bpm->allocPage(fileID, pageID + 1, index, false);
    	//memcpy(b1, pfs, sizeof(PFS));
    	b = bpm->getPage(fileID, pageID, index);
    	cout << "log: test page head" << b[0] << endl;
		bpm->close();
    	myFileManager->closeFile(fileID);
    	return 0;
    }
    int DestroyFile(const char *fileName){
    	myFileManager->destroyFile(fileName);
        return 0;
    }
    int OpenFile   (const char *fileName, RM_FileHandle &fileHandle){
    	int fileID;
    	myFileManager->openFile(fileName, fileID);
//		cout << fileName << ' ' << fileID << endl;
    	fileHandle.Open(bpm, fileID);
        return 0;
    }

    int CloseFile  (RM_FileHandle &fileHandle){
    	int fileID = fileHandle.getFileID();
    	bpm->close();
    	myFileManager->closeFile(fileID);
        return 0;
    }
};



#endif /* RECORDMANAGER_RECORDMANAGER_H_ */
