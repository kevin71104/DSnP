/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern	CirMgr*		cirMgr;
extern  GateList _DfsList;
extern  vector<GateList> _FecList;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
    CirMgr(){    }
    //release memory
    ~CirMgr() {
      resetList();
      _GateList.clear();
      vector<GateList> temp;
      GateList temp1;
      _DfsList.clear();
      _DfsList.swap(temp1);
      _FecList.clear();
      _FecList.swap(temp);
    }

    // Access functions
    // return '0' if "gid" corresponds to an undefined gate.
    CirGate* getGate(unsigned gid) const;

    // Member functions about circuit optimization
    void sweep();
    void optimize();

    // Member functions about simulation
    void  randomSim();
    void  fileSim(ifstream&);
    void  setSimLog(ofstream *logFile) { _simLog = logFile; }
    bool  separateFEC(unsigned num=31);

    // Member functions about fraig
    void strash();
    void printFEC() const;
    void fraig();
    void specialSim(vector< vector<unsigned> >& patternList);

    // Member functions about circuit construction
    bool readCircuit(const string&);

    // Member functions about circuit reporting
    void printSummary() const;
    void printNetlist() const;
    void printPIs() const;
    void printPOs() const;
    void printFloatGates() const;
    void printFECPairs() const;
    void writeAag(ostream&) const;
    void writeGate(ostream&, CirGate*) const;

    //void setGateList(size_t size) { _GateList.reserve(size); }
    static GateList	_GateList;
    //static GateList _DfsList;

private:
    IdList          PiList;     //storing PIGate's variable ID
    IdList          PoList;     //storing POGate's variable ID
    mutable size_t  _AIGNum;    //the number of AIGgates
    ofstream*       _simLog;

    void resetList(){
      for(size_t i=0; i<_GateList.size(); i++)
        if(_GateList[i])
          delete _GateList[i];
    }
    void buildDfsList(bool rebuild);
    void updateAigNum() const;
};

#endif // CIR_MGR_H
