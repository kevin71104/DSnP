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
       if( t == 0 ) return;
       while (t > 0){
           size_t p = (t-1)/2;
           if ( _data[p] <= d ) break; //child > parent
           _data[t] = _data[p];
           t = p;
       }
       _data[t] = d;
   }
   void delMin() { delData(0); }
   void delData(size_t i) {
       if(_data.empty()) return;
       //replace the removed node with the last node
       size_t n = _data.size()-1;
       _data[i] = _data[n];
       _data.resize(n);
       if(i == n) return;
       //compare to its parent
       bool flag = false;
       size_t t = i;
       size_t p;
       while(t > 0){    //having parent
           p = (t-1)/2;
           if(_data[p] <= _data[t]) break;
           flag = true;
           Data temp = _data[t];
           _data[t] = _data[p];
           _data[p] = temp;
           t = p;
       }
       // i.e. the last node is smaller than all the children of replaced node
       if(flag) return;
       size_t c;
       t = i;
       while(t <= (n-1-1)/2){   //having children(last node with index = n-1)
           c = 2*t + 1;
           if(c != n-1 ){
               //compared with the smaller child
               if(_data[c+1] < _data[c] ) c++;
           }
           if(_data[t] <= _data[c] ) break;
           Data temp = _data[t];
           _data[t] = _data[c];
           _data[c] = temp;
           t = c;
       }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
