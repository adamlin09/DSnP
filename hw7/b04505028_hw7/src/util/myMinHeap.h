/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) { 
      _data.push_back(d);
      int t = _data.size();
      while (t > 1) {
         int p = t / 2;
         if (!(d < _data[p-1]))
            break;
         _data[t-1] = _data[p-1];
         t= p;
      }
      _data[t-1] = d;
   }
   void delMin() { 
      size_t x = 1, y = 2*x, s = _data.size();
      Data temp = _data[s-1]; 
      _data[x-1] = temp;
      _data.pop_back(); --s;
      while (y <= s) {
         if (y < s) {
            if (_data[y] < _data[y-1])
               ++y;
         }
         if (_data[y-1] < _data[x-1]) {
            temp = _data[y-1];
            _data[y-1] = _data[x-1];
            _data[x-1] = temp;
         }
         else  break;
         x = y; y= 2*x;
      }
      /*while (y <= s) {
         if (y < s) {
            if (_data[y] < _data[y-1])
               ++y;
         }
         if (_data[s-1] < _data[y-1])
            break;
         _data[x-1] = _data[y-1];
         x = y;
         y = 2*x;
      }
      _data[x-1] = _data[s-1];
      _data.pop_back();*/
   }
   void delData(size_t i) { 
      size_t x = i+1, y = 2*x, z = x/2, s = _data.size();
      Data temp = _data[s-1];
      _data[x-1] = temp;
      _data.pop_back(); --s;
      if (x != 1) {
         while (_data[x-1] < _data[z-1]) {
            temp = _data[x-1];
            _data[x-1] = _data[z-1];
            _data[z-1] = temp;
            x = z; z = x/2;
         }
      }
      y = 2*x;
      while (y <= s) {
         if (y < s) {
            if (_data[y] < _data[y-1])
               ++y;
         }
         if (_data[y-1] < _data[x-1]) {
            temp = _data[y-1];
            _data[y-1] = _data[x-1];
            _data[x-1] = temp;
         }
         else  break;
         x = y; y= 2*x;
      }
      /*while (y <= s) {
         if (y < s) {
            if (_data[y] < _data[y-1])
               ++y;
         }
         if (_data[s-1] < _data[y-1])
            break;
         _data[x-1] = _data[y-1];
         x = y;
         y = 2*x;
      }
      _data[x-1] = _data[s-1];
      _data.pop_back();*/
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
