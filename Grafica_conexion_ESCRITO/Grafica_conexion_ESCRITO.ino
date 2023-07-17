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
//---- Variables de para el IMU -----------------------------//
    #include "quaterniones_angles.h"
    quaterniones_angles euler;  //objeto de orientacion
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



char estate;
char estate_c;




int R = 0;
int Y = 0;
int val = 0;

//::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::
unsigned long tiempo = 0;
unsigned long tiempo_prev = 0;
unsigned long delta_t = 0;
unsigned long delta_a = 0;
unsigned long delta_wifi = 0;
boolean flag = false;
boolean flag_Wifi = false;
/////BOTÓN/////////////
int boton = 2; //pin para BOTON 
int estate_b = 0;
//------- variables para la conexión -------//
#include "Track.h"
Track track;
int anglec = 45;
int anglez = 0;
///////////////////////////////////////////////
////////////////////////////////////////////////
void setup() {
  pinMode(boton, INPUT);
  Serial.begin(115200);
   tiempo = millis();  
  pid_a.init_pid();
  pid_c.init_pid();
  azimut.SetUp(pinA);
  cenit.SetUp(pinC);
  euler.IMU_inicializar();    // Inicializando IMU
 //euler.CalibrateGyro();
  
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
    euler.calculaEulerAngles();
     Serial.println(euler.GetRoll());
     Serial.println(euler.GetYaw());
    estate_b = digitalRead(boton);
    delay(1000);
     
    }while(estate_b == LOW);
  //MOVIMIENTO PRUEBA DE LOS MOTORES
  Serial.println("Inicializando prueba de motores en 5 segundos...");
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
  delay(1000);
  /*Serial.println("Inicializando conexión en 5 segundos...");
  delay(5000);
  track.Connect();*/
  
  delay(2000);
  
}

void loop() {
  for(;;){
     Serial.println("");
    Serial.println(tiempo);
     tiempo = millis();  
     Serial.print(anglez);
     Serial.print(',');
     Serial.print(anglec);
     Serial.print(',');
      
      pid_a.init_pid();
      pid_c.init_pid();
      euler.calculaEulerAngles();
        
      R = euler.GetRoll();
      Y = euler.GetYaw();
     
      Serial.print(R);
      Serial.print(',');
      Serial.print(Y);
    
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
//---- CONEXIÓN A INTERNET --> ACTUALIZACIÓN DE LOS ÁNGULOS ******************************************
          if (flag_Wifi == true)
          {
            Serial.println("conexión a internet \t");
            //Serial.println(delta_wifi);
            }
       
//****************************************************************************************************
      Serial.print("count  conexión a internet \t");
            Serial.println(delta_wifi);
    ////asignación de estados para el AZIMUT ////
        if(error_a == 0.0)   //apagalo otto apagalo
        {
          azimut.Stop(pinA);
          Serial.println("Error cero azimut wuu");
         
          
        }
        else if(error_a > 0.0)  //POSITIVO
        {
               
          if(error_a <= 180){     //////Para cuando conviene el movimiento cotrario
             pid_val_a = pid_a.val_pid();
             Serial.print("Error POSITIVO < 180 anti-horario \t");
             //giro_horario(pid_val_a, azimut, pinA, 'a' );
              val = 0;
              
              Serial.print("pid anti-horario de a");
              Serial.print(": ");
              Serial.println(pid_val_a);
              if (flag == true){ 
                val = 1500 - pid_val_a;
                if(val <= 1000){
                  val = 1000;
                }else if(val >= 1500){
                  val = 1500;
                }
                azimut.mov(pinA, val);
              //  delay(100);
                 Serial.print("val anti-horario: ");
                Serial.println(val);
                /* Serial.print(",");
                Serial.println(val);*/
              }
          
          }else if(error_a >180) {
            //sentido antihorario azimut
            pid_val_a = pid_a.val_pid();
            Serial.print("Error POSITIVO >180 horario \t");
            //giro_anti(pid_val_a, azimut, pinA, 'a');
            val = 0;
            
            Serial.print("pid horario de a");
            Serial.print(": ");
            Serial.println( pid_val_a);
           if (flag == true){  
              val = 1500 +  pid_val_a ;
              if(val >= 2000){
                val = 2000;
              }else if(val <= 1500){
                val = 1500;
              }
              azimut.mov(pinA, val);
              Serial.print("val horario: ");
              Serial.println(val);
              //delay(100);
           }
          }
           
          }
          
    
        else if (error_a < 0) //negativo
        {
           
          if(abs(error_a) >180){
            pid_val_a = pid_a.val_pid();
            Serial.print("Error negativo > 180 anti-horario \t");
           
            val = 0;
       /////////////////////////////////////////////////////     
            Serial.print("pid horario de a");
            Serial.print(": ");
            Serial.println(pid_val_a);
           if (flag == true){  
              val = 1500 +  pid_val_a;
              if(val <= 1000){
                val = 1000;
              }else if(val >= 1500){
                val = 1500;
              }
              azimut.mov(pinA, val);
              Serial.print("val anti-horario: ");
              Serial.println(val);
              //delay(100);
           }
          }
          else if(abs(error_a) <= 180){
            //sentido anti-horario azimut
            pid_val_a = pid_a.val_pid();
            Serial.print("Error negativo < 180 horario\t");

            val = 0;
           
            Serial.print("pid horario de a");
            Serial.print(": ");
            Serial.println(pid_val_a);
            if (flag == true){ 
              val = 1500 - pid_val_a;
              if(val >= 2000){
                val = 2000;
              }else if(val <= 1500){
                val = 1500;
              }
              azimut.mov(pinA, val);
            //  delay(100);
               Serial.print("val horario: ");
              Serial.println(val);
              /* Serial.print(",");
              Serial.println(val);*/
            }
          
          }   
          }
    ////asignación de estados para el CENIT ////
        if(error_c == 0.0)   //apagalo otto apagalo
        {
            cenit.Stop(pinC);
            Serial.println("Error cero cenit wuu");
        }
        else if(error_c > 0.0) //ERROR POSITIVO
        {
           Serial.println("Error positivo cenit");
           //giro_anti(pid_c.val_pid(), cenit, pinC, 'c');
            val = 0;
            pid_val_c = pid_c.val_pid();
            Serial.print("pid antihorario de c");
            Serial.print(": ");
            Serial.println(pid_val_c );
           if (flag == true){  
              val = 1500 - pid_val_c ;
              if(val <= 1000){
                val = 1000;
              }else if(val >= 1500){
                val = 1500;
              }
              cenit.mov(pinC, val);
              Serial.print("val antihorario: ");
              Serial.println(val);
              //delay(100);
           }
          }
    
        else if (error_c < 0.0) //ERROR NEGATIVO
        {
            //sentido horario
            Serial.println("Error negativo cenit");
           //giro_horario(pid_c.val_pid(), cenit, pinC, 'c');
           val = 0;
            pid_val_c = pid_c.val_pid();
            Serial.print("pid horario de c");
            Serial.print(": ");
            Serial.println(pid_val_c);
            if (flag == true){ 
              val = 1500 - pid_val_c;
              if(val >= 2000){
                val = 2000;
              }else if(val <= 1500){
                val = 1500;
              }
              cenit.mov(pinC, val);
            //  delay(100);
               Serial.print("val horario: ");
              Serial.println(val);
              /* Serial.print(",");
              Serial.println(val);*/
            }
          }
    
       
    
      //Serial.print(',');
     // Serial.println(Y);
     // Serial.print(',');
      //Serial.println(R);
  Serial.println(delta_a );
   Serial.println("");
  Serial.println("-----------------------------------------------")    ;
  Serial.println("");
 

  }
  
 }
