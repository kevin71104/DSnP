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

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
   friend class CirMgr;

public:
   CirGate(unsigned ln, GateType gt, unsigned Id, unsigned fin1 = UINT_MAX, unsigned fin2 = UINT_MAX)
      : line(ln), gtype(gt), id(Id), fanin1(fin1), fanin2(fin2) {}
   virtual ~CirGate() {}

   bool operator< (const CirGate& cg) { return (id < cg.id); }
   void setSymbol(const string& str){ symbol = str; }
   void connect();

   // Basic access methods
   string getTypeStr() const {
      switch(gtype){
         case UNDEF_GATE:
            return "UNDEF";
         case PI_GATE:
            return "PI";
         case PO_GATE:
            return "PO";
         case AIG_GATE:
            return "AIG";
         case CONST_GATE:
            return "CONST";
         case TOT_GATE:
            return "";
      }
   }
   unsigned getLineNo() const { return line; }
   GateType getTypeGtype() const { return gtype; }
   unsigned getId() const { return id; }
   string getSymbol() const { return symbol; }

   // Printing functions
   virtual void printGate(unsigned &lineno) = 0;
   void reportGate() const;
   void reportFanin(int& level, bool n = 0);
   void reportFanout(int& level, unsigned prev = UINT_MAX, bool n = 0);

   unsigned fanin1;
   unsigned fanin2;
   IdList fanout;

protected:

   // Data Members
   unsigned line;
   GateType gtype;
   unsigned id;
   string symbol;
   static unsigned _globalRef;
   unsigned _ref;

   static void setGlobalRef() { ++_globalRef; }
   bool isGlobalRef() const { return (_ref == _globalRef); }
   void setToGlobalRef() { _ref = _globalRef; }
};


class CirPiGate: public CirGate
{
public:
   CirPiGate(unsigned ln, unsigned Id)
      : CirGate(ln, PI_GATE, Id) {}
   ~CirPiGate() {}

   void printGate(unsigned &lineno);
};


class CirPoGate: public CirGate
{
public:
   CirPoGate(unsigned ln, unsigned fin1, unsigned Id)
      : CirGate(ln, PO_GATE, Id, fin1) {}
   ~CirPoGate() {}

   void printGate(unsigned &lineno);
};


class CirAigGate: public CirGate
{
public:
   CirAigGate(unsigned ln, unsigned fin1, unsigned fin2, unsigned Id)
      : CirGate(ln, AIG_GATE, Id, fin1, fin2) {}
   ~CirAigGate() {}

   void printGate(unsigned &lineno);
};


class CirConstGate: public CirGate
{
public:
   CirConstGate()
      : CirGate(0, CONST_GATE, 0) {}
   ~CirConstGate() {}

   void printGate(unsigned &lineno);
};


class CirUndefGate: public CirGate
{
public:
   CirUndefGate(unsigned Id)
      : CirGate(0, UNDEF_GATE, Id) {}
   ~CirUndefGate() {}

   void printGate(unsigned &lineno) {}
};


#endif // CIR_GATE_H
