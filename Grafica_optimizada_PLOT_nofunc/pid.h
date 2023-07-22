class PID{
  private:
  unsigned long  TIME = 0;
  float          delta_t= 0.0;
  unsigned long  prev_time = 0;
  int error_prev = 0.0;
  int PID = 0;
  int pid_d = 0;
  int pid_i = 0;
  int pid_p = 0;

  
  public: 

    void init_pid()
    {
      prev_time = TIME;
      TIME = millis();
      delta_t = (TIME-prev_time)*0.001; // Se divide entre 1000 para convertir a segundos
      }
  
    void Cal_PID(int error, int kp, int kd, int ki)
    {
      float pid_pf = 0.0;
      float pid_df = 0.0;
       pid_p = kp*(error);
       pid_d = kd*((error-error_prev)/delta_t);
        pid_pf = pid_p*0.1;
        pid_df = pid_d*0.1;
       if(-5.0 <error && error <5.0)
        {
        pid_i = pid_i+(ki*error);  
        }
      
        PID = pid_pf + pid_df + pid_i;

      error_prev = error;
      
      }

     int val_pid()
     {
       return PID;
      }
       
  };
