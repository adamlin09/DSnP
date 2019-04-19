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

class CirGate;
class AigGateV;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{  
   friend class CirMgr;
public:
   CirGate(int x, int y) : gateID(x), LineNo(y) { _ref = 0; }
   virtual ~CirGate() {}

   bool isGlobalRef(){ return (_ref == _globalRef); }
   void setToGlobalRef(){ _ref = _globalRef; }
   static void setGlobalRef() { _globalRef++; }
   void dfstraversal(GateList& dfsLsit);

   // Basic access methods
   string getTypeStr() const;
   unsigned getLineNo() const { return LineNo; }

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level);
   void reportFanout(int level);
   void printfanin(int num, int level, bool inv, GateList& reported);
   void printfanout(int num, int level, bool inv, GateList& reported);

private:

protected:
   vector<AigGateV> _faninList;
   vector<AigGateV> _fanoutList;
   size_t _ref;
   static size_t _globalRef;
   int gateID;
   GateType type;
   string sym;
   unsigned LineNo;
};

class const_0 : public CirGate
{
public:
   const_0(int x, int y = 0) : CirGate(x, y) { type = CONST_GATE; }
   void printGate() const;
};

class PI : public CirGate
{
public:
   PI(int x, int y) : CirGate(x, y) { type = PI_GATE; }
   void printGate() const;
};

class PO : public CirGate
{
public:
   PO(int x, int y) : CirGate(x, y) { type = PO_GATE; }
   void printGate() const;
};


class And : public CirGate
{
public:
   And(int x, int y) : CirGate(x, y) { type = AIG_GATE; }
   void printGate() const;
};

class UNDEF : public CirGate
{
public:
   UNDEF(int x, int y = 0) : CirGate(x, y) { type = UNDEF_GATE; }
   void printGate() const;
};

class AigGateV
{
   friend class cirMgr;
public:
   friend class cirMgr;
   //#define NEG 0x1;
   AigGateV(CirGate* g, size_t phase) : _gateV(size_t(g) + phase) { }
   CirGate* gate() const { return (CirGate*)(_gateV & ~size_t(0x1)); }
   bool isInv() const { return (_gateV & 0x1); }
private:
   size_t _gateV;
};

#endif // CIR_GATE_H
