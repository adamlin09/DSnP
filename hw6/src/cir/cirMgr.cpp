/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
size_t CirGate::_globalRef = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   int num = 1;
   ifstream fin(fileName);
   if (!fin) {
      cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
      return false;
   }
   string str, tok[6];
   size_t pos = 0, index = 0;
   getline(fin, str);
   if (str[0] == ' ') {
      lineNo = num - 1;
      return parseError(EXTRA_SPACE);
   }
   if (str[0] == '\t') {
      errInt = 9; lineNo = num - 1;
      return parseError(ILLEGAL_WSPACE);
   }
   while (pos != string::npos && index < 6) {
      pos = myStrGetTok(str, tok[index], pos);
      if (pos < str.size()-1) {
         if (str[pos+1] == ' ') {
            colNo = pos + 1; lineNo = num - 1;
            return parseError(EXTRA_SPACE);
         }
      }
      ++index;
   }
   if (tok[0] != "aag") {
      if (tok[0].substr(0,3) == "aag") {
         int temp;
         if (myStr2Int(tok[0].substr(3,tok[0].size()-3), temp)) {
            colNo = 3; lineNo = num - 1;
            return parseError(MISSING_SPACE);
         }
      }
      errMsg = tok[0]; lineNo = num - 1;
      return parseError(ILLEGAL_IDENTIFIER);
   }
   pos = 0; index = 0;
   int header[5]; colNo = 2;
   for (int i = 0; i < 5 ; ++i) {
      if (tok[i+1] == "") {
         colNo = colNo + 2; 
         switch (i) {
            case 0 : errMsg = "number of variables"; break;
            case 1 : errMsg = "number of PIs"; break;
            case 2 : errMsg = "number of latchs"; break;
            case 3 : errMsg = "number of POs"; break;
            case 4 : errMsg = "number of AIGs"; break;
         }
         return parseError(MISSING_NUM);
      }
      if (!myStr2Int(tok[i+1], header[i])) {
         size_t x = tok[i+1].find("\t", 0);
         if (x != string::npos) {
            string t = "\t";
            errInt = 9; colNo = colNo + 2 + x; lineNo = num - 1;
            return parseError(ILLEGAL_WSPACE);
         }
      }
      colNo = colNo + 1 + tok[i+1].size();
   }
   if (str[++colNo] == ' ') {
      lineNo = num - 1;
      return parseError(MISSING_NEWLINE);
   }
   if (header[0] < header[1] + header[4]) {
      errMsg = "Number of variables"; errInt = header[0]; lineNo = num - 1;
      return parseError(NUM_TOO_SMALL);
   }
   if (header[2] != 0) {
      errMsg = "latches"; lineNo = num - 1;
      return parseError(ILLEGAL_NUM);
   }
   _totalList.resize(header[0]+header[3]+1);
   _totalList[0] = new const_0(0);
   
   //PI
   for (int i = 0; i < header[1]; ++i) {
      int temp, pos, x; string s, a;
      if (!getline(fin, s)) {
         errMsg ="PI"; lineNo = num;
         return parseError(MISSING_DEF);
      }
      if (s == "") {
         lineNo = num; colNo = 0; errMsg = "PI literal ID";
         return parseError(MISSING_NUM);
      }
      if (!myStr2Int(s, temp)) {
         if (s[0] == ' ') {
            lineNo = num; colNo = 0;
            return parseError(EXTRA_SPACE);
         }
         pos = myStrGetTok(s, a);
         if (!myStr2Int(a, x)) {
            errMsg = "PI literal ID(" + a + ")"; lineNo = num;
            return parseError(ILLEGAL_NUM);
         }
         lineNo = num; colNo = pos;
         return parseError(MISSING_NEWLINE);
      }
      if (temp/2 == 0) {
         lineNo = num; colNo = 0; errInt = temp;
         return parseError(REDEF_CONST);
      }
      if (temp % 2 != 0) {
         lineNo = num; colNo = 0; errInt = temp; errMsg = "PI";
         return parseError(CANNOT_INVERTED);
      }
      if (_totalList[temp / 2] != 0) {
         lineNo = num; errInt = temp; errGate = _totalList[temp / 2];
         return parseError(REDEF_GATE);
      }
      if (temp / 2 > header[0]) {
         lineNo = num; colNo = 0; errInt = temp;
         return parseError(MAX_LIT_ID);
      }
      ++num;
      CirGate* in = new PI(temp / 2, num);
      _piList.push_back(in);
      _totalList[temp/2] = in;
   }
   //PO
   int polist[header[3]];
   for (int i = 0; i < header[3]; ++i) {
      CirGate* out; string s,a; int temp, x;
      if (!getline(fin, s)) {
         errMsg ="PO"; lineNo = num;
         return parseError(MISSING_DEF);
      }
      if (s == "") {
         lineNo = num; colNo = 0; errMsg = "PO literal ID";
         return parseError(MISSING_NUM);
      }
      if (!myStr2Int(s, temp)) {
         if (s[0] == ' ') {
            lineNo = num; colNo = 0;
            return parseError(EXTRA_SPACE);
         }
         pos = myStrGetTok(s, a);
         if (!myStr2Int(a, x)) {
            errMsg = "PO literal ID(" + a + ")"; lineNo = num;
            return parseError(ILLEGAL_NUM);
         }
         lineNo = num; colNo = pos;
         return parseError(MISSING_NEWLINE);
      }
      if (temp > header[0] * 2 + 1) {
         lineNo = num; colNo = 0; errInt = temp;
         return parseError(MAX_LIT_ID);
      }
      polist[i] = temp;
      ++num;
      out = new PO(header[0]+1+i, num);
      _poList.push_back(out);
      _totalList[header[0]+1+i] = out;
   }
   //Aig  &&  symbol
   vector<vector<int>> cir ;
   while (getline(fin, str)) {
      ++num; colNo = 0;
      if (str[0] == ' ') {
         lineNo = num - 1; colNo = 0;
         return parseError(EXTRA_SPACE);
      }
      if (str == "c")   break;
      string g[3]; int x = 0; colNo = 0;
      while(pos != string::npos) {
         pos = myStrGetTok(str, g[x], pos);   
         if (x == 2 && pos != string::npos) {
            lineNo = num - 1; colNo = pos;
            return parseError(MISSING_NEWLINE);
         }
         colNo = colNo + g[x].size() + 1;
         if (str[colNo] == ' ') {
            lineNo = num - 1;
            return parseError(EXTRA_SPACE);
         }
         ++x;
      }
      pos = 0;
      if (g[1] == "") {
         if (str[str.size()-1] != ' ') {
            lineNo = num - 1; colNo = colNo + str.size();
            return parseError(MISSING_SPACE);
         }
      }
      if (g[2] == "") {
         symbol.push_back(str);
      }
      else {
         int temp[3]; vector<int> t;
         for (int i = 0; i < 3; ++i){
            myStr2Int(g[i], temp[i]);
            t.push_back(temp[i]);
         }
         if (t[0] / 2 == 0) {
            lineNo = num - 1; colNo = 0; errInt = temp[0];
            return parseError(REDEF_CONST);
         }
         if (_totalList[t[0] / 2] != 0) {
            lineNo = num - 1; errInt = temp[0]; errGate = _totalList[temp[0] / 2];
            return parseError(REDEF_GATE);
         }
         if (temp[0] / 2 > header[0]) {
            lineNo = num - 1; colNo = 0; errInt = temp[0];
            return parseError(MAX_LIT_ID);
         }
         cir.push_back(t);
         CirGate *gate = new And(temp[0] / 2, num);
         _AigList.push_back(gate);
         _totalList[temp[0] / 2] = gate;
      }
   }
   comment.push_back(str);
   while (getline(fin, str)) {
      comment.push_back(str);
   }
   //AIG
   for (int i = 0; i < _AigList.size(); ++i) {
      CirGate *temp1 = getGate(cir[i][1] / 2);
      CirGate *temp2 = getGate(cir[i][2] / 2);
      if (temp1 == 0) {
         temp1 = new UNDEF(cir[i][1] / 2);
         _totalList[cir[i][1] / 2] = temp1;
      }
      if (temp2 == 0) {
         temp2 = new UNDEF(cir[i][2] / 2);
         _totalList[cir[i][2] / 2] = temp1;
      }
      if (cir[i][1] % 2 == 0) {
         AigGateV in(temp1, 0);
         AigGateV out(_AigList[i], 0);
         _AigList[i]->_faninList.push_back(in);
         temp1->_fanoutList.push_back(out);
      }
      else {
         AigGateV in(temp1, 1);
         AigGateV out(_AigList[i], 1);
         _AigList[i]->_faninList.push_back(in);
         temp1->_fanoutList.push_back(out);
      }
      if (cir[i][2] % 2 == 0) {
         AigGateV in(temp2, 0);
         AigGateV out(_AigList[i], 0);
         _AigList[i]->_faninList.push_back(in);
         temp2->_fanoutList.push_back(out);
      }
      else {
         AigGateV in(temp2, 1);
         AigGateV out(_AigList[i], 1);
         _AigList[i]->_faninList.push_back(in);
         temp2->_fanoutList.push_back(out);
      }
   }
   //PO
   for (int i = 0; i < header[3]; ++i) {
      CirGate* temp1 = getGate(polist[i]/2);
      if (temp1 == 0) {
            temp1 = new UNDEF(polist[1] / 2);
            _totalList[polist[1] / 2] = temp1;
      }
      if (polist[i] % 2 == 0) {   
         AigGateV temp2(_totalList[header[0]+1+i], 0);
         temp1->_fanoutList.push_back(temp2); 
      }
      else {
         AigGateV temp2(_totalList[header[0]+1+i], 1);
         temp1->_fanoutList.push_back(temp2);
      }
      if (polist[i] % 2 == 0) {   
         AigGateV temp2(temp1, 0);
         _totalList[header[0]+1+i]->_faninList.push_back(temp2); 
      }
      else {
         AigGateV temp2(temp1, 1);
         _totalList[header[0]+1+i]->_faninList.push_back(temp2); 
      }
   }
   //symbol
   for (int i = 0; i < symbol.size(); ++i) {
      int x; string temp, s;
      if (symbol[i][0] == 'i') {
         temp = symbol[i][1];
         if (myStr2Int(temp, x)){
            myStrGetTok(symbol[i], s, 2);
            _piList[x]->sym = s;
         }
         else {}
      }
      else if (symbol[i][0] == 'o') {
         temp = symbol[i][1];
         if (myStr2Int(temp, x)){
            myStrGetTok(symbol[i], s, 2);
            _poList[x]->sym = s;
         }
         else {}
      }
      else {

      } 
   }
   dfstraversal(_poList);
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << setw(7) << left << "  PI" << setw(9) << right << _piList.size() <<endl;
   cout << setw(7) << left << "  PO" << setw(9) << right << _poList.size() <<endl;
   cout << setw(7) << left << "  AIG" << setw(9) << right << _AigList.size() <<endl;
   cout << "------------------" << endl;
   cout << setw(7) << left << "  Total" << setw(9) << right << _piList.size()+_poList.size()+_AigList.size() <<endl;
}

void
CirMgr::printNetlist() const
{
   cout << endl;
   for (int i = 0; i < _dfsList.size(); ++i) {
      string t = _dfsList[i]->getTypeStr();
      int id = _dfsList[i]->gateID;
      string s = _dfsList[i]->sym;
      if (t == "CONST0"){
         cout << "[" << i << "] " << t <<endl;
      }
      else if (t == "AIG") {
         CirGate* in1; CirGate* in2;
         in1 = _dfsList[i]->_faninList[0].gate();
         in2 = _dfsList[i]->_faninList[1].gate();
         cout << "[" << i << "] " << setw(4) << left << t << id << " ";
         if (in1->getTypeStr() == "UNDEF") {
            cout << "*!";
         }
         else {
            if (_dfsList[i]->_faninList[0].isInv()) {
               cout << "!";
            }
         }
         cout << in1->gateID << " ";
         if (_totalList[in2->gateID] == 0) {
            cout << "*!";
         }
         else {
            if (_dfsList[i]->_faninList[1].isInv()) {
               cout << "!";
            }
         }
         cout << in2->gateID << endl;
      }
      else if (t == "PI"){
         cout << "[" << i << "] " << setw(4) << left << t << id << " ";
         if (s != "") {
            cout << "(" << s << ")";
         }
         cout << endl;
      }
      else if (t == "PO") {
         cout << "[" << i << "] " << setw(4) << left << t << id << " ";
         if (_dfsList[i]->_faninList[0].isInv()){
            cout << "!";
         }
         cout << _dfsList[i]->_faninList[0].gate()->gateID << " ";
         if (s != "") {
            cout << "(" << s << ")";
         }
         cout << endl;
      }
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (int i = 0; i < _piList.size(); ++i) {
      cout << " " << _piList[i]->gateID;
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (int i = 0; i < _poList.size(); ++i) {
      cout << " " << _poList[i]->gateID;
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   vector<int> fl, unused;
   //floating
   for (int i = 0; i < _AigList.size(); ++i) {
      CirGate* in1; CirGate* in2;
      in1 = _AigList[i]->_faninList[0].gate();
      in2 = _AigList[i]->_faninList[1].gate();
      if (in1->getTypeStr() == "UNDEF" || in2->getTypeStr() == "UNDEG") {
         fl.push_back(_AigList[i]->gateID);
      }
   }
   for (int i = 0; i < _poList.size(); ++i) {
      CirGate* in1;
      in1 = _poList[i]->_faninList[0].gate();
      if (in1->getTypeStr() == "UNDEF") {
         fl.push_back(_poList[i]->gateID);
      }
   }
   if (!fl.empty()) {
      quickSort(fl, 0, fl.size()-1);
      cout << "Gates with floating fanin(s):";
      for (int i = 0; i < fl.size(); ++i) {
         cout << " " << fl[i];
      }
      cout << endl;
   }
   //unused
   for (int i = 0; i < _AigList.size(); ++i) {
      if (_AigList[i]->_fanoutList.empty()) {
         unused.push_back(_AigList[i]->gateID);
      }
   }
   for (int i = 0; i < _piList.size(); ++i) {
      if (_piList[i]->_fanoutList.empty()) {
         unused.push_back(_piList[i]->gateID);
      }
   }
   if (!unused.empty()) {
      quickSort(unused, 0, unused.size()-1);
      cout << "Gates defined but not used  :";
      for (int i = 0; i < unused.size(); ++i) {
         cout << " " << unused[i];
      }
      cout << endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   GateList aig;
   for (int i = 0; i < _dfsList.size(); ++i) {
      if (_dfsList[i]->getTypeStr() == "AIG") {
         aig.push_back(_dfsList[i]);
      }
   }
   int max = _totalList.size() - _poList.size() - 1;
   int pi = _piList.size(), po = _poList.size(), aigs = aig.size();
   outfile << "aag " << max << " " << pi << " 0 " << po << " " << aigs << endl;
   for (int i = 0; i < pi; ++i) {
      outfile << _piList[i]->gateID * 2 << endl;
   }
   for (int i = 0; i < po; ++i) {
      if (_poList[i]->_faninList[0].isInv()) {
         outfile << _poList[i]->_faninList[0].gate()->gateID * 2 + 1 << endl;
      }
      else {
         outfile << _poList[i]->_faninList[0].gate()->gateID * 2 << endl;
      }
   }
   for (int i = 0; i < aigs; ++i) {
      outfile << aig[i]->gateID * 2 << " ";
      if (aig[i]->_faninList[0].isInv()) {
         outfile << aig[i]->_faninList[0].gate()->gateID *2 +1 << " ";
      }
      else {
         outfile << aig[i]->_faninList[0].gate()->gateID *2 <<  " ";
      }
      if (aig[i]->_faninList[1].isInv()) {
         outfile << aig[i]->_faninList[1].gate()->gateID *2 +1 << endl;
      }
      else {
         outfile << aig[i]->_faninList[1].gate()->gateID *2 <<  endl;
      }
   }
   for (int i = 0; i < pi; ++i) {
      if (_piList[i]->sym != "") {
         outfile << "i" << i << " " << _piList[i]->sym << endl;
      }
   }
   for (int i = 0; i < po; ++i) {
      if (_poList[i]->sym != "") {
         outfile << "o" << i << " " << _poList[i]->sym << endl;
      }
   }
   for (int i = 0; i < comment.size(); ++i) {
      outfile << comment[i] << endl;
   }
}

void 
CirMgr::dfstraversal(const GateList& sinkList)
{
   CirGate::setGlobalRef();
   int size = sinkList.size();
   for (int i = 0; i < size; ++i) {
      sinkList[i]->dfstraversal(this->_dfsList);
   }
}
// sort
void 
CirMgr::quickSort(vector<int> &A, int p, int r) const
{
    if (p < r){
        int q = partition(A, p, r);
        quickSort(A, p, q);
        quickSort(A, q+1, r);
    }
}

int 
CirMgr::partition(vector<int> &A, int p, int r) const
{
    int x = A[p];
    int i = p-1, j = r+1;
    while (1){
        j -= 1;
        while (A[j] > x){
            j -= 1;
        }
        i += 1;
        while (A[i] < x){
            i += 1;
        }
        if (i < j){
            int temp;
            temp = A[i];
            A[i] = A[j];
            A[j] = temp;
        }
        else{
            return j;
        }
    }   
}