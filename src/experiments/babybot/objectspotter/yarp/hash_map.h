#ifndef HASH_MAP_HACK
#define HASH_MAP_HACK


/*
#include "ace/Hash_Map_Manager.h"

#define first ext_id_
#define second int_id_

template <class t1, class t2, class t3, class t4>
class hash_map : public ACE_Hash_Map_Manager_Ex<t1, t2, t3, t4, ACE_Null_Mutex>
{
	public:
		t2& operator[](const t1&i) { 
				//return (*this)[i].value_; 
			//const t2 *entry;
			ACE_Hash_Map_Entry<t1, t2> *entry;
	        if (ACE_Hash_Map_Manager_Ex<t1, t2, t3, t4, ACE_Null_Mutex>::find (i,
                      entry) == 0)
			{
				return (*entry).int_id_;
			}
			bind(i,t2());
			ACE_Hash_Map_Manager_Ex<t1, t2, t3, t4, ACE_Null_Mutex>::find (i, entry);
			return (*entry).int_id_;
		}

		//ACE_Hash_Map_Iterator_Ex<t1,t2,t3,t4, ACE_Null_Mutex> find(const t1& val) {
			
			ACE_Hash_Map_Entry<t1, t2> *entry;
	        if (ACE_Hash_Map_Manager_Ex<t1, t2, t3, t4, ACE_Null_Mutex>::find (val,
                      entry) == 0)
			{
				for (ACE_Hash_Map_Iterator_Ex<t1,t2,t3,t4, ACE_Null_Mutex> it = begin(); it!=end(); it++)
				{
					if ((*it).first == val)
					{
						return it;
					}
				}
			}
			return ACE_Hash_Map_Iterator_Ex<t1,t2,t3,t4, ACE_Null_Mutex>(*this);
			
		//}

};

template <class T>
class hash : public ACE_Hash<T>
{
};

#define equal_to ACE_Equal_To

// this could fail badly
//#define first key_
//#define second value_
*/

//#include "my_pair.h"

#include <yarp/YARPNetworkTypes.h>
#include <ace/Hash_Map_Manager.h>

#include <utility>

#ifndef __int32
#define __int32 NetInt32
#endif

template <class _Key> struct myhash { };

inline size_t my__stl_hash_string(const char* __s)
{
  unsigned long __h = 0; 
  for ( ; *__s; ++__s)
    __h = 5*__h + *__s;
  
  return size_t(__h);
}

struct myhash<char*>
{
  size_t operator()(const char* __s) const { return my__stl_hash_string(__s); }
};

struct myhash<const char*>
{
  size_t operator()(const char* __s) const { return my__stl_hash_string(__s); }
};

struct myhash<char> {
  size_t operator()(char __x) const { return __x; }
};

struct myhash<unsigned char> {
  size_t operator()(unsigned char __x) const { return __x; }
};
struct myhash<signed char> {
  size_t operator()(unsigned char __x) const { return __x; }
};
struct myhash<short> {
  size_t operator()(short __x) const { return __x; }
};
struct myhash<unsigned short> {
  size_t operator()(unsigned short __x) const { return __x; }
};
//cck 20030814
//struct myhash<int> {
  //  size_t operator()(int __x) const { return __x; }
//};
struct myhash<__int32> {
  size_t operator()(__int32 __x) const { return __x; }
};
//struct myhash<__int64> {
//  size_t operator()(__int64 __x) const { return __x; }
//};
struct myhash<unsigned int> {
  size_t operator()(unsigned int __x) const { return __x; }
};
struct myhash<long> {
  size_t operator()(long __x) const { return __x; }
};
struct myhash<unsigned long> {
  size_t operator()(unsigned long __x) const { return __x; }
};

template <class T>
//class hash : public ACE_Hash<T>
class hash : public myhash<T>
{
	public:
  int SecondHashValue(const int & key)const {
    return 2*operator()(3*key) + 1; //seems really dumb
  }
};


#include "my_hash_map.h"

#define hash_map my_hash_map

#endif
