#ifndef RM_FILESCAN_H
#define RM_FILESCAN_H

#include "../fileio/FileManager.h"
#include "../utils/pagedef.h"
#include "../common/common.h"
#include "RecordManager.h"
#include "PageHead.h"
#include <cstring>
#include <iostream>

enum ClientHint {
    NO_HINT
};

class RM_FileScan {
	FileManager* myFileManager;
	BufPageManager* bpm;
	int recordSize;
	int pageNumber;
	
	int fileID;
	AttrType attrType;
	int attrLength;
	int attrOffset;
	CompOp compOp;
	void* value;

	int currentPage;
	int currentRecord;
	
	int indexNo;

public:	bool condINT(int v1, int v2){
		switch(compOp) {
			case EQ_OP: return v1 == v2;
			case LT_OP: return v1 < v2;
			case GT_OP: return v1 > v2;
			case LE_OP: return v1 <= v2;
			case GE_OP: return v1 >= v2;
			case NE_OP: return v1 != v2;
			case NO_OP: return true;
			default: break;
		}
		return false;
	}

	bool condFLOAT(float v1, float v2){
		switch (compOp){
			case EQ_OP: return v1 == v2;
			case LT_OP: return v1 < v2;
			case GT_OP: return v1 > v2;
			case LE_OP: return v1 <= v2;
			case GE_OP: return v1 >= v2;
			case NE_OP: return v1 != v2;
			case NO_OP: return true;
			default: break;
		}
		return false;
	}

	bool CondSTRING(char* v1, char* v2){
		switch(compOp) {
			case EQ_OP: return strcmp(v1, v2) == 0;
			case LT_OP: return strcmp(v1, v2) < 0;
			case GT_OP: return strcmp(v1, v2) > 0;
			case LE_OP: return strcmp(v1, v2) <= 0;
			case GE_OP: return strcmp(v1, v2) >= 0;
			case NE_OP: return strcmp(v1, v2) != 0;
			case NO_OP: return true;
			default: break;
		}
		return false;
	}

	bool vagueEqual(char* v1, char* v2) {
		FILE *fp = fopen("../vague", "w");
		fprintf(fp, "%s\n%s", v1, v2);
		fclose(fp);
		string s1 = "python ../python/vague.py";
		system(s1.c_str());

		fp = fopen("../vague", "r");
		int rval;
		fscanf(fp, "%d", &rval);
		fclose(fp);
		if(rval == 1) return true;
		return false;
	}

	bool findRecord(BufType b){
		int totalRecord = 8088 >> 5;
		PageHead* pagehead = (PageHead*)(b);

		for(; currentRecord < totalRecord; currentRecord++){
			bool isRecord = pagehead->getRecordHead(currentRecord);
			if(isRecord){
				// NULL return false
				//cout << "compOp: ---" << compOp << endl;
				//if (compOp == 7)
				//	cout << "hahaha " << indexNo << ' ' << recordSize << endl;
				int offset_1 = 96 + currentRecord * recordSize + indexNo;
				char v3 = *((char*)b + offset_1);
				//cout << v3 << endl;
				if (v3 == 0 && indexNo != -1){
				//	cout << "========!!=====(- -)======!!===" << endl;
					if (compOp == 7)
						return true;
					continue;
				}
				if(value == NULL){
					if (compOp == 7){
						continue;
					}
					return true;
				}
				if(attrType == MyINT){
					int offset = (96 + currentRecord * recordSize + attrOffset);
					int v1 = *((int*)((char*)b + offset));
					int v2 = *((int*)value);
					if(condINT(v1, v2)) return true;
				}
				if(attrType == FLOAT){
					int offset = (96 + currentRecord * recordSize + attrOffset);
					float *v = (float*)((char*)b+offset);
					float v1 = *v;
					float v2 = *((float*)value);
					if(condFLOAT(v1, v2)) return true;
				}
				if(attrType == STRING){
//					cout << "start check" << endl;
					char* v1 = new char[attrLength + 1];
					char* v2 = new char[attrLength + 1];
					memset(v1, 0, attrLength + 1);
					memset(v2, 0, attrLength + 1);
					int offset = (96 + currentRecord * recordSize + attrOffset);
//					cout << "offset" << offset << "attrLength"<< attrLength << "attrOffset" << attrOffset << endl;
//					for (int i = 0; i < 100; i++)
//						cout << b[i] << ' ';
//					cout << endl;
					memcpy(v1, (char*)b + offset, attrLength);
					memcpy(v2, value, attrLength);
					v1[attrLength] = '\0';
//					char* v2 = (char*)value;
//					cout << "he" << endl;
					v2[attrLength] = '\0';
//					cout << "he" << endl;
//					cout << "String compare: " << v1 << ' ' << v2 << endl;
					void* t1 = (void *)v1;
					void* t2 = (void *)v2;
//					cout << *(int*)t1 << ' ' <<  *(int*)t2 << endl;
//					for (int i = 0; i < attrLength; i++)
//						cout << v1[i] << v2[i] << ' ';
//					cout << endl;
					if(CondSTRING(v1, v2) && compOp != 6){
//						cout << "is equal" << endl; 
						return true;
					}
//					cout << "test" << endl;
					if(compOp == 6 && vagueEqual(v1, v2)){
						return true;
					}
				}
			}
		}
		return false;
	}

	RM_FileScan(FileManager *pfm, BufPageManager* bpm){
		myFileManager = pfm;
		this->bpm = bpm;
	}                           // Constructor
	~RM_FileScan(){
		myFileManager = NULL;
		bpm = NULL;
	}											// Destructor
    int OpenScan(const RM_FileHandle &fileHandle,  // Initialize file scan
                AttrType attrType,
                int attrLength,
                int attrOffset,
                CompOp compOp,
                void *value,
		int indexNo = -1,
                ClientHint pinHint = NO_HINT){
    	int index;
		this->fileID = fileHandle.getFileID();
		this->indexNo = indexNo;
//		cout << "in OpenScan " << fileID << endl;
		if (bpm == NULL)
			cout << "in RM_FileScan buffermannager is NULL" <<endl;
    	BufType b = bpm->getPage(fileID, 0, index);
    	recordSize = b[0];
    	pageNumber = b[1];
//	cout << "recordSize" << b[0] << endl;
//	cout << "pageNumber" << b[1] << endl;
    	this->fileID = fileHandle.getFileID();
    	this->attrType = attrType;
    	this->attrLength = attrLength;
    	this->attrOffset = attrOffset;
    	this->compOp = compOp;
    	this->value = value;
    	currentPage = 1;
    	currentRecord = 0;
//	cout << "========================" << endl;
	cout << this->compOp << compOp <<  endl;
	if (this->compOp == 7)
		return 1;
    	if(attrType < MyINT || attrType > STRING || compOp < EQ_OP || compOp > NO_OP) 
    		return 0;
//    	if((attrType == MyINT && attrLength != 4) || (attrType == FLOAT && attrLength != 4) || (attrType == STRING && attrLength < 0))
//    		return 0;
    	if((attrType == STRING && attrLength < 0))
    		return 0;
    	if((attrOffset + attrLength) > recordSize)
    		return 0;
    	return 1;
    }

    int GetNextRec(RM_Record &rec){	 // Get next matching record
//	cout << "la" << endl;
//	cout << currentPage << ' ' << pageNumber << endl;
    	for(; currentPage <= pageNumber; currentPage++){
    		int index;
    		BufType b = bpm->getPage(fileID, currentPage, index);
//		cout << "hi" << currentPage << ' ' << currentRecord << endl;
    		if(findRecord(b)){
//		cout << "hi" << currentPage << ' ' << currentRecord << endl;
    			RID rid(currentPage, currentRecord);
    			//char* pdata = (char*)(b + 24 + (currentRecord * recordSize) / 4);
    			char* pdata = (char*)b + 96 + currentRecord * recordSize;
    			rec.Set(pdata, recordSize, rid);
    			return currentRecord++;
    		}
//			cout << "ho" << endl;
    		currentRecord = 0;
    	}
    	return -1;
    }
    int CloseScan() {return 0;}                               // Terminate file scan
};
#endif
