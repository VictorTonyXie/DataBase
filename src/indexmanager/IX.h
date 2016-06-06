/*
 * IX.h
 *
 *  Created on: Dec 24, 2015
 *      Author: thomas
 */

#ifndef INDEXMANAGER_IX_H_
#define INDEXMANAGER_IX_H_


#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include "../common/common.h"

struct IX_FileHead
{
	AttrType attrType;
	int      attrLength;
	int      maxN;
	int      pageNum;
	int      firstEmptyPage;
	int      root;
};

enum IX_PageType {
	NODE,
	LEAF
};

struct IX_PageHead
{
	IX_PageType type;  // 0 for node, 1 for leaf
	int n;
};


#endif /* INDEXMANAGER_IX_H_ */
