#include "syscall.h"
#include "copyright.h"

int main()
{
	char* buffer;
	int i = Read(buffer, 255, 0);
	if(i == 3)
	WriteFile(buffer, 255, 1);
	return 0;
}
