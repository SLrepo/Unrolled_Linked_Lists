#ifndef UnrolledLL_h_
#define UnrolledLL_h_
// A simplified implementation of a generic list container class,
// including the iterator, but not the const_iterators.  Three
// separate classes are defined: a Node class, an iterator class, and
// the actual list class.  The underlying list is doubly-linked, but
// there is no dummy head node and the list is not circular.

#include <cassert>

const int NUM_ELEMENTS_PER_NODE=6;
// -----------------------------------------------------------------
// NODE CLASS
template <class T>
class Node{
public:
  Node(); //: num_elements(0), next_(NULL), prev_(NULL) {}
  Node(const T& v);//: num_elements(1), next_(NULL), prev_(NULL), elements[0](v) {}

  // REPRESENTATION
  //T value_;
  Node<T>* next_;
  Node<T>* prev_;
  T elements[NUM_ELEMENTS_PER_NODE];
  unsigned int num_elements;
};

template <class T> Node<T>::Node(){
  next_=NULL;
  prev_=NULL;
  num_elements=0;
}
template <class T> Node<T>::Node(const T& v){
  next_=NULL;
  prev_=NULL;
  num_elements=1;
  elements[0]=v;
}

// A "forward declaration" of this class is needed
template <class T> class UnrolledLL;

// -----------------------------------------------------------------
// LIST ITERATOR
template <class T>
class list_iterator {
public:
  list_iterator() : ptr_(NULL), offset(0) {}
  list_iterator(Node<T>* p, unsigned int os) : ptr_(p), offset(os) {}
  list_iterator(list_iterator<T> const& old) : ptr_(old.ptr_), offset(old.offset) {}
  ~list_iterator() {}

  list_iterator<T> & operator=(const list_iterator<T> & old) { 
    ptr_ = old.ptr_;  offset=old.offset; return *this; }

  // dereferencing operator gives access to the value at the pointer
  T& operator*()  { return ptr_->elements[offset];  }

  // increment & decrement operators
  list_iterator<T> & operator++() { // pre-increment, e.g., ++iter
    if (ptr_->next==NULL && ptr_->num_elements-1==offset){    
      offset=NUM_ELEMENTS_PER_NODE;
    }
    else if (offset==ptr_->num_elements-1){
      ptr_ = ptr_->next_; 
      offset=0;
    }
    else {
      offset++;
    }
    return *this;
  }
  list_iterator<T> operator++(int) { // post-increment, e.g., iter++
    list_iterator<T> temp(*this);
    if (ptr_->next_==NULL && ptr_->num_elements-1==offset){    
      offset=NUM_ELEMENTS_PER_NODE;
    }
    else if (offset==ptr_->num_elements-1){
      ptr_ = ptr_->next_; 
      offset=0;
    }
    else {
      offset++;
    }
    return temp;
  }
  list_iterator<T> & operator--() { // pre-decrement, e.g., --iter
    if (offset==0){
      ptr_ = ptr_->prev_; 
      offset=ptr_->num_elements-1;
    }
    else {
      offset--;
    }
    return *this;
  }
  list_iterator<T> operator--(int) { // post-decrement, e.g., iter--
    list_iterator<T> temp(*this);
    if (offset==0){
      ptr_ = ptr_->prev_; 
      offset=ptr_->num_elements-1;
    }
    else {
      offset--;
    }
    return temp;
  }

  friend class UnrolledLL<T>;

  // Comparions operators are straightforward
  bool operator==(const list_iterator<T>& r) const {
    return ptr_ == r.ptr_ && offset==r.offset; }
  bool operator!=(const list_iterator<T>& r) const {
    return ptr_ != r.ptr_ || offset!=r.offset; }

private:
  // REPRESENTATION
  Node<T>* ptr_;    // ptr to node in the list
  unsigned int offset;
};

// -----------------------------------------------------------------
// LIST CLASS DECLARATION
// Note that it explicitly maintains the size of the list.
template <class T> class UnrolledLL{
public:
  UnrolledLL() : head_(NULL), tail_(NULL), size_(0) {}
  UnrolledLL(const UnrolledLL<T>& old) { this->copy_list(old); }
  ~UnrolledLL() { this->destroy_list(); }
  UnrolledLL& operator= (const UnrolledLL<T>& old);

  unsigned int size() const { return size_; }
  bool empty() const { return head_ == NULL; }
  void clear() { this->destroy_list(); }
   
  void push_front(const T& v);
  void pop_front();
  void push_back(const T& v);
  void pop_back();
  void print(std::ostream& ostr) const;

  const T& front() const { return head_->elements[0];  }
  T& front() { return head_->elements[0]; }
  const T& back() const { return tail_->elements[tail_->num_elements-1]; }
  T& back() { return tail_->elements[tail_->num_elements-1]; }

  typedef list_iterator<T> iterator;
  iterator erase(iterator itr);
  iterator insert(iterator itr, T const& v);
  iterator begin() const { return iterator(head_, 0); }
  iterator end() const { return iterator(tail_, NUM_ELEMENTS_PER_NODE); }
  //Node<T>*& operator = (const Node<T>* a, const Node<T>* b) const {a=b;return a;}


private:
  void copy_list(UnrolledLL<T> const & old);
  void destroy_list();

  //REPRESENTATION
  Node<T>* head_;
  Node<T>* tail_;
  unsigned int size_;
};

// -----------------------------------------------------------------
// LIST CLASS IMPLEMENTATION
template <class T>
UnrolledLL<T>& UnrolledLL<T>::operator= (const UnrolledLL<T>& old) {
  if (&old != this) {
    this->destroy_list();
    this->copy_list(old);
  }
  return *this;
}

template <class T> 
void UnrolledLL<T>::push_back(const T& v) {
  // special case:  initially empty list
  if (!tail_) {
    Node<T>* newp = new Node<T>( v );
    head_ = tail_ = newp;
  }
  // normal case:  at least one node already
  else {
    if(tail_->num_elements<NUM_ELEMENTS_PER_NODE){
      tail_->num_elements++;
      tail_->elements[tail_->num_elements-1]=v;
      //delete [] newp;
    }
    else{
      Node<T>* newp = new Node<T>( v );
      newp -> prev_ = tail_;
      tail_ -> next_ = newp;
      tail_ = newp;
    }
  }
  ++size_;
}


template <class T> 
void UnrolledLL<T>::push_front(const T& v) {
  Node<T>* newp= new Node<T>(v);
  if (!tail_)
    head_=tail_=newp;
  else{
    head_->prev_=newp;
    newp->next_=head_;
    head_=newp;
  }
  size_++;
}


template <class T> 
void UnrolledLL<T>::pop_back() {
  if(size_==1) {
    delete [] head_;
    head_=tail_=NULL;
  }
  else{
    if (tail_->num_elements==1){
      Node<T>* temp=tail_;
      tail_=tail_->prev_;
      delete [] temp;
      tail_->next_=NULL;
    }
    else {
      tail_->num_elements--;
    }
  }
  size_--;
}


template <class T> 
void UnrolledLL<T>::pop_front() {
  if(size_==1) {
    delete [] head_;
    head_=tail_=NULL;
  }
  else{
    if(head_->num_elements==1){
      Node<T>* temp=head_;
      head_=head_->next_;
      delete temp;
      head_->prev_=NULL;
    }
    else{
      for(unsigned i=0; i<head_->num_elements-1; i++){
        head_->elements[i]=head_->elements[i+1];
      }

      head_->num_elements--;
    }
  }
  size_--;
}

template <class T>
void UnrolledLL<T>::print(std::ostream& ostr) const{
  UnrolledLL<T>::iterator i;
  //std::cout << "here"<<std::endl;
  for(i=this->begin(); i!=this->end(); i++){
    ostr << *i << " " << i.ptr_->num_elements <<" | ";

    //std::cout << "here"<<i.ptr_->num_elements<<std::endl;


  }
  
  ostr <<"size is "<< size_<<std::endl;
}


template <class T> 
typename UnrolledLL<T>::iterator UnrolledLL<T>::erase(iterator itr) {
  assert (size_ > 0);
  --size_;
  //iterator result(itr.ptr_);
  // One node left in the list.  
  if (size_==1) {
    delete [] head_;
    head_ = tail_ = 0;
    return iterator();
  }
  // Removing the head in a list if head only has one element
  else if (itr.ptr_ == head_ && head_->num_elements==1) {
    head_ = head_ -> next_;
    head_ -> prev_ = 0;
    iterator result(head_, 0);
    delete itr.ptr_;
    return result;
      }
  // Removing the tail in a list if tail only has one element
  else if (itr.ptr_ == tail_ && tail_->num_elements==1) {
    tail_ = tail_ -> prev_;
    tail_ -> next_ = 0;
    delete itr.ptr_;
    return iterator(tail_, NUM_ELEMENTS_PER_NODE);
  }
    //last element in the list
  else if (itr.ptr_==tail_ && itr.ptr_->num_elements-1==itr.offset){
    tail_->num_elements--;
    return iterator(tail_, NUM_ELEMENTS_PER_NODE);
  }
  // Normal remove
  else if (itr.ptr_->num_elements==1){
    itr.ptr_ -> prev_ -> next_ = itr.ptr_ -> next_;
    itr.ptr_ -> next_ -> prev_ = itr.ptr_ -> prev_;
    iterator result(itr.ptr_ -> next_, 0);
    return result;
    delete itr.ptr_;
  }

  else{
    if (itr.offset!=itr.ptr_->num_elements-1){
      for (unsigned i=itr.offset; i<itr.ptr_->num_elements-1;i++){
        itr.ptr_->elements[i]=itr.ptr_->elements[i+1];
      }
      itr.ptr_->num_elements--;
      return itr;
    }
    else{
      itr.ptr_->num_elements--;
      iterator result(itr.ptr_ -> next_, 0);
      return result;
    } 
  }
}


// insert BEFORE the node indicated by the iterator and return an iterator to the new node
template <class T> 
typename UnrolledLL<T>::iterator UnrolledLL<T>::insert(iterator itr, T const& v) {
  ++size_ ;
  if (itr.offset==0){
    Node<T>* p = new Node<T>(v);
    p -> prev_ = itr.ptr_ -> prev_;
    p -> next_ = itr.ptr_;
    itr.ptr_ -> prev_ = p;
    if (itr.ptr_ == head_)
      head_ = p;
    else
      p -> prev_ -> next_ = p;
    return iterator(p,0);
  }
  else if(itr.ptr_->num_elements==NUM_ELEMENTS_PER_NODE){
    // node is full, move the last element and create a new node
    Node<T>* p=new Node<T>(itr.ptr_->elements[NUM_ELEMENTS_PER_NODE-1]);
    p->next_=itr.ptr_->next_;
    p->prev_=itr.ptr_;
    itr.ptr_->next_->prev_=p;
    itr.ptr_->prev_->next_=p;
    //if the point does not point to the last element in the node, move the element backwards to make space for v.
    if (itr.offset!=NUM_ELEMENTS_PER_NODE-1){
      for(unsigned int i=NUM_ELEMENTS_PER_NODE-2; i >=itr.offset; i--){
        itr.ptr_->elements[i+1]=itr.ptr_->elements[i];
      }
    }
    itr.ptr_->elements[itr.offset]=v;
    return itr;
  }
  //not insert in front of the first element nor is the node full.
  else{
      for(unsigned int i=itr.ptr_->num_elements-1; i >=itr.offset; i--){
        itr.ptr_->elements[i+1]=itr.ptr_->elements[i];
      }
      itr.ptr_->num_elements++;
    itr.ptr_->elements[itr.offset]=v;
    return itr;
  }
}


template <class T> 
void UnrolledLL<T>::copy_list(UnrolledLL<T> const & old) {
  size_ = old.size_;
  // Handle the special case of an empty list.
  if (size_ == 0) {
    head_ = tail_ = 0;
    return;
  }
  // Create a new head node. 
  head_ = new Node<T>(old.head_ -> elements);
  // tail_ will point to the last node created and therefore will move
  // down the new list as it is built
  tail_ = head_;
  // old_p will point to the next node to be copied in the old list
  Node<T>* old_p = old.head_ -> next_;
  // copy the remainder of the old list, one node at a time
  while (old_p) {
    tail_ -> next_ = new Node<T>(old_p -> elements);
    tail_ -> next_ -> prev_ = tail_;
    tail_ = tail_ -> next_;
    old_p = old_p -> next_;
  }
}


template <class T>
void UnrolledLL<T>::destroy_list() {
  Node<T>* p=head_;
  unsigned int n;
  Node<T>* temp;
  for (n=0; n<size_; n++){
    temp=p;
    if(n!=size_-1){
      p=p->next_;
    }
    delete temp;
  }
  head_=tail_=0;
  size_=0;
  //delete i.ptr_;
}

#endif
