#ifndef DECLEAR_H
#define DECLEAR_H

#define MAXNAME 50
#define MAXATTRNUMBER 40
#define MAXATTRLENGTH 400
#define MAXRID 100000
#define MAXRELATION 5

#include <iostream>
#include "../recordmanager/rm_filescan.h"
#include "../systemmanager/sm_manager.h"
#include "../recordmanager/RecordManager.h"
#include "../common/common.h"
#include "../indexmanager/IX_Manager.h"

using namespace std;

// enum AttrType {
    // MyINT,
    // FLOAT,
    // STRING
// }; 

// enum ClientHint {
    // NO_HINT
// };

// struct AttrInfo {
   // char     attrName[MAXNAME + 1];           // Attribute name
   // AttrType attrType;            // Type of attribute
   // int      attrLength;          // Length of attribute
   // int     notNull;
   // bool     primaryKey;
   // AttrInfo(){
       // memset(attrName, 0, sizeof(attrName));
       // attrType = MyINT;
       // attrLength = 0;
       // notNull = 0;
       // primaryKey = false;
   // }
// };

struct RelAttr {
    AggeType type;
    char *relName;     // relation name (may be NULL) 
    char *attrName;    // attribute name  
    char *outkeyrel;
    char *outkeyattr;    
    RelAttr(){
        relName = NULL;
	attrName = NULL;
	outkeyrel = NULL;
	outkeyattr = NULL;
    }        
    //friend ostream &operator<<(ostream &s, const RelAttr &ra);
};

struct Value {
    AttrType type;     // type of value               
    void     *data;    // value                       
    friend ostream &operator<<(ostream &s, const Value &v){
        if(v.type == MyINT) {
            int *ddd = (int*)v.data;
            s << "INT : " << *ddd << endl;
        }
        else if(v.type == FLOAT) {
            float *ddd = (float*)v.data;
            s << "FLOAT : " << *ddd << endl;
        }
        else if(v.type == STRING) {
            char *ddd = (char*)v.data;
            s << "STRING : " << ddd << endl;
        }
        return s;
        }
    int getlength() const {
        if (type == MyINT){
            return sizeof(int);
        }
        else if (type == FLOAT){
            return sizeof(float);
        }
        else if (type == STRING){
            char *ddd = (char*)data;
            return strlen(ddd);
        }
    }
};

struct Condition {
    RelAttr lhsAttr;      // left-hand side attribute                     
    CompOp  op;           // comparison operator                          
    int     bRhsIsAttr;   // TRUE if right-hand side is an attribute
                          //   and not a value
    RelAttr rhsAttr;      // right-hand side attribute if bRhsIsAttr = TRUE
    Value   rhsValue;     // right-hand side value if bRhsIsAttr = FALSE
    //friend ostream &operator<<(ostream &s, const Condition &c);
};

// class RID {
// public:
  // static const int NULL_PAGE = -1;
  // static const int NULL_SLOT = -1;
  // RID() : page(NULL_PAGE), slot(NULL_SLOT) {}     // Default constructor
  // RID(int pageNum, int slotNum) : page(pageNum), slot(slotNum) {}
  // ~RID(){}                                        // Destructor

  // int GetPageNum(int &pageNum) const          // Return page number
  // { pageNum = page; return 0; }
  // int GetSlotNum(int &slotNum) const         // Return slot number
  // { slotNum = slot; return 0; }

  // int Page() const          // Return page number
  // { return page; }
  // int Slot() const          // Return slot number
  // { return slot; }

  // bool operator==(const RID & rhs) const
  // {
    // int p;
    // int s;
    // rhs.GetPageNum(p);
    // rhs.GetSlotNum(s);
    // return (p == page && s == slot);
  // }

// private:
  // int page;
  // int slot;
// };

class QL_Manager {
    public:
                                              // Constructor
        QL_Manager (SM_Manager *_smm, RM_Manager *_rmm, IX_Manager *_ixm):smm(_smm), rmm(_rmm), ixm(_ixm){}
        //QL_Manager (SM_Manager &smm, IX_Manager &ixm, RM_Manager &rmm);
        ~QL_Manager ();                         // Destructor
        
        void Print(int nSelAttrs, const RelAttr selAttrs[], const char* relName, int nrid, RID* rid) {
            cout << "QL PRINT Function" << endl;
            cout << relName << endl;
            cout << "NSelAttr : " << nSelAttrs << endl;
            for(int i = 0; i < nSelAttrs; i++) {
                cout << selAttrs[i].type << ' ' << (selAttrs[i].relName == NULL) << ' ' << selAttrs[i].attrName << endl;
            } 
            cout << "nrid" << nrid << endl;
	    for (int i = 0; i < nrid; i++)
			cout << rid[i] << endl;
            //select *
            int SelectPoint = 0;
            int newSelAttrs = 0;
            if (nSelAttrs == 1 && strcmp(selAttrs[0].attrName,"*") == 0){
				cout << "select *" << endl;
				SelectPoint = 1;
            }
            RM_FileScan rfs = RM_FileScan(rmm->getFileManager(), rmm->getBufPageManager());
        RM_Record rec;
            RM_FileHandle attrfh;
            int returnCode = 0; 
            rmm->OpenFile("attrcat",attrfh);
            returnCode = rfs.OpenScan(attrfh, STRING, strlen(relName), 16, EQ_OP, (void*)relName);
        int x = 0;
            DataAttrInfo * getAttr;
            if (SelectPoint == 0)
                getAttr = new DataAttrInfo[nSelAttrs];
            else
        getAttr = new DataAttrInfo[MAXATTRNUMBER];

        while (returnCode == 1){
//      cout << "ret : 1" << endl;
        x = rfs.GetNextRec(rec);
//      cout << "ret : 2" << endl;
        if (x == -1)
            break;
//      cout << x << endl;
        DataAttrInfo * mydata;
        rec.GetData((char*&) mydata);
//          cout << mydata->attrName << endl;
//      cout << cond->lhsAttr.attrName << endl;
        if (SelectPoint == 0){
            for (int i = 0; i < nSelAttrs; i++){
                if (strcmp(mydata->attrName, selAttrs[i].attrName) == 0 && strcmp(mydata->relName, relName) == 0){
//                  cout << "offset: " << mydata->offset << "i: " << i << endl;
//                  cout << sizeof(DataAttrInfo) << endl;
                    memcpy(&(getAttr[i]), mydata, sizeof(DataAttrInfo));
//                  cout << getAttr[i].offset << endl;
                    break;
                }
            }
        }
        else{
            if (strcmp(mydata->relName, relName) == 0){
                memcpy(&(getAttr[newSelAttrs]), mydata, sizeof(DataAttrInfo));
                newSelAttrs++;
            }
        }
        }
        rfs.CloseScan();
        rmm->CloseFile(attrfh);
        
            if (SelectPoint != 0){
        cout << "new SelAttrs" << newSelAttrs << endl;
        nSelAttrs = newSelAttrs;
        }
        cout << "Start print attrName: " << endl;
        for (int i = 0; i < nSelAttrs; i++)
        cout << getAttr[i].attrName << ' ' <<getAttr[i].offset << " length " << getAttr[i].attrLength << endl;
        double* p = new double[nSelAttrs];
	int all = 0;
	int use = 0;
	for (int i = 0; i < nSelAttrs; i++){
		p[i] = 0;	
	}
        //get record
        cout << "ALL RECORD========= (0_0)=========" << endl;
	FILE *fp = fopen("../output.csv", "w");
  for (int i = 0; i < nSelAttrs; i++){
     fprintf(fp, "%s\t", getAttr[i].attrName);
  }
  fprintf(fp,"\n");
  int label_1 = 0;
  for (int i = 0; i < nSelAttrs; i++){
    if (SelectPoint == 0 && selAttrs[i].type != NONE){
      label_1 = 1;
      break;
    }
  }
            for (int i = 0; i < nrid; i++){
                rmm->OpenFile(relName, attrfh);
        RM_Record tempRec;
        cout << rid[i] << endl;
                attrfh.GetRec(rid[i], tempRec);
        char *temp;
        temp = tempRec.data;
//      for (int i = 0; i < strlen(temp); i++)
//          cout << temp[i];
                for (int j = 0; j < nSelAttrs; j++){
//	    cout << getAttr[j].attrName << ": " << getAttr[j].offset << ' ';
            cout << getAttr[j].attrName << ": ";
//	    fprintf(fp, "%s\t", getAttr[j].attrName);
            char* buf = new char[MAXATTRLENGTH];
            memset(buf, 0, MAXATTRLENGTH);
            int length = 0;
            if (getAttr[j].attrType == MyINT)
                length = sizeof(int);
            else if (getAttr[j].attrType == FLOAT)
                length = sizeof(float);
            else
                length = getAttr[j].attrLength;
            memcpy(buf, temp + getAttr[j].offset, length);
        //  cout << "buf: ";
        //  for (int k = 0; k < 10; k++)
        //      cout << buf[k];
        //  cout << endl;
                        if (getAttr[j].attrType == MyINT){
                             int* t = (int*)buf;
			    if (SelectPoint != 0){
				cout << *t;
				fprintf(fp, "%d\t", *t);
			    }
			    else if (selAttrs[j].type == NONE){
			    	cout << *t;
				fprintf(fp, "%d\t", *t);
			    }
			    else if (selAttrs[j].type == SUM){
			    	p[j] = p[j] + (double)*t;
				cout << p[j];
			    }
			    else if (selAttrs[j].type == AVG){
				p[j] = p[j] + (double)*t;
				all++;
			    }
			    else if (selAttrs[j].type == 3){
				if (use == 0){
					p[j] = (double)*t;
					use++;
				}
				else {if (p[j] > *t)
					p[j] = (double)*t;

				}
			    }
			    else if (selAttrs[j].type == 4){
				if (use == 0){
					p[j] = (double)*t;
					use++;
				}
				else{ if ((int)p[j] < (*t))
					p[j] = (double)*t;
				}
			    }
            }
                        else if (getAttr[j].attrType == FLOAT){
                 	    float* t = (float*)buf;
			    if (SelectPoint != 0){
				cout << *t;
				fprintf(fp, "%f\t", *t);
			    }
   			    else if (selAttrs[j].type == NONE){
			    	cout << *t;
				fprintf(fp, "%f\t", *t);
			    }
			    else if (selAttrs[j].type == SUM){
			    	p[j] = p[j] + (double)*t;
			    }
			    else if (selAttrs[j].type == AVG){
				p[j] = p[j] + (double)*t;
				all++;
			    }
			    else if (selAttrs[j].type == MIN){
				if (use == 0){
					p[j] = (double)*t;
					use++;
				}
				else if ((float)p[j] > (*t))
					p[j] = (double)*t;
			    }
			    else if (selAttrs[j].type == MAX){
				if (use == 0){
					p[j] = (double)*t;
					use++;
				}
				else if (p[j] < (double)(*t))
					p[j] = (double)*t;
			    }
	
            }
                        if (getAttr[j].attrType == STRING){
                             char* t = buf;
		            if (SelectPoint != 0){
				cout << t;
				fprintf(fp, "%s\t", t);
			    }
			    else if (selAttrs[j].type == NONE){
			    	cout << t;
				fprintf(fp, "%s\t", t);
			    }
			}
                        cout << ", ";
		}
		cout << endl;
    if (label_1 != 1)
		fprintf(fp, "\n");
		rmm->CloseFile(attrfh);
            };
	    for (int j = 0; j < nSelAttrs; j++){
	//	cout << selAttrs[j].type << endl;
		if (selAttrs[j].type == MIN || selAttrs[j].type == MAX || selAttrs[j].type == SUM){
		      cout << getAttr[j].attrName << ": " << p[j] << endl;
		      fprintf(fp, "%f\t", p[j]);
		}
		if (selAttrs[j].type == AVG){
		      cout << getAttr[j].attrName << ": " << (p[j]/(double)all) << endl;
		      fprintf(fp, "%f\t", (p[j]/(double)all));
		}
	    }
	    fclose(fp);
	    delete []getAttr;
	    delete []p;
            cout << "-----------END PRINT-----------" << endl;
        }


        void Select (int           nSelAttrs,        // # attrs in Select clause
                    RelAttr selAttrs[],       // attrs in Select clause
                   int           nRelations,       // # relations in From clause
                   char *  relations[], // relations in From clause
                   int           isGroup,
                   int           nConditions,      // # conditions in Where clause
                    Condition conditions[],  // conditions in Where clause
                   const RelAttr& groupAttr) 
        {
        //cout << "shabi" << endl;
            cout << "QL Select Function" << endl;
          cout << "NSelAttr : " << nSelAttrs << endl;
            for(int i = 0; i < nSelAttrs; i++) {
                cout << selAttrs[i].type << ' ' << (selAttrs[i].relName == NULL) << ' ' << selAttrs[i].attrName;
		if (selAttrs[i].relName != NULL){
			cout << ' ' << selAttrs[i].relName << ' ';
		}
		cout << endl;
            } 
            cout << "NRELATIONS : " << nRelations << endl;
            for(int i = 0; i < nRelations; i++) {
                cout << relations[i] << endl;
            }

            cout << "ISGROUP : " << isGroup << endl;
            if(isGroup) {
                cout << (groupAttr.relName == NULL) <<  ' ' << groupAttr.attrName << endl; 
            }
            else {
                cout << "NCONDITIONS : " << nConditions << endl;
 //               for(int i = 0; i < nConditions; i++) {
 //                   cout << conditions[i].lhsAttr.attrName << ' ' << conditions[i].op << endl;
 //               }
            }
	    if (nRelations == 1){
		for (int i = 0; i < nSelAttrs; i++)
		    selAttrs[i].relName = relations[0];
	    }

  	    RM_FileScan rfs = RM_FileScan(rmm->getFileManager(), rmm->getBufPageManager());
	    RM_FileHandle attrfh;

	    int SelectPoint = 0;
	    if (nSelAttrs == 1 && strcmp(selAttrs[0].attrName,"*") == 0){
		cout << "select *" << endl;
		SelectPoint = 1;
            }

	    if (SelectPoint == 0){
            for (int i = 0; i < nSelAttrs; i++){
		if (selAttrs[i].relName == NULL){
		    cout << "Select Error: need relation" << endl;
		    return;
		}
		int judge = 0;
		for (int j = 0; j < nRelations; j++){
		    if (strcmp(relations[j], selAttrs[i].relName) == 0){
			judge = 1;
			break;
		    }		
		}
		if (judge == 0){
		    cout << "Select Error: can't get relation" << endl;
		    return;
		}
	    }
	    }
//==
            int newSelAttrs = 0;
	    int returnCode_0 = 0;
	    int attrIndex = 0;

	    rmm->OpenFile("attrcat",attrfh);
            DataAttrInfo * getAttr;
	    int* attrOrder;
            if (SelectPoint == 0){
            	getAttr = new DataAttrInfo[nSelAttrs];
		attrOrder = new int[nSelAttrs];
	    }
            else{
		getAttr = new DataAttrInfo[MAXATTRNUMBER];
		attrOrder = new int[MAXATTRNUMBER];
	    }
	    if (SelectPoint == 0){	    
		for (int i = 0; i < nSelAttrs; i++){
			int x_0 = 0;
			RM_Record selRec;
	    		returnCode_0 = rfs.OpenScan(attrfh, STRING, strlen(selAttrs[i].relName), 16, EQ_OP, (void*)(selAttrs[i].relName));
//			cout << "returnCode_0:" << returnCode_0 << selAttrs[i].relName << endl;
			while (returnCode_0 == 1){
				x_0 = rfs.GetNextRec(selRec);
				if (x_0  == -1){
					cout << "la" << endl;
					break;
				}
				DataAttrInfo *mydata;
				selRec.GetData((char*&) mydata);
//				cout << "check======>" << mydata->attrName << ' ' << selAttrs[i].attrName << endl;
				if (strcmp(mydata->attrName, selAttrs[i].attrName) == 0){
//					cout << "get! ==============> " << endl;
					memcpy(&(getAttr[attrIndex]),mydata, sizeof(DataAttrInfo));
					for (int j = 0; j < nRelations; j++){
						if (strcmp(relations[j], selAttrs[i].relName) == 0){
							attrOrder[attrIndex] = j;
							break;
						}
					}
					attrIndex++;
					break;
				}
			}
  			rfs.CloseScan();
		}
	    }
	    else{
		cout << "1: " << endl;
		cout << nRelations << endl;
		for (int i = 0; i < nRelations; i++){
			int x_0 = 0;
			RM_Record selRec;
			returnCode_0 = rfs.OpenScan(attrfh, STRING, strlen(relations[i]), 16, EQ_OP, (void*)relations[i]);
			cout << "2: " << endl;
			while (returnCode_0 == 1){
				x_0 = rfs.GetNextRec(selRec);
				if (x_0  == -1)
					break;
				DataAttrInfo *mydata;
				selRec.GetData((char*&) mydata);
				memcpy(&(getAttr[attrIndex]),mydata, sizeof(DataAttrInfo));
				attrOrder[attrIndex] = i;
				attrIndex++;
			}
  			rfs.CloseScan();
		}		
            };
            rmm->CloseFile(attrfh);
/*
	    while (returnCode_0 == 1){
//		cout << "ret : 1" << endl;
		x = rfs.GetNextRec(rec);
//		cout << "ret : 2" << endl;
		if (x == -1)
			break;
//		cout << x << endl;
		DataAttrInfo * mydata;
		rec.GetData((char*&) mydata);
//	        cout << mydata->attrName << endl;
//		cout << cond->lhsAttr.attrName << endl;
		if (SelectPoint == 0){
			for (int i = 0; i < nSelAttrs; i++){
				if (strcmp(mydata->attrName, selAttrs[i].attrName) == 0 && strcmp(mydata->relName, relName) == 0){
//					cout << "offset: " << mydata->offset << "i: " << i << endl;
//					cout << sizeof(DataAttrInfo) << endl;
					memcpy(&(getAttr[i]), mydata, sizeof(DataAttrInfo));
//					cout << getAttr[i].offset << endl;
					break;
				}
			}
		}
		else{
			if (strcmp(mydata->relName, relName) == 0){
				memcpy(&(getAttr[newSelAttrs]), mydata, sizeof(DataAttrInfo));
				newSelAttrs++;
			}
		}
	    }
	    rfs.CloseScan();
            rmm->CloseFile(attrfh);
	*/ 
	    cout << "check " << endl;   
            if (SelectPoint != 0){
//		cout << "new SelAttrs" << attrIndex << endl;
		nSelAttrs = attrIndex;
	    }
//	    cout << "Start print attrName: " << endl;
//	    for (int i = 0; i < nSelAttrs; i++)
//		cout << getAttr[i].relName << ' ' << getAttr[i].attrName << ' ' <<getAttr[i].offset << endl;
///==	    

	    int* leftRel = new int[nConditions + 2];
	    int* rightRel = new int[nConditions + 2];
		
	    memset(leftRel, 0, sizeof(int) * (nConditions + 2));
	    memset(rightRel, 0, sizeof(int) * (nConditions + 2));
	    for (int i = 0; i < nConditions; i++){
		if (conditions[i].lhsAttr.relName == NULL){
		    cout << "Select Error: need relation" << endl;
		    return;
		}
		if (conditions[i].bRhsIsAttr && conditions[i].rhsAttr.relName == NULL){
		    cout << "Select Error: need relation" << endl;
		    return;
		}
		int judge = 0;
		for (int j = 0; j < nRelations; j++){
		    if (strcmp(relations[j], conditions[i].lhsAttr.relName) == 0){
			judge = 1;
			leftRel[i] = j;
//			cout << "what i, j" << i << ' ' << j << endl;
			break;
		    }
		}
		if (judge == 0){
		    cout << "Select Error: can't get relation" << endl;
		    return;
		}
		
		int judge_1 = 0;
		for (int j = 0; j < nRelations; j++){
//		    cout << "kao      " << conditions[i].bRhsIsAttr << "||" << relations[j] <<"||" << j << "||";
//		   if ((conditions[i].bRhsIsAttr != 0))  cout << conditions[i].rhsAttr.relName << ' ';
//		 cout << endl;
		    if ((conditions[i].bRhsIsAttr != 0) && (strcmp(relations[j], conditions[i].rhsAttr.relName) == 0)){
			judge_1 = 1;
			rightRel[i] = j;
//			cout << "get i , j" << i << ' ' << j << endl;
			break;
		    }
		}
		if (judge_1 == 0 && (conditions[i].bRhsIsAttr != 0)){
		    cout << "Select Error: can't get relation" << endl;
		    return;
		}
	    }
//	    cout << "========================" << endl;
/*	   cout << nConditions << endl;
	    for (int i = 0; i < nConditions; i++){
		cout << "i: " << i << ' ' << leftRel[i] << ' ' << rightRel[i] << endl;
	    }*/
	    //get All Rid
            RID **rid = new RID*[MAXRELATION];
            for(int i = 0;i < MAXRELATION; i++)
                rid[i]=new RID[MAXRID];
 	    int* number = new int[MAXRELATION];
	    int* index = new int[MAXRELATION];
            for (int i = 0; i < nRelations; i++){
	    	int returnCode = 0;
		int nrid = 0;
		int x = 0;
	    	RM_Record rec_1;
		RID tempRid;
 	        rmm->OpenFile(relations[i], attrfh);
	    	returnCode = rfs.OpenScan(attrfh, MyINT, 0, 0, EQ_OP, NULL);
	        while (returnCode == 1){
		    x = rfs.GetNextRec(rec_1);
		    if (x == -1)
			break;
		    rec_1.GetRid(tempRid);
		    rid[i][nrid].Copy(tempRid);
//		    cout << rid[i][nrid] << endl;
		    nrid++;
//		    cout << "get Rid: " << tempRid << endl;
	    	}
		number[i] = nrid;
	    	rfs.CloseScan();
	    	rmm->CloseFile(attrfh);		
	    }
	
	    for (int i = 0; i < nRelations; i++)  
		index[i] = 0;   
	    
            //get all condition
	    DataAttrInfo *left = new DataAttrInfo[MAXRID];
	    DataAttrInfo *right = new DataAttrInfo[MAXRID];
	    int returnCode;
	    int rreturnCode;
            rmm->OpenFile("attrcat",attrfh);
            
	    for (int i = 0; i < nConditions; i++){
		RM_Record rec;
		RM_Record rrec;
            	returnCode = rfs.OpenScan(attrfh, STRING, strlen(conditions[i].lhsAttr.relName), 16, EQ_OP, (void*)(conditions[i].lhsAttr.relName));
		int x = 0;
		while (returnCode == 1){
			x = rfs.GetNextRec(rec);
			if (x == -1)
				break;
			DataAttrInfo * mydata;
			rec.GetData((char*&) mydata);
			if (strcmp(mydata->attrName, conditions[i].lhsAttr.attrName) == 0){
                            memcpy(&left[i], mydata, sizeof(DataAttrInfo));
			    break;
			}
	    	}
		if (conditions[i].bRhsIsAttr){
	            	rreturnCode = rfs.OpenScan(attrfh, STRING, strlen(conditions[i].rhsAttr.relName), 16, EQ_OP, (void*)(conditions[i].rhsAttr.relName));
			int x = 0;
			while (rreturnCode == 1){
			    x = rfs.GetNextRec(rrec);
			    if (x == -1)
		                break;
			    DataAttrInfo * rmydata;
			    rrec.GetData((char*&) rmydata);
			    if (strcmp(rmydata->attrName, conditions[i].rhsAttr.attrName) == 0){
                        	memcpy(&right[i], rmydata, sizeof(DataAttrInfo));
			   	break;
			    }
	    		}			
		}
		rfs.CloseScan();
	    }
            rmm->CloseFile(attrfh);

	    cout << "===========start print==========" << endl;
/*	    for (int i = 0; i < nConditions; i++){
		cout << "left condition: " << left[i].relName << ' ' << left[i].attrName << "---op ---";
	    	if (conditions[i].bRhsIsAttr){
		    cout << "right condition:"  << right[i].relName << ' ' << right[i].attrName;
		}
		cout << endl;
            }*/

	    //check left.type == right.type
	    for (int i = 0; i < nConditions; i++){
		AttrType rightType;
		if (conditions[i].op == 7)
		    continue;
		if (conditions[i].bRhsIsAttr)
		    rightType = right[i].attrType;
		else
		    rightType = conditions[i].rhsValue.type;
	    //	cout << "RightType: " << rightType << ' ' << left[i].attrType << endl;
		if (rightType != left[i].attrType){
		    cout << "Select Error: attrType is not equal"<< endl;
		    return;
		}
	    }
//	    cout << "qby -1" << endl;
//	    for (int i = 0; i < nConditions; i++)
//		cout << rightRel[i] << endl;
      FILE *fp = fopen("../output.csv", "w");
      for (int i = 0; i < nSelAttrs; i++)
       fprintf(fp, "%s->%s\t", getAttr[i].relName, getAttr[i].attrName);
     fprintf(fp, "\n");
            while (true){
		int inCondition = 1;
		if (nConditions == 0)
		    inCondition = 1;
		else{
//		    cout << "start Condition" << endl;
		    for (int i = 0; i < nConditions; i++){
			void* rValue;
		//	cout << "start Loop" << endl;
			if (conditions[i].bRhsIsAttr){
		//	    cout << "qby 1" << endl;
			    RM_Record tempRec;
			    rmm->OpenFile(right[i].relName,attrfh);
               		    attrfh.GetRec(rid[rightRel[i]][index[rightRel[i]]], tempRec);
		       	    char *temp;
			    temp = tempRec.data;
			    char* buf = new char[MAXATTRLENGTH];
			    memset(buf, 0, MAXATTRLENGTH);
			    int length = 0;
			    //cout << "---------------an condition----------" << endl;
			    if (right[i].attrType == MyINT)
				length = sizeof(int);
			    else if (right[i].attrType == FLOAT)
				length = sizeof(float);
			    else
				length = right[i].attrLength;
			    memcpy(buf, (char*)temp + right[i].offset, length);
			    char t = *(char*)((char*)temp + right[i].offset);
			    // if is null
			    if (t == 0){
				inCondition = 0;
			    	delete []buf;
				break;
			    }
			    else{
			//	 cout << "length: " << length << endl;
				 rValue = new char[length + 1];
				 strncpy((char*)rValue, buf, length);
				 if (right[i].attrType == MyINT){
					int* t = (int*)rValue;
			//		cout << " 1: "<< *t;
				 }
			    	 else if (right[i].attrType == FLOAT){
					float* t = (float*)rValue;
			//		cout << " 1: "<< *t;
				 }
			   	 else{
					char* t = (char*)rValue;
			//		cout << " 1: "<< t;
				 }
			// 	  cout << "----end condition----" << endl;
			    }
			    delete []buf;
			//    cout << "qby 5" << endl;
        	            rmm->CloseFile(attrfh);
			}
			else{
			//    cout << "aaaaaaa" << endl;
			    if (conditions[i].rhsValue.data == NULL)
			    	rValue = NULL;
			    else{
				int length = 0;
			    	if (conditions[i].rhsValue.type == MyINT)
				    length = sizeof(int);
			    	else if (conditions[i].rhsValue.type == FLOAT)
				    length = sizeof(float);
			    	else
				    length = strlen((char*)conditions[i].rhsValue.data);
			//	cout << "length" << length << endl;
				rValue = new char[length + 1];
			        memcpy(rValue, conditions[i].rhsValue.data, length);
			    	if (conditions[i].rhsValue.type == MyINT){
				    int *t = (int *)rValue;
			//	    cout << " 2: "<< *t;
			        }
			        else if (conditions[i].rhsValue.type == FLOAT){
				    float *t = (float *)rValue;
			//	    cout << " 2: " <<*t;
			        }
			        else{
				    char*t = (char *)rValue;
			//	    cout << " 2: " <<t;
			        }
			//	cout << ' ';
			//        cout << "----end condition----" << endl;
			    }
			}
			RM_Record tempRec;
			rmm->OpenFile(left[i].relName, attrfh);
			attrfh.GetRec(rid[leftRel[i]][index[leftRel[i]]], tempRec);
		       	char *temp;
			temp = tempRec.data;
			char* buf = new char[MAXATTRLENGTH];
			memset(buf, 0, MAXATTRLENGTH);
			int length = 0;
			if (left[i].attrType == MyINT)
			    length = sizeof(int);
			else if (right[i].attrType == FLOAT)
		 	    length = sizeof(float);
			else
			    length = right[i].attrLength;
			memcpy(buf, (char*)temp + right[i].offset, length);
			char t = *(char*)((char*)temp + right[i].offset);
			//filescan only need conditions[i].op
			rfs.OpenScan(attrfh, STRING, strlen(left[i].relName), 16, conditions[i].op, (void*)left[i].relName);
			if (conditions[i].op == 7){
			    if (t != 0){
				inCondition = 0;
				rfs.CloseScan();
	    			rmm->CloseFile(attrfh);
				break;
			    }
			}
			if (t == 0){
			    inCondition = 0;
			    rfs.CloseScan();
	    		    rmm->CloseFile(attrfh);
			    break;
			}
			// is null			
			if (left[i].attrType == MyINT){
			    if (!rfs.condINT(*(int*)buf, *(int*)rValue)){
				inCondition = 0;
	//			cout << "int not equal" << *(int*)buf << ' ' << *(int*)rValue << endl;
				rfs.CloseScan();
	    			rmm->CloseFile(attrfh);
				break;
			    }
			}                                                    
			else if (left[i].attrType == FLOAT){
			    if (!rfs.condFLOAT(*(float*)buf, *(float*)rValue)){
				inCondition = 0;
	//			cout << "float not equal" <<*(float*)buf <<' ' <<*(float*)rValue << endl;
				rfs.CloseScan();
	    			rmm->CloseFile(attrfh);
				break;
			    }
			}
			else if (left[i].attrType == STRING){
			    if (!rfs.CondSTRING((char*)buf, (char*)rValue)){
				inCondition = 0;
				char* t= (char*)rValue;
	//			cout << "string not equal" << buf<< ' ' << t <<  endl;
				rfs.CloseScan();
	    			rmm->CloseFile(attrfh);
				break;
			    }
			}
			rfs.CloseScan();
	    		rmm->CloseFile(attrfh);
		    }
		}	
//====================================================================================================
		if (inCondition == 1){
		//  cout << "======================correct================== " << endl;
		    for (int i = 0; i < nSelAttrs; i++){
			rmm->OpenFile(getAttr[i].relName, attrfh);
			RM_Record tempRec;
                	attrfh.GetRec(rid[attrOrder[i]][index[attrOrder[i]]], tempRec);
			char *temp;
			temp = tempRec.data;
			cout << getAttr[i].relName << "->" << getAttr[i].attrName << ": ";
			char* buf = new char[MAXATTRLENGTH];
			memset(buf, 0, MAXATTRLENGTH);
			int length = 0;
			if (getAttr[i].attrType == MyINT)
				length = sizeof(int);
			else if (getAttr[i].attrType == FLOAT)
				length = sizeof(float);
			else
				length = getAttr[i].attrLength;
			memcpy(buf, (char*)temp + getAttr[i].offset, length);
//			cout << "buf: ";
//			for (int k = 0; k < 10; k++)
//				cout << buf[k];
//			cout << endl;
                        if (getAttr[i].attrType == MyINT){
                             int* t = (int*)buf;
                             cout << *t;
			     fprintf(fp, "%d\t", *t);
			}
                        else if (getAttr[i].attrType == FLOAT){
			     float* t = (float*)buf;
                             cout << *t;
			     fprintf(fp, "%f\t", *t);
			}
                        if (getAttr[i].attrType == STRING){
                             char* t = buf;
			     cout << t;
			     fprintf(fp, "%s\t", t);
			}
                        cout << ", ";
			rfs.CloseScan();
			rmm->CloseFile(attrfh);
		    }
		    cout << endl;
		    fprintf(fp, "\n");
//		    cout << "END PRINT in SELECT" << endl;
		}
//		for (int i = 0; i < nRelations; i++)
//		    cout << number[i] << ' ';
//		cout << endl;
//		for (int i = 0; i < nRelations; i++)
//		    cout << index[i] << ' ';
//		cout << endl;
		int judge = 0;
		for (int i = 0; i < nRelations; i++){
		    if (index[i] != number[i] - 1){
			judge = 1;
		    }
		}
		if (judge == 0)
		    break;
		else{
		    index[nRelations - 1]++;
		    for (int j = nRelations - 1; j >= 0; j--){
			if (index[j] == number[j]){
			    index[j] = 0;
			    index[j-1]++;
			}
			else{
			    break;
			}
		    }
		}
	    }

   	    for(int i = 0; i < MAXRELATION; i++){
                 delete []rid[i];
            }
            delete []rid;
            rid = NULL;
      fclose(fp);
	    delete []number;
	    delete []index;
	    delete []left;
	    delete []right;
	    delete []leftRel;
	    delete []rightRel;
	    delete []getAttr;
	    delete []attrOrder;
	    number = NULL;
          cout << "END SELECT" << endl << endl; 
        }

        void Insert (const char  *relName,           // relation to insert into
                   int         nValues,            // # values to insert
                   const Value values[],            // values to insert
                   int         nattr,               //
                   const AttrInfo attr[]){
	        RM_FileHandle attrfh;
	        RID rid;
	        int countAttr = 0;
	        DataAttrInfo* dataInfo = NULL;
	        int size = 0;
	        int offset = 0;
	        int inSet[countAttr];
            int label[nValues];
 //           cout << "=============QL Insert Function==============" << endl;
 //           cout << "RELNAME : " << relName << endl;
   /*         cout << "nValues : " << nValues << endl;
            for(int i = 0; i < nValues; i++) {
                cout << values[i];

            }
            cout << "nAttr : " << nattr << endl;
            
            for(int i = 0; i < nattr; i++) {
                cout << attr[i].attrName << endl;
            }*/
        
//      cout << "debug: 1" << endl;
            smm->GetTable(relName, countAttr, dataInfo);
      	cout << "countAttr: " << countAttr << endl;
            cout << dataInfo[0].attrLength << endl;
            //insert into Table, check value type

            for (int i = 0; i < countAttr; i++)
                inSet[i] = -1;
            for (int i = 0; i < nValues; i++)
                label[i] = -1;

            if (nattr == 0){
                if (countAttr != nValues){
                    cout << "Insert Error: value number is incorrect" << endl;      
                    return;
                }
                for (int i = 0; i < countAttr; i++){
                    if (values[i].type == 3){
                        if (dataInfo[i].notNull == 1){
                            cout << "Insert Error: not Null: " << dataInfo[i].attrName <<  endl; 
                            return;
                        }
                        else{
                            label[i] = -2;
                        }
                    }
                    else{
                        if (dataInfo[i].attrType != values[i].type){
                            cout << "Insert Error: value type is incorrect: " << dataInfo[i].attrName << ' ' << dataInfo[i].attrType << endl;
                            return;
                        }
                        label[i] = i;
                    }       
                }
            }
            else{
                if (nattr != nValues){
                    cout << "Insert Error: value number is incorrect" << endl;  
                    return; 
                }
            }
        //get all dataInfo
/*      for (int i = 0; i < countAttr; i++)
        cout << dataInfo[i].notNull<< ' ';
        cout << endl;
        for (int i = 0; i < countAttr; i++)
        dataInfo[i].print();*/
            bool primaryKey = true;

            for (int i = 0; i < countAttr; i++){
                size += dataInfo[i].attrLength;
                if (dataInfo[i].primaryKey == true) {
                    primaryKey = false;
                } 
            }

            if (nattr != 0){
        //check attr type
                for (int i = 0; i < nValues; i++){
                    for (int j = 0; j < countAttr; j++){
                        if (strcmp(attr[i].attrName, dataInfo[j].attrName) == 0){
                            if (inSet[j] != -1){
                                cout << "Insert Error: same attribute: " << attr[i].attrName <<  endl;
                                return;
                            }
                // if value = null
                            if (values[i].type == 3){
                                inSet[j] = -2;
                                label[i] = -2;
                            }
                            else{
                                inSet[j] = i; 
                                label[i] = j;
                            }
                            break;
                        }
                    }
                }
                for (int i = 0; i < countAttr; i++){
                    if (inSet[i] <= -1 && dataInfo[i].notNull == 1){
                        cout << "Insert Error: not Null: " << dataInfo[i].attrName <<  endl; 
                        return;
                    }       
                }
            }
        //在relname中查找，重新获取长度
//      cout << "debug: 2   size" << size << endl;
            //cout << "label" << ' ';
            //for (int i = 0; i < nValues; i++)
             //   cout << label[i] << ' ';
            //cout << endl;
        
            char *buf = new char[size + countAttr];
//      cout << "debug: 3.1" << endl;
            memset(buf, 0, size + countAttr);

            for(int i = 0; i < nValues; i++) {
                if (label[i] <= -1)
                    continue;
//          cout << "debug: 3.2" << endl;
                int length = 0;
               // cout << label[i] <<"label[i]" << dataInfo[label[i]].attrName << ' ' << values[i] << endl;
                memcpy(buf + dataInfo[label[i]].offset, values[i].data, values[i].getlength());
//          cout << "length: " << values[i].getlength() << endl;
//          cout << "debug: 3" << endl;
                buf[size + label[i]] = 1;
                if (dataInfo[label[i]].primaryKey) {
                    primaryKey = true;
                    IX_IndexHandle ixh;
                    ixm->OpenIndex(relName, dataInfo[label[i]].attrName, ixh);
                    IX_IndexScan ixscan;
                    ixscan.OpenScan(ixh, EQ_OP, values[i].data);
                    RID tmprid;
                    if (ixscan.GetNextEntry(tmprid) != -1) {
                        printf("primary key already exist.\n");
                        ixm->CloseIndex(ixh);
                        return;
                    }
                    ixm->CloseIndex(ixh);
                }
                char checkname[512];
                memset(checkname, 0, sizeof checkname);
                sprintf(checkname, "%s.%s.check.index", relName, dataInfo[label[i]].attrName);
                if (access(checkname, 0) != -1) {
                    IX_IndexHandle ixh;
                    memset(checkname, 0, sizeof checkname);
                    sprintf(checkname, "%s.check", dataInfo[label[i]].attrName);
                    ixm->OpenIndex(relName, checkname, ixh);
                    IX_IndexScan ixscan;
                    ixscan.OpenScan(ixh, EQ_OP, values[i].data);
                    RID tmprid;
                    if (ixscan.GetNextEntry(tmprid) == -1) {
                        ixm->CloseIndex(ixh);
                        return;
                    }
                    ixm->CloseIndex(ixh);
                }
            }
//          cout << "debug: 4" << endl;
            if (!primaryKey) {
                printf("no primary key.\n");
                return;
            }
            rmm->OpenFile(relName, attrfh);
            attrfh.InsertRec(buf, rid);

            for(int i = 0; i < nValues; i++) {
                char indexname[100];
                memset(indexname, 0, sizeof indexname);
                sprintf(indexname, "%s.%s.index", relName, dataInfo[i].attrName);
                if (access(indexname, 0) != -1) {
                    IX_IndexHandle ixh;
                    ixm->OpenIndex(relName, dataInfo[i].attrName, ixh);
                    ixh.InsertEntry(values[i].data, rid);
                    ixm->CloseIndex(ixh);
                }
            }
//          cout << "Insert Correctly, Rid: " << rid.Page() << ' ' << rid.Slot() << endl;
            rmm->CloseFile(attrfh);
            delete []buf;
            delete []dataInfo;
            cout << "END INSERT" << endl;
        }

        void Search(const char* relName, const Condition* cond, int& nrid, RID*& rid) {//need two RM_Record, rid = newRID[500000];

            cout << "QL Search Function" << endl;
            cout << relName << endl;
            cout << (cond->lhsAttr.relName == NULL) << ' ' << cond->lhsAttr.attrName << endl;
            cout << "OP : " << cond->op << endl;
            if (cond->op != 7){
            if(cond->bRhsIsAttr) {
                cout << (cond->rhsAttr.relName == NULL) << ' ' << cond->rhsAttr.attrName << endl;
            }
            else {
                cout << cond->rhsValue;
            }}
			cout << "debug: qby 1" << endl;
            nrid = 0;
            rid = new RID[MAXRID];
            //check bRhsIsAttr, if bRhsIsAttr == true ,exit
            if (cond->bRhsIsAttr) {
                cout << "Error:  WHERE ? op value" << endl;
                return;
            }
            RM_FileScan rfs = RM_FileScan(rmm->getFileManager(), rmm->getBufPageManager());
            RM_FileHandle attrfh;
            RM_Record rec;
            RID tempRid;
            int returnCode;
            AttrType attrType;
            int attrLength;
            int attrOffset;
	    int attrIndexNo;
            int inRel = -1;

//      cout << "debug: qby 2" << endl;
            //get attrType
            rmm->OpenFile("attrcat",attrfh);
            returnCode = rfs.OpenScan(attrfh, STRING, strlen(relName), 16, EQ_OP, (void*)relName);
	    int x = 0;
	    while (returnCode == 1){
//		cout << "ret : 1" << endl;
		x = rfs.GetNextRec(rec);
//		cout << "ret : 2" << endl;
		if (x == -1)
			break;
//		cout << x << endl;
		DataAttrInfo * mydata;
		rec.GetData((char*&) mydata);
//	        cout << mydata->attrName << endl;
//		cout << cond->lhsAttr.attrName << endl;
		if (strcmp(mydata->attrName, cond->lhsAttr.attrName) == 0){
			attrType = mydata->attrType;
			attrLength = mydata->attrLength;
			attrOffset = mydata->offset;
			attrIndexNo = mydata->indexNo;
			inRel = 0;
			break;
		}
	    }
	    rfs.CloseScan();
            rmm->CloseFile(attrfh);
            
//      cout << "debug: qby 3" << endl;
            if (inRel == -1){
            cout << "Attribute not in Relation!" << endl;
        return;
        }

	//cout << "attrType: " << attrType << endl;
	//cout << "attrLength: " << attrLength << endl;
	//cout << "attrOffset: " << attrOffset << endl;	 
	//cout << "attrIndexNo: " << attrIndexNo << endl; 
        //cout << "attrType: " << attrType << endl;
        //cout << "attrLength: " << attrLength << endl;
        //cout << "attrOffset: " << attrOffset << endl;

        if (cond->op != NULL_OP && cond->op != LIKE_OP) {
            char buf[100];
            memset(buf, 0, sizeof buf);
            sprintf(buf, "%s.%s.index", relName, cond->lhsAttr.attrName);
            if (access(buf, 0) != -1) {
                IX_IndexHandle ixh;
                ixm->OpenIndex(relName, cond->lhsAttr.attrName, ixh);
                IX_IndexScan ixscan;
                //printf("%s\n", cond->rhsValue.data);
                ixscan.OpenScan(ixh, cond->op, cond->rhsValue.data);
                RID tmprid;
                nrid = 0;
                while (ixscan.GetNextEntry(tmprid) != -1) {
                    rid[nrid] = tmprid;
                    nrid += 1;
                }
                //ixh.PrintEntries();
                ixm->CloseIndex(ixh);
                printf("search from index, %d\n", nrid);
                return;
            }
        }

        cout << "a" << endl;
           returnCode = 0;
        rmm->OpenFile(relName, attrfh);
   //     for (int i = 0; i < 10; i++)
   //         cout << *((char*)(cond->rhsValue.data)+ i) << ' ';
   //      cout << endl;
        returnCode = rfs.OpenScan(attrfh, attrType, attrLength, attrOffset, cond->op, cond->rhsValue.data, attrIndexNo);
        cout << "Aaa" << returnCode << endl;
        RM_Record rec_1;
        while (returnCode == 1) {
   //         cout << "ret : 1" << endl;
            x = rfs.GetNextRec(rec_1);
   //         cout << "ret : 2" << endl;
            if (x == -1)
                break;
//      cout << x << endl;
//      char* t = new char[100];
//      char* temp = NULL;
//      rec_1.GetData((char*&) temp);

//      for (int i = 0; i < 100; i++)
//          cout << temp[i];
//      cout << t << endl;      
//      strncpy(t, temp, 100);
//      for (int i = 0; i < 100; i++)
//          cout << t[i];
//      cout << t << endl;
//      delete []t;
            rec_1.GetRid(tempRid);
            rid[nrid].Copy(tempRid);
      //    cout << rid[nrid] << endl;
            nrid++;
//      cout << "get Rid: " << tempRid << endl;
        }
	cout << "all: " << nrid << endl;
        rfs.CloseScan();
        rmm->CloseFile(attrfh);
            cout << "END SEARCH" << endl;
        }

        void Delete (const char *relName,            // relation to delete from
                   int        nrid,         // # conditions in Where clause
                   const RID rid[])  // conditions in Where clause
        {
            cout << "QL Delete Function" << endl;
            cout << relName << endl;
            cout << nrid << endl;
        RM_FileScan rfs = RM_FileScan(rmm->getFileManager(), rmm->getBufPageManager());
        RM_Record rec;
            RM_FileHandle attrfh;
            int countAttr = 0;
            DataAttrInfo* dataInfo = NULL;
            smm->GetTable(relName, countAttr, dataInfo);

            rmm->OpenFile(relName,attrfh);
        for (int j = 0; j < countAttr; j++) {
            char indexname[100];
            memset(indexname, 0, sizeof indexname);
            sprintf(indexname, "%s.%s.index", relName, dataInfo[j].attrName);
            if (access(indexname, 0) != -1) {
                IX_IndexHandle ixh;
                ixm->OpenIndex(relName, dataInfo[j].attrName, ixh);
                for (int i = 0; i < nrid; i++) {
                    RM_Record rec;
                    attrfh.GetRec(rid[i], rec);
                    char *data = rec.data + dataInfo[j].offset;
                    ixh.DeleteEntry(data, rid[i]);
                }
                ixm->CloseIndex(ixh);
            }
        }
        for (int i = 0; i < nrid; i++){
            attrfh.DeleteRec(rid[i]);
        }
        rmm->CloseFile(attrfh);
            cout << "END DELETE" << endl << endl;

        }


        void Update (const char *relName,            // relation to update
                   const RelAttr &updAttr,         // attribute to update
                   const int bIsValue,             // 0/1 if RHS of = is attribute/value
                   const RelAttr &rhsRelAttr,      // attr on RHS of =
                   const Value &rhsValue,          // value on RHS of =
                   int   nrid,              // # conditions in Where clause
                   const RID rid[])  // conditions in Where clause
        {
            cout << "QL Update Dunction" << endl;
            cout << (updAttr.relName == NULL) << ' ' << updAttr.attrName << endl;
            cout << "BISVALUE : " << bIsValue << endl;
            if(!bIsValue){
                cout << (rhsRelAttr.relName == NULL) << ' ' << rhsRelAttr.attrName << endl;
            } 
            else{
                cout << rhsValue;
            }
            cout << nrid << endl;
            //get position of updata attr

            RM_FileScan rfs = RM_FileScan(rmm->getFileManager(), rmm->getBufPageManager());
            RM_Record rec;
            RM_FileHandle attrfh;
            int returnCode = 0; 
            rmm->OpenFile("attrcat",attrfh);
            returnCode = rfs.OpenScan(attrfh, STRING, strlen(relName), 16, EQ_OP, (void*)relName);
            int x = 0;

            DataAttrInfo lAttrInfo;
            DataAttrInfo rAttrInfo;

            while (returnCode == 1){
//      cout << "ret : 1" << endl;
                x = rfs.GetNextRec(rec);
//      cout << "ret : 2" << endl;
                if (x == -1)
                    break;
//      cout << x << endl;
                DataAttrInfo * mydata;
                rec.GetData((char*&) mydata);
//          cout << mydata->attrName << endl;
//      cout << cond->lhsAttr.attrName << endl;
                if (strcmp(mydata->attrName, updAttr.attrName) == 0){
                    memcpy(&lAttrInfo, mydata, sizeof(DataAttrInfo));           
                }
                if (!bIsValue){
                    if (strcmp(mydata->attrName, rhsRelAttr.attrName) == 0){
                        memcpy(&rAttrInfo, mydata, sizeof(DataAttrInfo));
                    }
                }
            }
            rfs.CloseScan();
            rmm->CloseFile(attrfh);

            if (!bIsValue && ((lAttrInfo.attrType != rAttrInfo.attrType)
            || (lAttrInfo.attrLength != rAttrInfo.attrLength))){
                cout << "Updata Error: type is not equal " << endl;
                return;
            }
            if (bIsValue && ((lAttrInfo.attrType != rhsValue.type))){
                cout << "Updata Error: type is not equal " << endl;
                return;
            }

            rmm->OpenFile(relName, attrfh);
            char indexname[1024];
            memset(indexname, 0, sizeof indexname);
            sprintf(indexname, "%s.%s.index", relName, lAttrInfo.attrName);
            bool indexed = false;
            IX_IndexHandle ixh;
            if (access(indexname, 0) != -1) {
                ixm->OpenIndex(relName, lAttrInfo.attrName, ixh);
                indexed = true;
            }

            for (int i = 0; i < nrid; i++){
            RM_Record tempRec;
            cout << rid[i] << endl;
            attrfh.GetRec(rid[i], tempRec);
            char *temp;
            temp = (char*)tempRec.data;
        
//      for (int i = 0; i < strlen(temp); i++)
//          cout << temp[i];
            char* buf = new char[MAXATTRLENGTH];
            memset(buf, 0, MAXATTRLENGTH);
            int length = 0;
            if (lAttrInfo.attrType == MyINT)
                length = sizeof(int);
            else if (lAttrInfo.attrType == FLOAT)
                length = sizeof(float);
            else
                length = lAttrInfo.attrLength;
            cout << "lAttrInfo.offset" << lAttrInfo.offset << endl;
            cout << "attrLength => " << length << endl;
            memcpy(buf, temp + lAttrInfo.offset, length);
            char* rbuf = new char[MAXATTRLENGTH];
            memset(rbuf, 0, MAXATTRLENGTH);
            if (!bIsValue)
                memcpy(rbuf, temp + rAttrInfo.offset, length);
            else
                memcpy(rbuf, rhsValue.data, length);
//      for (int j = 0; j < length; j++){
//          cout << buf[j] << rbuf[j] << ' ';       
//      }
            if (lAttrInfo.attrType == STRING){
                for (int j = 0; j < length; j++)
                    if (rbuf[j] == 0){
                    length = j + 1;
                    break;
                }
            }
            if (updAttr.outkeyrel != NULL) {
                printf("outkey not null %s\n", updAttr.outkeyrel);
                if (indexed) {
                    ixm->CloseIndex(ixh);
                }
                printf("!!outkey %s.%s\n", updAttr.outkeyrel, updAttr.outkeyattr);
                IX_IndexHandle outixh;
                printf("outkey %s.%s\n", updAttr.outkeyrel, updAttr.outkeyattr);
                ixm->OpenIndex(updAttr.outkeyrel, updAttr.outkeyattr, outixh);
                IX_IndexScan outscan;
                outscan.OpenScan(outixh, EQ_OP, rbuf);
                RID outrid;
                if (outscan.GetNextEntry(outrid) == -1) {
                    ixm->CloseIndex(outixh);
                    rmm->CloseFile(attrfh);   
                    printf("out key doesn't exist.\n");
                    return;
                }
                printf("out key check passed.\n");
                ixm->CloseIndex(outixh);
                if (indexed) {
                    ixm->OpenIndex(relName, lAttrInfo.attrName, ixh);
                }
            } 

            if (indexed) {
                ixh.DeleteEntry(temp + lAttrInfo.offset, rid[i]);
                ixh.InsertEntry(rbuf, rid[i]);
            }

            memcpy(temp + lAttrInfo.offset, rbuf, length);
            //printf("!!!!!!!!! %s\n%s %d\n", rbuf, temp + lAttrInfo.offset, length);
            rbuf[0] = 1;
        //delete null
            memcpy(temp + lAttrInfo.indexNo, rbuf, 1);
            attrfh.UpdateRec(tempRec);
            cout << endl;
/*                for (int j = 0; j < nSelAttrs; j++){
//          cout << selAttrs[j].attrName << ' ' << getAttr[j].offset << ' ';
            cout << selAttrs[j].attrName << ' ';
            char* buf = new char[400];
            memset(buf, 0, 400);
            int length = 0;
            if (getAttr[j].attrType == MyINT)
                length = sizeof(int);
            else if (getAttr[j].attrType == FLOAT)
                length = sizeof(float);
            else
                length = getAttr[j].attrLength;
            memcpy(buf, temp + getAttr[j].offset, length);
//          cout << "buf: ";
//          for (int k = 0; k < 10; k++)
//              cout << buf[k];
//          cout << endl;
                        if (getAttr[j].attrType == MyINT){
                             int* t = (int*)buf;
                             cout << *t;
            }
                        else if (getAttr[j].attrType == FLOAT){
                 float* t = (float*)buf;
                             cout << *t;
            }
                        if (getAttr[j].attrType == STRING){
                             char* t = buf;
                 cout << t;
            }
                        cout << ' ';
        }*/
            }   
            rmm->CloseFile(attrfh);      
            if (indexed) {
                ixm->CloseIndex(ixh);
            }
            cout << "END UPDATE" << endl << endl;
        }
        
private:
    SM_Manager *smm;
    RM_Manager *rmm;
    IX_Manager *ixm;
};

#endif
