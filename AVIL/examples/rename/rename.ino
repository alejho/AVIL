#include <avil.h>
 
avil interpreter;
 
void setup(void) {
  
  if(!interpreter.init()){
     //something wrong during initialization...want to do something? 
  }

   //place a file called "foo" on your sd card
 
   //run the program "rename.av" on sd
   interpreter.run("rename.av \"foo\" \"fooRenamed\"");

   //you should see a new file called "fooRenamed" while "foo" doesn't exist anymore
}
 
 
void loop(void) {
   //nothing to do here...
}
