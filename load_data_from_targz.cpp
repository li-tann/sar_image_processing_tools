#include <iostream>
#include <string>

#include <zlib.h>

using namespace std;
int main()
{
	gzFile gzfp = gzopen("test.gz","rb");
	if(NULL == gzfp)
	{
		cout<<"Fail to open file."<<endl;
		return -1;
	}
 
	return 0;

}