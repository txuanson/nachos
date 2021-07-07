#include "syscall.h"
#include "copyright.h"

int main(int argc, char* argv[]){
	int i = CreateFile("2.txt");
	OpenFileID id = Open("2.txt", 0);

	CloseFile(id);
	return 0;
}
