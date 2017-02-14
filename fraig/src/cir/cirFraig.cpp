/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <map>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"
#include <cstdlib>
#include <algorithm>
#include <cmath>

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
/*
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
*/
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
  HashMap < StrashKey , CirGate* > strashMap( getHashSize( _DfsList.size() ) );
  bool merge = false;
  //put AIGGate & UNDEFGate in strashMap according to DFS order
  for(unsigned i=0; i< _DfsList.size(); i++){
    CirGate* temp = 0;
    CirGate* checkGate = _DfsList[i];

    //if(checkGate == 0) continue;
    if(checkGate->getType() == UNDEF_GATE || checkGate->getType() == AIG_GATE){
        if(!strashMap.insert( StrashKey( checkGate ), checkGate, temp)){
           cout << "Strashing: " << temp->getId() << " merging " << checkGate->getId() << "...\n";
           checkGate->reconnect( temp , 0 );
           merge = true;
        }
    }
  }
  if(merge) buildDfsList(true);

}

void
CirMgr::fraig()
{
    if(_FecList.empty()) return;

    //solve initialization
    SatSolver solver;
    solver.initialize();
    //assign Var & build CNF
    map<unsigned, Var> satVar;
    satVar[0] = solver.newVar();
    for(unsigned i=0; i<_DfsList.size(); i++){
        CirGate* gate = _DfsList[i];
        satVar[gate->getId()] = solver.newVar();
        //SatSolver::addAndCNF(vf, va, fa, vb, fb)", where vf, va and vb are the
        //variable IDs for this gate and its two fanins
        //(f = a & b), and fa, fb are bool variables to indicate
        if(gate->getType() == AIG_GATE){
            unsigned ip1 = gate->getFanin1();
            unsigned ip2 = gate->getFanin2();
            solver.addAigCNF( satVar[gate->getId()], satVar[ _GateList[ip1>>1]->getId() ], ip1%2,
                              satVar[ _GateList[ip2>>1]->getId() ], ip2%2 );
        }
    }

    bool result;
    Var newV;
    float Maxfail = 0;
    float limit;
    unsigned failLimit;
    vector< vector<unsigned> > patternList;     //get the special input pattern
    patternList.resize(PiList.size());

    /*for(unsigned i=0; i<_FecList.size(); i++){
        if( i == 0) cout << "report FEC:";
        cout << " " << _FecList[i].size() ;
    }*/

    while(! _FecList.empty()){
        limit = ( getFECsize()>1000 ? getFECsize() : 1000 ) /10;
        for(unsigned i=0; i<_FecList.size(); i++){
            if(Maxfail > limit) break;
            for(unsigned j=0; j<_FecList[i].size(); j++){
                if(_FecList[i][j] == 0) continue;
                if(Maxfail > limit ) break;
                unsigned fail = 0;  //# of none-equivalent times
                if(_FecList[i][j]->getType() == CONST_GATE)
                    failLimit = 1;
                else
                    failLimit = 1;
                unsigned checkId = _FecList[i][j]->getId();
                for(unsigned k=j+1; k<_FecList[i].size(); k++){
                    if(_FecList[i][k] == 0) continue;
                    if(Maxfail > limit) break;
                    unsigned compId = _FecList[i][k]->getId();
                    bool INV = (_FecList[i][j]->getValue() != _FecList[i][k]->getValue());
                    cerr << "\rProving: ( " << checkId << " , " << ( INV ? "!" : "") << compId << " )...";
                    newV = solver.newVar();
                    solver.addXorCNF( newV, satVar[checkId], false, satVar[compId], INV );
                    solver.assumeRelease();
                    solver.assumeProperty(satVar[0],false);
                    solver.assumeProperty(newV,true);
                    result = solver.assumpSolve();

                    //deal with SAT or not
                    if(!result){      //UNSAT -> equivalent
                        cerr << "UNSAT!!" << "Maxfail= " << Maxfail << "          ";
                        cout << "\nFraig: " << checkId  << " merging " << (INV ? "!" : "") << compId << "...\n";
                        _FecList[i][k]->reconnect(_FecList[i][j], INV);
                        _FecList[i].erase(_FecList[i].begin()+k);
                        k--;
                        fail = 0;
                        //Maxfail = 0 ;
                    }
                    else{            //SAT : sepatated
                        cerr << "SAT!!" << "Maxfail= " << Maxfail << "         ";
                        fail ++;
                        Maxfail ++ ;
                        if(_FecList[i][j]->getType() != CONST_GATE) {
                            //get the special pattern
                            for(unsigned l=0; l<PiList.size(); l++)
                                if(_GateList[PiList[l]]->getDfsId() != UINT_MAX)
                                    patternList[l].push_back( unsigned(solver.getValue( satVar[PiList[l]] ) ) );
                        }
                    }
                    //it means that _FecList[i][j] is compared to every elements in _FecList[i]
                    if(k == _FecList[i].size()-1){
                        if(_FecList[i].size() == 2)
                            _FecList[i][j]->setSeparate(true);
                        _FecList[i].erase(_FecList[i].begin()+j);
                        j--;
                    }
                    if( fail >= failLimit ) break;
                }//end compare(k-loop)
                if(_FecList[i].size() == 1){
                    _FecList[i][0]->setSeparate(true);
                    _FecList[i][0]->setFecNum(UINT_MAX);
                    _FecList.erase(_FecList.begin()+i);
                    i--;
                    break;
                }
            }//end a FEC group(j-loop)
        }//end (i-loop)
        Maxfail = 0;
        buildDfsList(true); //probably merged
        sweep();
        specialSim(patternList);
        cerr << "\nUpdating Total #FEC Group = " << _FecList.size() << '\n';
        if(_FecList.size() == 1 && _FecList[0].empty()){
            _FecList.erase(_FecList.begin());
            break;
        }
    }//end while-loop

    buildDfsList(true);
    if(_FecList.size() == 1)
        if(_FecList[0].empty())
            _FecList.pop_back();
}


/********************************************/
/*   Private member functions about fraig   */
/********************************************/
