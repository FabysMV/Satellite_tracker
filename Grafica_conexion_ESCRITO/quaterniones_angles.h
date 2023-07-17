#include "MPU9250.h" 
#define MPU9250_IMU_ADDRESS 0x68 
#define MAGNETIC_DECLINATION 3.67 // To be defined by user 
#define INTERVAL_MS_PRINT 10 
MPU9250 mpu; 
unsigned long lastPrintMillis = 0; 
/*Conexión:
 VCC = 3.3V
 GND = GND
 SCL = D22
 SDA = D21*/


class quaterniones_angles{
  private:
  
//  

public:
   int roll;
   int yaw;
  
  
  // DMP_FEATURE_LP_QUAT can also be used. It uses the 
  void IMU_inicializar()
  {
   Wire.begin(); 
   Serial.println("Starting..."); 
   MPU9250Setting setting; 
   // Sample rate must be at least 2x DLPF rate 
   setting.accel_fs_sel = ACCEL_FS_SEL::A16G; 
   setting.gyro_fs_sel = GYRO_FS_SEL::G1000DPS; 
   setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS; 
   setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_250HZ; 
   setting.gyro_fchoice = 0x03; 
   setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_20HZ; 
   setting.accel_fchoice = 0x01; 
   setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_45HZ; 
   mpu.setup(MPU9250_IMU_ADDRESS, setting); 
   mpu.setMagneticDeclination(MAGNETIC_DECLINATION); 
   mpu.selectFilter(QuatFilterSel::MADGWICK); 
   //mpu.selectFilter(QuatFilterSel::MAHONY); 
   mpu.setFilterIterations(80); 
   Serial.println("Calibration will start in 5sec."); 
   Serial.println("Please leave the device still on the flat plane."); 
   delay(5000); 
   Serial.println("Calibrating..."); 
   mpu.calibrateAccelGyro(); 
   Serial.println("Magnetometer calibration will start in 5sec."); 
   Serial.println("Please Wave device in a figure eight until done."); 
   delay(5000); 
   Serial.println("Calibrating..."); 
   mpu.calibrateMag(); 
   Serial.println("Ready!");    
  }

  void CalibrateGyro()
  {
     Serial.println("Calibration will start in 5sec."); 
     Serial.println("Please leave the device still on the flat plane."); 
     delay(5000); 
     Serial.println("Calibrating..."); 
     mpu.calibrateAccelGyro(); 
    
    }


  void CalibrateMag()
  {
     Serial.println("Magnetometer calibration will start in 5sec."); 
     Serial.println("Please Wave device in a figure eight until done."); 
     delay(5000); 
     Serial.println("Calibrating..."); 
     mpu.calibrateMag(); 
     Serial.println("Ready!"); 
    }
 
  void calculaEulerAngles()
  {
     unsigned long currentMillis = millis(); 
     if (mpu.update()&& currentMillis - lastPrintMillis > INTERVAL_MS_PRINT)
     { 
      Serial.println("imu aqui");
         roll = mpu.getRoll(); 
         yaw = mpu.getYaw(); 
         lastPrintMillis = currentMillis; 
        
     } 
  }

  
  int GetRoll(){
     if(roll>=0 && roll <266){roll = roll + 90;}
         else if(roll >= 270 && roll <=360){roll = roll -270;}  
         if(roll >= 180 && roll <=270){roll = 180;}
         if(roll >270 && roll <=370){roll = 0;} 
    return roll;}
  
 
  int GetYaw(){return yaw;} 

  };
