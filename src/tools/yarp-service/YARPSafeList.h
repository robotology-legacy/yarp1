#ifndef __YARP_SAFE_LIST__
#define __YARP_SAFE_LIST__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Containers_T.h>

//
// from YARPList.h

/**
 * \file YARPSafeList.h a STL-like list class derived from ACE List container.
 */

/**
 * the YARPSafeList iterator. The main difference with C++ STL
 * is that the iterator here is initialized for a specific
 * object and keeps a pointer to it internally.
 */
template <class T>
class YARPSafeListIterator : public ACE_DLList_Iterator<T>
{
public:
	/**
	 * Constructor. Creates a new iterator for a specific instance of YARPSafeList.
	 * @param i an ACE_DLList reference to link the iterator to.
	 */
	YARPSafeListIterator(ACE_DLList<T>& i) : ACE_DLList_Iterator<T>(i) {}

	/**
	 * Constructor. Creates a new iterator for a specific instance of YARPSafeList: same
	 * as above but const parameter.
	 * @param i an ACE_DLList reference to link the iterator to.
	 */
	YARPSafeListIterator(const YARPSafeListIterator& i) : ACE_DLList_Iterator<T>(i.dllist_) {}

	/**
	 * Destructor.
	 */
	~YARPSafeListIterator() {}

	/**
	 * move the iterator to the beginning of the list.
	 */
	int go_head(void) { return ACE_DLList_Iterator<T>::go_head(); }

	/**
	 * move the iterator to the end of the list.
	 */
	int go_tail(void) { return ACE_DLList_Iterator<T>::go_tail(); }

	/**
	 * dereferencing operator.
	 * @return a reference to the current item pointed to by the iterator.
	 */
	T& operator *() const { return *(T*)(ACE_DLList_Iterator<T>::operator*().item_); }
};

/**
 * A generic double-linked templatized list derived from the ACE correspondent list.
 */
template <class T>
class YARPSafeList : public ACE_DLList<T>
{
public:
	/**
	 * defines the iterator to be class YARPSafeList<T>::iterator
	 */
	typedef YARPSafeListIterator<T> iterator;

	/**
	 * defines the constant iterator to be class YARPSafeList<T>::const_iterator
	 */
	typedef const YARPSafeListIterator<T> const_iterator;

	friend class YARPSafeListIterator<T>;

	/**
	 * Constructor.
	 */
	YARPSafeList(void) : ACE_DLList<T> () {}

	/**
	 * Copy constructor.
	 * @param l is a reference to an instance of the same class type.
	 */
	YARPSafeList(const YARPSafeList<T>& l) 
    {
        // call destructors for each element
        YARPSafeList<T> *tmp=const_cast<YARPSafeList<T>*>(&l);
        YARPSafeList::iterator it (*tmp);
        for(;!it.done();it++)
        {
            push_back(*it);    
        }
    }
	
	/**
	 * Copy operator from the ACE list.
	 * @param l is a reference to an ACE double linked list.
	 */
	void operator= (const YARPSafeList<T> &l) 
    { 
        clear();
        
        YARPSafeList<T> *tmp=const_cast<YARPSafeList<T>*>(&l);
        YARPSafeList::iterator it (*tmp);
        for(;!it.done();it++)
        {
            push_back(*it);    
        }
    }

    /**
	 * Destructor.
	 */
	~YARPSafeList() { clear(); }

	/**
	 * Adds a new item to the tail of the list.
	 * @param new_item is a reference to the element type of the list.
	 */
	void push_back (const T& new_item)
    { 
        T* el = new T; 
        *el = new_item; 
        insert_tail(el); 
    }

	/**
	 * Adds a new item to the head of the list.
	 * @param new_item is a reference to the element type of the list.
	 */
	void push_front (const T& new_item) 
    { 
        T* el = new T; 
        *el = new_item; 
        insert_head(el); 
    }

	/**
	 * Removes the last element of the list. 
	 */
	void pop_back (void) 
    { 
        T* el = ACE_DLList<T>::delete_tail(); 
        delete el; 
    }

	/**
	 * Removes the first element of the list.
	 */
	void pop_front (void) 
    { 
        T* el = ACE_DLList<T>::delete_head(); 
        delete el; 
    }

	/**
	 * Erases the element pointed by the iterator.
	 * @param it is the iterator pointing to the element to be removed.
	 & @return negative on failure.
	 */
	int erase(iterator &it)	
    { 
        T *tmp=&(*it);
        int ret=it.remove();
        if (ret>=0)
        {
            delete tmp;
        }
        return ret;
    }

	/**
	 * Checks whether the list is empty.
	 * @return true if it's empty.
	 */
	bool empty() { return (ACE_DLList<T>::size() == 0); }

	/**
	 * Clears the list and frees memory.
	 */
	void clear() 
    { 
        // call destructors for each elements
        YARPSafeList<T>::iterator it (*this);
        for(;!it.done();it++)
        {
            T *tmp=&(*it);
            delete tmp;
        }

        ACE_DLList<T>::reset(); 
    }
};



#endif