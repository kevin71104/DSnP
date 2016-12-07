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
#include <climits>
#include "cirDef.h"

using namespace std;

<<<<<<< HEAD
//class CirGateV;
=======
<<<<<<< HEAD
//class CirGateV;
=======
class CirGateV;
>>>>>>> 393d561f9dd6f43be0bea6c73b1b03b182a01d13
>>>>>>> 6883550f17dca316a0f50d4d96282c5dcdc0d1e5


//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
	//friend class CirMgr;
public:
   CirGate(unsigned lineNum, GateType name, unsigned ID, unsigned ip1 = UINT_MAX, unsigned ip2 = UINT_MAX )
<<<<<<< HEAD
		: line(lineNum), gateType(name), gateId(ID), _fanin1(ip1), _fanin2(ip2), _ref(0) {}
=======
		: gateType(name), gateId(ID), _fanin1(ip1), _fanin2(ip2), line(lineNum){}
>>>>>>> 6883550f17dca316a0f50d4d96282c5dcdc0d1e5
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
<<<<<<< HEAD
				return	"CONST";	
			case TOT_GATE:
            return "";		
=======
<<<<<<< HEAD
				return	"CONST";	
			case TOT_GATE:
            return "";		
		}
		return "";
	}
   unsigned getLineNo() const { return line; }
   GateType getType() 	const { return gateType; }
   unsigned getFanin1() const { return _fanin1; }
   unsigned getFanin2() const { return _fanin2; }
   unsigned getId() 		const { return gateId; }
   string 	geySymbol() const { return symbol; }

   // Printing functions
   //pure virtual function, dereived class should implement it 
   virtual void printGate(unsigned& lineNum) const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void setSymbol(const string& str){ symbol = str; }

protected:
	GateType	 			gateType;
	unsigned				gateId;				//variable ID i.e.( >>1 = /2 )
   unsigned				_fanin1;				//literal ID 
   unsigned				_fanin2;				//literal ID 
   string 				symbol; 
   IdList				_fanout;				//variable ID
   unsigned				line;					//which line define this gate
   static unsigned 	_globalRef;
   unsigned				_ref;

   static void setGlobalRef() { ++ _globalRef; }
   bool isGlobalRef() const { return ( _ref == _globalRef); }
   void setToGlobalRef() { _ref = _globalRef; }

};
/*
class CirGateV
{
	#define NEG 0x1
	CirGateV(AIGGate* g, size_t phase):_gateV(size_t(g) + phase) { }
	CirGateV(size_t ref):_gateV(ref){}
	CirGate* gate() const { return (AIGGate*)(_gateV & ~size_t(NEG)); }
	bool isInv() const { return (_gateV & NEG); }
	size_t		_gateV;
};
*/
class UNDEFGate : public CirGate
{
public:
	UNDEFGate(unsigned id)
		:CirGate(0, UNDEF_GATE, id) {}
	~UNDEFGate() {}
	void printGate(unsigned& lineNum) const;
};

class PIGate : public CirGate
{
public:
	PIGate(unsigned line, unsigned id )
		:CirGate(line, PI_GATE, id) {}
	~PIGate() {}
	void printGate(unsigned& lineNum) const;
};

class POGate : public CirGate
{
public:
	POGate(unsigned line, unsigned id, unsigned ip1)
		:CirGate(line, PO_GATE, id, ip1) {}
	~POGate() {}
	void printGate(unsigned& lineNum) const;
};

class AIGGate : public CirGate
{
public:
	AIGGate(unsigned line, unsigned id, unsigned ip1, unsigned ip2 )
		:CirGate(line, AIG_GATE, id, ip1, ip2) {}
	~AIGGate() {}
	void printGate(unsigned& lineNum) const;
};

class CONSTGate : public CirGate
{
public:
	CONSTGate()
		:CirGate(0, CONST_GATE, 0) {}
	~CONSTGate() {}
	void printGate(unsigned& lineNum) const;	
=======
				return	"CONST";			
>>>>>>> 6883550f17dca316a0f50d4d96282c5dcdc0d1e5
		}
		return "";
	}
	
	void setSymbol(const string& str){ symbol = str; }
	void setFanout(const IdList& Fanout) { _fanout = Fanout; }
	void addFanout(unsigned Id) { _fanout.push_back(Id); }
	
   unsigned getLineNo() const { return line; }
   GateType getType() 	const { return gateType; }
   unsigned getFanin1() const { return _fanin1; }
   unsigned getFanin2() const { return _fanin2; }
   unsigned getId() 		const { return gateId; }
   string 	getSymbol() const { return symbol; }
   IdList	getFanout() const { return _fanout; }
   
   // Printing functions
   //pure virtual function, dereived class should implement it 
   virtual void printGate(unsigned& lineNum) const = 0;
   virtual void DFS(ostringstream& oss, unsigned& A) const {}
   void reportGate() const;
   void reportFanin(int level) const; 
   void reportFanout(int level) const;
   void printSymbol() const { if(symbol != "") cout<<" ("<<symbol<<")"; cout<<"\n"; }
   
   //for DFS
	static void setGlobalRef() 	{ ++ _globalRef; }
	void setToGlobalRef() const	{ _ref = _globalRef; }
	bool isGlobalRef() const 		{ return ( _ref == _globalRef); }
	
protected:
	unsigned							line;					//which line define this gate
	GateType	 						gateType;
	unsigned							gateId;				//variable ID i.e.( >>1 = /2 )
   unsigned							_fanin1;				//literal ID 
   unsigned							_fanin2;				//literal ID 
   string 							symbol; 
   IdList							_fanout;				//if inverted to o/p gate's input,store o/p gate's variableId*2+1
   static unsigned 				_globalRef;
   mutable unsigned				_ref;

	void printFanin (const int& totallevel, int nowlevel, bool inverted = false) const ;
	void printFanout (const int& totallevel, int nowlevel, bool inverted = false) const ; 
};
/*
class CirGateV
{
	#define NEG 0x1
	CirGateV(AIGGate* g, size_t phase):_gateV(size_t(g) + phase) { }
	CirGateV(size_t ref):_gateV(ref){}
	CirGate* gate() const { return (AIGGate*)(_gateV & ~size_t(NEG)); }
	bool isInv() const { return (_gateV & NEG); }
	size_t		_gateV;
};
*/
class UNDEFGate : public CirGate
{
public:
	UNDEFGate(unsigned id)
		:CirGate(0, UNDEF_GATE, id) {}
	~UNDEFGate() {}
	void printGate(unsigned& lineNum) const {}
};

class PIGate : public CirGate
{
public:
	PIGate(unsigned line, unsigned id )
		:CirGate(line, PI_GATE, id) {}
	~PIGate() {}
	void printGate(unsigned& lineNum) const ;
};

class POGate : public CirGate
{
public:
	POGate(unsigned line, unsigned id, unsigned ip1)
		:CirGate(line, PO_GATE, id, ip1) {}
	~POGate() {}
	void printGate(unsigned& lineNum) const ;
	void DFS(ostringstream& oss, unsigned& A) const ;
};

class AIGGate : public CirGate
{
public:
	AIGGate(unsigned line, unsigned id, unsigned ip1, unsigned ip2 )
		:CirGate(line, AIG_GATE, id, ip1, ip2) {}
	~AIGGate() {}
	void printGate(unsigned& lineNum) const ;
	void DFS(ostringstream& oss, unsigned& A) const ;
};

class CONSTGate : public CirGate
{
public:
<<<<<<< HEAD
	CONSTGate()
		:CirGate(0, CONST_GATE, 0) {}
	~CONSTGate() {}
	void printGate(unsigned& lineNum) const ;	
=======
	void printGate() const;	
>>>>>>> 393d561f9dd6f43be0bea6c73b1b03b182a01d13
>>>>>>> 6883550f17dca316a0f50d4d96282c5dcdc0d1e5
};

#endif // CIR_GATE_H
