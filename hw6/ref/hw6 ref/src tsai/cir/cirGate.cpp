/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <climits>
#include <vector>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

GateList CirMgr::gList;
extern CirMgr *cirMgr;

unsigned CirGate::_globalRef = 0;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
   cout << setfill('=') << setw(51) << '\n';
   ostringstream oss;
   oss << getTypeStr() << '(' << id << ')';
   if(symbol != "")
      oss << "\"" << symbol << "\"" ;
   oss << ", line " << line;
   cout << "= " << setfill(' ') << setw(47) << left << oss.str() << "=\n";
   cout << setfill('=') << setw(51) << right << '\n' << setfill(' ');
}

void
CirGate::reportFanin(int& level, bool n)
{
   static int templevel = 0;
   if(templevel)
      cout << setw(2 * templevel) << ' ';
   else
      setGlobalRef();
   if(n)
      cout << '!';
   cout << getTypeStr() << ' ' << id;
   if(isGlobalRef()){
      if(gtype != PI_GATE)
         cout << " (*)\n";
      return;
   }
   cout << '\n';
   if(templevel==level)
      return;
   setToGlobalRef();
   ++templevel;
   if(gtype == PO_GATE)
      cirMgr->gList[fanin1>>1]->reportFanin(level, bool(fanin1%2));
   if(gtype == AIG_GATE){
      cirMgr->gList[fanin1>>1]->reportFanin(level, bool(fanin1%2));
      cirMgr->gList[fanin2>>1]->reportFanin(level, bool(fanin2%2));
   }
   --templevel;
}

void
CirGate::reportFanout(int& level, unsigned prev, bool n)
{
   static int templevel = 0;
   if(templevel)
      cout << setw(2 * templevel) << ' ';
   else
      setGlobalRef();
   if(n){
      if(prev == (fanin1>>1))
            cout << '!';
      else if(prev == (fanin2>>1))
            cout << '!';
   }
   cout << getTypeStr() << ' ' << id;
   if(isGlobalRef()){
      if(gtype != PO_GATE)
         cout << " (*)\n";
      return;
   }
   cout << '\n';
   if(templevel==level)
      return;
   setToGlobalRef();
   ++templevel;
   for(unsigned i = 0 ; i < fanout.size() ; ++i)
      cirMgr->gList[fanout[i]>>1]->reportFanout(level, id, bool(fanout[i]%2));
   --templevel;
}

void
CirGate::connect()
{
   if(gtype == PO_GATE)
      return;
   for(unsigned i = 0 ; i < cirMgr->gList.size() ; ++i)
      if(cirMgr->gList[i]->getTypeGtype() == PO_GATE){
         if((cirMgr->gList[i]->fanin1>>1) == id){
            if(cirMgr->gList[i]->fanin1 == id*2)
               fanout.push_back(cirMgr->gList[i]->getId()<<1);
            else
               fanout.push_back((cirMgr->gList[i]->getId()<<1) + 1);
         }
      }
      else if(cirMgr->gList[i]->getTypeGtype() == AIG_GATE){
         if(cirMgr->gList[i]->fanin1 == id*2)
            fanout.push_back(cirMgr->gList[i]->getId()<<1);
         else if(cirMgr->gList[i]->fanin1 == id*2 + 1)
            fanout.push_back((cirMgr->gList[i]->getId()<<1) + 1);
         if(cirMgr->gList[i]->fanin2 == id*2)
               fanout.push_back(cirMgr->gList[i]->getId()<<1);
         else if(cirMgr->gList[i]->fanin2 == id*2 + 1)
               fanout.push_back((cirMgr->gList[i]->getId()<<1) + 1);
      }
}


void
CirPiGate::printGate(unsigned &lineno)
{
   if(isGlobalRef())
      return;
   setToGlobalRef();
   cout << "\n[" << lineno << "] ";
   ++lineno;
   cout << "PI  " << id;
   if(symbol != "")
      cout << " (" << symbol << ')';
}

void
CirPoGate::printGate(unsigned &lineno)
{
   if(isGlobalRef())
      return;
   setToGlobalRef();
   cirMgr->gList[fanin1>>1]->printGate(lineno);
   cout << "\n[" << lineno << "] ";
   ++lineno;
   cout << "PO  " << id << ' ';
   if(cirMgr->gList[fanin1>>1]->getTypeGtype() == UNDEF_GATE)
      cout << '*';
   if(fanin1 % 2)
      cout << '!';
   cout << (fanin1>>1);
   if(symbol != "")
      cout << " (" << symbol << ')';
}


void
CirAigGate::printGate(unsigned &lineno)
{
   if(isGlobalRef())
      return;
   setToGlobalRef();
   cirMgr->gList[fanin1>>1]->printGate(lineno);
   cirMgr->gList[fanin2>>1]->printGate(lineno);
   cout << "\n[" << lineno << "] ";
   ++lineno;
   cout << "AIG " << id << ' ';
   if(cirMgr->gList[fanin1>>1]->getTypeGtype() == UNDEF_GATE)
      cout << '*';
   if(fanin1 % 2)
      cout << '!';
   cout << (fanin1>>1) << ' ';
   if(cirMgr->gList[fanin2>>1]->getTypeGtype() == UNDEF_GATE)
      cout << '*';
   if(fanin2 % 2)
      cout << '!';
   cout << (fanin2>>1);
}

void
CirConstGate::printGate(unsigned &lineno)
{
   if(isGlobalRef())
      return;
   setToGlobalRef();
   cout << "\n[" << lineno << "] ";
   ++lineno;
   cout << "CONST0";
}
