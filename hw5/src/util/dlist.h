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

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() :_isSorted(false){
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
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
      //prefix
      iterator& operator ++ () { _node = _node->_next; return *(this); }
      //postfix
      iterator operator ++ (int) { 
			iterator temp( _node );
			_node = _node->_next;
			return temp; 
		}
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) { 
			iterator temp( _node );
			_node = _node->_prev;
			return temp;  
		}
      iterator& operator = (const iterator& i) { 
		  _node = i._node;
		  return *(this); 
		}

      bool operator != (const iterator& i) const {
			return ( _node != i._node );
		}
      bool operator == (const iterator& i) const { 
			return ( _node == i._node ); 
		}

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { 
		iterator begin(_head);
		return begin; 
	}
   iterator end() const { 
		iterator end( (_head->_prev) );
		return end; 
	}
   bool empty() const { return (_head == _head->_prev); }
   
   size_t size() const {  
		//if(empty()) return 0;
		size_t size = 0;
		iterator it(_head);
		const iterator dummy = end();
		while(it != dummy){
			++size;
			++it;
		}
		return size;
	}

   void push_back(const T& x) {
		if(empty()){
			DListNode<T>* newnode = new DListNode<T> (x,_head,_head);
			_head->_prev = newnode;
			_head->_next = newnode;
			_head = newnode;
			_isSorted = false;
			return;
		}
		DListNode<T>* dummy = _head->_prev;
		DListNode<T>* tail = dummy->_prev;
		DListNode<T>* newnode = new DListNode<T> (x,tail,dummy);
		tail->_next = newnode;
		dummy->_prev = newnode;	
		_isSorted = false;	
	}
   void pop_front() {
		if(empty())   return;
		DListNode<T>* dummy = _head->_prev;
		DListNode<T>* temp = _head->_next;
		delete _head;
		dummy->_next = temp;
		temp->_prev = dummy;
		_head = temp;
	}
   void pop_back() {
		if( empty() )   return;
		//if size=1 _head=tail delete tail need to modify _head
		if(_head->_prev == _head->_next){
			DListNode<T>* dummy = _head->_prev;
			delete _head;
			_head = dummy;
			dummy->_prev = dummy;
			dummy->_next = dummy;
			return ;
		}
		DListNode<T>* dummy = _head->_prev;
		DListNode<T>* tail = dummy->_prev;
		DListNode<T>* temp = tail->_prev;
		delete tail;
		temp->_next = dummy;
		dummy->_prev = temp;
	}

   // return false if nothing to erase
   bool erase(iterator pos) { 
		if( empty() )return false;
		if( pos==end() ) return false;
		if(pos._node == _head){
			pop_front();
			return true;
		}
		DListNode<T>* currentnode = pos._node;
		DListNode<T>* prevtemp = currentnode->_prev;
		DListNode<T>* nexttemp = currentnode->_next;
		delete currentnode;
		prevtemp->_next = nexttemp;
		nexttemp->_prev = prevtemp;
		return true;
	}
   bool erase(const T& x) {
		if( empty() )   return false;
		for(iterator iter = begin();iter != end();iter++){
			if( *iter == x ){
				erase(iter);
				return true;
			}
		}
		return false;
	}
   void clear() {
		if(empty())   return;
		size_t Size = size();
		for(size_t i=0; i<Size ; i++)
			pop_front();
	}  // delete all nodes except for the dummy node
	

	void sort() const {
		if( _isSorted ) return; 
		//bubblesort();
		quicksort(_head, (_head->_prev)->_prev , size());
		_isSorted = true;
	}//ascending order


private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted
   
   void quicksort(DListNode<T>* left , DListNode<T>* right , size_t size) const{
	   if(size <= 1 )return;
	   //find the medium data
	   DListNode<T>* index = left;
	   for(size_t i=1; i<= size_t(size/2) ; i++)
			index = index->_next;
		//set pivot
		T pivot = index->_data;
		//put the pivot data in the rightmost node
		myswap(index,right);
		//keep the position where new smaller data insert to
		DListNode<T>* swapindex = left;
		//use index run from left to right->_prev
		index = left;
		size_t leftlength = 0;
		while(index != right){
			if(index->_data <= pivot){
				//cerr<<index->_data<<endl;
				myswap(index , swapindex);
				swapindex = swapindex->_next;
				leftlength++;
			}
			index = index->_next;	
		}
		//put the pivot data in the swapindex node 
		//all left nodes have smaller data; right nodes have bigger
		myswap(swapindex,right);
		quicksort(left,swapindex->_prev,leftlength);
		quicksort(swapindex->_next , right , size-leftlength-1);
   }
   void bubblesort() const {
		for(iterator i = --end() ; i != begin() ; i--){
			bool flag = false;
			for(iterator j = begin() ; j!=i ; j++){
				iterator k(j);
				k++;
				if( *j > *k ){
					myswap(j._node,k._node);
					flag = true;
				}
			}
			if(!flag)   break;	
		} 
	}
   void myswap(DListNode<T>* a, DListNode<T>* b) const{
		T temp = a->_data;
		a->_data = b->_data;
		b->_data = temp;
	}

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
