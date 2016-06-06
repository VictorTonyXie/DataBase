/*
 * IX_Index.h
 *
 *  Created on: Dec 23, 2015
 *      Author: thomas
 */

#ifndef INDEXMANAGER_IX_INDEXSCAN_H_
#define INDEXMANAGER_IX_INDEXSCAN_H_

#include "../common/common.h"
#include "IX_IndexHandle.h"
#include <cstdio>

class IX_IndexScan
{
  public:
       IX_IndexScan  () : data(NULL) {}                  // Constructor
       ~IX_IndexScan () {}                               // Destructor
    int OpenScan      (IX_IndexHandle &indexHandle, // Initialize index scan
                      CompOp      compOp,
                      void        *value);
    int GetNextEntry  (RID &rid);                         // Get next matching entry
    int CloseScan     ();                                 // Terminate index scan
  private:
    CompOp    myCompOp;
    int       attrType;
    int       attrLength;
    char      *data;
    IX_IndexHandle *ixhandle;
    RID       indexID;
    void copyData(void* pData);
    int compareData(char* b);
};


#endif /* INDEXMANAGER_IX_INDEXSCAN_H_ */
