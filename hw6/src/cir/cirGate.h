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

//class CirGateV;


//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
	//friend class CirMgr;
public:
	 CirGate(unsigned lineNum, GateType name, unsigned ID)
		: line(lineNum), gateType(name), gateId(ID){}
	 virtual ~CirGate() {}

	 // Basic access methods
	 string getTypeStr() const {
		switch(gateType){
			case  UNDEF_GATE:
				return  "UNDEF";
			case  PI_GATE:
				return  "PI";
			case  PO_GATE:
				return  "PO";
			case  AIG_GATE:
				return  "AIG";
			case  CONST_GATE:
				return  "CONST";
			case TOT_GATE:
				return "";
		}
		return "";
	}

	//pure virtual function, dereived class should implement it
	virtual void printGate(unsigned& lineNum) const = 0;
	virtual void reportGate() const=0;
	virtual void printFanin (const int& totallevel, int nowlevel, bool inverted = false) const=0;
	virtual void printFanout (const int& totallevel, int nowlevel, bool inverted = false) const=0;

	//virtual function
	virtual void 		DFS(ostringstream& oss, unsigned& A) const{}
	virtual void 		printSymbol() const{}
	virtual unsigned	getFanin1() const{return 0;}
	virtual unsigned	getFanin2() const{return 0;}
	virtual string		getSymbol() const{return "";}
	virtual IdList		getFanout() const{return IdList();}
	virtual void 		setSymbol(const string& str){}
	virtual void 		setFanout(const IdList& Fanout){}
	virtual void 		addFanout(unsigned Id){}

	//getting functions
	unsigned	getLineNo() const { return line; }
	GateType	getType()   const { return gateType; }
	unsigned	getId()     const { return gateId; }

	// Printing functions
	void reportFanin(int level) const ;
	void reportFanout(int level) const ;

	//for DFS
	static void setGlobalRef()   { ++ _globalRef; }
	void setToGlobalRef() const  { _ref = _globalRef; }
	bool isGlobalRef() const     { return ( _ref == _globalRef); }

protected:
	unsigned              	line;       	//which line define this gate
	GateType               	gateType;
	unsigned              	gateId;       	//variable ID i.e.( >>1 = /2 )
	static unsigned         _globalRef;
	mutable unsigned        _ref;

};

/*
class CirGateV
{
	#define NEG 0x1
	CirGateV(AIGGate* g, size_t phase):_gateV(size_t(g) + phase) { }
	CirGateV(size_t ref):_gateV(ref){}
	CirGate* gate() const { return (AIGGate*)(_gateV & ~size_t(NEG)); }
	bool isInv() const { return (_gateV & NEG); }
	size_t    _gateV;
};
*/

class UNDEFGate : public CirGate
{
public:
	UNDEFGate(unsigned id)
		:CirGate(0, UNDEF_GATE, id) {}
	~UNDEFGate() {}
	void 	printGate(unsigned& lineNum) const {}
	IdList	getFanout() const { return _fanout; }
	void 	setFanout(const IdList& Fanout) { _fanout = Fanout; }
	void 	addFanout(unsigned Id) { _fanout.push_back(Id); }
	void 	reportGate() const;
	void 	printFanin (const int& totallevel, int nowlevel, bool inverted = false) const;
	void 	printFanout (const int& totallevel, int nowlevel, bool inverted = false) const;
private:
	IdList	_fanout;	//if inverted to o/p gate's input,store o/p gate's variableId*2+1
};

class PIGate : public CirGate
{
public:
	PIGate(unsigned line, unsigned id )
		:CirGate(line, PI_GATE, id) {}
	~PIGate() {}
	void 	printGate(unsigned& lineNum) const ;
	void 	printSymbol() const { if(symbol != "") cout<<" ("<<symbol<<")"; cout<<"\n"; }
	string	getSymbol() const { return symbol; }
	IdList	getFanout() const { return _fanout; }
	void 	setSymbol(const string& str){ symbol = str; }
	void 	setFanout(const IdList& Fanout) { _fanout = Fanout; }
	void 	addFanout(unsigned Id) { _fanout.push_back(Id); }
	void 	reportGate() const;
	void 	printFanin (const int& totallevel, int nowlevel, bool inverted = false) const;
	void 	printFanout (const int& totallevel, int nowlevel, bool inverted = false) const;
private:
	IdList	_fanout;	//if inverted to o/p gate's input,store o/p gate's variableId*2+1
	string	symbol;
};

class POGate : public CirGate
{
public:
	POGate(unsigned line, unsigned id, unsigned ip1)
		:CirGate(line, PO_GATE, id), _fanin1(ip1) {}
	~POGate() {}
	void 		printGate(unsigned& lineNum) const ;
	void 		DFS(ostringstream& oss, unsigned& A) const ;
	void 		printSymbol() const { if(symbol != "") cout<<" ("<<symbol<<")"; cout<<"\n"; }
	unsigned	getFanin1() const { return _fanin1; }
	string		getSymbol() const { return symbol; }
	void 		setSymbol(const string& str){ symbol = str; }
	void 		reportGate() const;
	void 		printFanin (const int& totallevel, int nowlevel, bool inverted = false) const;
	void 		printFanout (const int& totallevel, int nowlevel, bool inverted = false) const;
private:
	string		symbol;
	unsigned	_fanin1;	//literal ID
};

class AIGGate : public CirGate
{
public:
	AIGGate(unsigned line, unsigned id, unsigned ip1, unsigned ip2 )
		:CirGate(line, AIG_GATE, id), _fanin1(ip1), _fanin2(ip2) {}
	~AIGGate() {}
	void 		printGate(unsigned& lineNum) const ;
	void 		DFS(ostringstream& oss, unsigned& A) const ;
	unsigned	getFanin1() const { return _fanin1; }
	unsigned	getFanin2() const { return _fanin2; }
	IdList		getFanout() const { return _fanout; }
	void 		setFanout(const IdList& Fanout) { _fanout = Fanout; }
	void 		addFanout(unsigned Id) { _fanout.push_back(Id); }
	void 		reportGate() const;
	void 		printFanin (const int& totallevel, int nowlevel, bool inverted = false) const;
	void 		printFanout (const int& totallevel, int nowlevel, bool inverted = false) const;
private:
	IdList		_fanout;	//if inverted to o/p gate's input,store o/p gate's variableId*2+1
	unsigned	_fanin1;    //literal ID
	unsigned	_fanin2;    //literal ID
};

class CONSTGate : public CirGate
{
public:
	CONSTGate()
		:CirGate(0, CONST_GATE, 0) {}
	~CONSTGate() {}
	void 	printGate(unsigned& lineNum) const ;
	IdList	getFanout() const { return _fanout; }
	void 	setFanout(const IdList& Fanout) { _fanout = Fanout; }
	void 	addFanout(unsigned Id) { _fanout.push_back(Id); }
	void 	reportGate() const;
	void 	printFanin (const int& totallevel, int nowlevel, bool inverted = false) const;
	void 	printFanout (const int& totallevel, int nowlevel, bool inverted = false) const;
private:
	IdList		_fanout;	//if inverted to o/p gate's input,store o/p gate's variableId*2+1
};

#endif // CIR_GATE_H
