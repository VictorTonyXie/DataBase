#ifndef IX_MANAGER_H_
#define IX_MANAGER_H_

#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include "IX_IndexHandle.h"
#include "IX_IndexScan.h"
#include "IX.h"

class IX_Manager
{
  public:
    IX_Manager   (FileManager *pfm_, BufPageManager* bpm_)
  		: pfm(pfm_), bpm(bpm_) {}
    ~IX_Manager  ();
    int CreateIndex  (const char *fileName,          // Create new index
                     const char *indexName,
                     AttrType   attrType,
                     int        attrLength);
    int DestroyIndex (const char *fileName,          // Destroy index
                     const char *indexName);
    int OpenIndex    (const char *fileName,          // Open index
                     const char *indexName,
                     IX_IndexHandle &indexHandle);
    int CloseIndex   (IX_IndexHandle &indexHandle);  // Close index
  private:
    FileManager *pfm;
    BufPageManager *bpm;
};

#endif
