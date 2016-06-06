记录模块

header files:
	
	RecordManager.h :	包含RID，RM_Record, FileHead, RM_FileHandle, PFS, RM_Manager
	rm_filescan.h	:	包含RM_Filescan
	PageHead.h		:	包含PageHead页头类
	
RM Interface:

	RM_Manager		:	处理RM部分的新建，删除，打开，关闭文件记录，程序仅实例化一个这样的类
						所有请求均由这一个实例来处理
						函数如下：
							RM_Manager	   (PF_Manager &pfm)									:构造函数,	其中有页式文件系统的对象
							~RM_Manager    ()													:析构函数
							int CreateFile  (const char *fileName, int recordSize)				:调用FileManager::CreateFile新建一个页文件fileName
																								 该页中的所有记录大小都是recordSize.
							int DestroyFile (const char *fileName)								:调用FileManager::DestroyFile删除文件名为fileName的文件
							int OpenFile    (const char *fileName, RM_FileHandle &fileHandle)	:调用FileManagerManager::OpenFile打开fileName的文件，如果打
																								 开成功能通过fileHandle打开RM相关的文件，这里不考虑多次打开同一
																							 	 文件的情况，也不考虑DestroyFile会操作一个正在打开的文件。
																								 每次打开同一个文件用的是不同的fileHandle。
							int CloseFile   (RM_FileHandle &fileHandle)							:通过fileHandle获得fileID，调用FileManager:: CloseFile

	RM_FileHandle	:	操作使用RM Interface打开的文件中的记录
						函数如下:
							RM_FileHandle  	()												:构造函数
							~RM_FileHandle  ()												:析构函数
							int GetRec       (const RID &rid, RM_Record &rec) const			:如果不存在rid，需要提示错误，如果成功，rec中存放记录的拷贝
							int InsertRec 	(char *pData, RID &rid)							:插入一个pData指向的数据作为一个新的记录，如果成功，&rid将
																							 指向这个新插入记录的RID
							int DeleteRec 	(RID &rid)										:删除文件中记录为RID的记录，如果页中所含记录为空，调用
																							 FileManager::DisposePage处理该页
							int UpdateRec    (RM_Record &rec)								:更新所有和rec相关的记录，具体定义见RM_Record
																							 
	RM_FileScan		:	提供RM文件记录的检索功能，检索功能依赖于指定的几个条件
						函数如下:
							RM_FileScan  	()												:构造函数
							~RM_FileScan 	()												:析构函数
							int OpenScan     (const RM_FileHandle &fileHandle,  
											 AttrType      attrType,
											 int           attrLength,
											 int           attrOffset,
											 CompOp        compOp,
											 void          *value,
											 ClientHint    pinHint = NO_HINT)				:当value为空指针时，查询所有记录，如果不为空时，指向value，
																							 用于查询比较。attrType的定义在database.h中INT，FLOAT和STRING
																							 attrOffset为查询记录的初始位置(查询某个字段的值)，查询操作具
																							 体定义在database.h中
																							 EQ_OP					equal		a = v
																							 LT_OP					less than	a < v
																							 GT_OP					greater than a > v
																							 LE_OP					a <= v
																							 GE_OP					a >= v
																							 NE_OP					a <> v
																							 NO_OP					no comparison value = null
																							 
							int GetNextRec 	(RM_Record &rec)								:获取下一条record
							int CloseScan 	()												:终止检索
							
	RM_Record		:	提供文件记录的基本操作
						函数如下：
							int Set			(char *pData, int size, RID rid_)				:设置记录内容
							int GetData		(char *&pData)									:获取记录内容
							int GetRid		(RID &rid)										:获取记录RID
							
	RID				:	记录的Identifier
						函数如下:
							RID				()												:构造函数
							~RID			()												:析构函数
							int GetPageNum	(int &pageNum) const							:获取record的页数
							int GetSlotNum	(int &slotNum) const							:获取record的槽编号
							int Page		() const										:获取record的页数
							int Slot		() const   										:获取record的槽编号
						
	PageHead		:	数据页页头格式，暂时格式为一个int和84位的位图组成
						函数如下：
							bool getRecordHead	(int RecordID)								:查询RecordID对应记录是否存在
							int setRecordHead	(int RecordID, bool IsRecord)				:设置RecordID对应的槽
	
	FileHead		:	文件第一页格式，文件第一页存储
							int recordSize													:记录长度
							int pageNumber													:页的个数
							int recordPerPage												:每页记录个数
							int	recordNumber												:记录的总数

测试部分

	main.cpp		:	测试记录管理模块的正确性
						在test_RM_FileScan()函数中:
						首先新建文件，打开文件，然后插入32byte大小的数据，数据定义为：
							struct Rec{
								int b; float f; char a[24];}
						测试插入记录、删除记录、更新记录与获取属性值满足特定条件的记录。

	测试结果符合预期
	
	

系统管理模块

	SM_Manager : int CreateDb(const char *)
	             int OpenDb(const char *)
	             int CloseDb()
	             int DropDb(const char *)
	             int ShowDb(const char *)
	             int CreateTable(const char *tableName, int attrCount, AttrInfo *attrs)
	             int DropTable(const char *tableName)
	             int Exec(const char *instruction)

	测试代码 : main.cpp / test_SM_Manager()