#include "syscall.h"
#include "copyright.h"

int main()
{
	OpenFileID fileId = Open("2.txt", 0);

	char* buffer;
	int i = Read(buffer, 255, fileId);
	//if(i == 3)
	WriteFile(buffer, 255, 1);
	return 0;
}
