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
// "operator ()" is to check whether there has already been
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
       iterator(bool begin, size_t num, vector<Data>* b)
            :index(0), pos(0), _numBuckets(num), _buckets(b){
           if(begin)
               for(;index < _numBuckets; index ++)
                   if(!_buckets[index].empty()){
                       _data = &_buckets[index][pos];
                       break;
                   }
           else index = _numBuckets; //iterator end
       }
       ~iterator(){}

       Data& operator * () const{ return *_data;}
       Data& operator *() { return *_data;}

       //prefix
       iterator& operator ++ (){
           /*if(index == _numBuckets) return (*this);
           if(_buckets[index].size()-1 > pos )
               pos ++;
           else{
               for(++index; index < _numBuckets; ++index)
                   if(!_buckets[index].empty()) break;
               pos = 0;
           }
           if(index != _numBuckets) _data = &_buckets[index][pos];
           return (*this);*/
           if(index == _numBuckets) return (*this);
         else if(pos < _buckets[index].size() - 1) ++pos;
         else {
            while(++index < _numBuckets) {
               if(_buckets[index].size()) {
                  break;
               }
            }
            pos = 0;
         }
         if(index != _numBuckets) _data = &_buckets[index][pos];
         return (*this);
       }

       iterator& operator -- (){
           /*if(index == 0 && pos == 0) return (*this);
           if( pos > 0 )
               pos --;
           else
               for(--index; index >= 0 ;--index){
                   if(!_buckets[index].empty()){
                       pos = _buckets[index].size()-1;
                       break;
                   }
                   if (index == 0) break;
               }
           _data = &_buckets[index][pos];
           return (*this);*/
           if(pos > 0) --pos;
         else {
            size_t n = index;
            while(n > 0) {
               --n;
               if(_buckets[n].size()) {
                  pos = _buckets[n].size() - 1;
                  index = n;
                  break;
               }
            }
         }
         _data = &_buckets[index][pos];
         return (*this);
       }

       //postfix
       iterator operator ++ (int){
           iterator temp = (*this);
           ++(*this);
           return temp;
       }
       iterator operator -- (int){
           iterator temp = (*this);
           --(*this);
           return temp;
       }
       iterator& operator = (const iterator& i){
           _data = i._data;
           index = i.index;
           pos = i.pos;
           _numBuckets = i._numBuckets;
           _buckets = i._buckets;
       }
       bool operator == (const iterator& i){
           return ((pos == i.pos) && (index == i.index));
       }
       bool operator != (const iterator& i){
           return ((pos != i.pos) || (index != i.index));
       }
   private:
       Data*                    _data;
       size_t                   index; //_buckets[index]
       size_t                   pos; //_buckets[index][pos]
       const size_t             _numBuckets;
       vector<Data>*            _buckets;
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
   iterator begin() const { return iterator(true,_numBuckets,_buckets); }
   // Pass the end
   iterator end() const { return iterator(false,_numBuckets,_buckets); }
   // return true if no valid data
   bool empty() const {
        bool flag = true;
        for(size_t i=0; i < _buckets.size(); i++)
            if(!_buckets[i].empty()){
                flag = false;
                break;
            }
        return flag;
   }
   // number of valid data
   size_t size() const {
       size_t s = 0;
       for(size_t i=0; i < _buckets.size(); i++)
           s += _buckets[i].size();
       return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
       size_t index = bucketNum(d);
       for(size_t i=0; i<_buckets[index].size(); i++)
           if ( d == _buckets[index][i])
               return true;
       return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
       size_t num = bucketNum(d), pos;
       for(pos = 0; pos < _buckets[num].size(); ++pos)
           if(_buckets[num][pos] == d) break;
       if(pos == _buckets[num].size()) return false;
       d = _buckets[num][pos];
       return true;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
       size_t num = bucketNum(d), pos;
       for(pos = 0; pos < _buckets[num].size(); ++pos)
           if(_buckets[num][pos] == d) break;
       if(pos == _buckets[num].size()){
           _buckets[num].push_back(d);
           return false;
       }
       _buckets[num][pos] = d;
       return true;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
       size_t num = bucketNum(d), pos;
       for(pos = 0; pos < _buckets[num].size(); ++pos)
           if(_buckets[num][pos] == d) break;
       if(pos == _buckets[num].size()){
           _buckets[num].push_back(d);
           return true;
       }
       return false;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
       size_t num = bucketNum(d), pos;
       for(pos = 0; pos < _buckets[num].size(); ++pos)
           if(_buckets[num][pos] == d){
               _buckets[num].erase(_buckets[num].begin()+pos);
               return true;
           }
       return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;   //each _buckets[i] is a vector<data>

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
