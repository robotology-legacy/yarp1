
#ifndef FileProcessor_INC
#define FileProcessor_INC

#include <fstream>
#include <sstream>

#define FPMAXARG 10
#define FPMAXLEN 256


class FileProcessor
{
public:
  virtual void Apply(int argc, char *argv[])
    {
    }

  void Process(const char *fname)
    {
      std::ifstream fin(fname);
      char buf[FPMAXLEN];
      int savable = 0;
      while ((!fin.eof()) && (!fin.bad()))
	{
	  fin.getline(buf,sizeof(buf));
	  if (!fin.eof())
	    {

	      savable = 1;
	      //printf("{%s}\n", buf);
	      char bufs[FPMAXARG][FPMAXLEN];
	      char *bufs2[FPMAXARG];
	      bufs2[0] = "";
	      std::istringstream sin(buf);
	      int index = 1;
	      while ((!sin.eof()) && (!sin.bad()) && index<FPMAXARG)
		{
		  bufs[index][0] = '\0';
		  sin >> (&bufs[index][0]);
		  bufs2[index] = (&bufs[index][0]);
		  if (!sin.bad())
		    {
		      if (bufs[index][0]!='\0')
			{
			  index++;
			}
		    }
		}
#if 1
	      printf("Call: ");
	      for (int i=1; i<index; i++)
		{
		  printf("[%s] ", bufs[i]);
		}
	      printf("\n");
#endif
	      Apply(index, bufs2);
	    }
	}
    }
};


#endif
