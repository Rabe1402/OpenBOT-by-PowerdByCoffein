#include "KeplerOpenBOT.h"
const int CENTER_VALUE = 100;
const int MAX_LINE_ERROR = 60;
const int SPIN_THRESHOLD = 10;
const int LCD_UPDATE_INTERVAL = 100; // in ms

float motorl_offset = 13;
float motorr_offset = -13;
float kp = 2.0;
int line_error = 0;

float motorl_speed = 0;
float motorr_speed = 0;
int start_normal = 40;
unsigned long lastLcdUpdate = 0;

void setup() {
  KeplerOpenBOT_INIT();
  WRITE_LCD_CONTRAST(170);
  Serial.begin(9600);
  digitalWrite(SPICAMCSPIN, HIGH);
}

void loop() {

  byte spi_buffer[8] = {0};
 
  // read 8 bytes from OpenMV BEGIN
 
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

WRITE_LCD_INT(1, 1, spi_buffer[1], 4);
WRITE_LCD_INT(5, 1, spi_buffer[2], 4);


  //--- Button logic -----
if (READ_BUTTON_CLOSED(B1)==1) {
    start_normal = 1;
    WRITE_LCD_CLEAR(); 
}

if (READ_BUTTON_CLOSED(B4)==1) {
    start_normal = 0; 
    WRITE_LCD_CLEAR();
    WRITE_MOTOR(ML, 0);
    WRITE_MOTOR(MR, 0);
}

if (start_normal==0) {
  Serial.println("no program selected");
  Serial.println("press button B1 for normal startup");
  WRITE_LCD_TEXT(1, 1, "Select Program");
  WRITE_LCD_TEXT(1, 2, "B1 for normal startup");
  delay(100); 
}

if (start_normal==1) {

  byte spi_buffer[8] = {0};
 
  // read 8 bytes from OpenMV BEGIN
 
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



  Serial.print("SPI DATA: ");
  Serial.print(spi_buffer[1]);
  Serial.println(spi_buffer[2]);
  Serial.println("SPI END");
  
  WRITE_LED(1);
  delay(1);;
  WRITE_LED(0);


  line_error = spi_buffer[1] - CENTER_VALUE;

  motorl_speed = motorl_offset - kp * line_error;
  motorr_speed = motorr_offset + kp * line_error;

  motorl_speed = constrain(motorl_speed, -100, 100);
  motorr_speed = constrain(motorr_speed, -100, 100);

  WRITE_MOTOR(ML, (int)motorl_speed);
  WRITE_MOTOR(MR, (int)motorr_speed);

  WRITE_LCD_TEXT(1, 1, "Angle:");
  WRITE_LCD_INT(7, 2, line_error, 4);
  Serial.print("Angle: ");
  Serial.println(line_error);

  WRITE_LCD_TEXT(3, 4, "ML");
  WRITE_LCD_INT(1, 5, (int)motorl_speed, 4);

  WRITE_LCD_TEXT(12, 4, "MR");
  WRITE_LCD_INT(10, 5, (int)motorr_speed, 4);

  WRITE_LCD_TEXT(1, 6, spi_buffer[2]);
  Serial.println(spi_buffer[2]);

  // --- if perfect on line ---
  if (abs(motorl_speed + motorr_speed) < SPIN_THRESHOLD) {
    WRITE_MOTOR(ML, -30);
    WRITE_MOTOR(MR, 30);
    Serial.println("Perfect on line turning");
  } 
}
}
