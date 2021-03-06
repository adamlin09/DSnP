/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   if (!is){
     j.readin1 = false;
   }
   else{
     string key; int value;
     DBJsonElem readin;
     char temp = 0;
     while (temp != 125){
      is >> temp;
      if (temp == 34){
         is >> key;
         key.pop_back();
         is >> temp;
         is >> value;
         DBJsonElem readin(key, value);
         j._obj.push_back(readin);
      }
     }
     j.readin1 = true;
   }
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   os << "{" << endl;
   int size = j.size();
   for (int i = 0; i < size; i++){
     os << "  " << j._obj[i];
     if (i != size-1){
       os << ",";
     }
     os << endl;
   }
   os << "}" << endl;
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
   _obj.clear();
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   int size = _obj.size();
   for (int i = 0; i < size; ++i){
     if (elm.key() == _obj[i].key()){
       cerr << "Error: Element with key \"" << elm.key() << "\" already exists!!" << endl;
       return false;
     }
   }
   _obj.push_back(elm);
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   if (_obj.empty()){
     return NAN;
   }
   else{
     int sum = 0, size = _obj.size();
     float ave;
     sum = this->sum();
     ave = float(sum) / size;
     return ave;
   }
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   int size = _obj.size();
   if (_obj.empty()){
     idx = _obj.size();
     maxN = INT_MIN;
   }
   else{
     maxN = _obj[0].value();
     idx = 0;
     for (int i = 1; i < size; i++){
       if (_obj[i].value() > maxN){
         maxN = _obj[i].value();
         idx = i;
       }
     }
   }
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   int size = _obj.size();
   if (_obj.empty()){
     idx = _obj.size();
     minN = INT_MIN;
   }
   else{
     minN = _obj[0].value();
     idx = 0;
     for (int i = 1; i < size; i++){
       if (_obj[i].value() < minN){
         minN = _obj[i].value();
         idx = i;
       }
     }
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int s = 0, size = _obj.size();
   if (!_obj.empty()){
     for (int i = 0; i < size; ++i){
       s += _obj[i].value();
     }
   }
   return s;
}
