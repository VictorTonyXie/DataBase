/*
 * rm_filehandle.cpp
 *
 *  Created on: Oct 27, 2015
 *      Author: thomas
 */

#include "RecordManager.h"
#include "PageHead.h"

int RM_FileHandle::GetRec(const RID &rid, RM_Record &rec) const
{
	int page = rid.Page();
	int slot = rid.Slot();
	if (page > fileHead->pageNumber)
		return 0;
	int index;
	BufType b = bpm->getPage(fileID, page, index);
	PageHead *head = (PageHead*)(b);
	if (head->getRecordHead(slot))
	{
		char *data;
		data = (char*)b;
		data += 96 + recordSize * slot;
		rec.Set(data, recordSize, rid);
		return 1;
	}
	else
		return 0;
}

int RM_FileHandle::InsertRec(const char *pData, RID &rid)
{
	int headindex, pageindex;
//	cout << "InsertRec : " << pData << endl;
	fileHead = (FileHead*)(bpm->getPage(fileID, 0, headindex));
	bool ok = false;
	for (int i = 1; i <= fileHead->pageNumber; i++)
	{
		BufType b = bpm->getPage(fileID, i, pageindex);
		PageHead *head = (PageHead*)(b);
		//printf("%d\n", head->usedSlot);
		//printf("%d\n", recordPerPage);
		if (head->usedSlot < recordPerPage)
		{
			for (int j = 0; j < recordPerPage; j++)
			{
				if (head->getRecordHead(j) == 0)
				{
					char *data;
					data = (char*)b;
					data += 96 + recordSize * j;
					memcpy(data, pData, recordSize);
					head->usedSlot++;
					head->setRecordHead(j, true);
					bpm->markDirty(pageindex);
					rid = RID(i, j);
					ok = true;
					break;
				}
			}
			break;
		}
	}
	if (!ok)
	{
		fileHead = (FileHead*)(bpm->getPage(fileID, 0, headindex));
		int pageid = (++fileHead->pageNumber);
		bpm->markDirty(headindex);
		BufType b = bpm->allocPage(fileID, pageid, pageindex, false);
		memset(b, 0, 8 * 1024);
//		BufType b = bpm->getPage(fileID, pageid, pageindex);
		PageHead head = PageHead();
		head.usedSlot++;
		head.setRecordHead(0, true);
		char *data;
		data = (char*)b;
		memcpy(data, &head, sizeof(PageHead));
		data += 96;
		memcpy(data, pData, recordSize);
		bpm->markDirty(pageindex);
		rid = RID(pageid, 0);
		ok = true;
	}
	fileHead = (FileHead*)(bpm->getPage(fileID, 0, headindex));
	fileHead->recordNumber++;
	bpm->markDirty(headindex);
	return ok;
}

int RM_FileHandle::DeleteRec(const RID &rid)
{
	int page = rid.Page();
	int slot = rid.Slot();
	if (page > fileHead->pageNumber)
		return 0;
	int pageindex, headindex;
	BufType b = bpm->getPage(fileID, page, pageindex);
	PageHead *head = (PageHead*)(b);
	if (head->getRecordHead(slot))
	{
		head->usedSlot--;
		head->setRecordHead(slot, false);
		bpm->markDirty(pageindex);
		fileHead = (FileHead*)(bpm->getPage(fileID, 0, headindex));
		fileHead->recordNumber--;
		bpm->markDirty(headindex);
		return 1;
	}
	return 0;
}

int RM_FileHandle::UpdateRec(const RM_Record &rec)
{
	RID rid;
	rec.GetRid(rid);
	int page = rid.Page();
	int slot = rid.Slot();
	if (page > fileHead->pageNumber)
		return 0;
	int index;
	BufType b = bpm->getPage(fileID, page, index);
	PageHead *head = (PageHead*)(b);
	if (head->getRecordHead(slot))
	{
		char *data;
		data = (char*)b;
		data += 96 + recordSize * slot;
		char *pdata;
		rec.GetData(pdata);
		memcpy(data, pdata, recordSize);
		bpm->markDirty(index);
		return 1;
	}
	return 0;
}
