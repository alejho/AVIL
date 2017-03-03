#include <avil.h>
 
avil interpreter;
 
void setup(void) {
  
  if(!interpreter.init()){
     //something wrong during initialization...want to do something? 
  }
 
   //run the program "button.av" on sd
   interpreter.run("button.av");
}
 
 
void loop(void) {
   //nothing to do here...
}
