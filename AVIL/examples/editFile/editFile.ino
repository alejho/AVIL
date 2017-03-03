#include <avil.h>
 
avil interpreter;
 
void setup(void) {
  
  if(!interpreter.init()){
     //something wrong during initialization...want to do something? 
  }

   //place a file called "foo" (with at least 5 files of content)on your sd card
 
   //run the program "editl.av" on sd
   interpreter.run("editl.av \"foo\" 5");

   //you should now see the line number 5 of "foo" changed
}
 
 
void loop(void) {
   //nothing to do here...
}
