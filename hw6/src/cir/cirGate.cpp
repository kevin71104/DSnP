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
GateList CirMgr :: _GateList;


// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
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

void
CirGate::printFanin(const int& totallevel, int nowlevel, bool inverted) const
{	
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId ;
	// UNDEF_GATE ||  PI_GATE || CONST_GATE no fanin
	if(nowlevel == totallevel || gateType == UNDEF_GATE || gateType == PI_GATE || gateType == CONST_GATE){
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
	
	if( gateType == PO_GATE){
		//literalId
		CirGate* fGate1 = cirMgr->_GateList[_fanin1>>1];
		fGate1->printFanin(totallevel, nowlevel+1, _fanin1%2);
	}
	else if( gateType == AIG_GATE){
		CirGate* fGate1 = cirMgr->_GateList[_fanin1>>1];
		CirGate* fGate2 = cirMgr->_GateList[_fanin2>>1];
		fGate1->printFanin(totallevel,nowlevel+1, _fanin1%2);
		fGate2->printFanin(totallevel,nowlevel+1, _fanin2%2);		
	}
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

void 
CirGate::printFanout(const int& totallevel, int nowlevel, bool inverted) const 
{
	if(nowlevel != 0)
		cout<<setw(2*nowlevel)<<' ';
	if(inverted)
		cout<<'!';
	cout << this->getTypeStr() << " " << gateId ;
	// PO_GATE no fanout
	if(nowlevel == totallevel || gateType == PO_GATE){
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
PIGate::printGate(unsigned &lineNum) const
{
	if(isGlobalRef()) return;
	setToGlobalRef();
	cout<<"["<<lineNum<<"] PI  "<<gateId;
	printSymbol();
	lineNum++;
}

void
POGate::printGate(unsigned &lineNum) const
{
	if(isGlobalRef()) return;
	setToGlobalRef();
	unsigned variableId = _fanin1>>1;
	CirGate* faninGate = cirMgr->_GateList[variableId];
	faninGate->printGate(lineNum);
	
	cout<<"["<<lineNum<<"] PO  "<<gateId<<" ";
	if(faninGate->getType() == UNDEF_GATE) cout<<"*";
	if(_fanin1 % 2) cout<<"!";
	cout<<faninGate->getId();
	printSymbol();
	lineNum++;
}

void
AIGGate::printGate(unsigned &lineNum) const
{
	if(isGlobalRef()) return;
	setToGlobalRef();
	unsigned fId1 = _fanin1>>1;
	CirGate* fGate1 = cirMgr->_GateList[fId1];
	unsigned fId2 = _fanin2>>1;
	CirGate* fGate2 = cirMgr->_GateList[fId2];
	fGate1->printGate(lineNum);
	fGate2->printGate(lineNum);
	
	cout<<"["<<lineNum<<"] AIG "<<gateId<<" ";
	if(fGate1->getType() == UNDEF_GATE) cout<<"*";
	if(_fanin1 % 2) cout<<"!";
	cout<<fGate1->getId()<<" ";
	if(fGate2->getType() == UNDEF_GATE) cout<<"*";
	if(_fanin2 % 2) cout<<"!";
	cout<<fGate2->getId()<<"\n";
	lineNum++;
}

void
CONSTGate::printGate(unsigned &lineNum) const
{
	if(isGlobalRef()) return;
	setToGlobalRef();
	cout<<"["<<lineNum<<"] CONST0\n";
	lineNum++;
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
