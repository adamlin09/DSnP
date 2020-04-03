/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { 
         _node = _node->_next;
         return *(this); 
      }
      iterator operator ++ (int) { 
         iterator temp = *this;
         _node = _node->_next;
         return temp; 
      }
      iterator& operator -- () { 
         _node = _node->_prev;
         return *(this); 
      }
      iterator operator -- (int) { 
         iterator temp = *this;
         _node = _node->_prev;
         return temp;
      }

      iterator& operator = (const iterator& i) { 
         _node = i._node;
         return *(this); 
      }

      bool operator != (const iterator& i) const { 
         if (_node != i._node){
            return true;
         }
         return false; 
      }
      bool operator == (const iterator& i) const { 
         if (_node == i._node){
            return true;
         }
         return false; 
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { 
      if (empty()){
         return end();
      }
      iterator temp(_head);
      return temp; 
   }
   iterator end() const { 
      iterator temp(_head);
      while (temp._node->_next != _head){
         ++temp;
      }
      return temp; 
   }
   bool empty() const { 
      if (_head->_next == _head){
         return true;
      }
      return false; 
   }
   size_t size() const {  
      size_t s = 0;
      iterator temp(_head);
      while (temp != end()){
         s += 1;
         ++temp;
      }
      return s; 
   }

   void push_back(const T& x) { 
      if (empty()){
         DListNode<T>* p = _head;
         DListNode<T>* temp = new DListNode<T>(x, _head, _head);
         _head = temp;
         p->_prev = _head;
         p->_next = _head; 
      }
      else{
         DListNode<T>* p = end()._node;
         DListNode<T>* temp = new DListNode<T>(x, p->_prev, p);
         p->_prev->_next = temp;
         p->_prev = temp;
      }
   }
   void pop_front() { 
      if (!empty()){
         DListNode<T>* temp = _head;
         iterator x = end();
         _head = _head->_next;
         _head->_prev = end()._node;
         x._node->_next = _head;
         delete temp;
      }
   }
   void pop_back() { 
      if (!empty()){
         iterator d = end();
         if (_head->_next == d._node){
            _head = _head->_next;
         }
         DListNode<T>* temp = d._node->_prev;
         d._node->_prev = d._node->_prev->_prev;
         d._node->_prev->_next = d._node;
         //delete temp;
      }
   }

   // return false if nothing to erase
   bool erase(iterator pos) { 
      if (!empty()){
         if (pos._node == _head){
            _head = pos._node->_next;
         }
         DListNode<T>* temp = pos._node;
         temp->_prev->_next = temp->_next;
         temp->_next->_prev = temp->_prev;
         //delete temp;
         return true;
      }
      return false; 
   }
   bool erase(const T& x) { 
      iterator temp = find(x);
      if (temp != end()){
         if (temp._node == _head){
            _head = temp._node->_next;
         }
         temp._node->_prev->_next = temp._node->_next;
         temp._node->_next->_prev = temp._node->_prev;
         delete temp._node;
         return true;
      }
      return false; 
   }

   iterator find(const T& x) { 
      for (iterator temp = begin(); temp != end(); ++temp){
         if (*temp == x){
            return temp;
         }
      }
      return end(); 
   }

   void clear() { 
      while (!empty()){
         pop_back();
      }
   }  // delete all nodes except for the dummy node

   void sort() const { 
      /*for (iterator i= ++begin(); i != end(); ++i){
         T key = *i;
         iterator temp = i;
         iterator j = --temp;
         while (j != end() && *j > key){
            iterator x = j;
            ++x;
            *x = *j;
            --j;
         }
         ++j;
         *j = key;
      }*/
      size_t s = size();
      quickSort(_head, 0, s-1);
      _isSorted = true;
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   void quickSort(DListNode<T>* A, size_t p, size_t r) const{
    if (p < r){
        size_t q = partition(A, p, r);
        quickSort(A, p, q);
        quickSort(A, q+1, r);
    }
   }

   size_t partition(DListNode<T>* A , size_t p, size_t r) const{
      iterator x(A);
      for (size_t i = 0; i < p; ++i){
         ++x;
      }
      size_t i = p - 1, j = r + 1;
      iterator a = x;
      iterator b(A);
      --a;
      for (size_t i = 0; i < r; ++i){
         ++b;
      }
      ++b;
      while (1){
         j -= 1;
         --b;
         while (*b > *x){
            j -= 1;
            --b;
         }
         i += 1;
         ++a;
         while (*a < *x){
            i += 1;
            ++a;
         }
         if (i < j){
            T temp = *a;
            *a = *b;
            *b = temp;
         }
         else{
            return j;
         }
      }
   }
};

#endif // DLIST_H
