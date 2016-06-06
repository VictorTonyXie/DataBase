#include <iostream>
#include <stdio.h>
#include "parse_manager.h"
#include "../bufmanager/BufPageManager.h"
#include "../fileio/FileManager.h"
#include "../recordmanager/RecordManager.h"
#include "../recordmanager/rm_filescan.h"
#include "../systemmanager/sm_manager.h"

using namespace std;

int main(int argc, char **argv){
	FileManager* fm = new FileManager();
	BufPageManager* bpm = new BufPageManager(fm);
	RM_Manager* rm_m = new RM_Manager(fm, bpm);
	SM_Manager* sm_m = new SM_Manager(*rm_m);
	FileManager* fm2 = new FileManager();
	BufPageManager* bpm2 = new BufPageManager(fm2);
	IX_Manager* ix_m = new IX_Manager(fm2, bpm2);
	if(argc < 2) printf("Please Input Sql instruction filename\n");
	string filename = string(argv[1]);
	Parse_Manager PM(filename);
	PM.MainLoop(sm_m, rm_m, ix_m);
	return 0;
}