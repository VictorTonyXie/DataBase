#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <stack>
#include "parse_manager.h"

using namespace std;

char* Parse_Manager::readName(){
	char* relName = new char[MAXNAME];
	memset(relName, 0, MAXNAME);
	fscanf(fp, "%s", relName);
	return relName;
}

void Parse_Manager::readAttr(int& nattr, AttrInfo*& attrs){
	fscanf(fp, "%d", &nattr);
	attrs = new AttrInfo[nattr];
	for (int i = 0; i < nattr; i++){
		memset(attrs[i].attrName, 0, MAXNAME);
		fscanf(fp, "%s", attrs[i].attrName);
	}
}

void Parse_Manager::readInt(void*& data){
	int num;
	fscanf(fp, "%d", &num);
	data = new char[sizeof(int)];
	memcpy(data, (char*) &num, sizeof(int));
}

void Parse_Manager::readFloat(void*& data){
	float num;
	fscanf(fp, "%f", &num);
	cout << num << endl;
	data = new char[sizeof(float)];
	memcpy(data, (char*)&num, sizeof(float));
}

void Parse_Manager::readString(void*& data){
	char buf[1024];
	memset(buf, 0, 1024);
	fgets(buf, 1024, fp);
	char* tmp = new char[strlen(buf)];
	memset(tmp, 0, strlen(buf));
	memcpy(tmp, buf+1, strlen(buf) - 2);
	data = (void*)tmp;
//	printf("%s\n", (char*)data);
}

void Parse_Manager::readRelAttr(RelAttr& relattr) {
	char* rel = new char[MAXNAME];
	memset(rel, 0, MAXNAME);
	relattr.attrName = new char[MAXNAME];
	memset(relattr.attrName, 0, MAXNAME);
	fscanf(fp, "%s %s", rel, relattr.attrName);

	if(strcmp(rel, "null") != 0) {
		relattr.relName = new char[MAXNAME];
		memcpy(relattr.relName, rel, MAXNAME);
	}
	else
		relattr.relName = NULL;
	int loc = -1;
	for(int i = 0; i < strlen(relattr.attrName); i++) {
		if(relattr.attrName[i] == '_') {
			loc = i;
			break;
		}
	}
	if(loc != -1) {
		relattr.outkeyrel = new char[MAXNAME];
		memset(relattr.outkeyrel, 0, MAXNAME);
		relattr.outkeyattr = new char[MAXNAME];
		memset(relattr.outkeyattr, 0, MAXNAME);
		memcpy(relattr.outkeyrel, relattr.attrName, loc);
		memcpy(relattr.outkeyattr, relattr.attrName + loc + 1, strlen(relattr.attrName) - loc);

	}
	else{
		relattr.outkeyrel = NULL;
		relattr.outkeyattr = NULL;
	}

	delete rel;
}

void Parse_Manager::caland(RID*& rid1, RID* rid2, int& size1, int size2) {
	vector<RID> set;
	for (int i = 0; i < size2; i++){
		int flag = 0;
		for (int j = 0; j < size1; j++)
			if (rid1[j] == rid2[i]){
				flag = 1;
				break;
			}
		if (flag)
			set.push_back(rid2[i]);
	}
	size1 = set.size();
	delete []rid2;
	delete []rid1;
	rid1 = new RID[size1];
	for (int i = 0; i < size1; i++)
		rid1[i] = set[i];
}

void Parse_Manager::calor(RID*& rid1, RID* rid2, int& size1, int size2) {
	vector<RID> set;
	for (int i = 0; i < size1; i++)
		set.push_back(rid1[i]);
	for (int i = 0; i < size2; i++){
		int flag = 0;
		for (int j = 0; j < size1; j++)
			if (rid1[j] == rid2[i]){
				flag = 1;
				break;
			}
		if (!flag)
			set.push_back(rid2[i]);
	}
	size1 = set.size();
	delete []rid2;
	delete []rid1;
	rid1 = new RID[size1];
	for (int i = 0; i < size1; i++)
		rid1[i] = set[i];
}

void Parse_Manager::readWhere(char* relName, int&nrid, RID*& rrid) {
	int nConditions = 0;
	fscanf(fp, "%d", &nConditions);
	
	stack<int> op;
	stack<int> ridsize;
	stack<RID*> rid;
	
	int stmttype;
	fscanf(fp, "%d", &stmttype);
	while(stmttype != 5) {
		if(stmttype == 0) op.push(stmttype);
		else if(stmttype == 1) {
			if(op.top() == 0) op.pop();
			else{
				int size1 = ridsize.top();
				ridsize.pop();
				int size2 = ridsize.top();
				ridsize.pop();
				RID* rid1 = rid.top();
				rid.pop();
				RID* rid2 = rid.top();
				rid.pop();
				if(op.top() == 2){
					caland(rid1, rid2, size1, size2);
				}
				else{
					calor(rid1, rid2, size1, size2);
				}
				ridsize.push(size1);
				rid.push(rid1);
				op.pop();

				if(op.top() == 0) op.pop();
				else cout << "Syntax Error" << endl;
			}
		}

		else if(stmttype == 2 || stmttype == 3) {
			if(!op.empty() && (op.top() == 2 || op.top() == 3)){
				int size1 = ridsize.top();
				ridsize.pop();
				int size2 = ridsize.top();
				ridsize.pop();
				RID* rid1 = rid.top();
				rid.pop();
				RID* rid2 = rid.top();
				rid.pop();
				if(op.top() == 2){
					caland(rid1, rid2, size1, size2);
				}
				else{
					calor(rid1, rid2, size1, size2);
				}
				ridsize.push(size1);
				rid.push(rid1);

				op.pop();
			}
			op.push(stmttype);
		}
		else if(stmttype == 4){
			Condition *cond = new Condition;
			readRelAttr(cond->lhsAttr);
			fscanf(fp, "%d", &cond->op);

			if(cond->op != 7) {
				int righttype;
				fscanf(fp, "%d", &righttype);

				if(righttype == 0) {
					cond->bRhsIsAttr = 0;
					cond->rhsValue.type = MyINT;
					readInt(cond->rhsValue.data);
				}
				else if(righttype == 1) {
					cond->bRhsIsAttr = 0;
					cond->rhsValue.type = FLOAT;
					readFloat(cond->rhsValue.data);
				}
				else if(righttype == 2) {
					cond->bRhsIsAttr = 0;
					cond->rhsValue.type = STRING;
					readString(cond->rhsValue.data);
				}
				else if(righttype == 3) {
					cond->bRhsIsAttr = 1;
					readRelAttr(cond->rhsAttr);
				}
			}
			int ssize = 0;
			RID* srid;
			ql_manager->Search(relName, cond, ssize, srid);
			ridsize.push(ssize);
			rid.push(srid);
		}
		fscanf(fp, "%d", &stmttype);
	}

	while(!op.empty()){
		if(op.top() == 1) cout << "Syntax Error" << endl;
		else if(op.top() == 2 || op.top() == 3){
			int size1 = ridsize.top();
			ridsize.pop();
			int size2 = ridsize.top();
			ridsize.pop();
			RID* rid1 = rid.top();
			rid.pop();
			RID* rid2 = rid.top();
			rid.pop();
			if(op.top() == 2){
				caland(rid1, rid2, size1, size2);
			}
			else{
				calor(rid1, rid2, size1, size2);
			}
			ridsize.push(size1);
			rid.push(rid1);
			op.pop();
		}
	}
	nrid = ridsize.top();
	rrid = rid.top();
}

void Parse_Manager::readWhere(int& ncond, Condition*& cond) {
	fscanf(fp, "%d", &ncond);
	cond = new Condition[ncond];

	int stmttype;
	int left = 0;
	int offset = 0;

	fscanf(fp, "%d", &stmttype);
	while(stmttype != 5) {
		if(stmttype == 0) left++;
		else if(stmttype == 1) left--;
		else if(stmttype == 3){
			cout << "Syntax Error" << endl;
		}
		else if(stmttype == 4) {
			readRelAttr(cond[offset].lhsAttr);
			fscanf(fp, "%d", &cond[offset].op);

			if(cond[offset].op != 7) {

				int righttype;
				fscanf(fp, "%d", &righttype);

				if(righttype == 0) {
					cond[offset].bRhsIsAttr = 0;
					cond[offset].rhsValue.type = MyINT;
					readInt(cond[offset].rhsValue.data);
				}
				else if(righttype == 1) {
					cond[offset].bRhsIsAttr = 0;
					cond[offset].rhsValue.type = FLOAT;
					readFloat(cond->rhsValue.data);
				}
				else if(righttype == 2) {
					cond[offset].bRhsIsAttr = 0;
					cond[offset].rhsValue.type = STRING;
					readString(cond->rhsValue.data);
				}
				else if(righttype == 3) {
					cond[offset].bRhsIsAttr = 1;
					readRelAttr(cond[offset].rhsAttr);
				}
			}
			offset++;
		}
		fscanf(fp, "%d", &stmttype);
	}
}

void Parse_Manager::readInsertData(char* relName, int nattr, AttrInfo* attrs){
	int cnt;
	fscanf(fp, "%d", &cnt);
//	cout << "Parse_Manager::readInserData"<< endl;
	for (int i = 0; i < cnt; i++){
		int nvalue;
		fscanf(fp, "%d", &nvalue);
		Value* values = new Value[nvalue];
//		if(nvalue != nattr) cout << "Syntax Error" << endl;

		for (int j = 0; j < nvalue; j++){
			fscanf(fp, "%d", &values[j].type);

			if (values[j].type == MyINT){
				readInt(values[j].data);
			}
			else if(values[j].type == FLOAT){
				readFloat(values[j].data);
			}
			else{
				readString(values[j].data);
			}
		}
		ql_manager->Insert(relName, nvalue, values, nattr, attrs);
	}
}


void Parse_Manager::MainLoop(SM_Manager *smm, RM_Manager *rmm, IX_Manager *ixm) {

	string instruction = string("python python/parser.py ") + filename + string("> tmp");
	system(instruction.c_str());

	fp = fopen("tmp.gen", "r");
	ql_manager = new QL_Manager(smm, rmm, ixm);

	char cmd[256];
	int a = 1;
	while(true) {
		a = 0;
		memset(cmd, 0 , 256);
		fscanf(fp, "%s\n", cmd);
		cout << "cmd: " << cmd << endl;
		if (strcmp(cmd, "Syntax error") == 0)
			cout << "Syntax error" << endl;

		else if(strcmp(cmd, "createdatabase") == 0) { //CREATE DATABASE orderDB;
			char* DBName = readName();
			cout << "CREATE " << DBName<< endl;
			smm->CreateDb(DBName);
		}

		else if(strcmp(cmd, "dropdatabase") == 0) { //DROP DATABASE orderDB;
			char* DBName = readName();
			cout << "DROP " << DBName<< endl;
			smm->DropDb(DBName);
		}

		else if(strcmp(cmd, "use") == 0) { //USE orderDB;
			char* DBName = readName();
			cout << "USE " << DBName<< endl;
			smm->CloseDb();
			smm->OpenDb(DBName);
		}

		else if(strcmp(cmd, "showtables") == 0) { //SHOW TABLES
			cout << "showTables" << endl;
			smm->ShowDb();
		}

		else if(strcmp(cmd, "drop") == 0){ //DROP TABLE customer;
			char* relName = readName();
			cout << relName<< endl;
		}

		else if(strcmp(cmd, "desc") == 0){ //DESC customer;
			char* relName = readName();
			cout << relName<< endl;
		}

		else if(strcmp(cmd, "showtable") == 0){ //SHOW customer
			char* relName = readName();
			cout << relName << endl;
			smm->ShowTable(relName);
		}

		else if(strcmp(cmd, "createtable") == 0){ //CREATE TABLE customer {}
			char* relName = readName();
			cout << relName << endl;
			char* pk = readName();
			cout << pk << endl;
			char *temp = readName();
			if(strcmp(temp, "check") == 0){
				char *check_attr = readName();
				int ntype;
				fscanf(fp, "%d", &ntype);
				cout << check_attr << ' ' << ntype << endl;
				AttrType attrType;
				int attrLength;
				Value* value = new Value[ntype];
				for(int i = 0; i < ntype; i++) {
					fscanf(fp, "%d", &value[i].type);
					if (value[i].type == MyINT){
						readInt(value[i].data);
						attrType = MyINT;
						attrLength = 4;
					}
					else if(value[i].type == FLOAT){
						readFloat(value[i].data);
						attrType = FLOAT;
						attrLength = 4;
					}
					else{
						readString(value[i].data);
						attrType = STRING;
						attrLength = 1024;
					}
					cout << value[i];
				}
				char checkname[512];
				memset(checkname, 0, sizeof checkname);
				sprintf(checkname, "%s.check", check_attr);
				ixm->CreateIndex(relName, checkname, attrType, attrLength);
				IX_IndexHandle ixh;
				ixm->OpenIndex(relName, checkname, ixh);
				for (int i = 0; i < ntype; i++) {
					ixh.InsertEntry(value[i].data, RID(i, 0));
				}
				ixm->CloseIndex(ixh);
			}
			else if(strcmp(temp, "nocheck") == 0) {
				cout << "nocheck" << endl;
			}
			int attrCount;
			AttrInfo* attributes;
			fscanf(fp, "%d", &attrCount);
			attributes = new AttrInfo[attrCount];
			for (int i = 0; i < attrCount; i++){
				memset(attributes[i].attrName, 0, MAXNAME+1);
				fscanf(fp, "%s %d %d %d", attributes[i].attrName, &attributes[i].attrType, &attributes[i].attrLength, &attributes[i].notNull);
				if(strcmp(attributes[i].attrName, pk) == 0) {
					attributes[i].primaryKey = 1;
					ixm->CreateIndex(relName, attributes[i].attrName, attributes[i].attrType, attributes[i].attrLength);
				}
			}
			AttrInfo *x = new AttrInfo[attrCount];
			for (int i = 0; i < attrCount; i++)
				x[i] = attributes[i];
			smm->CreateTable(relName, attrCount, x);
			delete x;
		}

		else if(strcmp(cmd, "createindex") == 0) { //CREATE INDEX customer(name);
			char *relName = readName();
			char *attrName = readName();
			cout << relName << ' ' << attrName << endl;
			int attrNum;
			DataAttrInfo* attrs;
			smm->GetTable(relName, attrNum, attrs);
			for (int i = 0; i < attrNum; i++)
				if (strcmp(attrs[i].attrName, attrName) == 0) {
					//printf("createindex attrName : %s  attrLength : %d  attroffset : %d\n", attrs[i].attrName, attrs[i].attrLength, attrs[i].offset);
					if (ixm->CreateIndex(relName, attrName, attrs[i].attrType, attrs[i].attrLength) == -1)
						break;
					IX_IndexHandle ixh;
					ixm->OpenIndex(relName, attrName, ixh);
					RM_FileHandle relfh;
					rmm->OpenFile(relName, relfh);
					RM_FileScan scan(rmm->getFileManager(), rmm->getBufPageManager());
					scan.OpenScan(relfh, attrs[i].attrType, attrs[i].attrLength, 0, NO_OP, (void*)0);
					int cnt = 0;
					while (true) {
						RM_Record rec;
						if (scan.GetNextRec(rec) == -1)
							break;
						ixh.InsertEntry(rec.data + attrs[i].offset, rec.rid);
						//printf("%d %s  %lld\n", cnt++, rec.data + attrs[i].offset, (long long)(rec.data));
					}
					//printf("\n");
					rmm->CloseFile(relfh);
					ixm->CloseIndex(ixh);
					break;
				}
		}

		else if(strcmp(cmd, "dropindex") == 0) { //DROP INDEX customer(name);
			char *relName = readName();
			char *attrName = readName();
			cout << relName << ' ' << attrName << endl;
			ixm->DestroyIndex(relName, attrName);
		}

		else if(strcmp(cmd, "insert") == 0){
			char* relName = readName();
			int hasAttr;
			fscanf(fp, "%d", &hasAttr);
			AttrInfo* attrs; 
			int nattr = 0;
			if (hasAttr == 1)
				readAttr(nattr, attrs);
			readInsertData(relName, nattr, attrs);
		}

		else if(strcmp(cmd, "delete") == 0){
			char* relName = readName();
			
			char temp[100] = {};
			fscanf(fp, "%s", temp);

			if(strcmp(temp, "where") == 0) {
				int nrid = 0;
				RID *rid;
				readWhere(relName, nrid, rid);
				ql_manager->Delete(relName, nrid, rid);
			}
			else if(strcmp(temp, "enddelete") == 0) {
				ql_manager->Delete(relName, -1, NULL);
			}

			else {
				cout << "Syntax Error" << endl;
				break;
			}
		}

		else if(strcmp(cmd, "update") == 0) {
			char* relName = readName();
			RelAttr l, r;
			Value v;
			int bIsValue = 1;
			
			readRelAttr(l);

			int type;
			fscanf(fp, "%d", &type);
			if (type == 0){
				v.type = MyINT;
				readInt(v.data);
			}
			else if (type == 1){
				v.type = FLOAT;
				readFloat(v.data);
			}
			else if(type == 2){
				v.type = STRING;
				readString(v.data);
			}
			else if(type == 3) {
				bIsValue = 0;
				readRelAttr(r);
			}
			char* ddd = (char*)v.data;

			char temp[100] = {};
			fscanf(fp, "%s", temp);
			if(strcmp(temp, "where") == 0){
				int nrid = 0;
				RID* rid;
				readWhere(relName, nrid, rid);
				ql_manager->Update(relName, l, bIsValue, r, v, nrid, rid);
			}
			else if(strcmp(temp, "endupdate") == 0){
				ql_manager->Update(relName, l, bIsValue, r, v, -1, NULL);
			}
		}

		else if(strcmp(cmd, "select") == 0){
			int nattrs = 0;
			fscanf(fp, "%d", &nattrs);
			RelAttr *attr = new RelAttr[nattrs];
			for(int i = 0; i < nattrs; i++) {
				fscanf(fp, "%d", &attr[i].type);
				readRelAttr(attr[i]);
				//cout << attr[i].type << ' ' << (attr[i].relName == NULL) << ' ' << attr[i].attrName << endl; 
			}
			int nrela = 0;
			fscanf(fp, "%d", &nrela);
cout << nrela << endl;
//			cout << "get 3" << endl;
			//cout << nrela << endl;
			char **rela = new char*[nrela];
			for(int i = 0; i < nrela; i++) {
				rela[i] = new char[MAXNAME];
				memset(rela[i], 0, MAXNAME);
				fscanf(fp, "%s", rela[i]);
				//cout << rela[i] << endl;
			}
			char temp[100] = {};
			fscanf(fp, "%s", temp);
//			cout << "get 4" << endl;
			if(strcmp(temp, "where") == 0) {
				if(nrela == 1) {
					int nrid = 0;
					RID* rid;
					readWhere(rela[0], nrid, rid);
					ql_manager->Print(nattrs, attr, rela[0], nrid, rid);
				}
				else{
					int ncond = 0;
					Condition* cond;
					readWhere(ncond, cond);
//RelAttr gattr;
//ql_manager->Select(0,NULL,0,NULL,0,0,NULL,gattr);
					ql_manager->Select(nattrs, attr, nrela, rela, 0, ncond, cond, attr[0]);
				}
			}
			else if(strcmp(temp, "group") == 0) {
				RelAttr gattr;
				readRelAttr(gattr);
				ql_manager->Select(nattrs, attr, nrela, rela, 1, 0, NULL, gattr);
			}
			else if(strcmp(temp, "endselect") == 0) {
				ql_manager->Select(nattrs, attr, nrela, rela, 0, 0, NULL, attr[0]);
			}
			else{
				cout << "Syntax Error" << endl;
				break;
			}
		}
		else {
			cout << "over" << endl;
			break;
		}
	}
	fclose(fp);
}
