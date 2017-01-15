/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;
unsigned CirGate::	_globalRef = 0;
GateList CirMgr::_GateList;
extern  GateList _DfsList;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/

void
UNDEFGate::reportGate() const
{
	cout << setfill('=') << setw(51) << '\n';
	ostringstream oss;
	oss << "= " << this->getTypeStr() << "(" << gateId << ")";
	oss << ", line " << line;
	cout << setfill(' ') << setw(49) << left << oss.str() << "=\n";
	cout << setfill('=') << setw(51) << right << '\n' << setfill(' ');
}

void
PIGate::reportGate() const
{
	cout << setfill('=') << setw(51) << '\n';
	ostringstream oss;
	oss << "= " << this->getTypeStr() << "(" << gateId << ")";
	if(symbol != "") oss << "\"" << symbol << "\"";
	oss << ", line " << line;
	cout << setfill(' ') << setw(49) << left << oss.str() << "=\n";
	cout << setfill('=') << setw(51) << right << '\n' << setfill(' ');
}

void
POGate::reportGate() const
{
	cout << setfill('=') << setw(51) << '\n';
	ostringstream oss;
	oss << "= " << this->getTypeStr() << "(" << gateId << ")";
	if(symbol != "") oss << "\"" << symbol << "\"";
	oss << ", line " << line;
	cout << setfill(' ') << setw(49) << left << oss.str() << "=\n";
	cout << setfill('=') << setw(51) << right << '\n' << setfill(' ');
}

void
AIGGate::reportGate() const
{
	cout << setfill('=') << setw(51) << '\n';
	ostringstream oss;
	oss << "= " << this->getTypeStr() << "(" << gateId << ")";
	oss << ", line " << line;
	cout << setfill(' ') << setw(49) << left << oss.str() << "=\n";
	cout << setfill('=') << setw(51) << right << '\n' << setfill(' ');
}

void
CONSTGate::reportGate() const
{
	cout << setfill('=') << setw(51) << '\n';
	ostringstream oss;
	oss << "= " << this->getTypeStr() << "(" << gateId << ")";
	oss << ", line " << line;
	cout << setfill(' ') << setw(49) << left << oss.str() << "=\n";
	cout << setfill('=') << setw(51) << right << '\n' << setfill(' ');
}

void
CirGate::reportFanin(int level) const
{
	assert (level >= 0);
	for(size_t i=0 ; i< cirMgr->_GateList.size(); i++)
		(cirMgr->_GateList[i])->setToGlobalRef();
	CirGate::setGlobalRef();
	int Level = level;
	//by iterative calling printFanin
	printFanin(Level,0);
}

// UNDEF_GATE ||  PI_GATE || CONST_GATE no fanin
void
UNDEFGate::printFanin(const int& totallevel, int nowlevel, bool inverted) const
{
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId << '\n';
}

void
PIGate::printFanin(const int& totallevel, int nowlevel, bool inverted) const
{
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId << '\n';
}

void
CONSTGate::printFanin(const int& totallevel, int nowlevel, bool inverted) const
{
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId << '\n';
}

void
POGate::printFanin(const int& totallevel, int nowlevel, bool inverted) const
{
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId ;
	if(nowlevel == totallevel){
		cout << "\n";
		return;
	}
	//if has printed this gate's fanin(s) and have to print cout << (*)
	if(isGlobalRef()){
		cout << " (*)\n";
		return;
	}
	cout << "\n";
	this->setToGlobalRef();
	//literalId
	CirGate* fGate1 = cirMgr->_GateList[_fanin1>>1];
	fGate1->printFanin(totallevel, nowlevel+1, _fanin1%2);
}

void
AIGGate::printFanin(const int& totallevel, int nowlevel, bool inverted) const
{
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId ;
	if(nowlevel == totallevel){
		cout << "\n";
		return;
	}
	//if has printed this gate's fanin(s) and have to print cout << (*)
	if(isGlobalRef()){
		cout << " (*)\n";
		return;
	}
	cout << "\n";
	this->setToGlobalRef();

	CirGate* fGate1 = cirMgr->_GateList[_fanin1>>1];
	CirGate* fGate2 = cirMgr->_GateList[_fanin2>>1];
	fGate1->printFanin(totallevel,nowlevel+1, _fanin1%2);
	fGate2->printFanin(totallevel,nowlevel+1, _fanin2%2);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   for(size_t i=0 ; i< cirMgr->_GateList.size(); i++)
		(cirMgr->_GateList[i])->setToGlobalRef();
	CirGate::setGlobalRef();
	int Level = level;
	//by iterative calling printFanout
	printFanout(Level,0);
}

//POGate no fanout
void
POGate::printFanout(const int& totallevel, int nowlevel, bool inverted) const
{
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId << '\n';
}

void
PIGate::printFanout(const int& totallevel, int nowlevel, bool inverted) const
{
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId ;
	if(nowlevel == totallevel){
		cout << "\n";
		return;
	}
	//if has printed this gate's fanin(s) and this gate have fanins cout << (*)
	if(isGlobalRef() && !_fanout.empty()){
		cout << " (*)\n";
		return;
	}
	cout << "\n";
	this->setToGlobalRef();

	for(size_t i=0; i<_fanout.size(); i++){
		unsigned opId = _fanout[i]>>1 ;
		CirGate* opGate = cirMgr->_GateList[opId];
		opGate->printFanout(totallevel, nowlevel+1, _fanout[i]%2);
	}
}

void
UNDEFGate::printFanout(const int& totallevel, int nowlevel, bool inverted) const
{
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId ;
	if(nowlevel == totallevel){
		cout << "\n";
		return;
	}
	//if has printed this gate's fanin(s) and this gate have fanins cout << (*)
	if(isGlobalRef() && !_fanout.empty()){
		cout << " (*)\n";
		return;
	}
	cout << "\n";
	this->setToGlobalRef();

	for(size_t i=0; i<_fanout.size(); i++){
		unsigned opId = _fanout[i]>>1 ;
		CirGate* opGate = cirMgr->_GateList[opId];
		opGate->printFanout(totallevel, nowlevel+1, _fanout[i]%2);
	}
}

void
AIGGate::printFanout(const int& totallevel, int nowlevel, bool inverted) const
{
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId ;
	if(nowlevel == totallevel){
		cout << "\n";
		return;
	}
	//if has printed this gate's fanin(s) and this gate have fanins cout << (*)
	if(isGlobalRef() && !_fanout.empty()){
		cout << " (*)\n";
		return;
	}
	cout << "\n";
	this->setToGlobalRef();

	for(size_t i=0; i<_fanout.size(); i++){
		unsigned opId = _fanout[i]>>1 ;
		CirGate* opGate = cirMgr->_GateList[opId];
		opGate->printFanout(totallevel, nowlevel+1, _fanout[i]%2);
	}
}

void
CONSTGate::printFanout(const int& totallevel, int nowlevel, bool inverted) const
{
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId ;
	if(nowlevel == totallevel){
		cout << "\n";
		return;
	}
	//if has printed this gate's fanin(s) and this gate have fanins cout << (*)
	if(isGlobalRef() && !_fanout.empty()){
		cout << " (*)\n";
		return;
	}
	cout << "\n";
	this->setToGlobalRef();

	for(size_t i=0; i<_fanout.size(); i++){
		unsigned opId = _fanout[i]>>1 ;
		CirGate* opGate = cirMgr->_GateList[opId];
		opGate->printFanout(totallevel, nowlevel+1, _fanout[i]%2);
	}
}

void
PIGate::printGate() const
{
	cout<<"PI  "<<gateId;
	printSymbol();
}

void
POGate::printGate() const
{
	cout<<"PO  "<<gateId<<" ";
	unsigned fId1 = _fanin1>>1;
	CirGate* fGate1 = cirMgr->_GateList[fId1];
	if(fGate1->getType() == UNDEF_GATE) cout<<"*";
	if(_fanin1 % 2) cout<<"!";
	cout<<fGate1->getId();
	printSymbol();
}

void
AIGGate::printGate() const
{
	cout<<"AIG "<<gateId<<" ";
	unsigned fId1 = _fanin1>>1;
	CirGate* fGate1 = cirMgr->_GateList[fId1];
	unsigned fId2 = _fanin2>>1;
	CirGate* fGate2 = cirMgr->_GateList[fId2];
	if(fGate1->getType() == UNDEF_GATE) cout<<"*";
	if(_fanin1 % 2) cout<<"!";
	cout<<fGate1->getId()<<" ";
	if(fGate2->getType() == UNDEF_GATE) cout<<"*";
	if(_fanin2 % 2) cout<<"!";
	cout<<fGate2->getId()<<"\n";
}

void
CONSTGate::printGate() const
{
	cout<<"CONST0\n";
}


void
POGate::DFS(ostringstream& oss, unsigned& A) const
{
	if(isGlobalRef()) return;
	setToGlobalRef();
	unsigned fId1 = _fanin1>>1;
	CirGate* fGate1 = cirMgr->_GateList[fId1];
	fGate1->DFS(oss,A);
}

void
AIGGate::DFS(ostringstream& oss, unsigned& A) const
{
	if(isGlobalRef()) return;
	setToGlobalRef();
	unsigned fId1 = _fanin1>>1;
	CirGate* fGate1 = cirMgr->_GateList[fId1];
	unsigned fId2 = _fanin2>>1;
	CirGate* fGate2 = cirMgr->_GateList[fId2];
	fGate1->DFS(oss,A);
	fGate2->DFS(oss,A);
	A++;
	oss << 2*gateId << ' ' << _fanin1 << ' ' << _fanin2 << '\n';
}

//if inverted to o/p gate's input,fanoutId = o/p gate's variableId*2+1
void
POGate::DfsBuild(unsigned fanoutId, bool rebuild)
{
	setToGlobalRef();
	unsigned fId1 = _fanin1>>1;
	CirGate* fGate1 = cirMgr->_GateList[fId1];
	unsigned fout = gateId*2 + (_fanin1%2);
	fGate1->DfsBuild(fout,rebuild);
	_DfsList.push_back(this);
}

void
PIGate::DfsBuild(unsigned fanoutId, bool rebuild)
{
	if(rebuild){
		_fanout.push_back(fanoutId);
	}
	if(isGlobalRef()) return;
	setToGlobalRef();
	_DfsList.push_back(this);
}

void
AIGGate::DfsBuild(unsigned fanoutId, bool rebuild)
{
	if(rebuild)
		_fanout.push_back(fanoutId);
	if(isGlobalRef()) return;
	setToGlobalRef();
	unsigned fId1 = _fanin1>>1;
	CirGate* fGate1 = cirMgr->_GateList[fId1];
	unsigned fId2 = _fanin2>>1;
	CirGate* fGate2 = cirMgr->_GateList[fId2];
	unsigned fout1 = gateId*2 + (_fanin1%2);
	unsigned fout2 = gateId*2 + (_fanin2%2);
	fGate1->DfsBuild(fout1,rebuild);
	fGate2->DfsBuild(fout2,rebuild);
	_DfsList.push_back(this);
}

void
CONSTGate::DfsBuild(unsigned fanoutId, bool rebuild)
{
	if(rebuild)
		_fanout.push_back(fanoutId);
	if(isGlobalRef()) return;
	setToGlobalRef();
	_DfsList.push_back(this);
}

void
UNDEFGate::DfsBuild(unsigned fanoutId, bool rebuild)
{
	if(rebuild)
		_fanout.push_back(fanoutId);
	if(isGlobalRef()) return;
	setToGlobalRef();
	_DfsList.push_back(this);
}
