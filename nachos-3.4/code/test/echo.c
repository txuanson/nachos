#include "syscall.h"
#include "copyright.h"

int main(int argc, char* argv[])
{
	char* content = "";

	Print("Input something: ");
	Scan(content, MaxFileLength);
  Print(content);

	return 0;
}
