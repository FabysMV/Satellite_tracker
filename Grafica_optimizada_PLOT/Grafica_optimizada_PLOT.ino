// UPDATES:
// SE ELIMINÓ LA FUNCIÓN SWITCH POR SER COMPLETAMENTE INECESARIA, LAS FUNCIONES SE LLAMAN DIRECTAMENTE DESDE
// LAS FUNCIONES IF-ELSE

#include "motor_mov.h"
motor_mov azimut; //objeto de motor
motor_mov cenit;  //objeto de motor
#include "quaterniones_angles.h"
quaterniones_angles euler;  //objeto de orientacion
#include "pid.h"
PID pid_a;
PID pid_c; 
// Variables
/*usamos una clase para controlar los motores y mandamos a llamar a lo que nosotros designemos como velocidad alta,
  media y de tracker (que será la más baja)*/
/*Las oscilaciones son causa de una constante P alta
  Los rebotes que da al moverse son causa de un D alto
  El integral es de cuidado... Si quieres puedes mover los límites un poco
  derivativo entre más grande amortigua oscilaciones 
  proporcional conforme aumenta da más voltaje, para que vaya más rápido
  integral alto te ayuda a disminuir el error
  */
int pid_val_a = 0;    //Valor del pid para el azimut
char estate;
char estate_c;
int error_a = 0.0;  // error en el azimut 
int error_c = 0.0;  // error en el cenit


int anglec = 45;
int anglez = 180;
int R = 0;
int Y = 0;
int val = 0;
int pinC = 5;
int pinA = 18;
//::::::::::::::::::::::::::::::::::::
//////// valores para el PID ////////
int error_a_prev = 0.0;
int error_c_prev = 0.0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//::  ojo, las constantes que escribimos aqui son el resultado de multiplicar k_real*10, esto es para                           :://
//::  optimizar el tiempo de las operaciones al declararlas como integer;;; El ajuste a float se realiza en el documento pid.h  :://
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//**tuning constants azimut**//
int kp_a = 5;   //6       4       2       1       PROBAR LOS VALORES ASIGNADOS
float ki_a = 0.05;  //0.0005  0.05   0.005  0.0005  
int kd_a = 0.5;     //5       3     1       0.5     
float cv = 0.0;
//**tuning constants cenit**//
int kp_c = 8;
float ki_c = 0.05;
int kd_c = 8;
////////////////////////////////////
//:::::::::::::::::::::::::::::::::::
unsigned long tiempo = 0;
unsigned long tiempo_prev = 0;
void setup() {
  // ponemos los pines que vamos a usar de la esp32
  
  Serial.begin(115200);
  tiempo = millis();  
  pid_a.init_pid();
  pid_c.init_pid();
  azimut.SetUp(pinA);
  cenit.SetUp(pinC);
  delay(100);
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
  euler.IMU_inicializar();    // Inicializando IMU
  delay(2000);
  
}

void loop() {
 // for(;;){
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
    
    //:::::::::: Revisamos en qué estado estamos :::::::::::::: //
     if(R <= 3 || R >= 178){
        Serial.println('f');
        cenit.Stop(pinC);
        anglec = R; // esto hará que el error en cenit sea 0, provocando que el motor no se mueva para nada, pero el PID del azimut seguirá funcionando
     }
    
     
     /// en esta linea mandamos a llamar las posiciones deseadas y las actuales y calculamos el error
      error_a = anglez - Y;
      error_c = anglec - R;
     
      kp_a = analogRead(15)*0.02; //valor que ya se ha multiplicado por 10
      Serial.print(',');
      Serial.print(kp_a); 
      Serial.print(',');
      cv = analogRead(2)*.1;
      ki_a = cv*0.002;
      
      Serial.print(ki_a);
      Serial.print(',');
      
      kd_a =  analogRead(4)*0.02;  //valor que ya se ha multiplicado por 10
     
      Serial.println(kd_a);  
     
      
    //::::: Calculamos el PID::::::////////////////////////////////////
    pid_a.Cal_PID(error_a, kp_a, kd_a, ki_a);
    pid_c.Cal_PID(error_c, kp_c, kd_c, ki_c);
    //:::::::::::::::::::::::::::://////////////////////////////////
    tiempo_prev = tiempo;  
    ////asignación de estados para el AZIMUT ////
        if(error_a == 0.0)   //apagalo otto apagalo
        {
          azimut.Stop(pinA);
         
          
        }
        else if(error_a > 0.0)  //POSITIVO
        {
               
          if(error_a <= 180){
             pid_val_a = -(pid_a.val_pid());
          //   Serial.print("Error POSITIVO Conviene movimiento horario \t");
             giro_horario(pid_val_a, azimut, pinA, 'a' );
          
          }else if(error_a >180) {
            //sentido antihorario azimut
            pid_val_a = pid_a.val_pid();
          //  Serial.print("Error POSITIVO Conviene movimiento antihorario \t");
            giro_anti(pid_val_a, azimut, pinA, 'a');
           
          }
          }
    
        else if (error_a < 0.0) //negativo
        {
           
          if(abs(error_a) >= 180){
            pid_val_a = -(pid_a.val_pid());
           // Serial.print("Error negativo Conviene movimiento antihorario \t");
           giro_anti(pid_val_a, azimut, pinA, 'a');
          }
          else if(abs(error_a) < 180){
            //sentido horario azimut
            pid_val_a = pid_a.val_pid();
         //   Serial.print("Error negativo MOV-- horario\t");
            giro_horario(pid_val_a, azimut, pinA, 'a' );
          
          }   
          }
    ////asignación de estados para el CENIT ////
        if(error_c == 0.0)   //apagalo otto apagalo
        {
            cenit.Stop(pinC);
         //  Serial.println("Error cero cenit wuu");
        }
        else if(error_c > 0.0) //ERROR POSITIVO
        {
         //  Serial.println("Error positivo cenit");
           giro_anti(pid_c.val_pid(), cenit, pinC, 'c');
          }
    
        else if (error_c < 0.0) //ERROR NEGATIVO
        {
            //sentido horario
        //   Serial.println("Error negativo cenit");
           giro_horario(pid_c.val_pid(), cenit, pinC, 'c');
          }
    
       
    
      //Serial.print(',');
     // Serial.println(Y);
     // Serial.print(',');
      //Serial.println(R);
      
  //}
 }




////////// FUNCIONES ///////////////////
void giro_anti(float pid, motor_mov objeto, int pin, char tipo)
{
  val = 0;
  /*Serial.println(tipo);
  Serial.print("pid antihorario de ");
  Serial.print(tipo);
  Serial.print(": ");
  Serial.println(pid);*/
 if (tiempo - tiempo_prev <100){  
    val = 1500 - pid;
    if(val <= 1000){
      val = 1000;
    }else if(val >= 1500){
      val = 1500;
    }
    objeto.mov(pin, val);
    /*Serial.print("val antihorario: ");
    Serial.println(val);*/
    //delay(100);
 }
}


//////// funcion antihorario //////////

void giro_horario(float pid, motor_mov objeto, int pin, char tipo)  //giro horario
{
  
  val = 0;
 /* Serial.println(tipo);
  *  Serial.print("pid horario de ");
  Serial.print(tipo);
  Serial.print(": ");
  Serial.println(pid);*/
  if (tiempo - tiempo_prev <100){ 
    val = 1500 - pid;
    if(val >= 2000){
      val = 2000;
    }else if(val <= 1500){
      val = 1500;
    }
    objeto.mov(pin, val);
  //  delay(100);
  /*  Serial.print("val horario: ");
    Serial.println(val);*/
    /* Serial.print(",");
    Serial.println(val);*/
  }   
}
