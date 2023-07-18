#include <ESP32Servo.h>

// ****   NOTAS   ****  
//  Siempre hay que escribir la función de writeMicroseconds ANTES de poner la función attach    //
//  porque esa función (attach) manda los últimos datos escritos en en writeMicroseconds        //
//                                  TEN CUIDADO CON ESO                                        //

class motor_mov{
  private:
  Servo servo;
  //valores en microsegundos On a standard servo, this will set the angle of the shaft. 
  //On standard servos a parameter value of 1000 is fully counter-clockwise, 2000 is fully clockwise, and 1500 is in the middle.

  public:
////// aqui van los métodos /////
    void SetUp(int pin){  //aqui ponemos los pines correspondientes para la esp32 relacionados con los motores
      servo.attach(pin);
      }


    void Stop(int pin){
     
     servo.attach(pin);
      servo.writeMicroseconds(1500);
       // servo.attach(pin);  
          
      }
  
      
    void maxima(int pin){
       
    //  servo.attach(pin);
      servo.writeMicroseconds(1750); 
     
       //  servo.attach(pin); 
     // Serial.println("HOLI horario");
      }

     
    void maxima_ccw(int pin){
     
       servo.attach(pin);
       servo.writeMicroseconds(1250);
     //   servo.writeMicroseconds(1250);
       //  servo.attach(pin);
    //  Serial.println("HOLI antihorario");
       }


   void mov(int pin, int vale)
     {
        Serial.print("Ahi voy con:");
        Serial.println(vale);
        Serial.print("Ahi voy con PIN:");
        Serial.println(pin);
        servo.attach(pin);   
        servo.writeMicroseconds(vale);
      //  servo.writeMicroseconds(vale);
       
      }

  };
