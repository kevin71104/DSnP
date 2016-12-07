#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>

using namespace std;

class Row
{
public:
   Row(int column){_data = new int [column];num=column;}    //constructor with column_number
   Row(const Row& copyrow)	//copy constructor
   {
		_data = new int [copyrow.num];
		num=copyrow.num;
		for(int i=0;i<num;i++)
			_data[i]=copyrow[i];
	}
   const int operator[] (size_t i) const {return _data[i];}
   int& operator[] (size_t i){return _data[i];} 
   void setcell(int i,int a){_data[i]=a;}
   ~Row(){delete[] _data;}
private:
   int  *_data;
   int num;	//number of column
};

class Table
{
public:
   const Row& operator[] (size_t i) const{return _rows[i];}
   Row& operator[] (size_t i){return _rows[i];}
   bool read(const string&);
   void print();
   void sum();
   void exit(){std::exit(0);}	
   void ave();
   void max();
   void min();
   void count();
   void add();
private:
   vector<Row>  _rows;
   unsigned int num_column;   //The number of column
};

#endif // P2_TABLE_H
