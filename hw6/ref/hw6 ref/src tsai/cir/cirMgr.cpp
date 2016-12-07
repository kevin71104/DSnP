/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
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
   MISSING_SPACE,
   ILLEGAL_NUM,
   ILLEGAL_LATCH_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   MISSING_SYMBOL_NAME,
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
static unsigned errUint;
static CirGate *errGate = 0;

static bool
parseError(CirParseError err)
{
   switch (err) {
      //space
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;

      //newline
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;

      //identifier
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;

      //number
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing number!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_LATCH_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal Latch number "
              << errMsg << "!!" << endl;
         break;
      case NUM_TOO_SMALL://for headers, symbols
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errUint << ")!!" << endl;
         break;
      case NUM_TOO_BIG://for headers, symbols
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errUint << ")!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errUint << "(" << errUint/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errUint << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errUint
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errUint << ")!!" << endl;
         break;

      //symbol
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errUint
              << ")!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << "\" is redefined!!" << endl;
         break;
      case MISSING_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing symbolic name!!" << endl;
         break;

      //eof
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
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

   string str;
   unsigned M, I, L, O, A;
   stringstream ss;
   lineNo = colNo = 0;
   IdList pofaninList, aigfanin1List, aigfanin2List, aigidList;

   #define newline();                     \
   if(!ss.eof())                          \
      return parseError(MISSING_NEWLINE); \
   ++lineNo;                              \
   colNo = 0;                             \
   ss.clear();                            \
   ss.str("");


   //header
   {
   getline(ifs, str);

   //identifier
   {
   if(str[colNo] <= 32){
      errMsg = "aag";
      return parseError(MISSING_IDENTIFIER);
   }
   ss << str;
   ss >> errMsg;
   if(errMsg != "aag")
      return parseError(ILLEGAL_IDENTIFIER);
   colNo += errMsg.size();
   }

   //M
   {
   if(str[colNo] != 32)
      return parseError(MISSING_SPACE);
   ++colNo;
   if(str[colNo] <= 32)
      return parseError(MISSING_NUM);
   ss >> errMsg;
   if(!myStr2UInt(errMsg, M))
      return parseError(ILLEGAL_NUM);
   if(M < 0)
      return parseError(ILLEGAL_NUM);
   colNo += errMsg.size();
   }

   //I
   {
   if(str[colNo] != 32)
      return parseError(MISSING_SPACE);
   ++colNo;
   if(str[colNo] <= 32)
      return parseError(MISSING_NUM);
   ss >> errMsg;
   if(!myStr2UInt(errMsg, I))
      return parseError(ILLEGAL_NUM);
   if(I < 0)
      return parseError(ILLEGAL_NUM);
   colNo += errMsg.size();
   }

   //L
   {
   if(str[colNo] != 32)
      return parseError(MISSING_SPACE);
   ++colNo;
   if(str[colNo] <= 32)
      return parseError(MISSING_NUM);
   ss >> errMsg;
   if(!myStr2UInt(errMsg, L))
      return parseError(ILLEGAL_NUM);
   if(L != 0)
      return parseError(ILLEGAL_LATCH_NUM);
   colNo += errMsg.size();
   }

   //O
   {
   if(str[colNo] != 32)
      return parseError(MISSING_SPACE);
   ++colNo;
   if(str[colNo] <= 32)
      return parseError(MISSING_NUM);
   ss >> errMsg;
   if(!myStr2UInt(errMsg, O))
      return parseError(ILLEGAL_NUM);
   if(O < 0)
      return parseError(ILLEGAL_NUM);
   colNo += errMsg.size();
   }

   //A
   {
   if(str[colNo] != 32)
      return parseError(MISSING_SPACE);
   ++colNo;
   if(str[colNo] <= 32)
      return parseError(MISSING_NUM);
   ss >> errMsg;
   if(!myStr2UInt(errMsg, A))
      return parseError(ILLEGAL_NUM);
   if(A < 0)
      return parseError(ILLEGAL_NUM);
   colNo += errMsg.size();
   }

   if(M < A+I+L){
      errMsg = "M";
      errUint = M;
      return parseError(NUM_TOO_SMALL);
   }
   aigNum = A;
   }


   //inputs
   for(unsigned i = 0 ; i < I ; ++i){
      newline();
      if(ifs.eof())
         return parseError(MISSING_DEF);
      getline(ifs, str);
      if(str[colNo] <= 32)
         return parseError(MISSING_NUM);
      ss << str;
      ss >> errMsg;
      if(!myStr2UInt(errMsg, errUint))
         return parseError(ILLEGAL_NUM);
      if(errUint % 2)
         return parseError(CANNOT_INVERTED);
      if((errUint>>1) > M)
         return parseError(MAX_LIT_ID);
      if(errUint == 0 || errUint == 1)
         return parseError(REDEF_CONST);
      for(unsigned j = 0 ; j < piList.size() ; ++j)
         if(errUint>>1 == piList[j]){
            if(errGate)
               delete errGate;
            errGate = new CirPiGate(j+2, piList[j]);
            return parseError(REDEF_GATE);
         }
      piList.push_back(errUint>>1);
      colNo += errMsg.size();
   }


   //outputs
   for(unsigned i = 0 ; i < O ; ++i){
      newline();
      if(ifs.eof())
         return parseError(MISSING_DEF);
      getline(ifs, str);
      if(str[colNo] <= 32)
         return parseError(MISSING_NUM);
      ss << str;
      ss >> errMsg;
      if(!myStr2UInt(errMsg, errUint))
         return parseError(ILLEGAL_NUM);
      if((errUint>>1) > M)
         return parseError(MAX_LIT_ID);
      pofaninList.push_back(errUint);
      colNo += errMsg.size();
   }


   //AIGs
   for(unsigned i = 0 ; i < A ; ++i){
      newline();
      if(ifs.eof())
         return parseError(MISSING_DEF);
      getline(ifs, str);

      //Id
      {
      if(str[colNo] <= 32)
         return parseError(MISSING_NUM);
      ss << str;
      ss >> errMsg;
      if(!myStr2UInt(errMsg, errUint))
         return parseError(ILLEGAL_NUM);
      if(errUint % 2)
         return parseError(CANNOT_INVERTED);
      if((errUint>>1) > M)
         return parseError(MAX_LIT_ID);
      if(errUint == 0 || errUint == 1)
         return parseError(REDEF_CONST);
      for(unsigned j = 0 ; j < I ; ++j)
         if(errUint>>1 == piList[j]){
            if(errGate)
               delete errGate;
            errGate = new CirPiGate(j+2, piList[j]);
            return parseError(REDEF_GATE);
         }
      for(unsigned j = 0 ; j < aigidList.size() ; ++j)
         if(errUint>>1 == aigidList[j]){
            if(errGate)
               delete errGate;
            errGate = new CirAigGate(I+O+j+2, 0, 0, aigidList[j]);
            return parseError(REDEF_GATE);
         }
      aigidList.push_back(errUint>>1);
      colNo += errMsg.size();
      }

      //fanin1
      {
      if(str[colNo] != 32)
         return parseError(MISSING_SPACE);
      ++colNo;
      if(str[colNo] <= 32)
         return parseError(MISSING_NUM);
      ss >> errMsg;
      if(!myStr2UInt(errMsg, errUint))
         return parseError(ILLEGAL_NUM);
      if((errUint>>1) > M)
         return parseError(MAX_LIT_ID);
      aigfanin1List.push_back(errUint);
      colNo += errMsg.size();
      }

      //fanin2
      {
      if(str[colNo] != 32)
         return parseError(MISSING_SPACE);
      ++colNo;
      if(str[colNo] <= 32)
         return parseError(MISSING_NUM);
      ss >> errMsg;
      if(!myStr2UInt(errMsg, errUint))
         return parseError(ILLEGAL_NUM);
      if((errUint>>1) > M)
         return parseError(MAX_LIT_ID);
      aigfanin2List.push_back(errUint);
      colNo += errMsg.size();
      }
   }


   //construct circuit
   {
   bool flag = 0;
   unsigned poId = M;
   gList.push_back(new CirConstGate());
   for(unsigned i = 1 ; i <= M ; ++i){
      flag = 0;
      for(unsigned j = 0 ; j < I ; ++j)
         if(piList[j] == i){
            gList.push_back(new CirPiGate(j+2, i));
            flag = 1;
            break;
         }
      if(flag)
         continue;
      for(unsigned j = 0 ; j < A ; ++j)
         if(aigidList[j] == i){
            gList.push_back(new CirAigGate(I+O+j+2, (unsigned)aigfanin1List[j], (unsigned)aigfanin2List[j], i));
            flag = 1;
            break;
         }
      if(flag)
         continue;
      gList.push_back(new CirUndefGate(i));
   }
   for(unsigned i = 0 ; i < O ; ++i){
      gList.push_back(new CirPoGate(I+i+2, (unsigned)pofaninList[i], ++poId));
      poList.push_back(poId);
   }

   for(unsigned i = 0 ; i <= M ; ++i)
      gList[i]->connect();
   }


   //symbols
   for(unsigned i = 0 ; i < I+O ; ++i){
      newline();
      getline(ifs, str);
      if(ifs.eof())
         break;
      if(str[colNo] == 'c'){
         if(str.size() != 1)
            return parseError(MISSING_NEWLINE);
         break;
      }

      ss << str;
      ss >> errMsg;
      if(str[colNo] != 'i' && str[colNo] != 'o')
         return parseError(ILLEGAL_SYMBOL_TYPE);
      if(!myStr2UInt(errMsg.substr(1), errUint))
         return parseError(ILLEGAL_SYMBOL_TYPE);
      if(str[colNo] == 'i'){
         if(errUint >= I){
            errMsg = "Symbol index";
            return parseError(NUM_TOO_BIG);
         }
         if(gList[piList[errUint]] -> getSymbol() != "")
            return parseError(REDEF_SYMBOLIC_NAME);
         colNo += errMsg.size();
         if(str[colNo] != 32)
            return parseError(MISSING_SPACE);
         ++colNo;
         errMsg = str.substr((size_t)colNo);
         ss.str("");
         ss.ignore();
         if(errMsg == "")
            return parseError(MISSING_SYMBOL_NAME);
         gList[piList[errUint]] -> setSymbol(errMsg);
      }
      else if(str[colNo] == 'o'){
         if(errUint >= O){
            errMsg = "Symbol index";
            return parseError(NUM_TOO_BIG);
         }
         if(gList[poList[errUint]] -> getSymbol() != "")
            return parseError(REDEF_SYMBOLIC_NAME);
         colNo += errMsg.size();
         if(str[colNo] != 32)
            return parseError(MISSING_SPACE);
         ++colNo;
         errMsg = str.substr((size_t)colNo);
         ss.str("");
         ss.ignore();
         if(errMsg == "")
            return parseError(MISSING_SYMBOL_NAME);
         gList[poList[errUint]] -> setSymbol(errMsg);
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
   cout << "\nCircuit Statistics\n"
        << "==================\n"
        << "  PI " << setw(11) << piList.size() << '\n'
        << "  PO " << setw(11) << poList.size() << '\n'
        << "  AIG" << setw(11) << aigNum << '\n'
        << "------------------\n"
        << "  Total" << setw(9) << aigNum+piList.size()+poList.size() << '\n';
}

//gateId (!)fanin1 (!)fanin2
void
CirMgr::printNetlist() const
{
   unsigned lineno = 0;
   CirGate::setGlobalRef();
   for(unsigned i = 0 ; i < poList.size() ; ++i)
      gList[poList[i]]->printGate(lineno);
   cout << '\n';
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(unsigned i = 0 ; i < piList.size() ; ++i)
      cout << ' ' << piList[i];
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(unsigned i = 0 ; i < poList.size() ; ++i)
      cout << ' ' << poList[i];
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   bool flag = 0;
   ostringstream oss;
   oss << "Gates with floating fanin(s):";
   for(unsigned i = 0 ; i < gList.size() ; ++i)
      if(gList[i]->getTypeGtype() == PO_GATE){
         if(gList[ gList[i]->fanin1>>1 ]->getTypeGtype() == UNDEF_GATE){
            flag = 1;
            oss << ' ' << gList[i]->getId();
         }
      }
      else if(gList[i]->getTypeGtype() == AIG_GATE){
         if(gList[ gList[i]->fanin1>>1 ]->getTypeGtype() == UNDEF_GATE){
            flag = 1;
            oss << ' ' << gList[i]->getId();
         }
         else if(gList[ gList[i]->fanin2>>1 ]->getTypeGtype() == UNDEF_GATE){
            flag = 1;
            oss << ' ' << gList[i]->getId();
         }
      }
   if(flag)
      cout << oss.str();
   oss.str("");
   flag = 0;
   oss << "\nGates defined but not used  :";
   for(unsigned i = 0 ; i < gList.size() ; ++i)
      if(gList[i]->getTypeGtype() == AIG_GATE || gList[i]->getTypeGtype() == PI_GATE)
         if(gList[i]->fanout.empty()){
            flag = 1;
            oss << ' ' << gList[i]->getId();
         }
   if(flag)
      cout << oss.str();
   cout << '\n';
}

void
CirMgr::writeAag(ostream& outfile)
{
   outfile << "aag " << gList.size() << ' ' << piList.size() << " 0 " << poList.size() << ' ';
   ostringstream oss;
   CirGate::setGlobalRef();
   unsigned count=0;
   for(unsigned i = 0 ; i < poList.size() ; ++i){
      gList[poList[i]]->setToGlobalRef();
      writeDFL( gList[ gList[poList[i]]->fanin1 >> 1 ], count, oss );
   }
   outfile << count << '\n';
   for(unsigned i = 0 ; i < piList.size() ; ++i)
      outfile << piList[i] * 2 << '\n';
   for(unsigned i = 0 ; i < poList.size() ; ++i)
      outfile << gList[poList[i]]->fanin1 << '\n';
   outfile << oss.str();
   for(unsigned i = 0 ; i < piList.size() ; ++i)
      if(gList[piList[i]]->getSymbol() != "")
         outfile << "i" << i << ' ' << gList[piList[i]]->getSymbol() << '\n';
   for(unsigned i = 0 ; i < poList.size() ; ++i)
      if(gList[poList[i]]->getSymbol() != "")
         outfile << "o" << i << ' ' << gList[poList[i]]->getSymbol() << '\n';
}

void
CirMgr::writeDFL(CirGate* &cgate, unsigned &count, ostringstream& oss)
{
   if(cgate->isGlobalRef())
      return;
   cgate->setToGlobalRef();
   if(cgate->getTypeGtype() == AIG_GATE){
      ++count;
      writeDFL( gList[ cgate->fanin1 >> 1 ], count, oss );
      writeDFL( gList[ cgate->fanin2 >> 1 ], count, oss );
      oss << cgate->getId() * 2 << ' ' << cgate->fanin1 << ' ' << cgate->fanin2 << '\n';
   }
}
