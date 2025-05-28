#include "KeplerOpenBOT.h"
 
float motorl_offset;
float motorr_offset;
float motorl_speed;
float motorr_speed;
float kp;
int line_error;
int start_normal;

 
void setup()
{
  KeplerOpenBOT_INIT();
  WRITE_LCD_CONTRAST(160);
 
  motorl_offset = 20;
  motorr_offset = 20;
  kp = 0.5;
}
 
void loop()
{
 
  byte spi_buffer[8] = {0};
 
  // read 8 Bytes from OpenMV BEGIN
 
  digitalWrite(SPICAMCSPIN, LOW);
  delay(1); 
 
  if(SPI.transfer(1) == 85)
  { 
    spi_buffer[1]=SPI.transfer(0);
    spi_buffer[2]=SPI.transfer(0);
    spi_buffer[3]=SPI.transfer(0);
    spi_buffer[4]=SPI.transfer(0);
    spi_buffer[5]=SPI.transfer(0);
    spi_buffer[6]=SPI.transfer(0);
    spi_buffer[7]=SPI.transfer(0);
  }
  
  digitalWrite(SPICAMCSPIN, HIGH);
  // read 8 Bytes from OpenMV END
 
  //--- Button logic -----
  if (READ_BUTTON_CLOSED(B1)==1) 
  {
    start_normal = 1;
    WRITE_LCD_CLEAR(); 
  }

  if (READ_BUTTON_CLOSED(B4)==1) 
  {
    start_normal = 0; 
    WRITE_LCD_CLEAR();
    WRITE_MOTOR(ML, 0);
    WRITE_MOTOR(MR, 0);
  }

  if (start_normal==0) 
  {
    Serial.println("no program selected");
    Serial.println("press button B1 for normal startup");
    WRITE_LCD_TEXT(1, 1, "Select Program");
    WRITE_LCD_TEXT(1, 2, "B1 for normal startup");
    delay(100); 
  }

  if (start_normal==1) 
  {
   line_error = spi_buffer[1] - 100;
    motorl_speed = motorl_offset - kp * line_error;
    motorr_speed = motorr_offset + kp * line_error;
 
    WRITE_MOTOR(ML,(int)motorl_speed);
    WRITE_MOTOR(MR,(int)motorr_speed);
 
    WRITE_LCD_TEXT(1,1,"Angle:");
    WRITE_LCD_INT(7,2,line_error,4);
 
    WRITE_LCD_TEXT(3,4,"ML");
    WRITE_LCD_INT(1,5,(int)motorl_speed,4);
 
    WRITE_LCD_TEXT(12,4,"MR");
    WRITE_LCD_INT(10,5,(int)motorr_speed,4);
  
    WRITE_LCD_INT(1, 6, (int)spi_buffer[2], 4);
    
    //--- red line logic -----
    while (spi_buffer[2]==1) 
    {
      WRITE_MOTOR(ML, 0);
      WRITE_MOTOR(MR, 0);
      WRITE_LCD_CLEAR(); 
      WRITE_LCD_TEXT(1, 1, "RED LINE");
      WRITE_LCD_TEXT(1, 2, "END OF CODE");
    } 
  }
}

