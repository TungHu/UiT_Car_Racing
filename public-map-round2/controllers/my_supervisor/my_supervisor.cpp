#include <webots/Supervisor.hpp>
#include <math.h>
#include <string.h>
#include <iostream>

#define TIME_STEP 8
using namespace webots;
using namespace std;

// typedef struct {
  // double x, z;
  // bool c;
// } CheckPoint ;

int main()

{

    Supervisor *supervisor = new Supervisor();

    
    //Node *Master_node = supervisor->getFromDef("Speed_Line_Follower_Robot_V4");
    //Node *Master_root = supervisor->getRoot();
    Node *Master_node = supervisor->getFromDef("CheckLine1");
    
    while (supervisor->step(TIME_STEP) != -1) 
    {
      Node *Master_field = Master_node->getDef("children");
      Node *Child_node = getDef("CL_1");
      Field *Child_field = Child_node->getField("translation");
      double pos[3] = Child_field->getSFVec3f();
      cout << pos[0] << pos[1] << pos[2] << endl;
    }    
    
    delete supervisor;
    return 0;
}
