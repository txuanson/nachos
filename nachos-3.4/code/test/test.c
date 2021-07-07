#include "syscall.h"
#include "copyright.h"

int main(int argc, char* argv[]){
	int fileId = Open("2.txt", 0);
	char* buffer = "123456789";
	//int code = Seek(-1, fileId);
	int bytesRead = Read(buffer, 255, fileId);
	Write(buffer, bytesRead, ConsoleOutput);
	//if(bytesRead == -2)
	CloseFile(fileId);
	return 0;
}
