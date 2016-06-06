#ifndef SM_MANAGER_H
#define SM_MANAGER_H

#include "iostream"
#include <cstring>
#include <stdlib.h>
#include "../recordmanager/RecordManager.h"
#include "../recordmanager/rm_filescan.h"
#include <set>
#include <string>
#include <vector>

#define LOG_TAG "SM_Manager"
#define dbName_Null "dbName is Null"
#define dbName_opened "dbName is Opened"
#define dbName_closed "dbName is Closed"
#define dbName_not_exist "dbName not exist"
#define dbName_not_right "dbName is illegal"
#define FAIL -1
#define MAXNAME 50

using namespace std;


// Used by SM_Manager::CreateTable
struct AttrInfo {
   char     attrName[MAXNAME + 1];           // Attribute name
   AttrType attrType;            // Type of attribute
   int      attrLength;          // Length of attribute
   bool     notNull;
   bool     primaryKey;
   AttrInfo(){
	   memset(attrName, 0, sizeof(attrName));
	   attrType = MyINT;
	   attrLength = 0;
	   notNull = false;
	   primaryKey = false;
   }
};

struct TableInfo {
	char    tableName[MAXNAME + 1];
	TableInfo() {
		memset(tableName, 0, sizeof(tableName));
	}
};

// Used by Printer class
struct DataAttrInfo {
    int      offset;              						// Offset of attribute
    int      attrLength;         						 // Length of attribute
    int      indexNo;             						 // Attribute index number
    AttrType attrType;            						// Type of attribute
    char     relName[MAXNAME+1];  						// Relation name
    char     attrName[MAXNAME+1]; 						// Attribute name
    bool     notNull;
    bool     primaryKey;
    void 	 setAttributes(const AttrInfo &newAttrInfo);		 //设置
    void 	 print();
    DataAttrInfo() {
	    memset(relName, 0, MAXNAME+1);
	    memset(attrName, 0, MAXNAME+1);
	    offset = 0;
	    attrType = MyINT;
	    attrLength = 0;
	    indexNo = 0;
	    notNull = false;
	    primaryKey = false;
    }
    void copy(DataAttrInfo*);
    int getlength() const {
	if (attrType == MyINT){
		return sizeof(int);
	}
	else if (attrType == FLOAT){
		return sizeof(float);
	}
	else if (attrType == STRING){
		return attrLength;
	}
}
};

struct DataRelInfo {
	int		relLength;
	char	relName[MAXNAME+1];
	DataRelInfo(){
		relLength = 0;
		memset(relName, 0, MAXNAME+1);
   }
};

class Log{
	public:
		void LogDebug(const char* a, const char* b);
		void LogDebug(const char* a);
};

class SM_Manager {
  public:
       SM_Manager  (RM_Manager &);  					// 构造函数
       ~SM_Manager ();                                  // 析构函数
    int CreateDb    (const char *);
    int OpenDb      (const char *);                		// 打开目录
    int CloseDb     ();                                  // 关闭目录
    int DropDb      (const char *);
    int ShowDb      (const char *);
	int ShowDb		();
    int CreateTable (const char *,                // 创建表
                    int        ,
                    AttrInfo   *);
    int DropTable   (const char *);               // 删除表
	int ShowTable	(const char *);				 // 打印表的信息
	int GetTable    (const char* , int &, DataAttrInfo*&);
	int Exec        (const char *);
 private:
	char DataName[80];
	RM_Manager& rmm;
	RM_FileHandle attrfh;
	RM_FileHandle relfh;
	RM_FileHandle tablefh;
	bool IsOpenDB;
	Log* myLog;
};

#endif


