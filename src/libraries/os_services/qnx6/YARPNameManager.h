#include <stdlib.h>
//#include <sys/name.h>
#include <sys/dispatch.h> //+QNX6+
#include <errno.h>
#include <string.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include <stdio.h>
#include <sys/iofunc.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <fstream.h>
#include <sys/netmgr.h>
#include <unistd.h>


#define FILE_NAME "/net/server/home/cbeltran/register"

class YARPRegisteredName
{
	private:
		char 	_name[250];
		char	_nodo_name[250];
		int	_proc_id;
		int	_chid;
	public:
		YARPRegisteredName()
		{
			_proc_id = 0;
			_chid	 = 0;
		}
		YARPRegisteredName(const char * name, char * nodo_name, int proc_id, int chid)
		{
			strcpy(_name,name);
			strcpy(_nodo_name,nodo_name);
			_proc_id = proc_id;
			_chid 	 = chid;
		}
		
		YARPRegisteredName(YARPRegisteredName &n)
		{
			strcpy(_name,n._name);
			strcpy(_nodo_name,n._nodo_name);
			_proc_id = n._proc_id;
			_chid = n._chid;
		}
		
		int IsValid()
		{
			return ( _proc_id && _chid );
		}
		
		int Compare(const char * name)
		{
			if ( strcmp(_name,name) == 0)
				return 1;
			else return 0;
		}
		
		char * GetName()
		{
				return(_name);
		}
		
		void SetName(char * name)
		{
			strcpy(_name,name);
		}
		void SetNodo(char *nodo)
		{
			strcpy(_nodo_name,nodo);
		}
		void SetProcId(int proc_id)
		{
			_proc_id = proc_id;
		}
		void SetChId(int chid)
		{
			_chid = chid;
		}
		
		char * GetNode()
		{
			return _nodo_name;
		}
		
		int GetProcId()
		{
			return _proc_id;
		}
		
		int GetChId()
		{
			return _chid;
		}
		
		YARPRegisteredName& operator = (const YARPRegisteredName & n)
		{
			strcpy(_name,n._name);
			strcpy(_nodo_name,n._nodo_name);
			_proc_id = n._proc_id;
			_chid = n._chid;
		}

		
		friend istream& operator >> (istream& s, YARPRegisteredName & n)
		{
		
			s >> n._name 
			  >> n._nodo_name 
			  >> n._proc_id 
			  >> n._chid;
			
		}
		friend ostream& operator << (ostream& s, const YARPRegisteredName & n)
		{
			s << n._name   		<< " "
			  << n._nodo_name 	<< " "
			  << n._proc_id  	<< " " 
			  << n._chid 	   	<< endl;
			 
		}
		
};

struct item
{
	YARPRegisteredName * name_data;
	item * next;
};

class YARPNameManager
{
	private:
		item * header;
		item * tail;
		bool save_to_file;
		char file_name[250];
	public:
		YARPNameManager()
		{
			header = NULL;
			tail   = NULL;
			save_to_file = false;
		}
		
		YARPNameManager(char * name)
		{
			fstream f;
			YARPRegisteredName rname;
			
			header = NULL;
			tail   = NULL;
			save_to_file = true;
			strcpy(file_name,name);
			
			f.open(name, ios::in | ios::nocreate);
			
	        while (1 && f.is_open())
	        {
	        	f >> rname;
	        	
	        	if (f.eof()) 
	        		break;	
	        	Add(rname);
	        }
	         			
        	f.close();
        	
		}
		
		~YARPNameManager()
		{
			SaveToFile();
			DeleteAll();
		}
		
		void Add(YARPRegisteredName &rname)
		{
			YARPRegisteredName * newname = new YARPRegisteredName(rname);
			item * newitem = new item;
			
			newitem->name_data = newname;
			newitem->next = NULL;
			Delete(rname);
			if (tail != NULL)
				tail->next = newitem;
			if (header == NULL)
				header = newitem; 
			tail = newitem;
			
			//cout << "Adding: "<< *newname;
		}
		
		item * FindName(char * name)
		{
			item * index = header;
			
			while (index != NULL)
			{
				if (index->name_data->Compare(name))
					return index;
				index = index->next;
			}
			return NULL;
		}
		
		int FindName(char * name, YARPRegisteredName & n )
		{
			item * index = header;
			
			while (index != NULL)
			{
				if (index->name_data->Compare(name))
				{	
					n = *(index->name_data);
					return 1;
				}	
				index = index->next;
			}
			return 0;
		}
		
		void List()
		{
			item * index = header;
			
			while (index != NULL)
			{
				cout << *(index->name_data);
				index = index->next;
			}
		}
		
		void List(iostream & stream)
		{
			item * index = header;
			
			while (index != NULL)
			{
				stream << *(index->name_data);
				index = index->next;
			}
		}
		
		void Delete(YARPRegisteredName &rname)
		{
			item * to_be_deleted = NULL;
			item * index 		 = NULL;
			
			to_be_deleted = FindName(rname.GetName());
			
			if ( to_be_deleted != NULL)
			{
				if (to_be_deleted == header) 
					header = header->next; 
				else
				{
					index = header;
					while(index->next != to_be_deleted)
						index = index->next;
					index->next = to_be_deleted->next;	
					if (index->next == NULL) //It is the last one
						tail = index;					
				}
			    //cout << "Deleting: " << *(to_be_deleted->name_data);
				delete to_be_deleted->name_data;
				delete to_be_deleted;
			}
		}
		
		void SaveToFile()
		{
			fstream f;
			
			if (save_to_file)
			{
				f.open(file_name, ios::out | ios::trunc); 
				
				List(f);
				f.close();	
			}
		}
		
		void DeleteAll()
		{	
			while (header != NULL)
				Delete(*(header->name_data));
		}
		
};