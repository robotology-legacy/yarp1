#include "YARPBottle.h"

YARPBottle::YARPBottle()
{
	id.set(YBLabelNULL);
	top = 0;
	index = 0;
	len = 0;
	lastReadSeq = 0;
}

void YARPBottle::dump()
{
	cout << "Starting dump of internal string\n";
	cout << "Seq length:" << text.size() << endl;
	YARPVector<char>::iterator it(text);
	while (!it.done())
	{
		printf("%d\n", *it);
		it++;
	}
}

void YARPBottle::display()
{
	ACE_OS::printf("%s: ", id.c_str());
	rewind();
    int first = 1;
    while (more())
	{
	  char *str;
	  int ch = readRawInt();
	  _moveOn(sizeof(int));
	  if (!first)
	    {
	      printf(" ");
	    }
	  first = 0;
	  switch(ch)
	    {
	    case YBTypeInt:
	      printf("%d", readRawInt());
		  _moveOn(sizeof(int));
	      break;
	    case YBTypeVocab:
	      str = (char *) readRawText();
	      printf("<%s>", str);
		  _moveOn(strlen(str)+1+sizeof(int));
	      break;
	    case YBTypeDouble:
	      printf("%g", readRawFloat());
		  _moveOn(sizeof(double));
	      break;
	    case YBTypeString:
		  str = (char *) readRawText();
	      printf("(%s)", str);
		  _moveOn(strlen(str)+1+sizeof(int));
	      break;
		case YBTypeYVector:
		{
		  index = index-sizeof(int);
		  YVector tmp;
		  tryReadYVector(tmp);
		  int i;
		  printf("<");
		  for(i = 1; i < tmp.Length(); i++)
		  {
			  printf("%g ", tmp(i));
		  }
		  printf("%g>", tmp(i)); // last item
		  moveOn();
		  break;
		}
	    default:
	      printf("???");
	      break;
	    }
	 }
	printf("\n");
	rewind();
}

