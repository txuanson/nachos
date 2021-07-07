#include "syscall.h"
#include "copyright.h"

int main(int argc, char* argv[]){
	char* filename = "";

  Print("Input filename: ");
  Scan(filename, MaxFileLength);
  Delete(filename);
  
	return 0;
}
