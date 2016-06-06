#include <iostream>
//#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include "../recordmanager/RecordManager.h"

int main (int argc, char **argv )
{
   char *dbname;
   char command[80] = "mkdir ";

   if (argc != 2) { 
		cout << "error argc!" << endl;
		return 0;
   }

   // The database name is the second argument
   dbname = argv[1];

   // Create a subdirectory for the database
   system (strcat(command,dbname));

   if (chdir(dbname) < 0) {
		cout << "error chdir!" << endl;
		return 0;
   }
	return 0;
}
