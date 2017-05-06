#include <avil.h>
#include <sys.h>


avil interpreter;
char cmd[30];

void setup(void) {
  if(!interpreter.init()){
      Sys::userOutput(F("\n\rinit error\n\r"));
      while(1);
  }
  cmd[0]='\0';
}


void loop(void) {

  Sys::userOutput(F("\n\r>"));
  //get a user input
  if(Sys::userInput(cmd, 30)){
      Sys::userOutput(F("\n\r"));
      interpreter.run(cmd);
  }
  else{
      Sys::userOutput(F("bad input!\n"));
  }  
}


  
