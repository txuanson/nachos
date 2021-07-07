#include "syscall.h"
#include "copyright.h"

int main(int argc, char* argv[]) {

	char* srcFile = "", * destFile = "",* content = "";
  OpenFileID srcId, destId;
  int bytesRead, fileSize, offset;

	Print("Input source filename: ");
	Scan(srcFile, MaxFileLength);
  srcId = Open(srcFile, 1); // read-only

  Print("Input dest filename: ");
	Scan(destFile, MaxFileLength);
  CreateFile(destFile);
  destId = Open(destFile, 0); // read-write

  if( srcId != -1 && destId != -1) {
    fileSize = Seek(-1, srcId);
    offset = Seek(0, srcId);

    while (offset < fileSize) {
      bytesRead = Read(content, 1, srcId);
      Write(content, 1, destId);
      ++offset;
    }

    CloseFile(srcId);
    CloseFile(destId);
  }

	return 0;
}
