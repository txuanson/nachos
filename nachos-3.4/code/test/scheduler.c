#include "syscall.h"
void main(){
  int pingPID, pongPID;
  PrintString("Ping-Pong test starting ...\n\n");
  pingPID = Exec("./test/ping", 4);
  pongPID = Exec("./test/pong", 4);
  while (1) {}
}
