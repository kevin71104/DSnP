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

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   //release memory
   ~CirMgr() { 
		resetList();
	   _GateList.clear();
	}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
<<<<<<< HEAD
   void setGateList(size_t size) { _GateList.reserve(size); }
	
=======
   void setGateList(size_t size) { _GateList.reserve(size); _GateList.resize(size); }
<<<<<<< HEAD
	
	static GateList	_GateList;
	
private:
	IdList 				PiList;
	IdList				PoList;
	size_t				_AIGNum;		//the number of AIGgates

=======

>>>>>>> 6883550f17dca316a0f50d4d96282c5dcdc0d1e5
	static GateList	_GateList;
	
private:
	IdList 				PiList;		//storing PIGate's variable ID
	IdList				PoList;		//storing POGate's variable ID
	size_t				_AIGNum;		//the number of AIGgates
	
<<<<<<< HEAD
	void resetList(){for(size_t i=0; i<_GateList.size(); i++) delete _GateList[i];}

=======
>>>>>>> 393d561f9dd6f43be0bea6c73b1b03b182a01d13
>>>>>>> 6883550f17dca316a0f50d4d96282c5dcdc0d1e5
};

#endif // CIR_MGR_H
