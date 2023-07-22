//////////////////VARIABLES PARA EL IMU//////////////////////////////////////////////////
#include "MPU9250.h" 
#define MPU9250_IMU_ADDRESS 0x68 
#define MAGNETIC_DECLINATION 3.67 // To be defined by user 
#define INTERVAL_MS_PRINT 50 
MPU9250 mpu; 
unsigned long lastPrintMillis = 0; 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATES:
// SE ELIMINÓ LA FUNCIÓN SWITCH POR SER COMPLETAMENTE INECESARIA, LAS FUNCIONES SE LLAMAN DIRECTAMENTE DESDE
// LAS FUNCIONES IF-ELSE
// Se agregó una nueva librería que utiliza el magnetómetro del IMU y lo calibra
//-------------------------------------------------------------------------------------------------------------------------------------
//---- Variables de para el motor ---------------------//
    #include "motor_mov.h"
    motor_mov azimut; //objeto de motor
    motor_mov cenit;  //objeto de motor
    int pinC = 18;
    int pinA = 5;
//-----------------------------------------------------------//
//----------------------------------------------------------//
//---- Variables de para el PID ---------------------------//
    #include "pid.h"
    PID pid_a;
    PID pid_c; 
    int pid_val_a = 0;    //Valor del pid para el azimut
    int pid_val_c = 0;
    int error_a = 0;  // error en el azimut 
    int error_c = 0;  // error en el cenit
/*Las oscilaciones son causa de una constante P alta
  Los rebotes que da al moverse son causa de un D alto
  El integral es de cuidado... Si quieres puedes mover los límites un poco
  derivativo entre más grande amortigua oscilaciones 
  proporcional conforme aumenta da más voltaje, para que vaya más rápido
  integral alto te ayuda a disminuir el error
  */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//::  ojo, las constantes que escribimos aqui son el resultado de multiplicar k_real*10, esto es para                           :://
//::  optimizar el tiempo de las operaciones al declararlas como integer;;; El ajuste a float se realiza en el documento pid.h  :://
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//**tuning constants azimut**//
    int kp_a =27;   //6       4       2       1           5
    float ki_a = 0.37;  //0.0005  0.05   0.005  0.0005    0.02
    int kd_a = 18;     //5       3     1       0.5       0.5
    float cv = 0.0;
    //**tuning constants cenit**//
    int kp_c = 44;
    float ki_c = 0.37;
    int kd_c = 14;
//---------------------------------------------------//
int R = 0;
int Y = 0;
//::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::
unsigned long tiempo = 0;
unsigned long tiempo_prev = 0;
unsigned long delta_t = 0;
unsigned long delta_a = 0;
unsigned long delta_wifi = 0;
unsigned long delta_imu = 0;
boolean flag = false;
boolean flag_Wifi = false;
boolean flag_imu = false;
/////BOTÓN/////////////
int boton = 2; //pin para BOTON 
int estate_b = 0;
//------- variables para la conexión -------//
#include "Track.h"
Track track;
//Valores de los angulos a los que se va a posicionar el tracker
int anglec = 45; //angulo de cenit
int anglez = 0;  //angulo de azimut
///////////////////////////////////////////////

////////////////////////////////////////////////
void setup() {
   Serial.begin(115200);
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
  pinMode(boton, INPUT);
 
  tiempo = millis();  
  pid_a.init_pid(); //inicializa control PID para azimut
  pid_c.init_pid(); //inicializa control PID para cenit
  azimut.SetUp(pinA);  //se prepara motor para azimut en su pin correspondiente
  cenit.SetUp(pinC);   //se prepara motor para cenit en su pin correspondiente
/////MOVIMIENTO PRUEBA DE LOS MOTORES////////////////////////////////////////

 /* Serial.println("Inicializando prueba de motores en 5 segundos...");
  delay(5000);
  cenit.maxima(pinC);
  azimut.maxima(pinA);
  delay(1000);
  cenit.Stop(pinC);
  azimut.Stop(pinA);
  cenit.maxima_ccw(pinC);
  azimut.maxima_ccw(pinA);
  delay(1000);
  cenit.Stop(pinC);
  azimut.Stop(pinA);
  delay(1000);*/
 // ::::: Se inicializa la conexión con internet ::::///
 //Descomentarlo cuando sea necesario realizar la conexión a internet una vez que 
 //ya funcione el control de los motores con el imu
  /*Serial.println("Inicializando conexión en 5 segundos...");
  delay(5000);
  track.Connect();
  delay(2000);*/
  
 // euler.IMU_inicializar();   // Inicializando IMU
  //euler.CalibrateGyro();  //calibrando imu
  
//////// ENTRAR EN LOOP HASTA QUE SE PRESIONE EL BOTÓN DE CONEXIÓN///////////
//  do{
//    Serial.println("Presiona el botón para continuar");
//    estate_b = digitalRead(boton);
//    delay(1000);
//    }while(estate_b == LOW);
//    
//  euler.CalibrateMag();
//////// ENTRAR EN LOOP HASTA QUE SE PRESIONE EL BOTÓN DE CONEXIÓN///////////
  do{
    Serial.println("Presiona el botón para continuar");
    estate_b = digitalRead(boton);
    delay(1000);
     
    }while(estate_b == LOW);
 
}

void loop() {
  for(;;){
     tiempo = millis(); //se guarda el valor del tiempo en milisegundos
     Serial.println("");
     Serial.println(tiempo); 
     Serial.print(anglez);
     Serial.print(',');
     Serial.print(anglec);
     Serial.print(',');
     //Se inicializa nuevamente el control PID 
      pid_a.init_pid();
      pid_c.init_pid();
     //se guarda en otra variable el valor del tiempo en milisegundos, servirá para utilizar el IMU
     
        //euler.calculaEulerAngles();
        //se guardan en variables los valores de Roll y Yaw        
       // R = euler.GetRoll();
        //Y = euler.GetYaw();
      //unsigned long currentMillis = millis(); 
           if (mpu.update() && tiempo - lastPrintMillis > INTERVAL_MS_PRINT) { 
            Serial.println("imu aqui");
              Serial.print(mpu.getRoll()); 
              R = mpu.getRoll();
                if(R>=0 && R <266){R = R + 90;}
                 else if(R >= 270 && R <=360){R = R-270;}  
                 if(R>= 180 && R<=270){R = 180;}
                 if(R >270 && R<=370){R = 0;}
              Serial.print(","); 
              Serial.println(mpu.getYaw()); 
              Serial.println(""); 
             Y = mpu.getYaw();
             lastPrintMillis = tiempo; 
           } 
   
      Serial.print(R);
      Serial.print(',');
      Serial.println(Y);
    
/// en esta linea mandamos a llamar las posiciones deseadas y las actuales y calculamos el error
      error_a = anglez - Y;
      error_c = anglec - R;
//:::::::::: Revisamos que el ángulo de cenit sea válido:::::::::::::: //
     if(R <= 3 || R >= 178){
        Serial.println('f');
        cenit.Stop(pinC);
        error_c = 0;// esto hará que el error en cenit sea 0, provocando que el motor no se mueva para nada, pero el PID del azimut seguirá funcionando
     }
     
    /*  kp_a = analogRead(15)*0.02; //valor que ya se ha multiplicado por 10
      Serial.print(',');
      Serial.print(kp_a); 
      Serial.print(',');
      cv = analogRead(2)*.1;
      ki_a = cv*0.002;
      
      Serial.print(ki_a);
      Serial.print(',');
      
      kd_a =  analogRead(4)*0.02;  //valor que ya se ha multiplicado por 10
     
      Serial.println(kd_a); */
//--- Se imprimen los valores de los errores
      Serial.println("");
      Serial.print("Error de A y C:\t"); 
      Serial.print(error_a);
      Serial.print("\t");
      Serial.println(error_c);
      
//::::::: Calculamos el PID::::::::::::::::::::::::::::::::::::::::::::::::://
    pid_a.Cal_PID(error_a, kp_a, kd_a, ki_a);
    pid_c.Cal_PID(error_c, kp_c, kd_c, ki_c);
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://

//:::::::::::::::   Calculo de TIMERS   ::::::::::::::::::::::::::::::::::://
    tiempo_prev = tiempo; 
    tiempo = millis(); 
    delta_t = tiempo - tiempo_prev;
/////////////////////////////////////////////////////////////////////////////
 //::: TIMER PARA MOTORES ::::::::::::::
    if (flag == false)
    {
      delta_a = delta_a + delta_t;
      }
    else if (flag == true) 
    {
      flag = false;
      delta_a = 0;
      }
     if (delta_a >=10 )
     {
      flag = true;
      } 
 ////////////////////////////////////
 //::: TIMER PARA WIFI ::::::::::::::
     if (flag_Wifi == false)
        {
          delta_wifi = delta_wifi + delta_t;
          }
        else if (flag_Wifi == true) 
        {
          flag_Wifi = false;
          delta_wifi = 0;
          }
         if (delta_wifi >=10000 )
         {
          flag_Wifi = true;
          }  
//:::::::::::::::::::::::::::::::::::  
 ////////////////////////////////////   
//---- CONEXIÓN A INTERNET --> ACTUALIZACIÓN DE LOS ÁNGULOS ******************************************
          if (flag_Wifi == true) //si se ha cumplido el tiempo, se realiza la conexión
          {
            //falta poner la línea para realizar la conexión a internet
            Serial.println("conexión a internet \t");
            //Serial.println(delta_wifi);
            }
       
//****************************************************************************************************
            Serial.print("count  conexión a internet \t");
            Serial.println(delta_wifi);//se imprime la cuenta que lleva el timer de la conexión a internet para verificar que funciona correctamente
//****************************************************************************************************
////-----asignación de estados para el AZIMUT ////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::aqui se realizan los movimientos de los motores ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        if(error_a == 0.0)   //apagalo otto apagalo
        {
          azimut.Stop(pinA); //detiene el motor
          Serial.println("Error cero azimut wuu");
        }
        else if(error_a > 0.0)  // si el error es POSITIVO
        {
               
          if(error_a <= 180){     //////Para cuando conviene el movimiento contrario
             pid_val_a = pid_a.val_pid();
             Serial.print("Error POSITIVO < 180 anti-horario \t");
             
              if (flag == true){ // se mandará la orden de para mover los motores cuando el timer de los motores se haya desbordado
               azimut.giro_anti(pid_val_a, pinA, 'a' );
               }
          
          }else if(error_a >180) {
            //sentido antihorario azimut
            pid_val_a = -pid_a.val_pid();
            Serial.print("Error POSITIVO >180 horario \t");
            
           if (flag == true){azimut.giro_horario(pid_val_a, pinA, 'a' );}
          }
        }
          
    
        else if (error_a < 0) //negativo
        {
           
          if(abs(error_a) >180){
            pid_val_a = -pid_a.val_pid();
            Serial.print("Error negativo > 180 anti-horario \t");
           if (flag == true){  
            azimut.giro_anti(pid_val_a, pinA, 'a' );}
          }
          else if(abs(error_a) <= 180){
            //sentido anti-horario azimut
            pid_val_a = pid_a.val_pid();
            Serial.print("Error negativo < 180 horario\t");
            if (flag == true){ 
            azimut.giro_horario(pid_val_a, pinA, 'a' );}
          }
        }   
          
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
////-----asignación de estados para el CENIT ////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::aqui se realizan los movimientos de los motores ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        if(error_c == 0.0)   //apagalo otto apagalo
        {
            cenit.Stop(pinC); //para el motor
            Serial.println("Error cero cenit wuu");
        }
        else if(error_c > 0.0) //ERROR POSITIVO
        {
           Serial.println("Error positivo cenit");
           pid_val_c = pid_c.val_pid();
           if (flag == true){  
            cenit.giro_anti(pid_val_c, pinC, 'c' );}
          }
    
        else if (error_c < 0.0) //ERROR NEGATIVO
        {
            //sentido horario
            Serial.println("Error negativo cenit");
            pid_val_c = pid_c.val_pid();
            if (flag == true){cenit.giro_horario(pid_val_c, pinC, 'c' );}
         }
          
    
// valores para graficar en dado caso en el que se desee así       
      //Serial.print(',');
     // Serial.println(Y);
     // Serial.print(',');
      //Serial.println(R);
  Serial.println(delta_a ); //se imprime el valor del timer de los motores para verificar que funcione correctamente
   Serial.println("");
  Serial.println("-----------------------------------------------")    ;
  Serial.println("");
 
  }
  
 }
