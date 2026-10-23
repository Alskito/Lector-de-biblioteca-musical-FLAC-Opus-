#pragma once
 
#include <string>
#include <sstream>
#include "exception.h"
 
template <class T> class DList;
template <class T> class DListIterator;
 
template <class T>
class DLink {
private:
	DLink(T);
	DLink(T, DLink<T>*, DLink<T>*);
	DLink(const DLink<T>&);
 
	T	    value;
	DLink<T> *previous;
	DLink<T> *next;
 
	friend class DList<T>;
	friend class DListIterator<T>;
};
 
template <class T>
DLink<T>::DLink(T val) : value(val), previous(0), next(0) {}
 
template <class T>
DLink<T>::DLink(T val, DLink *prev, DLink* nxt) : value(val), previous(prev), next(nxt) {}
 
template <class T>
DLink<T>::DLink(const DLink<T> &source) : value(source.value), previous(source.previous), next(source.next) {}
 
template <class T>
class DList {
public:
	DList();
	DList(const DList<T>&);
	~DList();
 
	void addFirst(T);
	void add(T);
	T getFirst() const ;
	T removeFirst() ;
	T getLast() const ;
	T removeLast() ;
	int length() const;
	T get(int) const;
	bool contains(T) const;
	bool empty() const;
	void clear();
	std::string toString() const;
	void operator= (const DList&);
 
	void addBefore(DListIterator<T>&, T);
	void addAfter(DListIterator<T>&, T);
	T removeCurrent(DListIterator<T>&);
 
	bool set(int, T);
	int indexOf(T) const;
	int lastIndexOf(T) const;
	T remove(int);
	bool removeFirstOcurrence(T);
	bool removeLastOcurrence(T);
 
private:
	DLink<T> *head;
	DLink<T> *tail;
	int 	 size;
 
	// Private helpers
	DLink<T>* linkAt(int) const;
	T         removeLink(DLink<T>*);
	void      copy(const DList<T>&);
 
	friend class DListIterator<T>;
};
 
template <class T>
DList<T>::DList() {
	head = NULL;
	tail = NULL;
	size = 0;
}
 
template <class T>
DList<T>::~DList() {
	clear();
}
 
template <class T>
bool DList<T>::empty() const {
	return size == 0;
}
 
template <class T>
int DList<T>::length() const {
	return size;
}
 
template <class T>
bool DList<T>::contains(T val) const {
	DLink<T> *current;
 
	current = head;
	while (current != 0) {
		if (current->value == val) {
			return true;
		}
		current = current->next;
	}
	return false;
}
 
template <class T>
DLink<T>* DList<T>::linkAt(int index) const {
	int pos;
	DLink<T> *current;
 
	if (index < size / 2) {
		current = head;
		pos = 0;
		while (pos != index) {
			current = current->next;
			pos++;
		}
	} else {
		current = tail;
		pos = size - 1;
		while (pos != index) {
			current = current->previous;
			pos--;
		}
	}
	return current;
}
 
template <class T>
T DList<T>::removeLink(DLink<T> *victim) {
	T val = victim->value;
 
	if (victim->previous != 0) {
		victim->previous->next = victim->next;
	} else {
		head = victim->next;
	}
 
	if (victim->next != 0) {
		victim->next->previous = victim->previous;
	} else {
		tail = victim->previous;
	}
 
	delete victim;
	size--;
	return val;
}
 
template <class T>
void DList<T>::copy(const DList<T> &source) {
	DLink<T> *current;
 
	head = NULL;
	tail = NULL;
	size = 0;
 
	current = source.head;
	while (current != 0) {
		add(current->value);
		current = current->next;
	}
}
 
template <class T>
T DList<T>::getFirst() const  {
	if (empty()) {
		throw NoSuchElement();
	}
	return head->value;
}
 
template <class T>
T DList<T>::getLast() const  {
	if (empty()) {
		throw NoSuchElement();
	}
	return tail->value;
}
 
template <class T>
void DList<T>::addFirst(T val)  {
 
	DLink<T> * newLink = new DLink<T>(val);
 
	if (newLink == NULL) {
		throw OutOfMemory();
	}
 
	if (empty()) {
		head = newLink;
		tail = newLink;
	} else {
		head->previous = newLink;
		newLink->next = head;
		head = newLink;
	}
 
	size++;
}
 
template <class T>
void DList<T>::add(T val)  {
	if (empty()) {
		addFirst(val);
	} else {
		DLink<T> * newLink = new DLink<T>(val);
		if (newLink == NULL) {
			throw OutOfMemory();
		}
 
		newLink->previous = tail;
		tail->next = newLink;
		tail = newLink;
		size++;
	}
 
}
 
template <class T>
T DList<T>::removeFirst()  {

	if(empty()) {
		throw NoSuchElement();
	}
 
	DLink<T> * victim = head;
	T result = victim->value;
 

	if (head == tail) {
		head = NULL;
		tail = NULL;
		size--;

	} else {
		head = head->next;
		victim->next = NULL;
		head->previous = NULL;
		size--;
	}
 
	delete victim;
	return result;
}
 
template <class T>
T DList<T>::removeLast()  {
	if (empty()) {
		throw NoSuchElement();
	}
 
	return removeLink(tail);
}
 
template <class T>
T DList<T>::get(int index) const {
	if (index < 0 || index >= size) {
		throw IndexOutOfBounds();
	}
 
	return linkAt(index)->value;
}
 
template <class T>
void DList<T>::clear() {
	DLink<T> *current, *aux;
 
	current = head;
	while (current != 0) {
		aux = current->next;
		delete current;
		current = aux;
	}
	head = 0;
	tail = 0;
	size = 0;
}
 
template <class T>
std::string DList<T>::toString() const {
	std::stringstream aux;
	DLink<T> *current;
 
	current = head;
	aux << "[";
	while (current != 0) {
		aux << current->value;
		if (current->next != 0) {
			aux << ", ";
		}
		current = current->next;
	}
	aux << "]";
	return aux.str();
}
 
template <class T>
DList<T>::DList(const DList<T> &source)  {
	copy(source);
}
 
template <class T>
void DList<T>::operator=(const DList<T> &source)  {
	if (this == &source) {
		return;
	}
 
	clear();
	copy(source);
}
 
template <class T>
void DList<T>::addBefore(DListIterator<T> &itr, T val) {
	if (itr.current == 0) {
		throw NoSuchElement();
	}
 
	if (itr.current == head) {
		addFirst(val);
		itr.previous = head;
	} else {
		DLink<T> * newLink = new DLink<T>(val, itr.current->previous, itr.current);
		if (newLink == NULL) {
			throw OutOfMemory();
		}
 
		itr.current->previous->next = newLink;
		itr.current->previous = newLink;
		itr.previous = newLink;
		size++;
	}
}
 
template <class T>
void DList<T>::addAfter(DListIterator<T> &itr, T val) {
	if (itr.current == 0) {
		throw NoSuchElement();
	}
 
	if (itr.current == tail) {
		add(val);
	} else {
		DLink<T> * newLink = new DLink<T>(val, itr.current, itr.current->next);
		if (newLink == NULL) {
			throw OutOfMemory();
		}
 
		itr.current->next->previous = newLink;
		itr.current->next = newLink;
		size++;
	}
}
 
template <class T>
T DList<T>::removeCurrent(DListIterator<T> &itr) {
	if (itr.current == 0) {
		throw NoSuchElement();
	}
 
	DLink<T> * victim = itr.current;
	itr.current = victim->next;
	itr.previous = victim->previous;
 
	return removeLink(victim);
}
 
template <class T>
bool DList<T>::set(int index, T val)  {
	if (index < 0 || index >= size) {
		throw IndexOutOfBounds();
	}
 
	linkAt(index)->value = val;
	return true;
}
 
template <class T>
int DList<T>::indexOf(T val) const {
	int index;
	DLink<T> *current;
 
	index = 0;
	current = head;
	while (current != 0) {
		if (current->value == val) {
			return index;
		}
		index++;
		current = current->next;
	}
	return -1;
}
 
template <class T>
int DList<T>::lastIndexOf(T val) const {
	int index;
	DLink<T> *current;
 
	index = size - 1;
	current = tail;
	while (current != 0) {
		if (current->value == val) {
			return index;
		}
		index--;
		current = current->previous;
	}
	return -1;
}
 
template <class T>
T DList<T>::remove(int index)  {
	if (index < 0 || index >= size) {
		throw IndexOutOfBounds();
	}
 
	return removeLink(linkAt(index));
}
 
template <class T>
bool DList<T>::removeFirstOcurrence(T val) {
	DLink<T> *current;
 
	current = head;
	while (current != 0) {
		if (current->value == val) {
			removeLink(current);
			return true;
		}
		current = current->next;
	}
	return false;
}
 
template <class T>
bool DList<T>::removeLastOcurrence(T val) {
	DLink<T> *current;
 
	current = tail;
	while (current != 0) {
		if (current->value == val) {
			removeLink(current);
			return true;
		}
		current = current->previous;
	}
	return false;
}
 
template <class T>
class DListIterator {
public:
	DListIterator(DList<T>*);
	DListIterator(const DListIterator<T>&);
 
	bool begin();
	bool end();
	T& operator() () ;
	bool operator++ ();
	bool operator-- ();
	void operator= (T) ;
 
private:
	DLink<T> *current;
	DLink<T> *previous;
	DList<T> *theList;
 
	friend class DList<T>;
};
 
template <class T>
DListIterator<T>::DListIterator(DList<T> *aList) : theList(aList) {
	begin();
}
 
template <class T>
DListIterator<T>::DListIterator(const DListIterator<T> &source) : theList(source.theList) {
	begin();
}
 
template <class T>
bool DListIterator<T>::begin() {
	previous = 0;
	current = theList->head;
	return (current != 0);
}
 
template <class T>
T& DListIterator<T>::operator() ()  {
	if (current == 0) {
		throw NoSuchElement();
	}
	return current->value;
}
 
template <class T>
bool DListIterator<T>::end() {
	if (current == 0) {
		if (previous != 0) {
			current = previous->next;
		}
	}
	return (current == 0);
}
 
template <class T>
bool DListIterator<T>::operator++ () {
	if (current == 0) {
		if (previous == 0) {
			current = theList->head;
		} else {
			current = previous->next;
		}
	} else {
		previous = current;
		current = current->next;
	}
	return (current != 0);
}
 

template <class T>
bool DListIterator<T>::operator-- () {
	if (current == 0) {
		current = previous;
	} else {
		current = current->previous;
	}
 
	if (current != 0) {
		previous = current->previous;
	} else {
		previous = 0;
	}
	return (current != 0);
}
 
template <class T>
void DListIterator<T>::operator= (T val)  {
	if (current == 0) {
		throw NoSuchElement();
	}
	current->value = val;
}
