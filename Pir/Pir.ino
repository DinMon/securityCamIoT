#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//I2C pins declaration
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;       
 
//the time when the sensor outputs a low impulse
long unsigned int lowIn; 

//the time when the sensor outputs a low impulse
long unsigned int slookingInputTime;
long unsigned int elookingInputTime;
long unsigned int lookingTime = 3000;
 
//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 5000; 
 
boolean lockLow = true;
boolean takeLowTime; 
 
int pirPin = 3;    //the digital pin connected to the PIR sensor's output
int ledPin = 13;

bool isMotionDetected = false;
bool isCheckingPassword = false;
bool onlyOne = false;
bool waitingForValidation = false;

String str;
String topic;
String topicVal;

String password = "";
/////////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  lcd.begin(16,2);//Defining 16 columns and 2 rows of lcd display
  lcd.noBacklight();//To Power ON the back light
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  digitalWrite(pirPin, LOW);
   digitalWrite(13, LOW);
 //give the sensor some time to calibrate
  /*Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);*/
}
 
////////////////////////////
//LOOP
void loop(){
    ////////////////////// READ INPUT FROM KEYPARD AND SET LCD ///////////////
    lcd.setBacklight(isMotionDetected || isCheckingPassword); // set the backlight of LCD
    if (!waitingForValidation){
      
     if(digitalRead(4) == HIGH){
       password += "1";
       delay(300); // best delay for the touch pad
     }
     if(digitalRead(5) == HIGH){
       password += "2";
       delay(300);
     }
     if(digitalRead(6) == HIGH){
       password += "3";
       delay(300);
     }
     if(digitalRead(7) == HIGH){
       password += "4";
       delay(300);
     }
     if(digitalRead(8) == HIGH){
       password += "5";
       delay(300);
     }
     if(digitalRead(9) == HIGH){
       password += "6";
       delay(300);
     }
     if(digitalRead(10) == HIGH){
       password += "7";
       delay(300);
     }
     if(digitalRead(11) == HIGH){
       // Send password to check with picture when button 8 pressed
       Serial.print("Password:");
       Serial.print(password);
       Serial.print(";");
       Serial.println();
       delay(300);
       // send code to python
       password = "";
       waitingForValidation = true;
       
     }
     
     ///// IF No INPUT FROM CERTAIN TIME
     if (isCheckingPassword && password.equals("")){
       elookingInputTime = millis() - slookingInputTime;
       if (elookingInputTime > lookingTime){
         isCheckingPassword = false;
         onlyOne = false;
       }
     }
     
     
     if (waitingForValidation){
       lcd.clear();
       lcd.setCursor(0,1); //Defining positon to write from first row,first column .
       lcd.print("  Processing!!!"); //You can write 16 Characters per line . 
       delay(1000); 
     }
     
     if (!password.equals("")){
       lcd.setCursor(0,1); //Defining positon to write from first row,first column .
       lcd.print(password); //You can write 16 Characters per line .
       
     }
    }
      
     /////////////////////// READ INPUT FROM SERIAL PORT ///////////////
     if (Serial.available() > 0){
       str = Serial.readStringUntil(';');
       
       int semiIndex = str.indexOf(':');
       topic = str.substring(0, semiIndex);
       topicVal = str.substring(semiIndex + 1);
       Serial.println(topic);
     }
     
     if(topic.equals("CONFIRM")){
       waitingForValidation = false;
       isCheckingPassword = false;
       onlyOne = false;
       lcd.clear();
       lcd.setCursor(0,0); //Defining positon to write from first row,first column .
       lcd.print("    Unlock"); //You can write 16 Characters per line . 
      lcd.setCursor(0,1); //Defining positon to write from first row,first column .
       lcd.print(" Sucessfully!!!"); //You can write 16 Characters per line .
       delay(3000);
       topic = "";
       str = "";
       topicVal = ""; 
       lcd.clear();
     }
     
     /////////////////////// READ INPUT FROM PIR SENSOR /////////////
     if(digitalRead(pirPin) == HIGH){
       //digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
       isMotionDetected = true;
       if(lockLow){ 
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;           
         //Serial.println("---");
         //Serial.print("motion detected at ");
         //Serial.print(millis()/1000);
         //Serial.println(" sec");
         delay(50);
         }        
         takeLowTime = true;
       }
     
     if(digitalRead(pirPin) == LOW){      
       //digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state
       isMotionDetected = false;
       lcd.setBacklight(false);
       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause,
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){ 
           //makes sure this block of code is only executed again after
           //a new motion sequence has been detected
           lockLow = true;                       
           //Serial.print("motion ended at ");      //output
           //Serial.print((millis() - pause)/1000);
           //Serial.println(" sec");
           delay(50);
           }
       }
       if(!lockLow && !waitingForValidation)
       {
         if (onlyOne){
           isCheckingPassword = true;
           slookingInputTime = millis();
         }
         lcd.setCursor(0,0); //Defining positon to write from first row,first column .
         lcd.print("Enter Password"); //You can write 16 Characters per line .
         //delay(1000);
       }
       
       
  }
  
