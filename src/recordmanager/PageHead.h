#ifndef PAGEHEAD_H
#define PAGEHEAD_H

//#include "../fileio/FileManager.h"
//#include "../utils/pagedef.h"
#include <cstring>
#include <iostream>

class PageHead{
public:
	int usedSlot;
	char slotMap[84];
	PageHead(){
		usedSlot = 0;
		memset(slotMap, 0, sizeof(slotMap));
	}
	bool getRecordHead(int RecordID){
		char Record = slotMap[RecordID >> 3];
		//memcpy(&Record, b + 3 + (RecordID >> 3), 1);
		return (Record >> (7 - (RecordID % 8))) % 2;
	}
	int setRecordHead(int RecordID, bool IsRecord){
		char Record = slotMap[RecordID >> 3];
		//memcpy(&Record, b + 3 + (RecordID >> 3), 1);
		if(IsRecord) Record = Record | (1 << (7 - (RecordID % 8)));
		else Record = Record & (255 - (1 << (7 - (RecordID % 8))));
		//memcpy(b + 3 + (RecordID >> 3), &Record, 1);
		slotMap[RecordID >> 3] = Record;
		return 1;
	}
};

#endif
