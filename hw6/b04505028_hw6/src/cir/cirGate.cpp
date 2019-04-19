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
#include <algorithm>


using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
   printGate();
   GateList reported;
}

void
CirGate::reportFanin(int level)
{
   assert (level >= 0);
   GateList reported;
   printfanin(0, level, false, reported);
}

void
CirGate::printfanin(int num, int level, bool inv, GateList& reported)
{
   if (num <= level) {
      cout << setw(num*2) << "";
      if (inv) {
         cout << "!";
      }
      cout << getTypeStr() << " " << gateID;
      bool check = false;
      if (getTypeStr() == "AIG"){
         GateList::iterator pos = find(reported.begin(), reported.end(), this);
         if (pos != reported.end()) {
            cout << " (*)";
            check = true;
         }
         if (num != level) {
            reported.push_back(this);
         }
      }
      cout << endl;
      if (!check){
            if (!_faninList.empty()) {
            for (int i = 0; i < _faninList.size(); ++i) {
               if (_faninList[i].isInv()) {
                  _faninList[i].gate()->printfanin(num+1, level, true, reported);
               }
               else {
                  _faninList[i].gate()->printfanin(num+1, level, false, reported);           
               }
            }
         }
      }
   }
}

void
CirGate::reportFanout(int level)
{
   assert (level >= 0);
   GateList reported;
   printfanout(0, level, false, reported);
}

void
CirGate::printfanout(int num, int level, bool inv, GateList& reported)
{
   if (num <= level) {
      cout << setw(num*2) << "";
      if (inv) {
         cout << "!";
      }
      cout << getTypeStr() << " " << gateID;
      bool check = false;
      if (getTypeStr() == "AIG"){
         GateList::iterator pos = find(reported.begin(), reported.end(), this);
         if (pos != reported.end()) {
            cout << " (*)";
            check = true;
         }
         if (num != level) {
            reported.push_back(this);
         }
      }
      cout << endl;
      if (!check){
         if (!_fanoutList.empty()) {
            for (int i = 0; i < _fanoutList.size(); ++i) {
               if (_fanoutList[i].isInv()) {
                  _fanoutList[i].gate()->printfanout(num+1, level, true, reported);
               }
               else {
                  _fanoutList[i].gate()->printfanout(num+1, level, false, reported);           
               }
            }
         }
      }
   }
}

void
CirGate::dfstraversal(GateList& dfsList)
{
   int size = this->_faninList.size();
   for (int i = 0; i < size; ++i) {
      CirGate* g = this->_faninList[i].gate();
      if (g != 0){
         if (!g->isGlobalRef()) {
            g->setToGlobalRef();
            g->dfstraversal(dfsList);
         }
      }
   }
   dfsList.push_back(this);
}

string 
CirGate::getTypeStr() const
{
   switch (type) {
      case PI_GATE : return "PI";
      case PO_GATE : return "PO";
      case AIG_GATE : return "AIG";
      case UNDEF_GATE : return "UNDEF";
      case CONST_GATE : return "CONST0";
   }
}

void
const_0::printGate() const
{
   string temp = "= " + getTypeStr() + "(" + to_string(gateID) + "), line " + to_string(LineNo);
   cout << "==================================================" << endl;
   cout << setw(49) << left << temp;
   cout << "=" << endl;
   cout << "==================================================" << endl;
}

void
PI::printGate() const
{
   string temp;
   if (sym != ""){
      temp = "= " + getTypeStr() + "(" + to_string(gateID) + ")\"" + sym + "\", line " + to_string(LineNo);
   }
   else{
      temp = "= " + getTypeStr() + "(" + to_string(gateID) + "), line " + to_string(LineNo);
   }
   cout << "==================================================" << endl;
   cout << setw(49) << left << temp;
   cout << "=" << endl;
   cout << "==================================================" << endl;
}

void
PO::printGate() const
{
   string temp;
   if (sym != ""){
      temp = "= " + getTypeStr() + "(" + to_string(gateID) + ")\"" + sym + "\", line " + to_string(LineNo);
   }
   else{
      temp = "= " + getTypeStr() + "(" + to_string(gateID) + "), line " + to_string(LineNo);
   }
   cout << "==================================================" << endl;
   cout << setw(49) << left << temp;
   cout << "=" << endl;
   cout << "==================================================" << endl;
}

void
And::printGate() const
{
   string temp = "= " + getTypeStr() + "(" + to_string(gateID) + "), line " + to_string(LineNo);
   cout << "==================================================" << endl;
   cout << setw(49) << left << temp;
   cout << "=" << endl;
   cout << "==================================================" << endl;
}

void 
UNDEF::printGate() const
{
   string temp = "= " + getTypeStr() + "(" + to_string(gateID) + "), line " + to_string(LineNo);
   cout << "==================================================" << endl;
   cout << setw(49) << left << temp;
   cout << "=" << endl;
   cout << "==================================================" << endl;
}