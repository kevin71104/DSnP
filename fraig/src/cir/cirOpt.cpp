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
#include "util.h"

using namespace std;

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
     if(_GateList[i] -> isGlobalRef())
        continue;
     cout << "Sweeping: " << _GateList[i] -> getTypeStr() << '(' << _GateList[i] -> getId() << ") removed...\n";
     delete _GateList[i];
     _GateList[i] = 0;
  }
  buildDfsList(true);
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
