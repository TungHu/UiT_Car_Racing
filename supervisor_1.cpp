#include <webots/Supervisor.hpp>
#include <math.h>
#include <iostream>

#define TIME_STEP 8
#define OffSet 0.26 
using namespace webots;
using namespace std;
double Finish_time = 0;
bool Condition = false;

typedef struct {
  double x, z;
  double dx = 0;
  double dz = 0;
} Coord;

 Coord CP[10] = {{-0.44, 0.6, 0.03, 0.081}, {1.15, 0.6, 0.03, 0.081},{1.17, -0.99, 0.081, 0.03},{0.36, -0.92, 0.03, 0.081},
                 {-0.15, 0.06, 0.03, 0.081}, {-0.68, -0.89, 0.03, 0.081}, {-1.35, -0.34, 0.081, 0.03}, {-1.35, 0.56, 0.081, 0.03},
                 {-1.35, 1.03, 0.081, 0.03}, {-1.35, 1.25}};
    
Coord CL[28] = {{1.09, 1.18, 0.04, 0.081},
                {-0.4, 1.18, 0.02, 0.081}, {-0.4, 0.6, 0.02, 0.081}, {0.035,0.6, 0.02, 0.081},
                {0.275, 0.835, 0.02, 0.081}, {0.508, 0.6, 0.02, 0.081}, {0.745, 0.835, 0.02, 0.081},
                {0.985, 0.6, 0.02, 0.081}, {1.45, 0.6, 0.02, 0.081}, {1.45, -1.08, 0.081, 0.02},
                {1.17, -1.08, 0.081, 0.02},{1.17, 0.06, 0.081, 0.02}, {0.51, 0.06, 0.02, 0.081}, 
                {0.51, -0.5, 0.081, 0.02}, {0.65, -0.64, 0.02, 0.081}, {0.65, -0.92, 0.02, 0.081},
                {0.09, -0.92, 0.02, 0.081}, {0.1, -0.64, 0.02, 0.081}, {0.25, -0.5, 0.081, 0.02},
                {0.25, -0.08, 0.081, 0.02}, {0.11, 0.06, 0.02, 0.081}, {-0.53, 0.06, 0.02, 0.081},
                {-0.53, -0.89, 0.081, 0.02}, {-1.21, -0.89, 0.02, 0.081}, {-1.35, -0.75, 0.081, 0.02},
                {-1.35, -0.14, 0.081, 0.02}, {-1.35, 0.44, 0.081, 0.02}, {-1.35, 1.03, 0.081, 0.02}};
                
int LineType[28] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 3, 4, 0, 5, 6, 0, 7, 0, 0, 0, 8, 0, 9, 0, 0};

Coord MidPoint(Coord a, Coord b)
{
    Coord c = {(a.x + b.x) / 2, (a.z + b.z) / 2};
    return c;
}


//Duong thang
bool StraightLine(int index, Coord L, Coord R, double d) {
     double a = CL[index-1].z - CL[index].z;
     double b = CL[index].x - CL[index-1].x;
     double c = -CL[index-1].x * a - CL[index-1].z * b;
     
     if ((abs(a*L.x + b*L.z + c) / sqrt(a*a + b*b)) > d || (abs(a*R.x + b*R.z + c) / sqrt(a*a + b*b)) > d)
          return true;
     return false;
}

//Duong cong
bool CurveLine(int index, Coord L, Coord R, double Ia, double Ib, double R1, double R2, double x1, double x2, double z1, double z2 ) {
        if ( (((L.x - Ia) * (L.x - Ia) + (L.z - Ib) * (L.z - Ib)) > (R1*R1) && 
              ((L.x - Ia) * (L.x - Ia) + (L.z - Ib) * (L.z - Ib)) < (R2*R2))
             &&
             (((R.x - Ia) * (R.x - Ia) + (R.z - Ib) * (R.z - Ib)) > (R1*R1) &&
              ((R.x - Ia) * (R.x - Ia) + (R.z - Ib) * (R.z - Ib)) < (R2*R2))
             &&
             ((x1 - OffSet < L.x) && (L.x < x2 + OffSet) && (z1 - OffSet < L.z) && (L.z < z2 + OffSet))
             &&
             ((x1 - OffSet < R.x) && (R.x < x2 + OffSet) && (z1 - OffSet < R.z) && (R.z < z2 + OffSet))
             ) {
             //cout<< "CheckCurver:  " << ((L.x - Ia) * (L.x - Ia) + (L.z - Ib) * (L.z - Ib)) << "    " << ((R.x - Ia) * (R.x - Ia) + (R.z - Ib) * (R.z - Ib)) << "\n";
             return false;
             }
        return true;
}

bool CheckOutLine(Coord LP, Coord RP, int numLine) {
    switch (LineType[numLine-1]) {
    case 0: 
      return StraightLine(numLine, LP, RP, 0.13);
      
    case 1:
      //cout << MidPoint(CL[numLine], CL[numLine-1]).x << " " << MidPoint(CL[numLine], CL[numLine-1]).z<< endl;
      return CurveLine(numLine, LP, RP, -0.4, 0.89, 0.16, 0.42, -0.69, -0.4, 0.6, 1.18);
      
    case 2:
      return CurveLine(numLine, LP, RP, 1.31, -1.08, 0.01, 0.27, 1.17, 1.45, -1.08, -1.22);
      
   case 3:
      return CurveLine(numLine, LP, RP, 0.65, -0.5, 0.01, 0.27, 0.51, 0.65, -0.64, -0.5);
    
   case 4:
      return CurveLine(numLine, LP, RP, 0.65, -0.78, 0.01, 0.27, 0.65, 0.79, -0.92, -0.64);
    
   case 5:
      return CurveLine(numLine, LP, RP, 0.11, -0.78, 0.01, 0.27, -0.03, 0.1, -0.92, -0.64);
    
   case 6:
      return CurveLine(numLine, LP, RP, 0.11, -0.5, 0.01, 0.27, 0.11, 0.25, -0.64, -0.5);
    
   case 7: 
      return CurveLine(numLine, LP, RP, 0.11, -0.08, 0.01, 0.27, 0.11, 0.25, -0.08, 0.06);
    
   case 8: 
      return CurveLine(numLine, LP, RP, -1.21, -0.75, 0.01, 0.27, -1.35, -1.21, -0.89, -0.75);
    
   case 9: 
      return CurveLine(numLine, LP, RP, -1.35, 0.15, 0.16, 0.42, -1.35, -1.06, -0.14, 0.44);
   default: return false;     
  }
}

bool ForwardLine(Coord trans, Coord point) {
  if ((trans.x >= (point.x - point.dx)) && (trans.x <= (point.x + point.dx))  && (trans.z >= (point.z - point.dz)) && (trans.z <= (point.z + point.dz)))
    return true;
  return false;
} 

Coord CreatePoint(double x, double z, double dx = 0, double dz = 0)
{
    Coord a = {x, z, dx, dz};
    return a;
}

bool CheckPoint_10(double x, double z)
    {
       double delta = 0.02;
       return !(((-1.45 - delta) < x) && (x < (-1.25 + delta)) && ((1.03 - delta) < z) && (z < (1.24 + delta)));
    }

int main()

{
   
     
    Supervisor *supervisor = new Supervisor();
    
    int Next_CP = 1;
    int Next_Line = 1;
    // Node *Slave_node1 = supervisor->getFromDef("box2");
    // Node *Slave_node2 = supervisor->getFromDef("box3");
    // Node *Slave_node = supervisor->getFromDef("TrungDiem");
    Node *Master_node = supervisor->getFromDef("Speed_Line_Follower_Robot_V4");


    //Field *Slave_trans_field1 = Slave_node1->getField("translation");
    // Field *Slave_trans_field2 = Slave_node2->getField("translation");
    // Field *Slave_trans_field3 = Slave_node->getField("translation");
    Field *Master_trans_field = Master_node->getField("translation");
    Field *Master_rotation_field = Master_node->getField("rotation");

    
    Coord LPoint = {-0.04, -0.14};
    Coord RPoint = {0.04, -0.14};
    Coord pos1 = {0, 0}; // left point
    Coord pos2 = {0, 0}; // right point
    
    
    while (supervisor->step(TIME_STEP) != -1) 
    {
        const double *Master_trans = Master_trans_field->getSFVec3f();
        const double *Master_rotation = Master_rotation_field->getSFRotation();
        double temp = -1 * Master_rotation[1] * Master_rotation[3];
        if (temp != 0)
        {  
          pos1.x = Master_trans[0] + LPoint.x * cos(temp) - LPoint.z * sin(temp);
          pos1.z = Master_trans[2] + LPoint.x * sin(temp) + LPoint.z * cos(temp);
          
          pos2.x = Master_trans[0] + RPoint.x * cos(temp) - RPoint.z * sin(temp);
          pos2.z = Master_trans[2] + RPoint.x * sin(temp) + RPoint.z * cos(temp);
        }  
        else 
        {
          pos1.x = Master_trans[0] + LPoint.x;          
          pos1.z = Master_trans[2] + LPoint.z;
          
          pos2.x = Master_trans[0] + RPoint.x;          
          pos2.z = Master_trans[2] + RPoint.z;
        }
        
        Coord pos = MidPoint(pos1, pos2);
        // pos1.x += Master_trans[0];
        // pos1.z += Master_trans[2];
        
        // pos2.x += Master_trans[0];
        // pos2.z += Master_trans[2];
       
        
        // Slave_trans_field1->setSFVec3f(pos1);
        // Slave_trans_field2->setSFVec3f(pos2);
        // Slave_trans_field3->setSFVec3f(pos);
        
        
        // CHECK LINE
        if (Next_Line < 27) 
         {          
            if(ForwardLine(pos, CL[Next_Line]))
            {
                cout <<"\nCheckLine " << Next_Line << "\n\n";
                Next_Line++;  
            }        
         }
         
         
          
       
          
        int i = Next_CP - 1;
        
        
        if (i < 9)
        {
            if(CheckOutLine(pos1, pos2, Next_Line)) 
              {
                 cout << "\nL\t" << pos1.x << " : " << pos1.z << "\t\t\t" << "R\t" << pos2.x << " : " << pos2.z << "\t\t\t" << "C\t" << pos.x << " : " << pos.z << "\n";
                 supervisor->simulationSetMode(webots::Supervisor::SimulationMode::SIMULATION_MODE_PAUSE);  
              }  
                          
             
        //if (pos.x > CP[i].x - CP[i].dx && pos.x < CP[i].x + CP[i].dx && pos.z > CP[i].z - CP[i].dz && pos.z < CP[i].z + CP[i].dz )
        if (ForwardLine(pos, CP[i])) 
          {
            Finish_time = supervisor->getTime();
            cout <<"Check Point " << i+1 <<" || " << Finish_time << endl;
            Next_CP++;
          }
        }
        else 
        {
          
          // bool ForwardLine(Coord trans, Coord point)
          if (ForwardLine(CreatePoint(Master_trans[0], Master_trans[2] - 0.02), CP[Next_CP - 2]) || Condition)
          {
              Condition = true;
            cout << Master_trans[0] << " " << Master_trans[2] - 0.02 << " " << CP[Next_CP - 2].x << " " << CP[Next_CP - 2].z << endl;
            // for  (;;)
            {
                // bool CheckPoint_10(double x, double z)
                // {
                   // double delta = 0.02;
                   // return (((-1,45 - delta) < x) && (x < (-1.25 + delta)) && ((1.03 - delta) < z) && (z < (1.24 + delta)));
                // }
                double Start_time = supervisor->getTime();
                if (CheckPoint_10(pos1.x, pos1.z) && CheckPoint_10(pos2.x, pos2.z) && CheckPoint_10(pos1.x, pos1.z - 0.15) && CheckPoint_10(pos2.x, pos2.z - 0.15))
                {  
                  cout << "\nL\t" << pos1.x << " * " << pos1.z << "\t\t\t" << "R\t" << pos2.x << " : " << pos2.z << "\t\t\t" << "C\t" << pos.x << " : " << pos.z << "\n";
                  supervisor->simulationSetMode(webots::Supervisor::SimulationMode::SIMULATION_MODE_PAUSE);              
                }
                else 
                {
                  if (supervisor->getTime() - Start_time >= 1)
                  cout <<"Check Point " << i+1 <<" || " << Finish_time << endl;
                } 
             } 
          }  
            
            //cout << "\nL\t" << Pre_time_of_CP << " " << pos1.x << " : " << pos1.z << "\t\t\t" << "R\t" << pos2.x << " : " << pos2.z << "\t\t\t" << "C\t" << pos.x << " : " << pos.z << "\n";
        }
    }
    delete supervisor;
    return 0;
}
