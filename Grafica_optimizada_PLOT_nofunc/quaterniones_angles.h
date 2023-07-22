#include <MPU9250_RegisterMap.h>
#include <SparkFunMPU9250-DMP.h>
//#include <ESP32Servo.h>

/*Conexión:
 VCC = 3.3V
 GND = GND
 SCL = D22
 SDA = D21*/
MPU9250_DMP imu;
class quaterniones_angles{
  private:
  

public:

  
  // DMP_FEATURE_LP_QUAT can also be used. It uses the 
  void IMU_inicializar(){
      //Call imu.begin() to verify communication and initialize
      if (imu.begin() != INV_SUCCESS)
      {
        while (1)
        {
          Serial.println("Unable to communicate with MPU-9250");
          Serial.println("Check connections, and try again.");
          Serial.println();
          delay(5000);
        }
      }
    
      imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT | // Enable quat 6 ejes
                   DMP_FEATURE_GYRO_CAL, // calibraciÃ³n giroscopio
                  10); // Set DMP FIFO rate to 10 Hz
  }
 
  void calculaEulerAngles(){
    // Check for new data in the FIFO
    if ( imu.fifoAvailable() )
    {
      // Use dmpUpdateFifo to update the ax, gx, mx, etc. values
      if ( imu.dmpUpdateFifo() == INV_SUCCESS)
      {
        // computeEulerAngles can be used -- after updating the
        // quaternion values -- to estimate roll, pitch, and yaw
        //Serial.println("Computing... ");
        imu.computeEulerAngles();
        giveIMUData();
           
      }
     }//fin del if imu.fifoAvailable
  }

  void giveIMUData(void){  

  float q0 = imu.calcQuat(imu.qw);
  float q1 = imu.calcQuat(imu.qx);
  float q2 = imu.calcQuat(imu.qy);
  float q3 = imu.calcQuat(imu.qz);
   if( imu.roll>=0 && imu.roll <266){
    imu.roll = imu.roll + 90;
    }else if(imu.roll >= 270 && imu.roll <=360) {
    imu.roll = imu.roll -270;
    }  
  if(imu.roll >= 180 && imu.roll <=270)
  {
    imu.roll = 180;
    }
  if(imu.roll >270 && imu.roll <=370)
  {
    imu.roll = 0;
    }
   
 /* Serial.println("Roll: " + String(imu.roll) + ", Yaw: "
             + String(imu.yaw));
  Serial.println("Time: " + String(imu.time) + " ms");
  Serial.println();*/
  
  }
  
  float GetRoll(){
    
   /* if( imu.roll>=0 && imu.roll <266){
    imu.roll = imu.roll + 90;
    }else if(imu.roll >= 270 && imu.roll <=360) {
    imu.roll = imu.roll -270;
    }  
  if(imu.roll >= 180 && imu.roll <=270)
  {
    imu.roll = 180;
    }
  if(imu.roll >270 && imu.roll <=370)
  {
    imu.roll = 0;
    }*/
    return  imu.roll;}
  
 
  float GetYaw(){
    return imu.yaw;} 

 
  };
