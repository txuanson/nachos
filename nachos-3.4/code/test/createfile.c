#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255
#define STDIN 0
#define STDOUT 1

int main(int argc, char* argv[])
{
	CreateFile("testfile.txt");
	return 0;
}