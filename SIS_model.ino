// SIR_model.ino 
// Tangible Networks
//

#include <math.h>
#include<TN.h>                      // Requires TN.h, TN.cpp, Keywords.txt in folder <Arduino>/Libraries/TN/

// Model parameters
float infectionProbability = 0.02;  // Probability of infection by an infected neighbour per timestep. 0-->immune (set by pot)
float recoveryProbability  = 0.005;  // Probability of recovery per timestep, if infected
float potError = 0.01;              // To allow for noise in the pot reading
boolean isVaccinated = false;

int state = 0;                      // 0: susceptible, 1: infected, -1: immune


float inputs[] = {0,0,0};

TN Tn = TN(-1.0,1.0);                  // Create TN object.

void setup () {}                   // setup of the Unit is handled by the constructor of the TN class. Nice.

void loop() {
  
  readInputs();                    // store the inputs locally. Could just read as needed..

  updateState();
  
  sendInfection();
  ledWrite();

  delay(10);                     
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////S.I.R. functions.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void readInputs(){

    inputs[0] = Tn.analogRead(1);
    inputs[1] = Tn.analogRead(2);
    inputs[2] = Tn.analogRead(3);
   
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean updateState(){
  
// check if immune  
  if (Tn.pot()>=1-potError){
    state = -1;
    isVaccinated = true;
    return true;
  }
  else if (isVaccinated) {
    state = 0;
    isVaccinated = false;
  }
// if switch is pressed, infect this unit
  if (Tn.sw()){
    state = 1;  
  }

  else{
    // iterate over neighbours, do they infect me?
    for (int i=1; i<=3; i++){
      if (receiveInfection(i)){state=1;}  
     } 
  }
  
  float r = (float)rand()/RAND_MAX;
  if(r<=recoveryProbability*(1+Tn.masterRead()*(1-recoveryProbability))){
    state = 0;  
  }

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean receiveInfection(int i){
  if (inputs[i-1]>0){
      float r = (float)rand()/RAND_MAX;
      if (r<=(infectionProbability*(1-Tn.pot()))){
        return true;
      }
  }
      
  return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendInfection(){
  if (state==1){ 
    for (int i=1; i<=3; i++) {    
        Tn.analogWrite(i,state);
      }
  }
  else{
        for (int i=1; i<=3; i++) {    
        Tn.analogWrite(i,-1);
      }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ledWrite(){
  
  if (state==-1){    
    Tn.colour(0, 255, 0);    // immune        -> green
  }
  else if (state==1){
    Tn.colour(255, 0, 0);    // infected -> red
  }
  else if (state==0){
    Tn.colour(0, 0, 255);    // susceptible -> blue
  }  
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
