#include "YARPBottle.h"

YARPBottle::YARPBottle()
{
	id = 0;
	top = 0;
	index = 0;
	len = 0;
	lastReadSeq = 0;
}

void YARPBottle::dump()
{
	cout << "Starting dump of internal string\n";
	cout << "Seq length:" << text.size() << endl;
	std::vector<char>::iterator it = text.begin();
	while (it != text.end())
	{
		printf("%d\n", *it);
		it++;
	}
}

void YARPBottle::display()
{
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
	      printf("%s", ybc_label(readRawInt()));
		  _moveOn(sizeof(int));
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

