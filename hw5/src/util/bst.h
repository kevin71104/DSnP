/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
   
   BSTreeNode(const T& x, BSTreeNode<T>* p=0 , BSTreeNode<T>* l=0, BSTreeNode<T>* r=0):
		_key(x), _p(p), _left(l), _right(r) {}
		
   //data member
   T 				_key;
   BSTreeNode<T>*	_p;
   BSTreeNode<T>*	_left;
   BSTreeNode<T>*	_right;
};


template <class T>
class BSTree
{
	
public:
   // TODO: design your own class!!
   BSTree<T>(): _root(0), _size(0){ _dummy = new BSTreeNode<T>( T() ); }
   ~BSTree<T>(){ clear(); delete _dummy; }
   
   class iterator 
   { 
		friend class BSTree;
		
	public:
		iterator(BSTreeNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {}
      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_key; }
      T& operator * () { return _node->_key; }
      //prefix
      iterator& operator ++ () {
			//successor	
			if(_node->_right != 0){
				_node = _node->_right;
				while(_node->_left != 0)
					_node = _node->_left;
			}
			else{
				BSTreeNode<T>* parent = _node->_p;
				while(parent != 0 && _node == parent->_right){
					_node = parent;
					parent = parent->_p;
				}
				_node = parent;
			}
			return (*this);	
		}
      iterator& operator -- () {	
			//predecessor
			if(_node->_left != 0){
				_node = _node->_left;
				while(_node->_right != 0 )
					_node = _node->_right;
			}
			else{
				BSTreeNode<T>* parent = _node->_p;
				while(parent != 0 && _node == parent->_left){
					_node = parent;
					parent = parent->_p;
				}
				_node = parent;
			}	
			return (*this); 
		}
      //postfix
      iterator operator ++ (int) {
			iterator temp(_node);
			++(*this);
			return temp;
		}
      iterator operator -- (int) {
			iterator temp(_node);
			--(*this);
			return temp;
		}
      
      iterator& operator = (const iterator& i) { _node = i._node; return *(this);}

      bool operator != (const iterator& i) const {	return ( _node != i._node ); }
      bool operator == (const iterator& i) const { return ( _node == i._node ); }     
      
	private:
		BSTreeNode<T>* _node;
	};
	
	iterator begin() const {
		if(_root == 0) return iterator(_dummy);
		return iterator( minimum(_root) );
	}
	iterator end() const { return iterator(_dummy);	}
	
	bool empty() const { return ( _size == 0); }
	size_t size() const { return _size ; }
  
	bool erase (iterator pos) {
		if(empty() || pos == end() || pos._node == 0)   
			return false;
		BSTDelete(pos._node);
		return true;
	}
	bool erase ( const T& k ) {
		iterator pos(search(k, _root));
		return erase (pos);
	}
	void pop_front() { erase( begin() ); }
	void pop_back() { erase( --end() ); }
	
	void insert ( const T& k ) {
		BSTreeNode<T>* _insert = new BSTreeNode<T>(k);
		BSTInsert(_root, _insert);
	}		
	void clear() {
		if(empty()) return;
		BSTClear(_root);
		_root = 0;
		_size = 0;
		_dummy->_p = 0;
	}
   void sort() const {return;}
   void print() const {if(empty()) return;   preOrderPrint(_root,0);}
   
private:
	//data member
	BSTreeNode<T>* _root;
	//keep the rightmost node's _right is _dummy , dummy end
	BSTreeNode<T>* _dummy;
	size_t _size;
	void preOrderPrint(BSTreeNode<T>* _now, size_t height) const {
		for(size_t i=0;i < height;i++)
			cout<<"  ";
		if(_now == 0 || _now == _dummy){
			cout<<"[0]"<<endl;
			return;
		}
		else
			cout<<_now->_key<<endl;
		//if(_now->_left != 0)
			preOrderPrint(_now->_left,height+1);
		//if(_now->_right != 0)
			preOrderPrint(_now->_right,height+1);
	}
	void BSTClear(BSTreeNode<T>* _now){
		if(_now->_left != 0)
			BSTClear(_now->_left);
		if(_now->_right != 0 && _now->_right != _dummy)
			BSTClear(_now->_right);
		delete _now;
	}
	// r = _root
	void BSTInsert(BSTreeNode<T>* r, BSTreeNode<T>* _insert){
		BSTreeNode<T>* x = r;
		//trailing pointer y
		BSTreeNode<T>* y = 0;
		if( x == 0)
			x = _dummy;
		//y will be x's parent 
		while(x != 0 && x != _dummy){
			y = x;
			if(_insert->_key < x->_key)
				x = x->_left;
			else
				x = x->_right;
		}
		_insert->_p = y;
		if( y == 0)
			_root = _insert;
		else if ( _insert->_key < y->_key)
			y->_left = _insert;
		else	
			y->_right = _insert;	
		//_insert b/w the highest & _dummy, keep _dummy the rightmost node
		if( x == _dummy){
			_dummy->_p = _insert;
			_insert->_right = _dummy;
		}
		_size++ ;
	}
	//only deal with parent
	void transplant(BSTreeNode<T>* _delete, BSTreeNode<T>* _replace){
		if(_delete->_p == 0)
			_root = _replace;
		else if(_delete == (_delete->_p)->_left)
			(_delete->_p)->_left = _replace;
		else
			(_delete->_p)->_right = _replace;
		if(_replace != 0)
			_replace->_p = _delete->_p;
	}
	void BSTDelete(BSTreeNode<T>* target){
		if(target == 0)   return;
		if(target->_left == 0)
			transplant(target, target->_right);
		else if(target->_right == 0)
			transplant(target, target->_left);
		else if(target->_right == _dummy){
			BSTreeNode<T>* newmax = maximum(target->_left);
			newmax->_right = _dummy;
			_dummy->_p = newmax;
			transplant(target, target->_left);
		}
		else{
			//let y be the successor of target in the right subtree
			BSTreeNode<T>* y = target->_right;
			while( y->_left != 0 )
				y = y->_left;
			//if y's parent is not target, need to bridge the gap between target->_right & replacing node
			//y->_p & y->_right(since there is no y->_left) shoule be connected
			if(y->_p != target){
				transplant(y, y->_right);
				y->_right = target->_right;
				(target->_right)->_p = y;
			}
			//deal with target->_left 
			y->_left = target->_left;
			(target->_left)->_p = y;
			transplant(target,y);	
		}
		delete target;
		_size-- ;	
	}	
	BSTreeNode<T>* minimum(BSTreeNode<T>* _now) const{
		if( _now == 0) return 0;
		while( _now->_left != 0 )
			_now = _now->_left;
		return _now;
	}
	BSTreeNode<T>* maximum(BSTreeNode<T>* _now) const{
		if( _now == 0) return 0;
		while( _now->_right != 0 && _now->_right != _dummy )
			_now = _now->_right;
		return _now;
	}
	BSTreeNode<T>* search(const T& k , BSTreeNode<T>* _now) const{
		while( _now != 0 && k != _now->_key && _now != _dummy){
			if( k > _now->_key)
				_now = _now->_right;
			else
				_now = _now->_left;
		}
		if(_now == _dummy)   return 0;
		return _now;	
	}
	//return the next larger in the whole tree
	BSTreeNode<T>* sucessor(BSTreeNode<T>* _now) const{
		/*if( _now->_right != 0 )
			return minimum(_now->_right);
		BSTreeNode<T>* parent = _now->_p;
		while( parent != 0 && _now == parent->_right ){
			_now = parent;
			parent = parent->_p;
		}
		return parent;*/
		iterator it(_now);
		if(it == end())
			return 0;
		it++;
		return it._node;
	}
	//return the last smaller in the whole tree
	BSTreeNode<T>* predecessor(BSTreeNode<T>* _now) const{
		/*if( _now->_left != 0)
			return maximum(_now->_left);
			
		BSTreeNode<T>* parent = _now->_p;
		while( parent != 0 && _now == parent->_left ){
			_now = parent;
			parent = parent->_p;
		}
		return parent;*/
		iterator it(_now);
		if(it == begin());
			return 0;
		it--;
		return it._node;
	}
	
};

#endif // BST_H
