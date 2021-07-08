// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "syscall.h"
#include "system.h"

//----------------------------------------------------------------------
// User2System
//  Copy buffer from User memory space to System memory space
//   Input:  - User space address (int)
//           - Limit of buffer (int)
//   Output: - Buffer (char*)
//----------------------------------------------------------------------
char *User2System(int virtAddr, int limit) {
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1];  // string + 1 terminal character
    if (kernelBuf == NULL)
        return kernelBuf;

    memset(kernelBuf, 0, limit + 1);

    for (int i = 0; i < limit; i++) {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}
//----------------------------------------------------------------------
// System2User
//  Copy buffer from System memory space to User memory space
//   Input:  - User space address (int)
//           - Buffer limit (int)
//           - Buffer (char[])
//   Output: - Number of bytes copied (int)
//----------------------------------------------------------------------

int System2User(int virtAddr, int len, char *buffer) {
    if (len < 0) return -1;
    if (len == 0) return len;
    int i = 0;
    int oneChar = 0;
    do {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        ++i;
    } while (i < len && oneChar != 0);
    return i;
}
//----------------------------------------------------------------------
// IncreasePC
//  Inscrease the program counter
//  Pre-PC register assigned by PC register
//  PC register assigned by Next-PC register
//  Next-PC resgister assigned by 4-byte ahead register
//----------------------------------------------------------------------

void InscreasePC() {
    machine->registers[PrevPCReg] = machine->registers[PCReg];
    machine->registers[PCReg] = machine->registers[NextPCReg];
    machine->registers[NextPCReg] += 4;
}

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which) {
    int type = machine->ReadRegister(2);

    switch (which) {
        case SyscallException: {
            switch (type) {
                case SC_Halt: {
                    printf("\nShutdown, initiated by user program.\n");
                    interrupt->Halt();
                    return;
                }
                case SC_CreateFile: {
                    int virtAddr;
                    char *filename;

                    virtAddr = machine->ReadRegister(4);
                    filename = User2System(virtAddr, MaxFileLength);

                    if (strlen(filename) == 0) {
                        printf("\n\tError: Invalid filename.");
                        machine->WriteRegister(2, FAILED);
                        delete[] filename;
                        break;
                    }

                    /*if (filename == NULL) {
                        DEBUG('a', "\n\tError: Not enough memory in system to store filename");
                        printf("\n\tError: Not enough memory in system");
                        machine->WriteRegister(2, FAILED);
                        delete[] filename;
                        break;
                    }*/

                    if (!fileSystem->Create(filename, 0)) {
                        printf("\n\tError: Error when creating file: \"%s\"", filename);
                        machine->WriteRegister(2, FAILED);
                        delete[] filename;
                        break;
                    }

                    //printf("File '%s' created successfully!\n", filename);
                    machine->WriteRegister(2, SUCCESS);
                    delete[] filename;
                    break;
                }
                case SC_Open: {
                    int virtAddr = machine->ReadRegister(4);
                    int type = machine->ReadRegister(5);

                    int freeSlot = fileSystem->FindFreeSlot();
                    if (freeSlot == -1) {
                        printf("\n\tError: Not enough space in fileDir!\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }

                    if (type != 0 && type != 1) {
                        printf("\n\tError: Invalid open file flag!\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }

                    char *filename = User2System(virtAddr, MaxFileLength);

                    if (strlen(filename) == 0) {
                        printf("\n\tError: Invalid file name!\n");
                        machine->WriteRegister(2, FAILED);
                        delete[] filename;
                        break;
                    }

                    int findPos = fileSystem->FindByName(filename);
                    if (findPos != -1){
                      printf("\n\tError: File opened at id %d!\n", findPos);
                      machine->WriteRegister(2, FAILED);
                      delete[] filename;
                      break;
                    }

                    if ((fileSystem->openFile[freeSlot] = fileSystem->Open(filename, type)) == NULL) {
                        printf("\n\tError: Unable to open file!\n");
                        machine->WriteRegister(2, FAILED);
                        delete[] filename;
                        break;
                    }

                    //printf("\"%s\" opened successfully! Id = %d\n", filename, freeSlot);
                    machine->WriteRegister(2, freeSlot);

                    delete[] filename;
                    break;
                }

                case SC_Close: {
                    OpenFileID openFileId = machine->ReadRegister(4);

                    if(openFileId < 0 || openFileId >= 10){
                      printf("\n\tError: Invalid file id!\n");
                      machine->WriteRegister(2, FAILED);
                      break;
                    }

                    //??? needed?
                    if (fileSystem->openFile[openFileId] == NULL) {
                        printf("\n\tError: Close file failed!\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }


                    //printf("\nFile with name \"%s\" closed successfully!\n", fileSystem->openFile[openFileId]->filename);
                    delete fileSystem->openFile[openFileId];
                    fileSystem->openFile[openFileId] = NULL;
                    machine->WriteRegister(2, 0);
                    break;
                }

                case SC_Read: {
                    int virtAddr = machine->ReadRegister(4);
                    int charCount = machine->ReadRegister(5);
                    int fileId = machine->ReadRegister(6);

                    if(charCount <= 0){
                      printf("\n\tError: Invalid Char Count!\n");
                      machine->WriteRegister(2, FAILED);
                      break;
                    }

                    if (fileId < 0 || fileId >= 10 || fileId == ConsoleOutput) {
                        printf("\n\tError: Invalid fileId!\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }

                    if (fileSystem->openFile[fileId] == NULL) {
                        printf("\n\tError: Requested file is not opened!\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }

                    char *buffer = User2System(virtAddr, charCount);

                    if (fileId == ConsoleInput) {
                        int bytesRead = gSynchConsole->Read(buffer, charCount);
                        System2User(virtAddr, bytesRead + 1, buffer);
                        machine->WriteRegister(2, bytesRead);
                        delete[] buffer;
                        break;
                    }

                    int before = fileSystem->openFile[fileId]->GetCurrentPos();

                    // if (before == fileSystem->openFile[fileId]->Length()) {
                    //     printf("\n\tError: End of file!\n");
                    //     machine->WriteRegister(2, -2);
                    //     break;
                    // }

                    if (fileSystem->openFile[fileId]->Read(buffer, charCount) > 0) {
                        int after = fileSystem->openFile[fileId]->GetCurrentPos();
                        int bytesRead = after - before;
                        System2User(virtAddr, bytesRead + 1, buffer);
                        machine->WriteRegister(2, bytesRead);

                    } else {
                        //EOF
                        machine->WriteRegister(2, -2);
                    }
                    //printf("%s\n", buffer);

                    delete[] buffer;
                    break;
                }

                case SC_Write: {
                    int virtAddr = machine->ReadRegister(4);
                    int charCount = machine->ReadRegister(5);
                    int fileId = machine->ReadRegister(6);

                    if(charCount <= 0){
                      printf("\n\tError: Invalid Char Count!\n");
                      machine->WriteRegister(2, FAILED);
                      break;
                    }

                    if (fileId <= 0 || fileId >= 10) {
                        printf("\n\tError: Invalid fileId!\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }

                    if (fileSystem->openFile[fileId] == NULL) {
                        printf("\n\tError: Requested file is not opened!\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }

                    if (fileSystem->openFile[fileId]->type == 1) {
                        printf("\n\tError: Permission denied! The requested file is in read-only mode!\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }

                    char *buffer = User2System(virtAddr, charCount);

                    if (fileId == ConsoleOutput) {
                        int bytesWrite = gSynchConsole->Write(buffer, charCount);
                        machine->WriteRegister(2, bytesWrite);
                        delete[] buffer;
                        break;
                    }

                    int before = fileSystem->openFile[fileId]->GetCurrentPos();

                    if (fileSystem->openFile[fileId]->Write(buffer, charCount) > 0) {
                        int after = fileSystem->openFile[fileId]->GetCurrentPos();
                        int bytesWrite = after - before;
                        machine->WriteRegister(2, bytesWrite);

                    } else {
                        //EOF
                        machine->WriteRegister(2, -2);
                    }

                    delete[] buffer;
                    break;
                }

                case SC_Seek: {
                    int pos = machine->ReadRegister(4);
                    int fileId = machine->ReadRegister(5);

                    if(fileId == ConsoleInput || fileId == ConsoleOutput){
                        printf("\n\tError: You cannot use Seek in console\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }

                    if(fileId < ConsoleInput || fileId >= 10){
                        printf("\n\tError: Invalid file id!\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }

                    if (fileSystem->openFile[fileId] == NULL) {
                        printf("\n\tError: Requested file is not opened!\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }

                    pos = pos == -1 ? fileSystem->openFile[fileId]->Length() : pos;
                    if(pos < 0 || pos > fileSystem->openFile[fileId]->Length()){
                        printf("\n\tError: Invalid seek position!\n");
                        machine->WriteRegister(2, FAILED);
                        break;
                    }
                    else {
                        fileSystem->openFile[fileId]->Seek(pos);
                        machine->WriteRegister(2, pos);
                    }
                    break;
                }

                case SC_Delete:{
                    int virtAddr = machine->ReadRegister(4);
                    char* buffer = User2System(virtAddr, MaxFileLength);

                    int findPos = fileSystem->FindByName(buffer);
                    if(findPos != -1){
                      printf("\n\tError: File in use (fileId = %d)!\n", findPos);
                      machine->WriteRegister(2, FAILED);
                      delete[] buffer;
                      break;
                    }

                    if(fileSystem->Remove(buffer)){
                      machine->WriteRegister(2, SUCCESS);
                    }
                    else {
                      printf("\n\tError: Delete failed!\n");
                      machine->WriteRegister(2, FAILED);
                    }

                    delete[] buffer;
                    break;
                }

                case SC_Scan:{
                  int virtAddr = machine->ReadRegister(4);
                  int charCount = machine->ReadRegister(5);

                  if(charCount <= 0){
                    printf("\n\tError: Invalid Limit!\n");
                    machine->WriteRegister(2, FAILED);
                    break;
                  }

                  char* buffer = User2System(virtAddr, charCount);

                  int bytesRead = gSynchConsole->Read(buffer, charCount);
                  System2User(virtAddr, bytesRead + 1, buffer);

                  delete[] buffer;
                  break;
                }

                case SC_Print:{
                  int virtAddr = machine->ReadRegister(4);

                  char* buffer = User2System(virtAddr, MaxFileLength);
                  int offset = 0;

                  while(buffer[offset] != 0){
                    gSynchConsole->Write(buffer + offset, 1);
                    ++offset;
                  }
                  gSynchConsole->Write(buffer + offset, 1);

                  delete[] buffer;
                  break;
                }
            }

            InscreasePC();
            break;
        }
        case NoException:
            interrupt->Halt();
            return;

        case PageFaultException:
            printf("\n\tError: No valid translation found.\n");
            ASSERT(FALSE);
            break;

        case ReadOnlyException:
            printf("\n\tError: Write attempted to page marked \"read-only\".\n");
            ASSERT(FALSE);
            break;

        case BusErrorException:
            printf("\n\tError: Translation resulted in an invalid physical address.\n");
            ASSERT(FALSE);
            break;

        case AddressErrorException:
            printf("\n\tError: Unaligned reference or one that was beyond the end of the address space.\n");
            ASSERT(FALSE);
            break;

        case OverflowException:
            printf("\n\tError: Integer overflow in add or sub.\n");
            ASSERT(FALSE);
            break;

        case IllegalInstrException:
            printf("\n\tError: Unimplemented or reserved instr.\n");
            ASSERT(FALSE);
            break;

        case NumExceptionTypes:
            printf("\n\tError: NumExceptionTypes\n");
            ASSERT(FALSE);
            break;
    }
    return;
}
