/****************************************************************************
  FileName     [ test.cpp ]
  PackageName  [ test ]
  Synopsis     [ Test program for simple database db ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "dbTable.h"

using namespace std;

extern DBTable dbtbl;

class CmdParser;
CmdParser* cmdMgr = 0; // for linking purpose

int
main(int argc, char** argv)
{
   if (argc != 2) {  // testdb <cvsfile>
      cerr << "Error: using testdb <cvsfile>!!" << endl;
      exit(-1);
   }

   ifstream inf(argv[1]);

   if (!inf) {
      cerr << "Error: cannot open file \"" << argv[1] << "\"!!\n";
      exit(-1);
   }

   if (dbtbl) {
      cout << "Table is resetting..." << endl;
      dbtbl.reset();
   }
   if (!(inf >> dbtbl)) {
      cerr << "Error in reading csv file!!" << endl;
      exit(-1);
   }

   cout << "========================" << endl;
   cout << " Print table " << endl;
   cout << "========================" << endl;
   cout << dbtbl << endl;

   // TODO
   // Insert what you want to test here by calling DBTable's member functions
   
	//addcol
	vector<int> a;
	for(size_t i=0;i<dbtbl.nRows();i++)
		a.push_back(INT_MAX);
	dbtbl.addCol(a);
	cout<<dbtbl<<endl;
	a[0]=2;
	a[1]=2;
	a[2]=-99;
	dbtbl.addCol(a);	
	cout<<dbtbl<<endl;
	cout<<"SORT"<<endl;
	DBSort dbsort;
	dbsort.pushOrder(1);
	dbtbl.sort(dbsort);
	cout<<dbtbl;
	/*//delCol
   dbtbl.delCol(1);
   cout<<dbtbl;
   //delRow
   dbtbl.delRow(1);
   cout<<dbtbl;*/
   cout<<"AVE"<<endl;
   for(size_t i=0;i<dbtbl.nCols();i++)
		cout<<fixed<<setprecision(1)<<setw(6)<<right<<dbtbl.getAve(i);
	cout<<endl;
	cout<<"COUNT"<<endl;
	for(size_t i=0;i<dbtbl.nCols();i++)
		cout<<setw(6)<<right<<dbtbl.getCount(i);
	cout<<endl;
	cout<<"MAX"<<endl;
	for(size_t i=0;i<dbtbl.nCols();i++)
		cout<<setw(6)<<right<<dbtbl.getMax(i);
	cout<<endl;
	cout<<"MIN"<<endl;
	for(size_t i=0;i<dbtbl.nCols();i++)
		cout<<setw(6)<<right<<dbtbl.getMin(i);
	cout<<endl;
	cout<<"SUM"<<endl;
	for(size_t i=0;i<dbtbl.nCols();i++)
		cout<<setw(6)<<right<<dbtbl.getSum(i);
	cout<<endl;
	cout<<"PRINTCOL"<<endl;
	for(size_t i=0;i<dbtbl.nCols();i++){
		cout<<"column"<<i<<endl;
		dbtbl.printCol(i);
	}
	cout<<endl<<"PRINTROW"<<endl;
	for(size_t i=0;i<dbtbl.nRows();i++){
		cout<<"row"<<i<<endl;
		cout<<dbtbl[i];
	}
	return 0;
}
