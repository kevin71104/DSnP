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
#include <algorithm>
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
   ILLEGAL_SYMBOL_INDEX,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   ILLEGAL_LATCHES,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   MISSING_SYMBOL,
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
	ostream& os = cerr;
	//ostream& os = cout;
   switch (err) {
      case EXTRA_SPACE:
         os	<< "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
            << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         os	<< "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
            << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         os << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
				<< ": Illegal white space char(" << errInt
				<< ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         os	<< "[ERROR] Line " << lineNo+1 << ": Illegal "
            << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         os	<< "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
            << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         os << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
            << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         os << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
            << ": Symbolic name contains un-printable char(" << errInt
            << ")!!" << endl;
         break;
      case MISSING_NUM:
         os << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
            << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         os << "[ERROR] Line " << lineNo+1 << ": Missing \""
            << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         os << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
            << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         os << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
            << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         os << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
            << ": " << errMsg << " " << errInt << "(" << errInt/2
            << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         os << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
            << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
            << endl;
         break;
      case REDEF_GATE:
         os << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
            << "\" is redefined, previously defined as "
            << errGate->getTypeStr() << " in line " << errGate->getLineNo()
            << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         os << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
            << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         os << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
            << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         os << "[ERROR] Line " << lineNo+1 << ": " << errMsg
            << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         os << "[ERROR] Line " << lineNo+1 << ": " << errMsg
            << " is too big (" << errInt << ")!!" << endl;
         break;
		case ILLEGAL_LATCHES:
			os << "[ERROR] Line " << lineNo+1 << ": Illegal latches!!" << endl;
			break;
		case MISSING_SYMBOL:
			os << "[ERROR] Line " << lineNo+1 << ": Missing \"symbolic name\"!!" << endl;
			break;
		case ILLEGAL_SYMBOL_INDEX:
			os << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
            << ": Illegal symbol index (" << errMsg << ")!!" << endl;
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

CirGate*
CirMgr::getGate(unsigned gid) const
{ 
	if (gid >= _GateList.size()) 
		return 0;
	CirGate* gate = _GateList[gid];
	if(gate->getType() == UNDEF_GATE) 
		return 0;
	else return gate;
}

//not handling illegal white-space
bool
CirMgr::readCircuit(const string& fileName)
{
	ifstream ifs(fileName.c_str());
   if (!ifs) {
      cerr << "Error: \"" << fileName << "\" does not exist!!" << endl;
      return false;
   }
   
   lineNo = colNo = 0;
   string 				temp;
   unsigned 			M, I, L, O, A;
   stringstream 		ss;
   vector<IdList> 	fanoutList;		//fanoutList[a] : (variableId a) gate's _fanout
	
	//after read in every line check if there is newline at the end
	//ss.str("") reset ss and can be inserted later
	#define 	newline();                    			\
				if(!ss.eof())                          \
					return parseError(MISSING_NEWLINE); \
				++lineNo;                              \
				colNo = 0;                             \
				ss.clear();                            \
				ss.str("");										
   
   //check after this has exactly 1 space and followed with a string
   #define 	correctip();									\
				if( temp[colNo] != 32 ) 					\
					return parseError(MISSING_SPACE);	\
				colNo++;											\
				if( temp[colNo] == 32)						\
					return parseError(EXTRA_SPACE);		\
				if(temp[colNo] <= 32){ 						\
					errMsg = "number of variables";		\
					return parseError(MISSING_NUM);		\	
				}													\
				ss >> errMsg;
	//line & col starts at 1, parseError has +1 inside			
   //header
   {
		getline(ifs,temp);
		//identifer
		errMsg = "Identifier";
		if( temp[colNo] == 32) return parseError(EXTRA_SPACE);
		if( temp[colNo] < 32) return parseError(MISSING_IDENTIFIER);
		ss << temp;
		ss >> errMsg;
		colNo += 3;
		if( isdigit(errMsg[3]) ) return parseError(MISSING_SPACE);
		if( errMsg != "aag" ) return parseError(ILLEGAL_IDENTIFIER);
		if( temp.size() == 3 ){
			errMsg = "number of variables";		
			return parseError(MISSING_NUM);		
		}		
		
		// M
		correctip();
		if(!myStr2UInt(errMsg, M)){
			string error = "number of Ms("+errMsg+")";
			errMsg = error;
			return parseError(ILLEGAL_NUM);
		}
		colNo += errMsg.size();
			
		//I
		correctip();
		if(!myStr2UInt(errMsg, I)){
			string error = "number of Is("+errMsg+")";
			errMsg = error;
			return parseError(ILLEGAL_NUM);
		}
		colNo += errMsg.size();

		//L
		correctip();
		if(!myStr2UInt(errMsg, L)){
			string error = "number of Ls("+errMsg+")";
			errMsg = error;
			return parseError(ILLEGAL_NUM);
		}
		colNo += errMsg.size();

		//O
		correctip();
		if(!myStr2UInt(errMsg, O)){
			string error = "number of Os("+errMsg+")";
			errMsg = error;
			return parseError(ILLEGAL_NUM);
		}
		colNo += errMsg.size();
			
		//A
		correctip();
		//if(!myStr2UInt(errMsg, A)) return parseError(ILLEGAL_NUM);
		if(!myStr2UInt(errMsg, A)){
			string error = "number of AIGs("+errMsg+")";
			errMsg = error;
			return parseError(ILLEGAL_NUM);
		}
		colNo += errMsg.size();
		if(!ss.eof()) return parseError(MISSING_NEWLINE);
			
		if( M < (I+L+A) ){
			errMsg = "Number of variables";
			errInt = M;
			return parseError(NUM_TOO_SMALL);
		}
		if(L != 0) return parseError(ILLEGAL_LATCHES);
		_AIGNum = A;
	}
	
	//set _GateList, fanoutList
	//M gates + O output gates + 1 CONST gate
	_GateList.reserve(M+O+1);
	_GateList.push_back(new CONSTGate);
	for(size_t i=1; i <= M; i++)
		_GateList.push_back(new UNDEFGate(i));
	fanoutList.reserve(M+1);
	fanoutList.resize(M+1);
	
	//PIs
	for(unsigned i=0;i<I;i++){
		newline();
		getline(ifs,temp);
		if(ifs.eof() || temp[colNo] < 32){
			errMsg = "PI";
			return parseError(MISSING_DEF);
		}
		if(temp[colNo] == 32) return parseError(EXTRA_SPACE);
		ss << temp;
		ss >> errMsg;
		if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_NUM);
		if(errInt == 0 || errInt == 1) return parseError(REDEF_CONST);
		if(errInt%2){
			errMsg = "PI"; 
			return parseError(CANNOT_INVERTED);
		}
		if(errInt > 2*M) return parseError(MAX_LIT_ID);
		unsigned variableId = errInt>>1;
		//store variableID
		PiList.push_back(variableId);
		//prevent redefinition
		if(_GateList[variableId]->getType() == PI_GATE){
			errGate = _GateList[variableId];
			return parseError(REDEF_GATE);
		}
		delete _GateList[variableId];
		_GateList[variableId] = new PIGate(i+2,variableId);
		colNo += errMsg.size();
	}
		
	//POs
	unsigned PoID = M+1;
	for(unsigned i=0; i<O; i++,PoID++){
		newline();
		getline(ifs,temp);
		if(ifs.eof()|| temp[colNo] < 32){
			errMsg = "PO";
			return parseError(MISSING_DEF);
		}
		if(temp[colNo] == 32) return parseError(EXTRA_SPACE);
		ss << temp;
		ss >> errMsg;
		if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_NUM);
		if((errInt>>1) > M) return parseError(MAX_LIT_ID);
		unsigned literalId = errInt;
		colNo += errMsg.size();
		_GateList.push_back( new POGate(I+i+2,PoID,literalId) );
		PoList.push_back(PoID);
		fanoutList[literalId>>1].push_back((PoID*2+literalId%2));
	}
		
	//AIGs
	for(unsigned i=0; i<A ; i++){
		newline();
		getline(ifs,temp);
		if(ifs.eof() || temp[colNo] < 32){
			errMsg = "AIG";
			return parseError(MISSING_DEF);
		}
		if(temp[colNo] == 32) return parseError(EXTRA_SPACE);
		ss << temp;
		ss >> errMsg;
		
		//AND gate variable ID
		if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_NUM);
		if(errInt == 0 || errInt == 1) return parseError(REDEF_CONST);
		if(errInt%2){
			errMsg = "AIG";
			return parseError(CANNOT_INVERTED);
		 }
		if((errInt>>1) > M) return parseError(MAX_LIT_ID);
		unsigned variableId = errInt>>1;
		colNo += errMsg.size();
		if(_GateList[variableId]->getType() == PI_GATE || \
			_GateList[variableId]->getType() == AIG_GATE ){
			errGate = _GateList[variableId];
			return parseError(REDEF_GATE);
		}
		
		//fanin1
		correctip();
		if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_NUM);
		if((errInt>>1) > M) return parseError(MAX_LIT_ID);
		unsigned fanin1 = errInt;
		colNo += errMsg.size();
			
		//fanin2
		correctip();
		if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_NUM);
		if((errInt>>1) > M) return parseError(MAX_LIT_ID);
		unsigned fanin2 = errInt;
		colNo += errMsg.size();
		
		delete _GateList[variableId];
		_GateList[variableId] = new AIGGate(I+O+i+2,variableId,fanin1,fanin2);
		fanoutList[fanin1>>1].push_back((variableId*2+fanin1%2));
		fanoutList[fanin2>>1].push_back((variableId*2+fanin2%2));
	}
	
	//set fanout
	//for(size_t i=0; i<M; i++)	_GateList[i]->setFanout();
	for(size_t i=0; i<fanoutList.size(); i++) ::sort(fanoutList[i].begin(),fanoutList[i].end());
	for(size_t i=0; i<=M; i++) _GateList[i]->setFanout(fanoutList[i]);
	
	//set symbols, ending by line consisting only "c"
	for(size_t i=0;i<I+O+1;i++){
		newline();
		getline(ifs, temp);
		if(ifs.eof()) break;
		//end symbol
		ss << temp;
		ss >> errMsg;
		if(temp[colNo] == 'c'){
			//consists more than "c"
			colNo++;
			if(temp.size() > 1 ) return parseError(MISSING_NEWLINE);
			newline();
			break;
		}
		//number followed by 'i' || 'o'
		if(temp[0] == 'i'){
			colNo++;
			if (temp[1] == 32) return parseError(EXTRA_SPACE);
			//index
			errMsg = errMsg.substr(1);
			if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_SYMBOL_INDEX);
			//number starting from 0~(I-1) 
			if(errInt >= I){
				errMsg = "PI index";
				return parseError(NUM_TOO_BIG);
			}
			if(_GateList[PiList[errInt]]->getSymbol() != ""){
				errMsg = "i";
				return parseError(REDEF_SYMBOLIC_NAME);
			}
			colNo += errMsg.size();
			if(colNo == temp.size())return parseError(MISSING_SYMBOL);
			if( temp[colNo] != 32 ) return parseError(MISSING_SPACE);	
			colNo++;	
			if(colNo == temp.size())return parseError(MISSING_SYMBOL);	
			errMsg = temp.substr(colNo);
			//reset ss
			ss.str("");
			ss.ignore();
			//containing""								
			if(errMsg.empty()) return parseError(ILLEGAL_SYMBOL_TYPE);
			//check whether containing unprintable character
			for(size_t j=0; j<errMsg.size();j++)
				if(errMsg[j] < 32){
					colNo += j;
					errInt = unsigned(errMsg[j]-char(0));
					return parseError(ILLEGAL_SYMBOL_NAME);
				}
			_GateList[PiList[errInt]]->setSymbol(errMsg);
		}
		else if (temp[0] == 'o'){
			colNo++;
			if (temp[1] == 32) return parseError(EXTRA_SPACE);
			//index
			errMsg = errMsg.substr(1);
			if(!myStr2UInt(errMsg,errInt)) return parseError(ILLEGAL_SYMBOL_INDEX);
			//number starting from 0~(O-1)
			if(errInt >= O){
				errMsg = "PO index";
				return parseError(NUM_TOO_BIG);
			}
			if(_GateList[PoList[errInt]]->getSymbol() != ""){
				errMsg = "o";
				return parseError(REDEF_SYMBOLIC_NAME);
			}
			colNo += errMsg.size();
			if(colNo == temp.size())return parseError(MISSING_SYMBOL);
			if( temp[colNo] != 32 )	return parseError(MISSING_SPACE);	
			colNo++;	
			if(colNo == temp.size())return parseError(MISSING_SYMBOL);	
			errMsg = temp.substr(colNo);
			//reset ss and set eof 
			ss.str("");
			ss.ignore();
			//containing""								
			if(errMsg.empty()) return parseError(ILLEGAL_SYMBOL_TYPE);
			//check whether containing unprintable character
			for(size_t j=0; j<errMsg.size();j++)
				if(errMsg[j] < 32){
					colNo += j;
					errInt = unsigned(errMsg[j]-char(0));
					return parseError(ILLEGAL_SYMBOL_NAME);
				}
			_GateList[PoList[errInt]]->setSymbol(errMsg);
		}
		else if(temp[0] == 32 ) return parseError(EXTRA_SPACE);
		else if(temp.empty()){
			errMsg = "";
			 return parseError(ILLEGAL_SYMBOL_TYPE);
		}
		else{
			errMsg = temp.substr(0,1);
			return parseError(ILLEGAL_SYMBOL_TYPE);
		}
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
	cout<<"\nCircuit Statistics\n"
		 <<"==================\n"
		 <<"  PI "	<<setw(11)<<PiList.size()
		 <<"\n  PO "<<setw(11)<<PoList.size()
		 <<"\n  AIG"<<setw(11)<<_AIGNum
		 <<"\n------------------\n"
		 <<"  Total"<<setw(9)<<PiList.size()+PoList.size()+_AIGNum<<endl;
}

void
CirMgr::printNetlist() const
{
	unsigned lineNum = 0;
	//reset _ref to _globalRef
	for(size_t i=0 ; i<_GateList.size(); i++)
		_GateList[i]->setToGlobalRef();
	CirGate::setGlobalRef();
	cout<<"\n";
	for(size_t i=0; i<PoList.size();i++)
		_GateList[PoList[i]]->printGate(lineNum);
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(size_t i=0; i<PiList.size(); i++) cout<<" "<<PiList[i];
   cout << endl;
}

void
CirMgr::printPOs() const
{	
   cout << "POs of the circuit:";
   for(size_t i=0; i<PoList.size(); i++) cout<<" "<<PoList[i];
   cout << endl;
}

/*EX:
Gates with floating fanin(s): 7
Gates defined but not used  : 9 10
 */

void
CirMgr::printFloatGates() const
{
	vector<unsigned> floatingId;
	vector<unsigned> unusedId;
	for(size_t i=0; i<_GateList.size(); i++){
		CirGate* checkgate = _GateList[i];
		//checking _fanin1 & _fanin2
		//check undefined gate i.e. AIGGAte & PIGate without _fanout
		if(checkgate->getType() == PO_GATE){
			unsigned fId1 = (checkgate->getFanin1())>>1;
			CirGate* fGate1 = cirMgr->_GateList[fId1];
			if( fGate1->getType() == UNDEF_GATE)
				floatingId.push_back(checkgate->getId());
		}
		else if(checkgate->getType() == AIG_GATE){	
			unsigned fId1 = (checkgate->getFanin1())>>1;
			CirGate* fGate1 = cirMgr->_GateList[fId1];
			unsigned fId2 = (checkgate->getFanin2())>>1;
			CirGate* fGate2 = cirMgr->_GateList[fId2];
			if( fGate1->getType() == UNDEF_GATE || fGate2->getType() == UNDEF_GATE)
				floatingId.push_back(checkgate->getId());
			if((checkgate->getFanout()).empty())
				unusedId.push_back(checkgate->getId());
		}
		else if(checkgate->getType() == PI_GATE)
			if((checkgate->getFanout()).empty())
				unusedId.push_back(checkgate->getId());
	}
	
	//floating fanin
	if(!floatingId.empty()){
		::sort(floatingId.begin(),floatingId.end());
		cout<<"Gates with floating fanin(s):";
		for(size_t i=0; i<floatingId.size(); i++)
			cout<<" "<<floatingId[i];
		cout<<"\n";
	}
	//defined not used
	if(!unusedId.empty()){
		::sort(unusedId.begin(),unusedId.end());
		cout<<"Gates defined but not used  :";
		for(size_t i=0; i<unusedId.size(); i++)
			cout<<" "<<unusedId[i];
		cout<<"\n";
	}
	
}

void
CirMgr::writeAag(ostream& outfile) const
{
	//reset _ref
	for(size_t i=0 ; i<_GateList.size(); i++)
		_GateList[i]->setToGlobalRef();
	CirGate::setGlobalRef();
	unsigned A = 0;
	ostringstream ossAIG, ossPO, PISymbol, POSymbol;
	//DFS and get AIG info in oss and # in A
	for(size_t i=0;i < PoList.size();i++){
		_GateList[PoList[i]]->DFS(ossAIG,A);
		ossPO << _GateList[PoList[i]]->getFanin1() << '\n';
		if(_GateList[PoList[i]]->getSymbol() != "")
			POSymbol << 'o' << i << ' ' << _GateList[PoList[i]]->getSymbol() << '\n' ;
	}
	//identifier
	outfile << "aag " << _GateList.size()-PoList.size()-1 << ' ' << PiList.size() << " 0 " 
			  << PoList.size() << ' ' << A << '\n' ;
	for(size_t i=0 ; i < PiList.size() ; i++){
		outfile << 2*PiList[i] << '\n';
		if(_GateList[PiList[i]]->getSymbol() != "")
			PISymbol << 'i' << i << ' ' << _GateList[PiList[i]]->getSymbol() << '\n' ;
	}
	outfile << ossPO.str() << ossAIG.str() << PISymbol.str() << POSymbol.str();
	//comment
	outfile << "c\n" <<	"AAG output by Kevin Hsu\n";
}


