/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;
   public:
      const Data& operator * () const { return (*x)[num]; }
      iterator& operator ++ () { 
         if (num == x->size() - 1) {
            num = 0;
            x += 1;
            if (x != y) {
               while (x->empty()) {
                  x += 1;
                  if (x == y)
                     break;
               }
            }
         }
         else 
            num += 1;
         return (*this); 
      }
      bool operator != (const iterator& i) const { return (x != i.x || num != i.num || y != i.y) ? true : false; }
      iterator& operator = (const iterator& i) { x = i.x; y = i.y; num = i.num; return(*this); }
   private:
      vector<Data>* x;
      vector<Data>* y;
      size_t num;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const { 
      iterator temp;
      temp.x = _buckets + _numBuckets;
      temp.num = 0;
      temp.y = _buckets + _numBuckets;
      for (int i = 0; i < _numBuckets; ++i) {
         if (!_buckets[i].empty()) {
            temp.x = &_buckets[i];
            temp.num = 0;
            temp.y = _buckets + _numBuckets;
            break;
         }
      }
      return temp;
   }
   // Pass the end
   iterator end() const {
      iterator temp;
      temp.x = _buckets + _numBuckets;
      temp.num = 0;
      temp.y = _buckets + _numBuckets;
      return temp;
   }
   // return true if no valid data
   bool empty() const { return (_numBuckets == 0) ? true : false; }
   // number of valid data
   size_t size() const { 
      size_t s = 0; 
      for (int i = 0; i < _numBuckets; i++) {
         s = s + _buckets[i].size();
      }
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
      size_t temp = bucketNum(d);
      for (int i = 0; i < _buckets[temp].size(); ++i) {
         if (_buckets[temp][i] == d)
            return true;
      }
      return false; 
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const { 
      size_t temp = bucketNum(d);
      for (int i = 0; i < _buckets[temp].size(); ++i) {
         if (_buckets[temp][i] == d) {
            d = _buckets[temp][i];
            return true;
         }
      }
      return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      size_t temp = bucketNum(d);
      for (int i = 0; i < _buckets[temp].size(); ++i) {
         if (_buckets[temp][i] == d) {
            _buckets[temp][i] = d;
            return true;
         }
      }
      insert (d);
      return false; 
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) { 
      if (check(d)) {
         return false;
      }
      size_t num = bucketNum(d);
      _buckets[num].push_back(d);
      return true; 
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      if (check(d)) {
         int n = bucketNum(d), s = _buckets[n].size();
         for (typename vector<Data>::iterator i = _buckets[n].begin(); i != _buckets[n].end();) {
            if (*i == d) {
               *i = _buckets[n][s-1];
               _buckets[n].pop_back();
               return true;
            }
            else{
               ++i;
            }
         }
      }
      return false; 
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
