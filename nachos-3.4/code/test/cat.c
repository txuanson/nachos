#include "syscall.h"
#include "copyright.h"

int main(int argc, char* argv[]) {

	char* filename = "",* content = "";
  OpenFileID fileId;
  int bytesRead, fileSize, offset;

	Print("Input filename: ");
	Scan(filename, MaxFileLength);

  fileId = Open(filename, 1); // read-only
  if( fileId != -1 ) {
    fileSize = Seek(-1, fileId);
    offset = Seek(0, fileId);

    while(offset < fileSize){
      bytesRead = Read(content, 1, fileId);
      Print(content);
      ++offset;
    }
    CloseFile(fileId);
  }

	return 0;
}
