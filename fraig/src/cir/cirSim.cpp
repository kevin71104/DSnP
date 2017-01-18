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
#include <cmath>
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
    //make sure value & ~value will have same () overloading
    unsigned operator() () const { return ( value > ~value ? value : ~value); }
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

//sorting functions
struct sortGList{
    bool operator() (CirGate* g1, CirGate* g2){
        return (g1->getId() < g2->getId());
    }
}sortGateList;

struct sortVecGList{
    bool operator() (GateList& l1, GateList& l2){
        return (l1[0]->getId() < l2[0]->getId());
    }
}sortVecGateList;

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/

//get simulation value and separate original FEC groups
//num : # of bits tested (default value is 31)
//if # of FEC groups doesn't decrease return true
bool
CirMgr::separateFEC(unsigned num)
{
    //update simulation value
    for(unsigned i = 0 ; i < _DfsList.size() ; i++)
        _DfsList[i]->updateValue();

    vector<GateList> newFecList;
    //using hashmap to separate original Fec groups(i.e. _FecList[i])
    for(unsigned i=0; i<_FecList.size(); i++){
        HashMap< FecKey, GateList > newFecGroup(getHashSize( _FecList[i].size() ));
        for(unsigned j=0; j<_FecList[i].size(); j++){
            GateList tempList(1,_FecList[i][j]);
            GateList* info;
            //check if there has GateList with same sim value
            //if not insert it
            //if yes get the ptr of that GateList and push_back this gate
            if (! newFecGroup.insertDataptr(FecKey(_FecList[i][j]->getValue()), tempList, info) )
                info->push_back(_FecList[i][j]);
        }
        //put newFecGroup into newFecList
        //*it will get pair<FecKey,GateList> -> (*it).second = newFecGroup
        for(HashMap<FecKey,GateList>::iterator it = newFecGroup.begin(); it != newFecGroup.end(); it++){
            //if GateList has only 1 gate -> separated
            if( (*it).second.size() == 1)
                (*it).second[0]->setSeparate(true);
            else
                newFecList.push_back( (*it).second );
        }
    }

    bool fail = (_FecList.size() <= newFecList.size());
    _FecList.swap(newFecList);
/*
    //sort _FecList
    for(unsigned i=0; i<_FecList.size(); i++)
        ::sort(_FecList[i].begin(), _FecList[i].end(), sortGateList);
    ::sort(_FecList.begin(), _FecList.end(), sortVecGateList);
*/
    //if need to output log file
    if(_simLog){
        for(unsigned i=0; i<= num; i++){
            for(unsigned j=0; j<PiList.size(); j++)
                *_simLog << ( (_GateList[PiList[j]]->getValue() >> (num-i) )%2 );
            *_simLog << ' ';
            for(unsigned j=0; j<PoList.size(); j++)
                *_simLog << ( (_GateList[PoList[j]]->getValue() >> (num-i) )%2 );
            *_simLog << endl;
        }
    }
    return fail;
}

//_FecList will only contain FEC group
//i.e. those have distinct value will set separate = true, and remove from _FecList
void
CirMgr::randomSim()
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

    unsigned maxFail = log(_DfsList.size())/log(2.1), failNum = 0, testNum = 0;
    cout << "MAX_FAIL = " << maxFail << '\n';
    while(failNum < maxFail){
        //set simulation value
        for(unsigned i=0; i<PiList.size(); i++)
            _GateList[PiList[i]]->setValue( unsigned(rnGen(INT_MAX) + rnGen(INT_MAX)) );
        if( separateFEC() ) failNum++;
        else failNum = 0;
        cerr << "\b\b\b\b\b     \rTotal #FEC Group = " << _FecList.size();
        testNum++;
    }

    //sort _FecList
    for(unsigned i=0; i<_FecList.size(); i++)
        ::sort(_FecList[i].begin(), _FecList[i].end(), sortGateList);
    ::sort(_FecList.begin(), _FecList.end(), sortVecGateList);

    //update FecNum
    for(unsigned i=0; i< _GateList.size(); i++){
      if(_GateList[i] != 0)
        _GateList[i]->setFecNum(UINT_MAX);
    }
    for(unsigned i=0; i< _FecList.size(); i++)
        for(unsigned j=0; j< _FecList[i].size(); j++)
          _FecList[i][j]->setFecNum(i);

    cerr << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b          \r";
    cout << "Total #FEC Group = " << _FecList.size() << '\n';
    cout << testNum*32 << " patterns simulated.\n";
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
        _FecList.clear();
        return;
      }
      for(unsigned i=0; i<PiList.size(); i++){
          if(pattern[i] != '0' && pattern[i] != '1'){
            cerr << "Error: Pattern(" << pattern << ") contains a non-0/1 character('" << pattern[i] << "').\n";
            _FecList.clear();
            return;
          }
          unsigned tempval = _GateList[PiList[i]]->getValue();
          tempval = tempval<<1 | (unsigned(pattern[i] - '0'));
          dynamic_cast<PIGate*>(_GateList[PiList[i]])->setValue(tempval);
      }

      num ++;
      patternFile >> pattern;

      //check if it's enough bits to simulate or reach eof
      if( num % 32 == 0 || patternFile.eof() )
          separateFEC( (num-1)%32 );
  }//end while

  //sort _FecList
  for(unsigned i=0; i<_FecList.size(); i++)
      ::sort(_FecList[i].begin(), _FecList[i].end(), sortGateList);
  ::sort(_FecList.begin(), _FecList.end(), sortVecGateList);

  //update FecNum
  for(unsigned i=0; i< _GateList.size(); i++){
    if(_GateList[i] != 0)
      _GateList[i]->setFecNum(UINT_MAX);
  }
  for(unsigned i=0; i< _FecList.size(); i++)
      for(unsigned j=0; j< _FecList[i].size(); j++)
        _FecList[i][j]->setFecNum(i);

  cout << "\nTotal #FEC Group = " << _FecList.size() << '\n';
  cout << num << " patterns simulated.\n";
}

void
CirMgr::specialSim( vector< vector<unsigned> >& patternList)
{

    //if(_FecList.size()>1){
        _FecList.clear();
        _FecList.push_back(GateList(0));
        if(! _GateList[0]->isSeparate())
            _FecList[0].push_back(_GateList[0]);
            for(unsigned i=0; i<_DfsList.size(); i++)
            if( ! _DfsList[i]->isSeparate() && _DfsList[i]->getType() == AIG_GATE)
                _FecList[0].push_back(_DfsList[i]);
    //}

    unsigned num = patternList[0].size();
    for(unsigned i=0; i<num; i++){
        for(unsigned j=0; j<PiList.size(); j++){
            if(_GateList[PiList[j]]->getDfsId() != UINT_MAX)
                _GateList[PiList[j]]->setValue(patternList[j][i]);
        }
        separateFEC();
    }
/*
    //sort _FecList
    for(unsigned i=0; i<_FecList.size(); i++)
        ::sort(_FecList[i].begin(), _FecList[i].end(), sortGateList);
    ::sort(_FecList.begin(), _FecList.end(), sortVecGateList);*/

    //update FecNum
    for(unsigned i=0; i< _GateList.size(); i++)
      if(_GateList[i] != 0)
        _GateList[i]->setFecNum(UINT_MAX);
    for(unsigned i=0; i< _FecList.size(); i++)
        for(unsigned j=0; j< _FecList[i].size(); j++)
          _FecList[i][j]->setFecNum(i);
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
