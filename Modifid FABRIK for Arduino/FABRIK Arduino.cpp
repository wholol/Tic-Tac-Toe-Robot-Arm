//#include <FABRIK.h>
#include <Braccio.h>
#include <Servo.h>


Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo wrist_ver;
Servo gripper;


//calcaulations should be done in cm.
const double link_length_1 = 125;     //link length 1 
const double link_length_2 = 125;     //link length 2  
const double link_length_3 = 200;     //link length 3
const double tolerance = 0.02;        //tolerance value
const double link1_posX = 0;      //origin
const double link1_posY = 53;      //origin
const double link2_posX = 32.3;   //link_length_1 * cos 15
const double link2_posY = 120;  //link_length_! * sin 15
const double link3_posX = 64.7;    //link_length_1 + link_length_2 * sin 15
const double link3_posY = 241;  //link_length_1 + link_length_2 * cos 15
const double link4_posX = 116.46;    //link_length_1 + link_length_2 + link_length_3 * sin 15
const double link4_posY = 434.67;  //link_length_1 + link_length_2 + link_length_3 * cos 15
const int open_gripper = 10;
const int close_gripper = 73;



class FABRIK{                       //FABRIK class
  private:
    double link1_posX, link1_posY, link2_posX, link2_posY, link3_posX, link3_posY, link4_posX, link4_posY,tolerance; //initial postion values.l
    static const int joint_number = 4;     //number of joints
    double posX_array[4];               //vector that stors x positions for the LOCAL frame
    double posY_array[4];               //vector that stores y positions  for the LOCAL frame
    double Lambda[3] = {0};           //lambda vector. initializes it to 0
    double link_length_array[3];        //stores the link length values
    double magnitude_array[3] = {0};     //this stores the magitude array
    double target_posX,target_posY;          //sets the target position for x and y for the local frame.
    double xCoord, yCoord;              //the target coordinates on the GLOBAL frame.
    double atan2_theta_1;               //atan2 value fo theta_1
    double atan2_theta_2;               //atan2 value for theta_2
    double atan2_theta_3;               //atan2 valye for theta_3
    double curr_theta_1 = 15;              //delay the angle output
    double curr_theta_2 = 90;
    double curr_theta_3 = 90;
    double curr_base_theta = 0;
    double theta_array[4];
   
  public:
  FABRIK(const double& link1_posX,const double& link1_posY,const double& link2_posX,const double& link2_posY,const double& link3_posX,const double& link3_posY      //constructors variables for fabrik algorithm
        ,const double& link4_posX,const double& link4_posY,const double& link_length_1,const double& link_length_2,const double& link_length_3,const double& tolerance);
  void target_position(const double& target_xPos,const double& target_yPos,const double& height);         
  double compute_target_magnitude();    //computes the targets magnitude
  double link_magnitude_difference(int i);    //computes the magnitude difference for the targets
  bool check_max_distance();          //checks the maximum possible distance
  double compute_difference();    
  double theta_1();   //this function computes theta_1
  double theta_2();   //this function computes theta_2
  double theta_3();   //this function computes theta_3
  void move_robot(int gripper_status);    //movs the robot after FABRIK is done.
  void compute_algorithm();   //FABRIK implemnetation is used here
  double theta_base_angle();    //solve the base angle.
  void reset_robot();       //resets the arms and robot to default position.
  void printpositions();    //prints the positions of the joints
};


FABRIK::FABRIK(const double& link1_posX,const double& link1_posY,const double& link2_posX,const double& link2_posY,const double& link3_posX,const double& link3_posY      //constructors variables for fabrik algorithm
        ,const double& link4_posX,const double& link4_posY,const double& link_length_1,const double& link_length_2,const double& link_length_3,const double& tolerance)
        {                   
        posX_array[0] = link1_posX;             
        posX_array[1] = link2_posX;            
        posX_array[2] = link3_posX;
        posX_array[3] = link4_posX;
        posY_array[0] = link1_posY;
        posY_array[1] = link2_posY;     
        posY_array[2] = link3_posY;
        posY_array[3] = link4_posY;
        this->tolerance = tolerance;   
        this->link1_posX = link1_posX;     
        this->link2_posX = link2_posX;  
        this->link3_posX = link3_posX;  
        this->link4_posX = link4_posX;  
        this->link1_posY = link1_posY;     
        this->link2_posY = link2_posY;  
        this->link3_posY = link3_posY;  
        this->link4_posY = link4_posY; 
        link_length_array[0] = link_length_1;
        link_length_array[1] = link_length_2;
        link_length_array[2] = link_length_3; 
}
       

void FABRIK::printpositions(){  //this function sets the target position.

        Serial.println("theta 1 :");
        Serial.println(theta_1());
        Serial.println("theta 2 :");
        Serial.println(theta_2());
        Serial.println("theta 3 :");
        Serial.println(theta_3());
        Serial.println("theta base :");
        Serial.println(theta_base_angle());
}


void FABRIK::target_position(const double& xCoord,const double& yCoord,const double& height){  //this function sets the target position.
        this->xCoord = xCoord;      //coordinate from 3D space
        this->yCoord = yCoord;      //coordinate from 3D space
        this->target_posX = sqrt(pow(xCoord,2) + pow(yCoord,2));      //the 2D target position in the X axis is the magnitude of the global coordinate system.
        this->target_posY = height;
}


double FABRIK::compute_target_magnitude(){       //this function computes the magnitude of the target position. PURPOSE: to check max distance available.
        return sqrt(pow(target_posX - posX_array[0],2) + pow(target_posY-posY_array[0],2));
    }


double FABRIK::link_magnitude_difference(int i){      //computes the magnitude of two links in cartesian space.funtion used in algorithm itself.
        return (sqrt(pow((posX_array[i + 1] - posX_array[i]) , 2) + pow(posY_array[i + 1] - posY_array[i],2)));
    }

bool FABRIK::check_max_distance(){             //checks if the link can actually reach the desired position.
        double max_distance = link_length_array[0] + link_length_array[1] +link_length_array[2];
        if (max_distance <= compute_target_magnitude()){
            return false;
        }
        return true;
    }

double FABRIK::compute_difference(){      //computes the magnitude difference between the target and end effector position
        return sqrt(pow((target_posX - posX_array[3]), 2) + pow(target_posY - posY_array[3],2));
    }

double FABRIK::theta_3(){       //computes theta 3
    
    atan2_theta_3 = atan2(posY_array[3] - posY_array[2],posX_array[3] - posX_array[2])*(180 / 3.14159);
    
    if (atan2_theta_3  > atan2_theta_2 ){   //if arm is going upwards
      return (90 - (atan2_theta_3  - atan2_theta_2));
    }
    
    else{ //if going downwards

      if ( 90 +  atan2_theta_2 + fabs(atan2_theta_3) >= 180){
        return 180;
      }
      else{
        return (90 +  atan2_theta_2 + fabs(atan2_theta_3));    
      }
    }
}


double FABRIK::theta_2(){   //computes theta 2
    
    
    atan2_theta_2 = atan2(posY_array[2] - posY_array[1],posX_array[2] - posX_array[1])*(180 / 3.14159);
  
    if (atan2_theta_2 > atan2_theta_1){   //if theta 2 moves upwawards
     
      return (90 - (atan2_theta_2 - atan2_theta_1));
    }

    else{

      if (90 + (atan2_theta_1 + fabs(atan2_theta_2) ) >= 180){
        return 180;
      }
       return (90 + (atan2_theta_1 + fabs(atan2_theta_2) )); //if the arm moves downwards
    }
}


double FABRIK::theta_1(){   //computes theta 1 
    
   atan2_theta_1 = atan2(posY_array[1] - posY_array[0],posX_array[1] - posX_array[0])*(180 / 3.14159);
   if (90 - atan2_theta_1 <= 15){
    
    return 15;
   }
   else{
    return (90 - atan2_theta_1);  
   }   
}

double FABRIK::theta_base_angle(){    //solve for base angle.
  
  if (xCoord > 0){
    return atan2(yCoord,xCoord)*(180 / 3.14159);        
  }

  if (xCoord < 0 && yCoord >= 0){
    return ((atan2(yCoord,xCoord) + 3.14159)*(180 / 3.14159));
  }

  if (xCoord < 0 && yCoord < 0){
    return ((atan2(yCoord,xCoord) - 3.14159)*(180 / 3.14159));
  }

  else{
    return atan2(yCoord,xCoord)*(180 / 3.14159);
  }
}

void FABRIK::move_robot(int gripper_status){

    theta_array[0] = 0;
    theta_array[1] =  theta_1() - curr_theta_1;    //future pos - current. if value is postive, increment, else decrement
    theta_array[2] =  theta_2() - curr_theta_2;     //future pos - current. if future > current, increment servo, else decrement.
    theta_array[3] =  theta_3() - curr_theta_3;     //future pos - current. if future > current, increment servo, else decrement.
    SortArray(theta_array);   //last index (2) being the maximum, (1) being the medium, (0) being the least

    for (int i = 1 ; i < 4; ++i){
      Serial.println(theta_array[i]);
    }
    
    
    
    
    
    //greater,lesser,greater
     if (curr_theta_1 >= theta_1() && curr_theta_2 <= theta_2() && curr_theta_3 >= theta_3()){   //if all thre anlges are greater than future angle calcualted
      while (curr_theta_1 >= theta_1()){
        curr_theta_1 -= 1;
        Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
      }
      
        while (curr_theta_2 <= theta_2()){
          curr_theta_2 += 1;
        Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }
        
        while (curr_theta_3 >= theta_3()){
          curr_theta_3 -= 1;
           Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        } 
         
    }

    //lesser,lesser,greater
    if (curr_theta_1 <= theta_1() && curr_theta_2 <= theta_2() && curr_theta_3 >= theta_3()){   //if all thre anlges are greater than future angle calcualted
      while (curr_theta_1 <= theta_1()){
        curr_theta_1 += 1;
        Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
      }
        while (curr_theta_2 <= theta_2()){
          curr_theta_2 += 1;
          Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }
        
        while (curr_theta_3 >= theta_3()){
          curr_theta_3 -= 1;
           Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        } 
    }
    
    //lesser,greater,lesser
     if (curr_theta_1 <= theta_1() && curr_theta_2 >= theta_2() && curr_theta_3 <= theta_3()){   //if all thre anlges are greater than future angle calcualted
      while (curr_theta_1 <= theta_1()){
        curr_theta_1 += 1;
         Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
      }
        while (curr_theta_2 >= theta_2()){
          curr_theta_2 -= 1;
           Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }
        while (curr_theta_3 <= theta_3()){
          curr_theta_3 += 1;
           Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }  
    }

    //lesser, lesser ,lesser
    if (curr_theta_1 <= theta_1() && curr_theta_2 <= theta_2() && curr_theta_3 <= theta_3()){   //if all thre anlges are greater than future angle calcualted
      while (curr_theta_1 <= theta_1()){
        curr_theta_1 += 1;
         Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
      }
      
        while (curr_theta_2 <= theta_2()){
          curr_theta_2 += 1;
          Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }
        
        while (curr_theta_3 <= theta_3()){
          curr_theta_3 += 1;
           Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }    
    }

    //greater,greater,greater
    if (curr_theta_1 >= theta_1() && curr_theta_2 >= theta_2() && curr_theta_3 >= theta_3()){   //if all thre anlges are greater than future angle calcualted
      while (curr_theta_1 >= theta_1()){
        curr_theta_1 -= 1;
      Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
      }
        
        while (curr_theta_2 >= theta_2()){
          curr_theta_2 -= 1;
          Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }
        
        while (curr_theta_3 >= theta_3()){
          curr_theta_3 -= 1;
           Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }  
    }

     //greater,lesser, lesser   
     if (curr_theta_1 >= theta_1() && curr_theta_2 <= theta_2() && curr_theta_3 <= theta_3()){   //if all thre anlges are greater than future angle calcualted
      while (curr_theta_1 >= theta_1()){
        curr_theta_1 -= 1;
        Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
      }
      
        while (curr_theta_2 <= theta_2()){
          curr_theta_2 += 1;
          Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }
        
        while (curr_theta_3 <= theta_3()){
          curr_theta_3 += 1;
           Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        } 
      }   
    

    //lssser,greater,greater
    if (curr_theta_1 <= theta_1() && curr_theta_2 >= theta_2() && curr_theta_3 >= theta_3()){   //if all thre anlges are greater than future angle calcualted
      while (curr_theta_1 <= theta_1()){
        curr_theta_1 += 1;
        Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
      }
      
        while (curr_theta_2 >= theta_2()){
          curr_theta_2 -= 1;
          Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }
        
        while (curr_theta_3 >= theta_3()){
          curr_theta_3 -= 1;
           Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        } 
    }

    //greater,greater,lesser
    if (curr_theta_1 >= theta_1() && curr_theta_2 <= theta_2() && curr_theta_3 <= theta_3()){   //if all thre anlges are greater than future angle calcualted
      while (curr_theta_1 >= theta_1()){
        curr_theta_1 -= 1;
        Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
      }

        while (curr_theta_2<= theta_2()){
          curr_theta_2 -= 1;
          Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }
        
        while (curr_theta_3 <= theta_3()){
          curr_theta_3 -= 1;
           Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,73);
        }   
    }
    
    
    Braccio.ServoMovement(10,theta_base_angle(),curr_theta_1,curr_theta_2,curr_theta_3,60,gripper_status);
    delay(2000);
    
}


void FABRIK::reset_robot(){   //reset arm position and values.
        posX_array[0] = link1_posX;
        posX_array[1] = link2_posX;
        posX_array[2] = link3_posX;
        posX_array[3] = link4_posX;
        posY_array[0] = link1_posY;
        posY_array[1] = link2_posY;
        posY_array[2] = link3_posY;
        posY_array[3] = link4_posY;
        
        Braccio.ServoMovement(10,180,15,90,90,60,73);     //gripper: 10 for tongue open, 73 to close. WRIST ROTATION, should be const.
        curr_theta_1 = 15;
        curr_theta_2 = 90;
        curr_theta_3 = 90;
        delay(4000);
}


void FABRIK::compute_algorithm(){       //this function computes the FABRIK algorithm itself.
        compute_target_magnitude();

        if (check_max_distance() == false){
          Serial.println("over the maximum distance limit.");
            return;
        }

        while(1){
            posX_array[3] = target_posX;    //set the end effector array to target
            posY_array[3] = target_posY;    //set the end effector to target
        
            for (int i = 2; i >=0; i--){            //backward difference
                magnitude_array[i] = link_magnitude_difference(i);
                Lambda[i] = link_length_array[i]/magnitude_array[i];
                posX_array[i] = (1 - Lambda[i]) * posX_array[i + 1] + Lambda[i] * posX_array[i];
                posY_array[i] = (1 - Lambda[i]) * posY_array[i + 1] + Lambda[i] * posY_array[i];
            }


            posX_array[0] = link1_posX;     //reset position to zero
            posY_array[0] = link1_posY;     //reset position to zero

            for (int j = 0; j <= 2; ++j){
                magnitude_array[j] = link_magnitude_difference(j);
                Lambda[j] = link_length_array[j] / magnitude_array[j];
                posX_array[j + 1] = (1 - Lambda[j]) * posX_array[j] + Lambda[j] * posX_array[j + 1];
                posY_array[j + 1] = (1 - Lambda[j]) * posY_array[j] + Lambda[j] * posY_array[j + 1];
            }

            double difference = fabs(compute_difference());
            
            if (difference < tolerance){      
                break;
            }
        }
    }



FABRIK fab(link1_posX,link1_posY,link2_posX,link2_posY,link3_posX,link3_posY      //constructors variables for fabrik algorithm
        ,link4_posX,link4_posY,link_length_1,link_length_2,link_length_3,tolerance);
        
void setup() {
  //Initialization functions and set up the initial position for Braccio
  //All the servo motors will be positioned in the "safety" position:
  //Base (M1):90 degrees
  //Shoulder (M2): 45 degrees
  //Elbow (M3): 180 degrees
  //Wrist vertical (M4): 180 degrees
  //Wrist rotation (M5): 90 degrees
  //gripper (M6): 10 degrees
  Braccio.begin();
  Serial.begin(115200);
}

void loop() {


  /* calls target postion
   *  computes algorithm functino called
   *  spit out the angles.
   *  reset position of robot arm
   */


/*
  fab.reset_robot();
 
  fab.target_position(250,100,0);     //1 cm error
  fab.compute_algorithm();
  fab.printpositions();
  fab.move_robot(open_gripper);

  
  fab.move_robot(close_gripper);
  fab.printpositions();
  fab.reset_robot();
  fab.target_position(350,50,0);     //1 cm error
  fab.compute_algorithm();
  fab.printpositions();
  fab.move_robot(open_gripper);

  
  fab.move_robot(close_gripper);
  fab.printpositions();
  */

 if (Serial.available() > 0){
      
      int AI_position = Serial.parseInt();
      Serial.println(AI_position);
    }

    
  

  

   
  //if (rceived from pc) -> construct->target_position->compute_algorithm->spit theta angles->reset
  //tbd: y coordinate system
  //reset joints?
  

}
