#include "IX_Manager.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
using namespace std;

int IX_Manager::CreateIndex(const char *fileName, const char *indexName, AttrType attrType, int attrLength)
{
	int fileID;
	char buf[100];
	memset(buf, 0, sizeof buf);
	sprintf(buf, "%s.%s.index", fileName, indexName);
	if (access(buf, 0) != -1) {
		return -1;
	}
	pfm->createFile(buf);
	pfm->openFile(buf, fileID);

	IX_FileHead filehead;
	filehead.attrType = attrType;
	filehead.attrLength = attrLength;
	filehead.maxN = 8000 / (attrLength + sizeof(RID)) - 2;
	filehead.pageNum = 1;
	filehead.firstEmptyPage = 1;
	filehead.root = -1;

	int index;
	char* b = (char*)bpm->allocPage(fileID, 0, index);
	memcpy(b, (char*)(&filehead), sizeof(IX_FileHead));
	bpm->markDirty(index);

	bpm->close();
	pfm->closeFile(fileID);

	return 0;
}

int IX_Manager::DestroyIndex(const char *fileName, const char *indexName)
{
	char buf[100];
	memset(buf, 0, sizeof buf);
	sprintf(buf, "%s.%s.index", fileName, indexName);
	pfm->destroyFile(buf);
	return 0;
}

int IX_Manager::OpenIndex(const char *fileName, const char *indexName, IX_IndexHandle &indexHandle)
{
	int fileID;
	char buf[100];
	memset(buf, 0, sizeof buf);
	sprintf(buf, "%s.%s.index", fileName, indexName);
	pfm->openFile(buf, fileID);
	indexHandle = IX_IndexHandle(bpm, fileID);
	return 0;
}

int IX_Manager::CloseIndex(IX_IndexHandle &indexHandle)
{
	bpm->close();
	pfm->closeFile(indexHandle.getFileID());
	return 0;
}





int IX_IndexHandle::allocPage()
{
	int ans = filehead->firstEmptyPage;
    if (filehead->firstEmptyPage == filehead->pageNum)
    	filehead->firstEmptyPage++;
    else
    {
    	int index;
    	char* b = (char*)bpm->getPage(fileID, filehead->firstEmptyPage, index);
    	filehead->firstEmptyPage = *((int*)b);
    }
    filehead->pageNum++;
    bpm->markDirty(fhindex);
    return ans;
}

RID* IX_IndexHandle::getP(char* b, int i)
{
	return (RID*)(b + sizeof(IX_PageHead) + i * (sizeof(RID) + attrLength));
}

char* IX_IndexHandle::getK(char* b, int i)
{
	return b + sizeof(IX_PageHead) + i * (sizeof(RID) + attrLength) + sizeof(RID);
}

bool IX_IndexHandle::fillinData(char* dst, char* src)
{
	memcpy(dst, src, attrLength);
	return true;
}

bool IX_IndexHandle::fillinData(char *dst)
{
	if (attrType == MyINT)
		memcpy(dst, &dataInt, 4);
	if (attrType == FLOAT)
		memcpy(dst, &dataFloat, 4);
	if (attrType == STRING)
		memcpy(dst, &dataString, attrLength);
	return true;
}

bool IX_IndexHandle::lessThanData(char* b)
{
	if (attrType == MyINT)
	{
		int x = *((int*)b);
		return x < dataInt;
	}
	if (attrType == FLOAT)
	{
		float x = *((float*)b);
		return x < dataFloat;
	}
	char *buf = new char[attrLength + 5];
	memcpy(buf, b, attrLength);
	buf[attrLength] = 0;
	bool ans = (strcmp(buf, dataString) < 0);
	delete buf;
	return ans;
}

bool IX_IndexHandle::equalData(char* b)
{
	if (attrType == MyINT)
	{
		int x = *((int*)b);
		return x == dataInt;
	}
	if (attrType == FLOAT)
	{
		float x = *((float*)b);
		return x == dataFloat;
	}
	char *buf = new char[attrLength + 5];
	memcpy(buf, b, attrLength);
	buf[attrLength] = 0;
	return (strcmp(buf, dataString) == 0);
}

char* IX_IndexHandle::getMaxData(int pageID)
{
	int index;
	char* b = (char*)bpm->getPage(fileID, pageID, index);
	bpm->access(index);
	IX_PageHead* pagehead = (IX_PageHead*)b;
	if (pagehead->type == LEAF)
		return getK(b, pagehead->n - 1);
	else
		return getK(b, pagehead->n);
}

enum DFSstatus {
	NOTHING,
	MODIFY,
	SPLIT,
	MERGE
};


void IX_IndexHandle::copyData(void *pData)
{
	if (attrType == MyINT)
	{
		dataInt = *((int*)pData);
		data = (char*)(&dataInt);
	}
	if (attrType == FLOAT)
	{
		dataFloat = *((float*)pData);
		data = (char*)(&dataFloat);
	}
	if (attrType == STRING)
	{
		if (dataString != NULL)
			delete dataString;
		dataString = new char[attrLength + 5];
		memcpy(dataString, pData, attrLength);
		dataString[attrLength] = 0;
		data = dataString;
	}
}

int IX_IndexHandle::InsertEntry(void *pData, const RID &rid)
{
	copyData(pData);
	dataRID= rid;

	bpm->access(fhindex);
	if (filehead->root == -1)
	{
		filehead->root = allocPage();
		int index;
		char* b = (char*)bpm->getPage(fileID, filehead->root, index);
		bpm->markDirty(index);
		IX_PageHead *pagehead = (IX_PageHead*)b;
		pagehead->n = 1;
		pagehead->type = LEAF;
		RID* p = getP(b, 0);
		(*p) = rid;
		char* k = getK(b, 0);
		fillinData(k);
		p = getP(b, 1);
		(*p) = RID(-1, -1);
	}
	else
	{
		//depth = 0;
		if (insertDFS(filehead->root) == SPLIT)
		{
			int newpage = allocPage();
			splitPage(filehead->root, newpage);
			int newroot = allocPage();
			int index;
			char* b = (char*)bpm->getPage(fileID, newroot, index);
			//printf("%lld\n", (long long)(b));
			bpm->markDirty(index);
			IX_PageHead *pagehead = (IX_PageHead*)b;
			pagehead->n = 1;
			pagehead->type = NODE;
			RID *p = getP(b, 0);
			(*p) = RID(filehead->root, -1);
			p = getP(b, 1);
			(*p) = RID(newpage, -1);
			char *k = getK(b, 0);
			fillinData(k, getMaxData(filehead->root));
			filehead->root = newroot;
		}
	}
	return 0;
}


int IX_IndexHandle::insertDFS(int pageID)
{
	int index;
	char* b = (char*)bpm->getPage(fileID, pageID, index);
	bpm->access(index);
	IX_PageHead *pagehead = (IX_PageHead*)b;
	if (pagehead->type == LEAF) // leaf
	{
		int slot = pagehead->n;
		for (int i = 0; i < pagehead->n; i++)
		{
			char *k = getK(b, i);
			if (lessThanData(k) == false)
			{
				slot = i;
				break;
			}
		}
		insertSlot(b, slot, data, dataRID);
		bpm->markDirty(index);
		if (pagehead->n > filehead->maxN)
			return SPLIT;
		return NOTHING;
	}
	else // node
	{
		int slot = pagehead->n;
		for (int i = 0; i < pagehead->n; i++)
		{
			char *k = getK(b, i);
			if (lessThanData(k) == false)
			{
				slot = i;
				break;
			}
		}
		RID *rid = getP(b, slot);
		int status = insertDFS(rid->Page());
		if (status == SPLIT)
		{
			int newpage = allocPage();
			splitPage(rid->Page(), newpage);
			RID newrid = RID(newpage, -1);
			insertSlot(b, slot, getMaxData(rid->Page()), *rid);
			*getP(b, slot + 1) = newrid;
			bpm->markDirty(index);
			if (pagehead->n > filehead->maxN)
				return SPLIT;
		}
		return NOTHING;
	}
	return NOTHING;
}

bool IX_IndexHandle::insertSlot(char* b, int slotID, char *data, RID &rid)
{
	IX_PageHead *pagehead = (IX_PageHead*)b;
	RID *r1 = getP(b, pagehead->n + 1);
	RID *r2 = getP(b, pagehead->n);
	*r1 = (*r2);
	for (int i = (pagehead->n) - 1; i >= slotID; i--)
	{
		r1 = getP(b, i + 1);
		r2 = getP(b, i);
		*r1 = (*r2);
		fillinData(getK(b, i + 1), getK(b, i));
	}
	pagehead->n++;
	r1 = getP(b, slotID);
	*r1 = rid;
	fillinData(getK(b, slotID), data);
	return true;
}

bool IX_IndexHandle::splitPage(int pageID1, int pageID2)
{
	int index;
	char* b1 = (char*)bpm->getPage(fileID, pageID1, index);
	bpm->markDirty(index);
	char* b2 = (char*)bpm->getPage(fileID, pageID2, index);
	bpm->markDirty(index);
	//printf("%lld\n", (long long)(b1));
	//printf("%lld\n", (long long)(b2));
	IX_PageHead *pagehead1 = (IX_PageHead*)b1;
	IX_PageHead *pagehead2 = (IX_PageHead*)b2;
	if (pagehead1->type == LEAF)
	{
		pagehead2->n = pagehead1-> n / 2;
		pagehead2->type = pagehead1->type;
		pagehead1->n -= pagehead2->n;
		memcpy(b2 + sizeof(IX_PageHead), getP(b1, pagehead1->n), pagehead2->n * (sizeof(RID) + attrLength) + sizeof(RID));
		RID *rid = getP(b1, pagehead1->n);
		*rid = RID(pageID2, -1);
	}
	else
	{
		pagehead2->n = pagehead1->n / 2;
		pagehead2->type = pagehead1->type;
		pagehead1->n -= pagehead2->n + 1;
		memcpy(b2 + sizeof(IX_PageHead), getP(b1, pagehead1->n + 1), (pagehead2->n + 1) * (sizeof(RID) + attrLength));
	}
	//printf("%d %d %d %d\n", pagehead1->type, pagehead1->n, pagehead2->type, pagehead2->n);
	return true;
}

void IX_IndexHandle::PrintEntries()
{
	printf("root: %d\n", filehead->root);
	printDFS(filehead->root);
	printf("\n");
}

void IX_IndexHandle::printDFS(int pageID)
{
	int index;
	char* b = (char*)bpm->getPage(fileID, pageID, index);
	bpm->access(index);
	IX_PageHead *pagehead = (IX_PageHead*)b;
	if (pagehead->type == NODE)
	{
		for (int i = 0; i <= pagehead->n; i++)
		{
			RID *r = getP(b, i);
			//printf("%d\n", r->Page());
			printDFS(r->Page());
		}
	}
	else
	{
		for (int i = 0; i < pagehead->n; i++)
		{
			char *k = getK(b, i);
			if (attrType == MyINT)
				printf("%d ", *((int*)k));
			if (attrType == FLOAT)
				printf("%f ", *((float*)k));
			if (attrType == STRING)
				printf("%s\n", k);
		}
	}
}

int IX_IndexHandle::ForcePages()
{
	bpm->close();
	return 0;
}

int IX_IndexHandle::DeleteEntry(void *pData, const RID &rid)
{
	bpm->access(fhindex);
	//printf("begin delete %d\n", filehead->root);
	if (filehead->root == -1)
		return -1;
	copyData(pData);
	dataRID= rid;
	//printf("begin dfs\n");
	return deleteDFS(filehead->root);
}

int IX_IndexHandle::deleteDFS(int pageID)
{
	int index;
	char* b = (char*)bpm->getPage(fileID, pageID, index);
	bpm->access(index);
	IX_PageHead *pagehead = (IX_PageHead*)b;
	//printf("DFS: %d, %d\n", pageID, pagehead->type);
	if (pagehead->type == NODE)
	{
		int slot = pagehead->n;
		for (int i = 0; i < pagehead->n; i++)
		{
			char *k = getK(b, i);
			if (lessThanData(k) == false)
			{
				slot = i;
				break;
			}
		}
		//printf("slot: %d\n", slot);
		RID *rid = getP(b, slot);
		//printf("page: %d / %d\n", rid->Page(), filehead->pageNum);
		return deleteDFS(rid->Page());
	}
	else
	{
		while (true)
		{
			for (int i = 0; i < pagehead->n; i++)
			{
				char *k = getK(b, i);
				if (lessThanData(k) == false)
				{
					if (equalData(k) == false)
						return -1;
					RID *rid = getP(b, i);
					if (dataRID == *rid)
					{
						deleteSlot(b, i);
						bpm->markDirty(index);
						return 0;
					}
				}
			}
			RID *rid = getP(b, pagehead->n);
			if (rid->Page() == -1)
				return -1;
			pageID = rid->Page();
			b = (char*)bpm->getPage(fileID, pageID, index);
			bpm->access(index);
			pagehead = (IX_PageHead*)b;
		}
	}
	return 0;
}

bool IX_IndexHandle::deleteSlot(char* b, int slotID)
{
	IX_PageHead *pagehead = (IX_PageHead*)b;
	for (int i = slotID; i < pagehead->n; i++)
	{
		RID *r1 = getP(b, i);
		RID *r2 = getP(b, i + 1);
		*r1 = (*r2);
		fillinData(getK(b, i), getK(b, i + 1));
	}
	pagehead->n--;
	return true;
}

int IX_IndexHandle::GetLowerBound(void *pData, RID &indexid)
{
	searchCMP = 0;
	copyData(pData);
	if (filehead->root == -1)
		return -1;
	bpm->access(fhindex);
	//printf("getlowerbound\n");
	indexid = searchDFS(filehead->root);
	if (indexid.Page() == -1)
		return -1;
	return 0;
}

int IX_IndexHandle::GetUpperBound(void *pData, RID &indexid)
{
	searchCMP = 1;
	copyData(pData);
	if (filehead->root == -1)
		return -1;
	bpm->access(fhindex);
	indexid = searchDFS(filehead->root);
	if (indexid.Page() == -1)
		return -1;
	return 0;
}

RID IX_IndexHandle::searchDFS(int pageID)
{
	int index;
	char* b = (char*)bpm->getPage(fileID, pageID, index);
	bpm->access(index);
	IX_PageHead *pagehead = (IX_PageHead*)b;
	if (pagehead->type == NODE)
	{
		int slot = pagehead->n;
		for (int i = 0; i < pagehead->n; i++)
		{
			char *k = getK(b, i);
			if ((searchCMP == 0 && lessThanData(k) == false)
			  ||(searchCMP == 1 && lessThanData(k) == false && equalData(k) == false))
			{
				slot = i;
				break;
			}
		}
		RID *rid = getP(b, slot);
		return searchDFS(rid->Page());
	}
	else
	{
		while (true)
		{
			for (int i = 0; i < pagehead->n; i++)
			{
				char *k = getK(b, i);
				if ((searchCMP == 0 && lessThanData(k) == false)
				  ||(searchCMP == 1 && lessThanData(k) == false && equalData(k) == false))
				{
					return RID(pageID, i);
				}
			}
			RID *rid = getP(b, pagehead->n);
			if (rid->Page() == -1) {
				return RID(-1, -1);
			}
			pageID = rid->Page();
			b = (char*)bpm->getPage(fileID, pageID, index);
			bpm->access(index);
			pagehead = (IX_PageHead*)b;
		}
	}
	return RID(-1, -1);
}

int IX_IndexHandle::GetNextIndexID(RID &indexid, RID &indexid2)
{
	int pageID = indexid.Page();
	int index;
	char* b = (char*)bpm->getPage(fileID, pageID, index);
	bpm->access(index);
	IX_PageHead *pagehead = (IX_PageHead*)b;
	if (indexid.Slot() + 1 < pagehead->n)
	{
		indexid2 = RID(pageID, indexid.Slot() + 1);
		return 0;
	}
	else
	{
		while (true)
		{
			RID *rid = getP(b, pagehead->n);
			pageID = rid->Page();
			if (pageID == -1)
			{
				indexid2 = RID(-1, -1);
				return -1;
			}
			b = (char*)bpm->getPage(fileID, pageID, index);
			bpm->access(index);
			pagehead = (IX_PageHead*)b;
			if (pagehead->n > 0)
			{
				indexid2 = RID(pageID, 0);
				return 0;
			}
		}
	}
}

int IX_IndexHandle::GetRID(RID &indexid, RID &rid)
{
	int pageID = indexid.Page();
	if (pageID == -1)
		return -1;
	int index;
	char* b = (char*)bpm->getPage(fileID, pageID, index);
	bpm->access(index);
	rid = *getP(b, indexid.Slot());
	return 0;
}

char* IX_IndexHandle::GetKey(RID &indexid)
{
	int pageID = indexid.Page();
	int index;
	char* b = (char*)bpm->getPage(fileID, pageID, index);
	bpm->access(index);
	return getK(b, indexid.Slot());
}

int IX_IndexHandle::GetFirst(RID &indexid)
{
	int pageID = filehead->root;
	bpm->access(fhindex);
	while (true)
	{
		if (pageID == -1)
			return -1;
		int index;
		char *b = (char*)bpm->getPage(fileID, pageID, index);
		bpm->access(index);
		IX_PageHead *pagehead = (IX_PageHead*)b;
		if (pagehead->type == NODE)
			pageID = getP(b, 0)->Page();
		else
		{
			if (pagehead->n > 0)
			{
				indexid = RID(pageID, 0);
				return 0;
			}
			pageID = getP(b, pagehead->n)->Page();
		}
	}
	return -1;
}





void IX_IndexScan::copyData(void *pData)
{
	if (data != NULL)
		delete data;
	data = new char[attrLength + 5];
	memcpy(data, pData, attrLength);
	data[attrLength] = 0;
}

int IX_IndexScan::OpenScan(IX_IndexHandle &indexHandle,
                      	   CompOp      compOp,
						   void        *value)
{
	myCompOp = compOp;
	attrType = indexHandle.getAttrType();
	attrLength = indexHandle.getAttrLength();
	//printf("attrLength: %d\n", attrLength);
	copyData(value);
	ixhandle = &indexHandle;
	switch (compOp)
	{
		case EQ_OP:
		case GE_OP:
		{
			ixhandle->GetLowerBound(data, indexID);
			break;
		}
		case GT_OP:
		{
			ixhandle->GetUpperBound(data, indexID);
			break;
		}
		default:
		{
			ixhandle->GetFirst(indexID);
			break;
		}
	}
	return 0;
}

int IX_IndexScan::GetNextEntry(RID &rid)
{
	while (true)
	{
		//cout << "ixscan" << indexID << endl;
		if (ixhandle->GetRID(indexID, rid) == -1)
			return -1;
		char *k = ixhandle->GetKey(indexID);
		int res = compareData(k);
		if (myCompOp == EQ_OP && res != 0)
			return -1;
		if (myCompOp == LT_OP && res != -1)
			return -1;
		if (myCompOp == LE_OP && res == 1)
			return -1;
		if (myCompOp == NE_OP && res == 0)
		{
			RID newid;
			if (ixhandle->GetNextIndexID(indexID, newid) == -1)
				return -1;
			indexID = newid;
			continue;
		}
		RID newid;
		ixhandle->GetNextIndexID(indexID, newid);
		indexID = newid;
		return 0;
	}
}

int IX_IndexScan::compareData(char* b)
{
	if (attrType == MyINT)
	{
		int x = *((int*)b);
		int dataInt = *((int*)data);
		if (x < dataInt)
			return -1;
		if (x == dataInt)
			return 0;
		return 1;
	}
	if (attrType == FLOAT)
	{
		float x = *((float*)b);
		float dataFloat = *((float*)data);
		if (x < dataFloat)
			return -1;
		if (x == dataFloat)
			return 0;
		return 1;
	}
	char *buf = new char[attrLength + 5];
	memcpy(buf, b, attrLength);
	buf[attrLength] = 0;
	int ans = strcmp(buf, data);
	if (ans < 0) ans = -1;
	if (ans > 0) ans = 1;
	delete buf;
	return ans;
}

int IX_IndexScan::CloseScan()
{
	return 0;
}
