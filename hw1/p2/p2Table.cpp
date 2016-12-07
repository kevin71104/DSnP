#include "p2Table.h"
#include <iomanip>
#include <climits>
#include <cstdlib>
#include <algorithm>

using namespace std;

// Implement member functions of class Row and Table here

bool
Table::read(const string& csvFile)
{
    fstream fs;
    fs.open(csvFile.c_str(),ios::in);
    if(fs.is_open())
    {
        string getrow;
		bool flag=1; 
        while(getline(fs,getrow,'\r') && !getrow.empty())
        {	
			string substr;	//substring of getrow
			if(flag)   //first enter while loop calculate num_column
            {
				int k=0;
				for(unsigned int j=0;j<getrow.length();j++)
				if(getrow[j]==',') k++;
				num_column=(k+1);
			}
			Row storerow(num_column);
			int find=0,find_prev=0;
			for(unsigned int i=0;i<num_column;i++)
			{
				find_prev=find;
				if(i)	
				{
					find=getrow.find(',',find_prev+1);
					substr=getrow.substr(find_prev+1,find-find_prev-1);
				}
				else    //first time entering while loop
				{
					find=getrow.find(',',find_prev);
					substr=getrow.substr(find_prev,find-find_prev);
				}
				if(substr.empty())
					storerow[i]=INT_MAX;
				else 
					storerow[i]= atoi(substr.c_str());
			}
			_rows.push_back(storerow);
			flag=0; 
        }
        return true;
    }
    else return false;
}

void
Table::print()
{
	for(unsigned int i=0;i<_rows.size();i++)
		for(unsigned int j=0;j<num_column;j++)
		{
			if(_rows[i][j]<=100 && _rows[i][j]>=(-99))
				cout<<setw(4)<<right<<_rows[i][j];
			else 
				cout<<setw(4)<<right<<"";
			if(j==(num_column-1))cout<<endl;
		}
}
void
Table::sum()
{
	int index=0;
	cin>>index;
	int sum=0;
	for(unsigned int i=0;i<_rows.size();i++)
		if(_rows[i][index]<=100 && _rows[i][index]>=(-99))
			sum+=_rows[i][index];
	cout<<"The summation of data in column #"<<index<<" is "<<sum<<"."<<endl;
}
void
Table::ave()
{
	int index=0;
	cin>>index;
	int data_num=0;	//the number of valid data in the column
	double sum=0.0;
	for(unsigned int i=0;i<_rows.size();i++)
		if(_rows[i][index]<=100 && _rows[i][index]>=(-99))
		{
			sum+=_rows[i][index];
			data_num++;
		}
	cout<<"The average of data in column #"<<index<<" is "<<fixed<<setprecision(1)<<sum/data_num<<"."<<endl;
}
void
Table::max()
{
	int index=0;
	cin>>index;
	int compare=-99;	//to store previous largest number 	
	for(unsigned int i=0;i<_rows.size();i++)
		if(_rows[i][index]<=100 && _rows[i][index]>=(-99))
			compare=std::max(compare,_rows[i][index]);
	cout<<"The maximum of data in column #"<<index<<" is "<<compare<<"."<<endl;
}
void 
Table::min()
{
	int index=0;
	cin>>index;
	int compare=100;	//to store previous smallest number 	
	for(unsigned int i=0;i<_rows.size();i++)
		if(_rows[i][index]<=100 && _rows[i][index]>=(-99))
			compare=std::min(compare,_rows[i][index]);
	cout<<"The minimum of data in column #"<<index<<" is "<<compare<<"."<<endl;
}
void
Table::count()
{
	int index=0;
	cin>>index;
	int count=0;
	for(unsigned int i=0;i<_rows.size();i++)
		if(_rows[i][index]<=100 && _rows[i][index]>=(-99))
		{
			count++;
			for(unsigned int j=i+1;j<_rows.size();j++)
				if(_rows[i][index]==_rows[j][index])//find same number count-- and break
				{
					count--;
					break;
				}
		}
	cout<<"The number of distinct data in column #"<<index<<" is "<<count<<"."<<endl;
}
void
Table::add()
{
	Row addrow(num_column);	//row which will be added
	string getrow;
	for(unsigned int i=0;i<num_column;i++)
	{
		cin>>getrow;
		if(getrow=="-") 
			addrow[i]=INT_MAX;
		else
			addrow[i]=atoi(getrow.c_str());
	}
	_rows.push_back(addrow);
}
