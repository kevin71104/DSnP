/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

#ifndef FECKEY_H
#define FECKEY_H

class FecKey
{
  public:
    FecKey(unsigned val): value(val) {}
    unsigned operator() () const { return value; }
    //~ :bitwise not
    bool operator == (const FecKey& k) const { return (value == k.value || value == ~ k.value); }
  private:
    unsigned value;
};

#endif

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/

void
CirMgr::updateSim()
{
  for(unsigned i = 0 ; i < _DfsList.size() ; i++)
    _DfsList[i]->updateValue();
}

//_FecList will only contain FEC group
//i.e. those have distinct value will set separate = true, and remove from _FecList
void
CirMgr::randomSim()
{
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  //if _FecList is empty, put CONST & AIG gates in _DfsList in one FEC Group
  if(_FecList.empty()){
      _FecList.push_back(GateList(0));
      if(! _GateList[0]->isSeparate())
          _FecList[0].push_back(_GateList[0]);
      for(unsigned i=0; i<_DfsList.size(); i++)
          if( ! _DfsList[i]->isSeparate() && _DfsList[i]->getType() == AIG_GATE)
              _FecList[0].push_back(_DfsList[i]);
  }

  unsigned num = 0;     // # of bits
  string pattern;
  patternFile >> pattern;

  //until read to eof
  while(patternFile.good()){
      //pattern size is worng
      if(pattern.size() != PiList.size()){
        cerr << "Error: Pattern(" << pattern << ") length(" << pattern.size()
             << ") does not match the number of inputs(" << PiList.size()
             << ") in a circuit!!\n";
        return;
      }
      for(unsigned i=0; i<PiList.size(); i++){
          if(pattern[i] != '0' && pattern[i] != '1'){
            cerr << "Error: Pattern(" << pattern << ") contains a non-0/1 character('" << pattern[i] << "').\n";
            return;
          }
          unsigned tempval = _GateList[PiList[i]]->getValue();
          tempval = tempval<<1 | (unsigned(pattern[i] - '0'));
          dynamic_cast<PIGate*>(_GateList[PiList[i]])->setValue(tempval);
      }

      num ++;
      patternFile >> pattern;

      //check if it's enough bits to simulate or reach eof
      if( num % 32 == 0 || patternFile.eof() ){
          updateSim();
          vector<GateList> newFecList;
          //using hashmap to separate original Fec groups
          for(unsigned i=0; i<_FecList.size(); i++){
              HashMap< FecKey, GateList > newFecGroup(_FecList.size());
              for(unsigned j=0; j<_FecList[i].size(); j++)
          }
      }
  }


}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
