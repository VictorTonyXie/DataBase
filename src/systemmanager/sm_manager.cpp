#include "sm_manager.h"

void DataAttrInfo::setAttributes(const AttrInfo &newAttrInfo){
	memset(relName, 0, MAXNAME + 1);
	memset(attrName, 0, MAXNAME + 1);
	strcpy(this->attrName, newAttrInfo.attrName);
	this->attrType = newAttrInfo.attrType;
	this->attrLength = newAttrInfo.attrLength;
	this->notNull = newAttrInfo.notNull;
	this->primaryKey = newAttrInfo.primaryKey;
}

void DataAttrInfo::print(){
//	cout << "start print" << endl;
	cout << "TableName: " << relName << ",attrName: " << attrName <<",attrLength: " << attrLength <<endl;
}

void DataAttrInfo::copy(DataAttrInfo* newAttrInfo){
	memset(relName, 0, MAXNAME + 1);
	memset(attrName, 0, MAXNAME + 1);
	strcpy(this->attrName, newAttrInfo->attrName);
	strcpy(this->relName, newAttrInfo->relName);		
	this->attrType = newAttrInfo->attrType;
	this->attrLength = newAttrInfo->attrLength;
	this->notNull = newAttrInfo->notNull;
	this->primaryKey = newAttrInfo->primaryKey;
	this->offset = newAttrInfo->offset;
	this->indexNo = newAttrInfo->indexNo;	
}

void Log::LogDebug(const char* a, const char* b){
	cout << a << ' ' << b << endl;
}

void Log::LogDebug(const char* a){
	cout << LOG_TAG << ' ' << a << endl;
}

SM_Manager::SM_Manager(RM_Manager &_rmm):rmm(_rmm){
	IsOpenDB = false;
	myLog = new Log();
}

SM_Manager::~SM_Manager(){
	
}

int SM_Manager::CreateDb (const char *dbName){
	char command[80] = "mkdir ";
	system(strcat(command, dbName));
	chdir(dbName);
	cout << sizeof(DataAttrInfo) << endl;
	rmm.CreateFile("attrcat", sizeof(DataAttrInfo));
	rmm.CreateFile("tablelist", sizeof(TableInfo));
	chdir("..");
	return 0;
}

int SM_Manager::OpenDb (const char *dbName){
	if (dbName == NULL){
		myLog->LogDebug(LOG_TAG, dbName_Null);
		return FAIL;
	}
	if (IsOpenDB == true){
		myLog->LogDebug(LOG_TAG, dbName_opened);
		return FAIL;
	}
	if (chdir(dbName) < 0){
		myLog->LogDebug(dbName_not_exist);
		return FAIL;
	}
//	cout << "OpenDb" << endl;
//	char dest[80];
	strncpy(DataName, dbName, 80);
//	cout << dest << endl;
	rmm.OpenFile("attrcat", attrfh);
	cout << attrfh.getFileID() << endl;
	rmm.CloseFile(attrfh);
//	rmm.OpenFile("relcat", relfh);
//	cout << relfh.getFileID() << endl;
	IsOpenDB = true;
	return 0;
}

int SM_Manager::CloseDb(){
	if (IsOpenDB == false){
		myLog->LogDebug(dbName_closed);
		return FAIL;
	}
	//rmm.CloseFile(attrfh);
	chdir("..");
//	rmm.CloseFile(relfh);
	IsOpenDB = false;
	return 0;
}

int SM_Manager::DropDb(const char *dbname){
	CloseDb();
	char command1[80] = "rm -rf ";
	cout << "SM_Manager::DropDb()" << endl;
//	char command1[80] = "rd /s ";
	system (strcat(command1, dbname));
	return 0;
}

int SM_Manager::CreateTable (const char *relName,                // 创建表, 这里需要检查attribute里面没有重复的名字
                    int        attrCount,
					AttrInfo   *attributes){
	if (strcmp(relName, "relcat") == 0 || strcmp(relName, "attrcat") == 0) {
		myLog->LogDebug(dbName_not_right);
		return FAIL;
	}
//	cout << "aaaa" << endl;
	rmm.OpenFile("attrcat", attrfh);
	int fileID = attrfh.getFileID();
//	cout << "fileID " << fileID << endl;
	DataAttrInfo* d = new DataAttrInfo[attrCount];
	for (int i = 0; i < attrCount; i++)
		d[i] = DataAttrInfo();
	int size = 0;
	int len = 0;
	int returnCode = 0;
	int ix = 0;
	RID rid;
	for (int i = 0; i < attrCount; i++){
		len += attributes[i].attrLength;
	}
	for (int i = 0; i < attrCount; i++) {
		d[i].setAttributes(attributes[i]);
		d[i].offset = size;
		d[i].indexNo = ix + len;
		ix++;
		size += attributes[i].attrLength;
		strcpy (d[i].relName, relName);
		returnCode = attrfh.InsertRec((char*) &d[i], rid);
	}
	rmm.CloseFile(attrfh);
	rmm.OpenFile("tablelist", tablefh);
	TableInfo tbinfo;
	strcpy(tbinfo.tableName, relName);
//	cout << "CreateTable: " << tbinfo.tableName << endl;
	tablefh.InsertRec((char*)&tbinfo, rid);
	rmm.CloseFile(tablefh);
//	rmm.CreateFile(relName, size);
	//add null, every slot --> null or not null
	rmm.CreateFile(relName, size + attrCount);
/*	DataRelInfo rel;
	strcpy(rel.relName, relName);
	rel.relLength = size;
	relfh.InsertRec((char*) &rel, rid);*/
	delete []d;
	return 0;
}

int SM_Manager::DropTable (const char * deleteRelName){
	RM_FileScan rfs = RM_FileScan(rmm.getFileManager(), rmm.getBufPageManager());
	RM_Record rec;
	//RM_FileHandle attrfh;
	//RM_FileHandle relfh;
	//打开rel文件，查询所有relName属性 = deleteRelName的记录，删除该记录
/*	rfs.OpenScan(relfh, STRING, strlen(deleteRelName), 4, EQ_OP, (void*)deleteRelName);
	while (rfs.GetNextRec(rec) != -1){
		relfh.DeleteRec(rec.rid);
	}
	rfs.CloseScan();*/
	//打开attr文件，查询所有relName属性 = deleteRelName的记录，删除该记录
	rmm.OpenFile("attrcat", attrfh);
	rfs.OpenScan(attrfh, STRING, strlen(deleteRelName), 16, EQ_OP, (void*)deleteRelName);
	while (rfs.GetNextRec(rec) != -1){
		attrfh.DeleteRec(rec.rid);
	}
	rfs.CloseScan();
	rmm.CloseFile(attrfh);
	rmm.OpenFile("tablelist", tablefh);
	rfs.OpenScan(tablefh, STRING, strlen(deleteRelName), 0, EQ_OP, (void*)deleteRelName);
	if (rfs.GetNextRec(rec) != -1){
		tablefh.DeleteRec(rec.rid);
	}
	rfs.CloseScan();
	rmm.CloseFile(tablefh);
	rmm.DestroyFile(deleteRelName);
	return 0;
}

int SM_Manager::ShowTable (const char *readRelName){
	//打开attr文件，查询所有relName属性 = readRelName的记录， 打印该记录
	RM_FileScan rfs = RM_FileScan(rmm.getFileManager(), rmm.getBufPageManager());	
 	RM_Record rec;
	int returnCode;
//	cout << "ShowTable" << endl;
	rmm.OpenFile("attrcat", attrfh);
	returnCode = rfs.OpenScan(attrfh, STRING, strlen(readRelName), 16, EQ_OP, (void*)readRelName);
	int x = 0;
//	cout << "TableName : "<< readRelName << endl;
//	cout << "returnCode: " << returnCode << endl;
	while (returnCode == 1){
//		cout << "ret : 1" << endl;
		x = rfs.GetNextRec(rec);
//		cout << "ret : 2" << endl;
		if (x == -1)
			break;
		cout << x << endl;
		DataAttrInfo * mydata;
		rec.GetData((char*&) mydata);
		mydata->print();
	}
	cout << "Table End" << endl;
	rfs.CloseScan();
	rmm.CloseFile(attrfh);
	return 0;
}

int SM_Manager::GetTable (const char *readRelName, int &number, DataAttrInfo* &dataInfo){
	//打开attr文件，查询所有relName属性 = readRelName的记录， 将attr个数传给number, dataAttrInfo传给dataInfo,内存释放需要外面执行
	//属性个数存在上限100
	RM_FileScan rfs = RM_FileScan(rmm.getFileManager(), rmm.getBufPageManager());	
 	RM_Record rec;
	int returnCode;
//	cout << "GetTable" << endl;
	rmm.OpenFile("attrcat", attrfh);
	returnCode = rfs.OpenScan(attrfh, STRING, strlen(readRelName), 16, EQ_OP, (void*)readRelName);
	int x = 0;
	dataInfo = new DataAttrInfo[100];
	number = 0;
//	cout << "TableName : "<< readRelName << endl;
	while (returnCode == 1){
//		cout << "ret : 1" << endl;
		x = rfs.GetNextRec(rec);
//		cout << "ret : 2" << endl;
		if (x == -1)
			break;
//		cout << x << endl;
		DataAttrInfo * mydata;
		rec.GetData((char*&) mydata);
		dataInfo[number].copy(mydata);
		number++;
	}
//	cout << "Table End" << endl;
	rfs.CloseScan();
	rmm.CloseFile(attrfh);
	return 0;
}

int SM_Manager::ShowDb(const char *dbName) {
	CloseDb();
	if (OpenDb(dbName) != 0)
		return -1;
	rmm.OpenFile("tablelist", tablefh);
	RM_FileScan rfs = RM_FileScan(rmm.getFileManager(), rmm.getBufPageManager());
	 RM_Record rec;
	rfs.OpenScan(tablefh, STRING, 0, 0, NO_OP, NULL);
	cout << "----- begin -----" << endl;
	while (rfs.GetNextRec(rec) != -1) {
		cout << rec.rid << " , " << rec.data << endl;
	}
	cout << "----- end -----" << endl;
	rmm.CloseFile(tablefh);
	return 0;
}

int SM_Manager::ShowDb() {
	CloseDb();
	if (OpenDb(DataName) != 0)
		return -1;
	rmm.OpenFile("tablelist", tablefh);
	RM_FileScan rfs = RM_FileScan(rmm.getFileManager(), rmm.getBufPageManager());
	 RM_Record rec;
	rfs.OpenScan(tablefh, STRING, 0, 0, NO_OP, NULL);
	cout << "----- begin -----" << endl;
	while (rfs.GetNextRec(rec) != -1) {
		cout << rec.rid << " , " << rec.data << endl;
	}
	cout << "----- end -----" << endl;
	rmm.CloseFile(tablefh);
	return 0;
}


int SM_Manager::Exec(const char *instr) {
	vector<string> argv;
	string cur;
	cout << instr << endl;
	for (int i = 0; ; i++) {
		if (instr[i] == ' ' || instr[i] == '(' || instr[i] == ')' || instr[i] == ',' || instr[i] == 0) {
			if (cur.length() > 0) {
				argv.push_back(cur);
				cur = "";
			}
			if (instr[i] == 0){
				break;
			}
		}
		else {
			cur = cur + instr[i];
		}
	}
	if (argv.size() < 3)
		return -1;
	if (argv[0] == "CREATE" && argv[1] =="DATABASE") {
		CreateDb(argv[2].c_str());
		return 0;
	}
	if (argv[0] == "DROP" && argv[1] == "DATABASE") {
		DropDb(argv[2].c_str());
		return 0;
	}
	if (argv[0] == "USE" && argv[1] == "DATABASE") {
		CloseDb();
		OpenDb(argv[2].c_str());
		return 0;
	}
	if (argv[0] == "SHOW" && argv[1] == "DATABASE") {
		ShowDb(argv[2].c_str());
		return 0;
	}
	if (argv[0] == "CREATE" && argv[1] == "TABLE") {
		string tablename = argv[2];
		vector<AttrInfo> attrs;
		for (int i = 3; i < argv.size();) {
			AttrInfo attr;
			if (argv[i] == "PRIMARY" && argv[i + 1] == "KEY") {
				attr.primaryKey = true;
				i += 2;
			}
			cout << argv[i] << endl;
			strcpy(attr.attrName, argv[i].c_str());
			i += 1;
			if (argv[i] == "int") {
				attr.attrType = MyINT;
				attr.attrLength = 4;
				i += 1;
				cout << "lala" << endl;
			} else if (argv[i] == "float") {
				attr.attrType = FLOAT;
				attr.attrLength = 4;
				i += 1;
			} else if (argv[i] == "char") {
				attr.attrType = STRING;
				attr.attrLength = atoi(argv[i + 1].c_str());
				i += 2;
			}
			if (i + 1 < argv.size() && argv[i] == "NOT" && argv[i + 1] == "NULL") {
				attr.notNull = true;
				i += 2;
			}
			attrs.push_back(attr);
		}
		AttrInfo *x = new AttrInfo[attrs.size()];
		for (int i = 0; i < attrs.size(); i++)
			x[i] = attrs[i];
		CreateTable(tablename.c_str(), attrs.size(), x);
		delete x;
		return 0;
	}
	if (argv[0] == "DROP" && argv[1] == "TABLE") {
		DropTable(argv[2].c_str());
		return 0;
	}
	if (argv[0] == "SHOW" && argv[1] == "TABLE") {
		ShowTable(argv[2].c_str());
		return 0;
	}
	return 0;
}
