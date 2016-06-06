#ifndef PARSE_MANAGER
#define PARSE_MANAGER

#include <string>
#include <stdlib.h>
#include "declear.h"
using namespace std;

class Parse_Manager {
	string filename;
	FILE *fp;
	QL_Manager* ql_manager;

public:
	Parse_Manager(string filename_) : filename(filename_) {}
	void MainLoop(SM_Manager *smm, RM_Manager *rmm, IX_Manager *ixm);
	char* readName();
	void readAttr(int& nattr, AttrInfo*& attrs);
	void readInsertData(char* relName, int nattr, AttrInfo* attrs);

	void readInt(void*& data);
	void readString(void*& data);
	void readFloat(void*& data);

	void readWhere(char* relName, int&nrid, RID*& rrid);
	void readWhere(int& ncond, Condition*& cond);

	void caland(RID*& rid1, RID* rid2, int& size1, int size2);
	void calor(RID*& rid1, RID* rid2, int& size1, int size2);

	void readRelAttr(RelAttr& relattr);
};


#endif
