/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"
#include <cstdlib>

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

#ifndef STRASHKEY_H
#define STRASHKEY_H

class StrashKey
{
  public:
    StrashKey(CirGate* gate) {
      if(gate->getType() == UNDEF_GATE){
        fanin1 = UINT_MAX;
        fanin2 = UINT_MAX;
        return;
      }
      fanin1 = gate->getFanin1();
      fanin2 = gate->getFanin2();
      if (fanin1 > fanin2)
        swap(fanin1,fanin2);
    }
    unsigned operator() () const {
      srandom(fanin2);
      return ( (fanin1<<9) ^ unsigned(random())  );
    }
    bool operator == (const StrashKey& k) const { return (fanin1 == k.fanin1 && fanin2 == k.fanin2); }

  private:
    unsigned fanin1;
    unsigned fanin2;
};

#endif

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
  HashMap < StrashKey , CirGate* > strashMap(_DfsList.size());
  bool merge = false;
  //put AIGGate & UNDEFGate in strashMap according to DFS order
  for(unsigned i=0; i< _DfsList.size(); i++){
    CirGate* temp = 0;
    CirGate* checkGate = _DfsList[i];

    if(checkGate == 0) return;
    else if(checkGate->getType() == UNDEF_GATE || checkGate->getType() == AIG_GATE){
      if(! strashMap.insert( StrashKey(checkGate) , checkGate , temp ) ){
        merge = true;
        if( temp->getFanoutSize() >= checkGate->getFanoutSize()){
          //stored data's _fanout is bigger. Thus, temp substitutes for checkGate
          cout << "Strashing: " << temp->getId() << " merging " << checkGate->getId() << "...\n" ;
          checkGate->reconnect(temp,0);
        }
        else{
          cout << "Strashing: " << checkGate->getId() << " merging " << temp->getId() << "...\n" ;
          temp->reconnect(checkGate,0);
          strashMap.replaceInsert( StrashKey(checkGate) , checkGate );
        }
      }
    }
  }
  if(merge) buildDfsList(true);
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
