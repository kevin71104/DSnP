/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGateV;


//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate() {}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { 
		switch(gateType){
			case	UNDEF_GATE:
				return	"UNDEF";
			case	PI_GATE:
				return	"PI";
			case	PO_GATE:
				return	"PO";
			case	AIG_GATE:
				return	"AIG";
			case	CONST_GATE:
				return	"CONST";			
		}
	}
   unsigned getLineNo() const { return 0; }

   // Printing functions
   //pure virtual function, dereived class should implement it 
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

protected:
	GateType	 	gateType;
	//variable ID i.e. even number
	size_t 			gateId;
   string 			symbol;
   static size_t 	_globalRef;
   size_t			_ref;

   static void setGlobalRef() { ++_globalRef; }
   bool isGlobalRef() const { return (_ref == _globalRef); }
   void setToGlobalRef() { _ref = _globalRef; }

};

class UDEFGate : public CirGate
{
public:
	void printGate() const;
};

class PIGate : public CirGate
{
public:
	void printGate() const;
};

class POGate : public CirGate
{
public:
	void printGate() const;
};

class AIGGate : public CirGate
{
public:
	void printGate() const;
private:
	AIGGateV		_faninList[2];
};

class AIGGateV
{
	#define NEG 0x1
	AIGGateV(AIGGate* g, size_t phase):_gateV(size_t(g) + phase) { }
	AIGGate* gate() const { return (AIGGate*)(_gateV & ~size_t(NEG)); }
	bool isInv() const { return (_gateV & NEG); }
	size_t		_gateV;
};

class CONSTGate : public CirGate
{
public:
	void printGate() const;	
};

#endif // CIR_GATE_H
