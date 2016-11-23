/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}

//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
	vector<string> options;
	if(! CmdExec::lexOptions(option,options))
		return CMD_EXEC_ERROR;
	if (options.empty())
		return CmdExec::errorOption(CMD_OPT_MISSING, "");
	bool doarray=false;
	size_t numObjects=0,arraySize=0;
	for(size_t i=0 ; i<options.size() ; i++)
	{
		int temp;
		if(myStrNCmp("-Array",options[i],2) == 0)
		{
			if(doarray)
				return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
			doarray=true;
		}
		else if( myStr2Int(options[i],temp) )
		{
			if(temp <= 0)
				return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[i]);
			if( numObjects && arraySize )
				return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
			else if( (!doarray) || arraySize ) 
				numObjects=(size_t)temp;
			else 
				arraySize=(size_t)temp; 
		}
		else{
			cerr << "Error: " << options[i] << " is not a number!!\n";
			return CMD_EXEC_ERROR; 
		}
	}
	if(doarray){
		if( numObjects == 0 || arraySize == 0)
			return CmdExec::errorOption(CMD_OPT_MISSING,"-Array");
		try{
			mtest.newArrs(numObjects,arraySize);
		}
		catch(bad_alloc){
			return CMD_EXEC_ERROR;
		}
	}
	else
	{
		if(numObjects==0)
			return CmdExec::errorOption(CMD_OPT_MISSING,"MTNew");
		mtest.newObjs(numObjects);
	}
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
	vector<string> options;
	if(! CmdExec::lexOptions(option,options))
		return CMD_EXEC_ERROR;
	if (options.empty())
		return CmdExec::errorOption(CMD_OPT_MISSING, "");
	bool doInd=false , doRan=false , doarray=false ;
	int objId=-1;
	size_t numRandId=0;
	string spec;
	for(size_t i=0 ; i<options.size() ; i++)
	{
		int temp;
		if(myStrNCmp("-Array",options[i],2) == 0)
		{
			if(doarray)
				return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
			doarray=true;
		}
		else if(myStrNCmp("-Index",options[i],2) == 0)
		{
			if(spec.size())
				return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
			doInd=true;
			spec=options[i];
		}
		else if(myStrNCmp("-Random",options[i],2) == 0)
		{
			if(spec.size())
				return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
			doRan=true;
			spec=options[i];
		}
		else if( myStr2Int(options[i],temp) )
		{
			if(temp < 0)
				return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[i]);
			if( objId != -1 && numRandId )
				return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
			if(doInd)
				objId= temp;
			else if(doRan)
				numRandId= (size_t)temp;
			else 
				return CmdExec::errorOption(CMD_OPT_ILLEGAL,options[i]);
		}
		else{
			cerr << "Error: " << options[i] << " is not a number!!\n";
			return CMD_EXEC_ERROR; 
		}
	}
	if(doInd){
		if(objId != -1){
			if(doarray){
				if(objId< (int) mtest.getArrListSize())
					mtest.deleteArr(objId);
				else{
					cerr<<"Size of array list ("<<mtest.getArrListSize()\
						 <<") is <= "<<objId<<"!!\n";
					cerr << "Error: Illegal option!! (" << objId << ")" << endl;
					return CMD_EXEC_ERROR;
				}
			}
			else{
				if(objId< (int) mtest.getObjListSize())
					mtest.deleteObj(objId);
				else{
					cerr<<"Size of object list ("<<mtest.getObjListSize()\
						 <<") is <= "<<objId<<"!!\n";
					cerr << "Error: Illegal option!! (" << objId << ")" << endl;
					return CMD_EXEC_ERROR;
				}
			}
		}
		else
			return CmdExec::errorOption(CMD_OPT_MISSING,spec);
	}
	else if(doRan){
		if(numRandId){
			if(doarray)
			{
				int size= (int) mtest.getArrListSize();
				if(size==0){
					cerr<<"Size of array list is 0!!\n";
					return CmdExec::errorOption(CMD_OPT_ILLEGAL,spec);
				}
				else
					for(size_t i=0;i<numRandId;i++)
					{
						int randId=rnGen( size );
						mtest.deleteArr(randId);
					}
			}
			else 
			{
				int size = (int) mtest.getObjListSize();
				if(size==0){
					cerr<<"Size of object list is 0!!\n";
					return CmdExec::errorOption(CMD_OPT_ILLEGAL,spec);
				}
				else
					for(size_t i=0;i<numRandId;i++)
					{	
						int randId=rnGen( (int) mtest.getObjListSize() );
						mtest.deleteObj(randId);
					}
			}
		}
		else 
			return CmdExec::errorOption(CMD_OPT_MISSING,spec);
	}
	else 
		return CmdExec::errorOption(CMD_OPT_MISSING,"");
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


