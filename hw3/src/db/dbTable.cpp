/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   //cout<<endl;
   bool flag=false;
   for(unsigned int i=0;i<r._data.size();i++){
		if(flag)
			os<<" ";
		if(r[i]==INT_MAX)
			os<<".";
		else 
			os<<r[i];
		flag=true;
	}
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   //cout<<endl;
	for(unsigned int i=0;i<t._table.size();i++)
		for(unsigned int j=0;j<t.nCols();j++){
			if(t[i][j]==INT_MAX)
				os<<setw(6)<<right<<".";
			else 
				os<<setw(6)<<right<<t[i][j];
			if(j==t.nCols()-1)
				os<<endl;
		}		
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
   string getrow;
   while(getline(ifs,getrow,'\r') && !getrow.empty() && getrow!="\n")
   {	
		//clean the first element=\n
		while(getrow[0]=='\n') 
			getrow.erase(getrow.begin());	
		DBRow storerow;
		string substr;	
		size_t find=0,find_prev=0;
		bool flag=true;
		while(find!=std::string::npos)
		{
			find_prev=find;
			//first time entering while loop
			if(flag)	
			{
				find=getrow.find(',',find_prev);
				substr=getrow.substr(0,find);
				flag=false;
			}
			else    
			{
				find=getrow.find(',',find_prev+1);
				substr=getrow.substr(find_prev+1,find-find_prev-1);
			}
			if(substr.empty())
				storerow.addData(INT_MAX);
			else 
				storerow.addData(atoi(substr.c_str()));
		}
		t._table.push_back(storerow);
	}
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
// erase the cth cell
void
DBRow::removeCell(size_t c)
{
   // TODO
   _data.erase(_data.begin()+c);
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   for(unsigned int i=0;i<_sortOrder.size();i++){
		int n=_sortOrder[i];
		if(r1[n]>r2[n])
			return false;
		else if(r1[n]<r2[n])
			return true;
	}
	return true;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
   _table.erase(_table.begin(),_table.end());
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
   for(size_t i=0;i<d.size();i++)
		_table[i].addData(d[i]);
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   _table.erase(_table.begin()+c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
   int max=INT_MIN;
   bool flag=false;
	for(size_t i=0;i<_table.size();i++)
		if(_table[i][c]!=INT_MAX){
			if(!flag)
				flag=true;
			if(max<_table[i][c])
				max=_table[i][c];
		}
	if(flag)
		return (float)max;
	else
		return NAN;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   int min=INT_MAX;
   bool flag=false;
	for(size_t i=0;i<_table.size();i++)
		if(_table[i][c]!=INT_MAX){
			if(!flag)
				flag=true;
			if(min>_table[i][c])
				min=_table[i][c];
		}
	if(flag)
		return (float)min;
	else
		return NAN;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   bool flag=false;
   int sum=0;
   for(size_t i=0;i<_table.size();i++)
	   if(_table[i][c]!=INT_MAX){
			sum+=_table[i][c];
			if(!flag) flag=true;
		}
   if(flag)
		return (float)sum;
	else
		return NAN;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   int count=0;
   for(size_t i=0;i<nRows();i++)
   {
		if(_table[i][c]!=INT_MAX){
			bool flag=true;
			for(size_t j=0;j<i;j++)
				if(_table[j][c]==_table[i][c]){
					flag=false;
					break;
				}
			if(flag) 
				count++;
		}
	}
   return count;
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   bool flag=false;
   int sum=0;
   int data_num=0;
   for(size_t i=0;i<nRows();i++)
	   if(_table[i][c]!=INT_MAX){
			sum+=_table[i][c];
			if(!flag) flag=true;
			data_num++;
		}
   if(flag)
		return (float) sum/data_num;
	else
		return NAN;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
		std::sort(_table.begin(), _table.end(), s);
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   bool flag=false;
   for(size_t i=0;i<_table.size();i++){
		if(flag)
			cout<<" ";
		if(_table[i][c]!=INT_MAX)
			cout<<_table[i][c];
		else
			cout<<".";
		flag = true;
   }
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

