#include "yarp/YARPDebugUtils.h"

using namespace YARPDebugUtils;

void YARPDebugUtils::print(const YVector &v, const char *prefix)
{
	if (prefix != NULL)
		ACE_OS::printf("%s ", prefix);

	const int n = v.Length();

	ACE_OS::printf("<");
	if (n == 0)
	{
		ACE_OS::printf("empty");
	}
	else
	{
		int k = 1;
		for(k = 1; k <= n-1; k++)
		{
			ACE_OS::printf("(%.3lf) ", v(k));
		}
		ACE_OS::printf("(%.3lf)", v(k));
	}
	ACE_OS::printf(">\n");
}
