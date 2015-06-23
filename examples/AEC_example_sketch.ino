/*++++++++++++++++++++++++++++++ Arduino Ethernet Controller +++++++++++++++++++++++++++
  
This example sets up a basic Ethernet controlable Arduino with the AEC API.
The API makes the Arduino Programming more easy and gives you featues which interact 
with the Arduino Ethernet Controller Pro App.

These functions are:

  - setup the element which should be importet in the app.
  - give the Arduino a name that is displayed in the network scan.
  - create easy feedbacks to display in your feedback cell.
  - change the arduino name and ip address over the app
  - control (pwm) pins over the app without programming
  
  
You can find a full documentation for the API on our GitHub Wiki

*/

//include all the necessary libraries
#include <AEC.h>
#include <EEPROM.h>
#include <Ethernet.h>
#include <SPI.h>

//Variables for start(,,) Method
int ip[] = {192,168,178,178};
byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x44, 0x5F};
String deviceName = "Arduino 1";

//Variables for searchForAction(,) and searchForColor(,,,,) method. This variable are changed by the AEC library.
char comand;
int value, valueR, valueG, valueB, alpha; 

AEC myAEC;

void setup(){
  
  Serial.begin(9600);
  //initialize the API with your ip, mac, and device name
  myAEC.start(ip, mac, deviceName);
  
  /*create elements you can import over your app (Pro Version V1.2). It's needless to add all one by one.
    This methods work only with an ATmega2560!!!
  
  myAEC.createButton("Button Nr.1",'a');
  myAEC.createSlider("Slider Nr.1",'c', 0, 255);;
  myAEC.createColor("MyLed",'x');
  myAEC.createPinButton("Pin 13", 13);
  myAEC.createPinSlider("PWM 9", 9);
  myAEC.createFeedback('c');
  myAEC.createFeedback('b');
  
  */
  
  Serial.println(Ethernet.localIP());
  pinMode(12, OUTPUT); //Control the output pin 12
  pinMode(11, OUTPUT); //Control the output pin 11
}


void loop(){ 
  
//search for incomming ethernet connections
myAEC.searchForClient();  

//get filtered the client comand.
myAEC.searchForAction(comand, value);

//get filtered the client comand and convert it into RGB(0-255) and alpha (0-100) values
//(Pro Version V1.1)
myAEC.searchForColor('x', valueR, valueG, valueB, alpha);

//the receiveTrigger() checks for a changings in the incomming comands and triggers if the comand changes.
//Be aware that the comand even changes if you just refresh the website!

if(myAEC.receiveTrigger() == true){
  Serial.print("comand:\t");
  Serial.print(comand);
  Serial.print("\tvalue:\t");
  Serial.print(value);
  Serial.print("\tR:\t");
  Serial.print(valueR);
  Serial.print("\tG:\t");
  Serial.print(valueG);
  Serial.print("\tB:\t");
  Serial.print(valueB);
  Serial.print("\tAlpha:\t");
  Serial.println(alpha);
}

//create some HTML request website
//if no HTML request is set, default website will appear

//  myAEC.setHTMLrequest("<h1>Your Favourite Website in HTML as String</h1>");

//do some routine for evaluating the gotten comands and values 
switch(comand){
  case 'a':
   digitalWrite(12, value);
  break;
  case 'c':
   analogWrite(11, value);
  break;
  
  default:
  break; 
}

//send some Feedback back to your App
myAEC.Feedback('c', "LED is really bright");
myAEC.Feedback('b', "Thats a cool feedback");
}
