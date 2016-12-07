#include <iostream>
#include <string>
#include "p2Table.h"
#include <cstdlib>
#include <fstream>

using namespace std;

int main()
{
	Table table;

	// TODO: read in the csv file
	string csvFile;
	cout << "Please enter the file name: ";
	cin >> csvFile;
	if (table.read(csvFile))
		cout << "File \"" << csvFile << "\" was read in successfully." << endl;
	else 
		exit(-1); // csvFile does not exist.

	// TODO read and execute commands
	while (true) 
	{
		string command;
		cin>>command;
		//perform SUM
		if(command=="SUM")	 
			table.sum();
		//perform EXIT
		if(command=="EXIT")    
			table.exit();
		//perform PRINT
		if(command=="PRINT")	
			table.print();
		//perform AVERAGE
		if(command=="AVE")	
			table.ave();
		//perform MAX
		if(command=="MAX")
			table.max();
		//perform MIN
		if(command=="MIN")
			table.min();
		//perform COUNT
		if(command=="COUNT")
			table.count();
		//perform ADD
		if(command=="ADD")
			table.add();
   }
}
