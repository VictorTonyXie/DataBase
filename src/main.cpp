/*
 * testfilesystem.cpp
 *
 *  Created on: 2015年10月6日
 *      Author: lql
 */
#include "bufmanager/BufPageManager.h"
#include "fileio/FileManager.h"
#include "common/common.h"
#include "recordmanager/RecordManager.h"
#include "recordmanager/rm_filescan.h"
#include "systemmanager/sm_manager.h"
#include "indexmanager/IX_Manager.h"
#include <iostream>

#include <cstdlib>
#include <algorithm>
using namespace std;

using namespace std;

void test_RM_Manager()
{
    FileManager* fm = new FileManager();
    BufPageManager* bpm = new BufPageManager(fm);
    RM_Manager* rm_m = new RM_Manager(fm, bpm);
    rm_m->CreateFile("testfile.txt", 32);
    int fileID;
    fm->openFile("testfile.txt", fileID); //打开文件，fileID是返回的文件id
    for (int pageID = 0; pageID < 10; ++ pageID) {
        int index;
        //为pageID获取一个缓存页
        BufType b = bpm->getPage(fileID, pageID, index);
        cout << b[0] << endl; 		//读取缓存页中第一个整数
        bpm->access(index); //标记访问
    }
}

struct Rec{
	int b;
	float f;
	char a[24];
};

void test_RM_FileScan()
{
 /*   FileManager* fm = new FileManager();
    BufPageManager* bpm = new BufPageManager(fm);
    RM_Manager* rm_m = new RM_Manager(fm, bpm);
    //	fm->createFile("testfile.txt"); //新建文件
    rm_m->CreateFile("testfile.txt", 32);
    RM_FileHandle file;
    rm_m->OpenFile("testfile.txt", file); //打开文件，fileID是返回的文件id
    
    RM_FileScan *scan = new RM_FileScan(fm, bpm);
    //char a[25];
    //for(int i = 0; i < 24; i++) a[i] = 97 + i;
    Rec r;
    r.b = 1; r.f = 1.2;
    for(int i = 0; i < 24; i++) r.a[i] = 65 + i;
   	char d[33];
	memcpy(d, &r, 32);
	d[32] = 0;
	RID rid;
	file.InsertRec(d, rid);

	Rec r2;
    r2.b = 10; r2.f = 1.2;
    for(int i = 0; i < 24; i++) r2.a[i] = 97 + i;
   	char d2[33];
	memcpy(d2, &r2, 32);
	d2[32] = 0;
	file.InsertRec(d2, rid);

	for (int i = 1; i <= 10; i++)
	{
		r.b = i; r.f = i + 1.0 / float(i);
		for (int j = 0; j < 24; j++)
			r.a[j] = '0' + i + j;
		file.InsertRec((char*)(&r), rid);
	}

	file.DeleteRec(RID(1, 3));

	RM_Record rec;
	file.GetRec(RID(1, 5), rec);
	char *data;
	rec.GetData(data);
	Rec *rp = (Rec*)data;
	rp->a[0] = '!';
	file.UpdateRec(rec);

    //float a = 1.2;
    char a[25];
    for(int i = 0; i < 24; i++) a[i] = 65 + i;
    a[24] = 0;

    void* value = (void*)&a;
    AttrType attrType = STRING;
    CompOp compOp = EQ_OP;
    int flag = scan->OpenScan(file, attrType, 24, 8, compOp, value);
    
    int loc = scan->GetNextRec(rec);
    cout << rec.recordSize << ' ' << rec.rid.Page() << ' ' << rec.rid.Slot() << endl;
    data = rec.data;
    int* dd = (int*)data;
    cout << dd[0] << endl;
    cout << loc << endl;
    loc = scan->GetNextRec(rec);
    cout << rec.recordSize << ' ' << rec.rid.Page() << ' ' << rec.rid.Slot() << endl;
    data = rec.data;
    dd = (int*)data;
    cout << dd[0] << endl;
    cout << loc << endl;*/
}

void test_RM_FileHandle()
{
	cout << "test_rm_filehandle" << endl;
	FileManager* fm = new FileManager();
	BufPageManager* bpm = new BufPageManager(fm);
	RM_Manager* rm_m = new RM_Manager(fm, bpm);
	rm_m->CreateFile("testfile.txt", 32);
	RM_FileHandle file;
	rm_m->OpenFile("testfile.txt", file);
	char data[33];
	data[32] = 0;
	for (int i = 0; i < 32; i++)
		data[i] = '0' + i;
	RID rid;
	for (int i = 0; i < 300; i++)
	{
		file.InsertRec(data, rid);
		cout << rid << endl;
	}
	RM_Record rec;
	file.GetRec(rid, rec);
	char *pdata;
	rec.GetData(pdata);
	memcpy(data, pdata, 32);
	printf("%s\n", data);
	rid = RID(5, 50);
	int x = file.GetRec(rid, rec);
	printf("%d\n", x);
	rm_m->CloseFile(file);
	rm_m->OpenFile("testfile.txt", file);
	rid = RID(1, 100);
	file.GetRec(rid, rec);
	rec.GetData(pdata);
	memcpy(data, pdata, 32);
	printf("%s\n", data);
	file.DeleteRec(rid);
	x = file.GetRec(rid, rec);
	printf("%d\n", x);
	for (int i = 0; i < 32; i++)
		data[i] = 'A' + i;
	rid = RID(1, 10);
	rec.Set(data, 32, rid);
	file.UpdateRec(rec);
	x = file.GetRec(rid, rec);
	printf("%d\n", x);
	file.GetRec(rid, rec);
	rec.GetData(pdata);
	memcpy(data, pdata, 32);
	printf("%s\n", data);
}

void test_SM_Manager(){
//	char command[80] = "./create ";
//	char command1[80] = "rmdir ";
	char command[80] = "./create ";
	char command1[80] = "rm -r ";
	char dbname[10] = "1";
	int whetherDel = 0;
	FileManager* fm = new FileManager();
	BufPageManager* bpm = new BufPageManager(fm);
	RM_Manager* rm_m = new RM_Manager(fm, bpm);
	SM_Manager* sm_m = new SM_Manager(*rm_m);
	char pathbuf[100];
	//create File
	sm_m->CreateDb(dbname);
	getcwd(pathbuf, 100);
	cout << pathbuf << endl;
	//use File
	sm_m->OpenDb("1");
	cout << "yes" << endl;
	getcwd(pathbuf, 100);
	cout << pathbuf << endl;
	AttrInfo x[3];
	string str = "lalala";
	memcpy(x[0].attrName, str.c_str(), str.length());
	x[0].attrType = MyINT;
	x[0].attrLength = 4;
	string str1 = "hahaha";
	memcpy(x[1].attrName, str1.c_str(), str1.length());
	x[1].attrType = MyINT;
	x[1].attrLength = 4;
	string str2 = "kakaka";
	memcpy(x[2].attrName, str2.c_str(), str2.length());
	x[2].attrType = MyINT;
	x[2].attrLength = 4;
	sm_m->CreateTable("a", 2, x);
	sm_m->CreateTable("b", 3, x);
	//sm_m->CreateTable("a", 1, &x[2]);
	sm_m->CloseDb();
	sm_m->OpenDb("1");
	cout << "======show a======" << endl;
	sm_m->ShowTable("a");
	cout << "======show b======" << endl;
	sm_m->ShowTable("b");
	cout << "======drop a======" << endl;
	sm_m->DropTable("a");
	cout << "======show a======" << endl;
	sm_m->ShowTable("a");
	cout << "======show b======" << endl;
	sm_m->ShowTable("b");
	sm_m->CloseDb();
	//test Exec
	sm_m->Exec("CREATE DATABASE qyj");
	sm_m->Exec("DROP DATABASE qyj");
	sm_m->Exec("CREATE DATABASE qyj");
	sm_m->Exec("USE DATABASE qyj");
	sm_m->Exec("CREATE TABLE example(name char(10), age int, score float NOT NULL)");
	sm_m->Exec("CREATE TABLE example2(id int, gender, int)");
	sm_m->Exec("SHOW DATABASE qyj");
	sm_m->Exec("SHOW TABLE example");
	//drop File
	cout << "Clean? " << endl;
	cin >> whetherDel;
	if (whetherDel == 0){
		chdir ("..");
		system ("cd");
		system (strcat(command1, dbname));
	}
	return;
}

void test_IX_Manager()
{
	FileManager* fm = new FileManager();
	BufPageManager* bpm = new BufPageManager(fm);
	IX_Manager *ixm = new IX_Manager(fm, bpm);
	ixm->CreateIndex("qyj", 0, MyINT, 4);
	IX_IndexHandle ixhandle;
	ixm->OpenIndex("qyj", 0, ixhandle);
	int a[100005];
	printf("insert begin.\n");
	for (int i = 0; i < 100; i++)
	{
		printf("%d\n", i);
		a[i] = rand();
		ixhandle.InsertEntry(&a[i], RID(0, i));
	}
	printf("insert finished.\n");
	ixhandle.PrintEntries();
	for (int i = 0; i < 50; i++)
	{
		printf("%d\n", i);
		ixhandle.DeleteEntry(&a[i], RID(0, i));
		//printf("\n");
	}
	ixhandle.PrintEntries();
	//sort(a + 50, a + 100);
	for (int i = 50; i < 100; i++)
		printf("%d ", a[i]);
	printf("\n");
	ixhandle.ForcePages();
	IX_IndexScan scan;
	int value = 500782188;
	scan.OpenScan(ixhandle, NE_OP, &value);
	RID rid;
	while (scan.GetNextEntry(rid) != -1)
		printf("%d ", a[rid.Slot()]);
	printf("\n");

}

int main() {
	setbuf(stdout,NULL);
    //test_RM_Manager();
    //test_RM_FileScan();
    //test_RM_FileHandle();
	//test_SM_Manager();
	test_IX_Manager();
	return 0;
}
