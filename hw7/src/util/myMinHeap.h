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
   const Data& min() const { return _data.front(); }
   void insert(const Data& d) {
       size_t t = _data.size(); //new node's index
       _data.push_back(d);
       while (t > 0){
           size_t p = (t-1)/2;
           if ( _data[p] < d ) break; //child > parent
           _data[t] = _data[p];
           t = p;
       }
       _data[t] = d;
   }
   void delMin() { delData(0); }
   void delData(size_t i) {
       Data temp = _data[i];
       size_t p = i, t = 2 * p + 1 ;
       size_t n = _data.size() - 1;
       if( p >= n) return;
       while(t <= n ){
           if( t < n ) // has right child
               if(_data[t+1] < _data[t])
                   ++t;
           if( _data[n] < _data[t]) break; //can just use end to replace p
           _data[p] = _data[t];
           p = t;
           t = 2 * p + 1;
        }
        _data[p] = _data[n--];
        _data.pop_back();
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
