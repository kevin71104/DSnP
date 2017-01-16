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
	friend class CirMgr;
public:
	 CirGate(unsigned lineNum, GateType name, unsigned ID)
		: line(lineNum), gateId(ID), gateType(name), dfsId(UINT_MAX){}
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
	virtual void reportGate() const=0;
	virtual void printGate() const = 0;
	virtual void printFanin (const int& totallevel, int nowlevel, bool inverted = false) const=0;
	virtual void printFanout (const int& totallevel, int nowlevel, bool inverted = false) const=0;
	virtual void DfsBuild(unsigned fanoutId, bool rebuild)=0;

	//virtual function
	virtual bool isAig() const { return false; }
	virtual void DFS(ostringstream& oss, unsigned& A) const{
		if(isGlobalRef()) return;
		setToGlobalRef();
	}
	virtual void 		reconnect(CirGate* replace, bool int_or_not){}
	virtual void 		printSymbol() const{}
	virtual void 		setFanin1(unsigned Id){}
	virtual void 		setFanin2(unsigned Id){}
	virtual void 		setSymbol(const string& str){}
	virtual void 		setFanout(const IdList& Fanout){}
	virtual void 		addFanout(unsigned Id){}
	virtual	void 		clearFanout(){}
	//virtual void 		removeFanout(unsigned Id){}
	virtual IdList		getFanout() const{return IdList();}
	virtual string		getSymbol() const{return "";}
	virtual unsigned	getFanin1() const{return 0;}
	virtual unsigned	getFanin2() const{return 0;}
	virtual unsigned  getFanoutSize() const{}

	//getting functions
	unsigned 	getDfsId()	const { return dfsId; }
	unsigned	getId()     const { return gateId; }
	GateType	getType()   const { return gateType; }
	unsigned	getLineNo() const { return line; }

	// Printing functions
	void 	reportFanin(int level) const ;
	void 	reportFanout(int level) const ;

	//for DFS
	bool 		isGlobalRef() const     { return ( _ref == _globalRef); }
	void 		setToGlobalRef() const  { _ref = _globalRef; }
	static void setGlobalRef()   		{ ++ _globalRef; }

	void 		setDfsId(unsigned Id) { dfsId = Id; }

protected:
	unsigned			line;       	//which line define this gate
	unsigned 			dfsId;
	unsigned			gateId;       	//variable ID i.e.( >>1 = /2 )
	GateType			gateType;
	static unsigned		_globalRef;
	mutable unsigned	_ref;			//can be altered in const functions

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

	//printing functions
	void 	printGate() const {}
	void 	printFanin (const int& totallevel, int nowlevel, bool inverted = false) const;
	void 	printFanout (const int& totallevel, int nowlevel, bool inverted = false) const;
	void 	reportGate() const;

	//getting functions
	IdList	  getFanout() const { return _fanout; }
	unsigned  getFanoutSize() const{ return _fanout.size();}

	//setting functions
	void 	setFanout(const IdList& Fanout) { _fanout = Fanout; }
	void 	addFanout(unsigned Id) { _fanout.push_back(Id); }
	void  clearFanout(){ IdList temp; _fanout.swap(temp); }
	void 	DfsBuild(unsigned fanoutId, bool rebuild);

private:
	IdList	_fanout;	//if inverted to o/p gate's input,store o/p gate's variableId*2+1
};

class PIGate : public CirGate
{
public:
	PIGate(unsigned line, unsigned id )
		:CirGate(line, PI_GATE, id) {}
	~PIGate() {}

	//printing functions
	void 	reportGate() const;
	void 	printGate() const ;
	void 	printSymbol() const { if(symbol != "") cout<<" ("<<symbol<<")"; cout<<"\n"; }
	void 	printFanin (const int& totallevel, int nowlevel, bool inverted = false) const;
	void 	printFanout (const int& totallevel, int nowlevel, bool inverted = false) const;

	//getting functions
	string		getSymbol() const { return symbol; }
	IdList		getFanout() const { return _fanout; }
	unsigned  getFanoutSize() const{ return _fanout.size();}

	//setting functions
	void 	setSymbol(const string& str){ symbol = str; }
	void 	setFanout(const IdList& Fanout) { _fanout = Fanout; }
	void 	addFanout(unsigned Id) { _fanout.push_back(Id); }
	void  clearFanout(){ IdList temp; _fanout.swap(temp); }
	void 	DfsBuild(unsigned fanoutId, bool rebuild);

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

	//printing functions
	void 	reportGate() const;
	void 	printGate() const ;
	void 	printSymbol() const { if(symbol != "") cout<<" ("<<symbol<<")"; cout<<"\n"; }
	void 	printFanin (const int& totallevel, int nowlevel, bool inverted = false) const;
	void 	printFanout (const int& totallevel, int nowlevel, bool inverted = false) const;

	//getting functions
	void 		DFS(ostringstream& oss, unsigned& A) const ;
	string		getSymbol() const { return symbol; }
	unsigned	getFanin1() const { return _fanin1; }

	//setting functions
	void 	setSymbol(const string& str){ symbol = str; }
	void 	DfsBuild(unsigned fanoutId, bool rebuild);
	void 	setFanin1(unsigned Id){_fanin1 = Id;}

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

	//printing functions
	void 	reportGate() const;
	void 	printGate() const ;
	void 	printFanin (const int& totallevel, int nowlevel, bool inverted = false) const;
	void 	printFanout (const int& totallevel, int nowlevel, bool inverted = false) const;

	//getting functions
	bool 		isAig() const { return true; }
	void 		DFS(ostringstream& oss, unsigned& A) const ;
	IdList		getFanout() const { return _fanout; }
	unsigned	getFanin1() const { return _fanin1; }
	unsigned	getFanin2() const { return _fanin2; }
	unsigned  getFanoutSize() const{ return _fanout.size();}

	//setting functions
	void 	setFanout(const IdList& Fanout) { _fanout = Fanout; }
	void 	addFanout(unsigned Id) { _fanout.push_back(Id); }
	void  clearFanout(){ IdList temp; _fanout.swap(temp); }
	void 	DfsBuild(unsigned fanoutId, bool rebuild);
	void 	reconnect(CirGate* replace, bool int_or_not);
	void 	setFanin1(unsigned Id){ _fanin1 = Id;}
	void 	setFanin2(unsigned Id){ _fanin2 = Id;}
	/*void 	removeFanout(unsigned Id){
		for(unsigned i=0; i< _fanout.size(); i++)

	}*/

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

	//printing functions
	void 	reportGate() const;
	void 	printGate() const ;
	void 	printFanin (const int& totallevel, int nowlevel, bool inverted = false) const;
	void 	printFanout (const int& totallevel, int nowlevel, bool inverted = false) const;

	//getting functions
	IdList		getFanout() const { return _fanout; }
	unsigned  getFanoutSize() const{ return _fanout.size();}

	//setting functions
	void 	setFanout(const IdList& Fanout) { _fanout = Fanout; }
	void 	addFanout(unsigned Id) { _fanout.push_back(Id); }
	void  clearFanout(){ IdList temp; _fanout.swap(temp); }
	void 	DfsBuild(unsigned fanoutId, bool rebuild);

private:
	IdList		_fanout;	//if inverted to o/p gate's input,store o/p gate's variableId*2+1
};

#endif // CIR_GATE_H
