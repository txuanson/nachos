#include "syscall.h"
#include "copyright.h"

int main()
{
	CreateFile("2.txt");
	CreateFile("3.txt");
	CreateFile("4.txt");
	CreateFile("5.txt");
	CreateFile("6.txt");
	CreateFile("7.txt");
	CreateFile("8.txt");
	CreateFile("9.txt");
	CreateFile("10.txt");

	Open("2.txt", 0);
	Open("3.txt", 0);
	Open("4.txt", 0);
	Open("5.txt", 0);
	Open("6.txt", 0);
	Open("7.txt", 0);
	Open("8.txt", 0);
	Open("9.txt", 0);
	CloseFile(9);
	Open("10.txt", 0);
	return 0;
}
