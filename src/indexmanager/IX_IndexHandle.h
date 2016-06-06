/*
 * IX_IndexHandle.h
 *
 *  Created on: Dec 23, 2015
 *      Author: thomas
 */

#ifndef INDEXMANAGER_IX_INDEXHANDLE_H_
#define INDEXMANAGER_IX_INDEXHANDLE_H_

#include "../bufmanager/BufPageManager.h"
#include "IX.h"

class IX_IndexHandle
{
  public:
	IX_IndexHandle  () {}
    IX_IndexHandle  (BufPageManager *bpm_, int fileID_)
  	{
    	 bpm = bpm_;
    	 fileID = fileID_;
    	 filehead = (IX_FileHead*)(bpm->getPage(fileID, 0, fhindex));
    	 bpm->access(fhindex);
    	 maxN = filehead->maxN;
    	 attrType = filehead->attrType;
    	 attrLength = filehead->attrLength;
    	 dataInt = 0;
    	 dataFloat = 0;
    	 dataString = NULL;
    	 data = NULL;
  	}
    ~IX_IndexHandle () {}                               // Destructor
    int InsertEntry     (void *pData, const RID &rid);  // Insert new index entry
    int DeleteEntry     (void *pData, const RID &rid);  // Delete index entry
    int ForcePages      ();                             // Copy index to disk
    void PrintEntries   ();
    int GetLowerBound   (void *pData, RID &indexid);
    int GetUpperBound   (void *pData, RID &indexid);
    int GetFirst        (RID &indexid);
    int GetNextIndexID  (RID &indexid, RID &indexid2);
    int GetRID          (RID &indexid, RID &rid);
    char *GetKey        (RID &indexid);

    int getAttrType()   { return attrType; }
    int getAttrLength() { return attrLength; }
    int getFileID()     { return fileID; }

  private:
    BufPageManager  *bpm;
    int             fileID;
    IX_FileHead     *filehead;
    int             fhindex;
    int             maxN;
    AttrType        attrType;
    int             attrLength;
    char            *data;
    int             dataInt;
    float           dataFloat;
    char            *dataString;
    RID             dataRID;

    int allocPage();
    RID* getP(char* b, int i);
    char* getK(char* b, int i);
    bool fillinData(char* dst);
    bool fillinData(char* dst, char* src);
    bool lessThanData(char* b);
    bool equalData(char* b);
    char* getMaxData(int pageID);

    int insertDFS(int pageID);
    bool insertSlot(char* b, int slotID, char *data, RID &rid);
    bool splitPage(int pageID1, int pageID2);

    void printDFS(int pageID);

    int deleteDFS(int pageID);
    bool deleteSlot(char* b, int slotID);

    void copyData(void *pData);

    RID searchDFS(int pageID);
    int searchCMP; // 0 for lowerbound, 1 for upperbound
};


#endif /* INDEXMANAGER_IX_INDEXHANDLE_H_ */
