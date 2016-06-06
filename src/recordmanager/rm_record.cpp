#include "RecordManager.h"
//#include "../utils/MyBitMap.h"

#include <cstdio>
#include <cstring>
#include <iostream>
using namespace std;
unsigned char MyBitMap::h[61] = {};

RM_Record::RM_Record() : recordSize(-1), data(NULL), rid(-1, -1)
{
}

RM_Record::~RM_Record()
{
	if (data != NULL) {
		delete [] data;
	}
}

int RM_Record::Set(char *pData, int size, RID rid_)
{
	if(recordSize != -1 && (size != recordSize))
		return 0;
	recordSize = size;
  	this->rid.Copy(rid_);
	if (data == NULL){
		data = new char[recordSize];
		memset(data, 0, sizeof(data));
	}
  	memcpy(data, pData, size);
	return 1;
}

int RM_Record::GetData(char *&pData) const 
{
//	while(1){
//	}
	if (data != NULL && recordSize != -1)
  	{
//		cout << "print data" << endl;
//		strncpy(pData, data, recordSize);
		pData = data;
		return 1;
	}
	else			 
		return 0;
}

int RM_Record::GetRid(RID &rid) const 
{
	if (data != NULL && recordSize != -1)
	{
    		rid.Copy(this->rid);
    		return 1;
  	}
	else 
		return 0;
}
