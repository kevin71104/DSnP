/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
//
//    size_t operator() () const { return 0; }
//
//    bool operator == (const HashKey& k) const { return true; }
//
// private:
// };
//
template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap() : _numBuckets(0), _buckets(0) {}
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
      iterator(HashMap<HashKey, HashData>* h=0, size_t b=0,size_t bn=0 )
            :_hash(h), _bId(b), _bnId(bn){}
      iterator(const iterator& i): _hash(i._hash), _bId(i._bId), _bnId(i._bnId) {}
       ~iterator(){}

       //*_hash[_bId] is vector<HashNode>
       const HashNode& operator * () const{ return (*_hash)[_bId][_bnId];}
       HashNode& operator *() { return (*_hash)[_bId][_bnId];}

       //prefix
       iterator& operator ++ (){
         if( _hash == 0) return (*this);
         if(_bId >= _hash->_numBuckets) return (*this);
         if (++_bnId >= (*_hash)[_bId].size()) {
           while ((++_bId < _hash->_numBuckets) && (*_hash)[_bId].empty());
           _bnId = 0;
         }
         return (*this);
       }

       iterator& operator -- (){
         if (_hash == 0) return (*this);
         if (_bnId == 0) {
           if (_bId == 0) return (*this);
           while ((*_hash)[--_bId].empty())
             if (_bId == 0) return (*this);
           _bnId = (*_hash)[_bId].size() - 1;
         }
         else
            --_bnId;
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
           _bId = i._bId;
           _bnId = i._bnId;
           _hash = i._hash;
           return (*this);
       }
       bool operator == (const iterator& i){
           return (_hash == i._hash && _bId == i._bId && _bnId == i._bnId);
       }
       bool operator != (const iterator& i){
           return ( !(*this) == i);
       }

   private:
       size_t                       _bId;
       size_t                       _bnId; //(_hash*)[_bId][_bnId]
       HashMap<HashKey, HashData>*  _hash;
   };

   void init(size_t b) { reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }

   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
     if (_buckets == 0) return end();
     for (size_t i = 0; i < _numBuckets; ++i)
       if (!_buckets[i].empty())
          return iterator(const_cast< HashMap<HashKey, HashData>* >(this), i, 0);
     return end();
   }
   // Pass the end
   iterator end() const { return iterator(const_cast<HashMap<HashKey, HashData>*>(this),_numBuckets, 0);}
   
   // return true if no valid data
   bool empty() const {
      for(size_t i=0; i < _numBuckets; i++)
           if(!_buckets[i].empty()) return false;
      return true;
   }
   // number of valid data
   size_t size() const {
     size_t s = 0;
     for (size_t i = 0; i < _numBuckets; ++i) s += _buckets[i].size();
     return s;
   }

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k, HashData& n) const {
     size_t b = bucketNum(k);
     for (size_t i = 0; i < _buckets[b].size(); i++)
       if (_buckets[b][i].first == k) {
          n = _buckets[b][i].second;
          return true;
       }
     return false;
   }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const {
     size_t b = bucketNum(k);
     for (size_t i = 0; i < _buckets[b].size(); i++)
       if (_buckets[b][i].first == k) {
          d = _buckets[b][i].second;
          return true;
       }
     return false;
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) { return false; }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) {
     size_t b = bucketNum(k);
     for (size_t i = 0; i < _buckets[b].size(); i++)
       if (_buckets[b][i].first == k) {
          return false;
       }
     _buckets[b].push_back(HashNode(k, d));
     return true;
   }

   // return true if inserted successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> still do the insertion
   bool replaceInsert(const HashKey& k, const HashData& d) {
      size_t b = bucketNum(k);
      for (size_t i = 0; i < _buckets[b].size(); i++)
         if (_buckets[b][i].first == k) {
            _buckets[b][i].second = d;
            return false;
         }
      _buckets[b].push_back(HashNode(k, d));
      return true;
   }

   // Need to be sure that k is not in the hash
   void forceInsert(const HashKey& k, const HashData& d) {
      _buckets[bucketNum(k)].push_back(HashNode(k, d));
    }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) { return false; }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//
//    size_t operator() () const { return 0; }
//
//    bool operator == (const CacheKey&) const { return true; }
//
// private:
// };
//
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
