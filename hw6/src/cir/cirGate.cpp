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
<<<<<<< HEAD
unsigned CirGate::	_globalRef = 0;
GateList CirMgr:: 	_GateList;
=======
cirGate::	_globalRef = 0;
cirMgr:: 	_GateList;
>>>>>>> 393d561f9dd6f43be0bea6c73b1b03b182a01d13

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
}

