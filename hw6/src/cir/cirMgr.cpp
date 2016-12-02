/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static string errMsg;
static unsigned errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

bool
myStr2UInt(const string& str, unsigned& num)
{
   num = 0;
   size_t i = 0;
   bool valid = false;
   for (; i < str.size(); ++i) {
      if (isdigit(str[i])) {
         num *= 10;
         num += unsigned(str[i] - '0');
         valid = true;
      }
      else return false;
   }
   return valid;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
	ifstream ifs(fileName.c_str());
   if (!ifs) {
      cerr << "Error: \"" << fileName << "\" does not exist!!" << endl;
      return false;
   }
   
   string temp;
   unsigned M, I, L, O, A;
   stringstream ss;
   lineNo = colNo = 0;
   IdList pofaninList, aigfanin1List, aigfanin2List, AigList;
	
	//after read in every line check if there is newline at the end
	#define 	newline();                    			\
				if(!ss.eof())                          \
					return parseError(MISSING_NEWLINE); \
				++lineNo;                              \
				colNo = 0;                             \
				ss.clear();                            \
				ss.str("");										
   
   //check after this has exactly 1 space and followed with a number
   #define 	correctip();									\
				if( temp[colNo] != 32 ) 					\
					return parseError(MISSING_SPACE);	\
				colNo++;											\
				if(temp[colNo] <= 32) 						\
					return parseError(MISSING_NUM);		\
				ss >> errMsg;
				
   //header
   {
		getline(ifs,temp);
		//identifer
		{
			if( temp[colNo] < 32) return parseError(MISSING_IDENTIFIER);
			ss << temp;
			ss >> errMsg;
			if( errMsg != "aag" ) return parseError(ILLEGAL_IDENTIFIER);
			colNo += errMsg.size();
		}
		// M
		{
			correctip();
			if(!myStr2UInt(errMsg, M)) return parseError(ILLEGAL_NUM);
			colNo += errMsg.size();
		}
		//I
		{
			correctip();
			if(!myStr2UInt(errMsg, I)) return parseError(ILLEGAL_NUM);
			colNo += errMsg.size();
		}
		//L
		{
			correctip();
			if(!myStr2UInt(errMsg, L)) return parseError(ILLEGAL_NUM);
			colNo += errMsg.size();
		}
		//O
		{
			correctip();
			if(!myStr2UInt(errMsg, O)) return parseError(ILLEGAL_NUM);
			colNo += errMsg.size();
		}
		//A
		{
			correctip();
			if(!myStr2UInt(errMsg, A)) return parseError(ILLEGAL_NUM);
			colNo += errMsg.size();
		}
		if( M < (I+L+A) ){
			errMsg = "M";
			errInt = M;
			return parseError(NUM_TOO_SMALL);
		}
		_AIGNum = A;
	}
		//PIs
		for(unsigned i=0;i<I;i++){
			newline();
			if(ifs.eof()){
				errMsg = "PI";
				return parseError(MISSING_DEF);
			}
			getline(ifs,errMsg);
			if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_NUM);
			if(errInt%2) return parseError(ILLEGAL_NUM);
			if(errInt > 2*M){
				errMsg = "PI index";
				return parseError(NUM_TOO_BIG);
			}
			if(errInt == 0) return parseError(REDEF_CONST);
			//prevent redefinition
			for(unsigned j=0;j<PiList.size();j++)
				if( (errInt>>1) == PiList[j]){ 
					errGate = new PIGate(j+2, PiList[j]);
					return parseError(REDEF_GATE);
				}
			//store variableID
			PiList.push_back(errInt>>1);
			colNo += errMsg.size();
		}
		
		//POs
		for(unsigned i=0; i<O; i++){
			newline();
			if(ifs.eof()){
				errMsg = "PO";
				return parseError(MISSING_DEF);
			}
			getline(ifs,errMsg);
			if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_NUM);
			if((errInt>>1) > M) return parseError(MAX_LIT_ID);
			pofaninList.push_back(errInt);
			colNo += errMsg.size();
		}
		
		//AIGs
		for(unsigned i=0; i<A ; i++){
			newline();
			if(ifs.eof()){
				errMsg = "AIG";
				return parseError(MISSING_DEF);
			}
			getline(ifs,temp);
			ss << temp;
			ss >> errMsg;
			
			//AND gate variable ID
			if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_NUM);
			if(errInt%2) return parseError(ILLEGAL_NUM);
			if((errInt>>1) > M) return parseError(MAX_LIT_ID);
			if(errInt == 0) return parseError(REDEF_CONST);
			for(unsigned j=0;j<PiList.size();j++)
				if( (errInt>>1) == PiList[j]){ 
					errGate = new PIGate(j+2, PiList[j]);
					return parseError(REDEF_GATE);
				}
			for(unsigned j=0; j<AigList.size(); j++)
				if( (errInt>>1) == AigList[j] ){
					errGate = new AIGGate(I+O+j+2,AigList[j],0,0);
					return parseError(REDEF_GATE);
				}
			AigList.push_back(errInt>>1);
			colNo += errMsg.size();
			
			//fanin1
			correctip();
			if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_NUM);
			if((errInt>>1) > M) return parseError(MAX_LIT_ID);
			aigfanin1List.push_back(errInt);
			colNo += errMsg.size();
			
			//fanin2
			correctip();
			if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_NUM);
			if((errInt>>1) > M) return parseError(MAX_LIT_ID);
			aigfanin2List.push_back(errInt);
			colNo += errMsg.size();
		}
	
	return true;
}  


/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
}

void
CirMgr::printNetlist() const
{
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   cout << endl;
}

//Gates with floating fanin(s): 
//Gates defined but not used : 

void
CirMgr::printFloatGates() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
}
