#ifndef __YBLABELCONVERTER__
#define __YBLABELCONVERTER__

#include <YARPList.h>
#include <YARPString.h>

class YARPBehaviorLabelConverter
{
public:
	struct YBLabelConverterEntry
	{
		YARPString	descriptor;
		YARPString	verbose;
		int			value;
	};

	typedef YARPList<YBLabelConverterEntry> ListEntries;
	typedef ListEntries::iterator ListEntriesIterator;

	YARPBehaviorLabelConverter();
	~YARPBehaviorLabelConverter();

	int read(const YARPString &path, const YARPString &fileName);

	int convertToValue(const YARPString &descriptor)
	{
		if (descriptor == _label.descriptor)
			return _label.value;

		ListEntriesIterator it(_list);
		while(!it.done())
		{
			if ((*it).descriptor == descriptor)
			return (*it).value;
			it++;
		}

		ACE_ASSERT(0);	// shouldn't be getting here control file
		return -1;
	}

	const char *convertToVerbose(const YARPString &descriptor)
	{
		if (descriptor == _label.descriptor)
			return _label.verbose.c_str();

		ListEntriesIterator it(_list);
		while(!it.done())
		{
			if ((*it).descriptor == descriptor)
			return (*it).verbose.c_str();
			it++;
		}

		ACE_ASSERT(0);	// shouldn't be getting here control file
		return NULL;
	}
	
	void dump();

private:
	int _skip(FILE *fp, const YARPString &str);
	int _refresh();
	int _readFullString(FILE *fp, char *s, int max);

	YARPString _fileName;
	YARPString _path;
	ListEntries _list;
	YBLabelConverterEntry _label;
};



#endif