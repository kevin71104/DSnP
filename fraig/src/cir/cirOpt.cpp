/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"


// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
  CirGate::setGlobalRef();
  //set DFS CONST PI to ref
  for(unsigned i = 0; i< _DfsList.size(); i++)
     _DfsList[i]->setToGlobalRef();
  for(unsigned i = 0; i< PiList.size(); i++)
     _GateList[ PiList[i] ]->setToGlobalRef();
  _GateList[0]->setToGlobalRef();

  for(unsigned i = 1; i < _GateList.size() - PoList.size() ; ++i){
     if(_GateList[i] == 0)
        continue;
     if(_GateList[i]->isGlobalRef())
        continue;
     cerr << "Sweeping: " << _GateList[i] -> getTypeStr() << '(' << _GateList[i] -> getId() << ") removed...\n";
     delete _GateList[i];
     _GateList[i] = 0;
  }
  buildDfsList(true);
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
//const 0 1 as fanins
//AIG with same fanin (INV or not)
void
CirMgr::optimize()
{
  CirGate* check;
  bool done;
  do{
    done = true;
    //check same input
    for(unsigned int i=0 ; i< _DfsList.size() ; i++){
      if(_DfsList[i] == 0) continue;
      if(_DfsList[i]->getType() != AIG_GATE) continue;
      unsigned ip1 = _DfsList[i]->getFanin1();
      unsigned ip2 = _DfsList[i]->getFanin2();
      if( ip1>>1 == ip2>>1){
        //same phase replace this gate by its input
        if(ip1 == ip2){cerr << "111111111111111111\n";
        cout << "Simplifying: " << _GateList[ip1>>1]->getId()
             << " merging " << (ip1 % 2 ? "!" : "") << _DfsList[i]->getId() << "...\n";
        _DfsList[i]->reconnect(_GateList[ip1>>1],ip1 % 2);
        }
        else{
          cout << "Simplifying: 0 merging " << _DfsList[i]->getId() << "...\n";
          _DfsList[i]->reconnect(_GateList[0],0);
        }
      }
    }

    //const 0 1 fanins
    IdList constList= _GateList[0]->getFanout();
    for(unsigned i = 0;i< constList.size(); i++ ){
      unsigned fanoutId = constList[i];
      check = _GateList[fanoutId>>1];
      if( check == 0) continue;
      if( check->getType() == PO_GATE) continue;
      //fanoutId%2 : const 1
      if(fanoutId % 2){
        if( check->getFanin1() == 1){
          //reconnect fanin2 gate with output
          cout << "Simplifying: " << _GateList[check->getFanin2() >>1]->getId()
               << " merging " << (check->getFanin2() % 2 ? "!" : "") << check->getId() << "...\n";
          check->reconnect(_GateList[check->getFanin2() >>1], check->getFanin2() % 2);
        }
        else{
          //reconnect fanin1 gate with output
          cout << "Simplifying: " << _GateList[check->getFanin1() >>1]->getId()
               << " merging " << (check->getFanin1() % 2 ? "!" : "") << check->getId() << "...\n";
          check->reconnect(_GateList[check->getFanin1() >>1], check->getFanin1() % 2);
        }
      }
      else{
        cout << "Simplifying: 0 merging " << check->getId() << "...\n";
        check->reconnect(_GateList[0],0);
      }
      done = false;
    }
  }
  while (!done);
  buildDfsList(true);
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
