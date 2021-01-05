
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#include <avr/sleep.h>
#endif
#define PIN 7 // Hier wird angegeben, an welchem digitalen Pin die WS2812 LEDs bzw. NeoPixel angeschlossen sind
#define TASTERPIN 2
#define BEEPPIN 11
#define NUMPIXELS 40 // Hier wird die Anzahl der angeschlossenen WS2812 LEDs bzw. NeoPixel angegeben
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int mode = 4; 
int color = 0;

//Button vars
int button_released=0;
boolean longpress=false;
unsigned long pressed_time=0;
unsigned int loops=0;


int pause=10; // 100 Millisekunden Pause bis zur Ansteuerung der nächsten LED.

int black[] = {0,0,0};
int colors[][3] = {
  {255,255,255},
  {255,0,0},
  {0,255,0},
  {0,0,255},
  {255,255,0},//yellow
  {0,255,255},//turkis
  {130,0,130},//lila
  {60,120,13},
  {95,40,0},//orange
  
};
int colcount= 9;


void INT_PINisr(void)
  /* ISR fuer Pin 2 */
  {
  /* detach Interrupt, damit er nur einmal auftritt */
  Serial.print(".");

  
  }


void setup() {
  Serial.begin(115200);
  Serial.println("Init pixels");
  pixels.begin(); // Initialisierung der NeoPixel
  pinMode(TASTERPIN,INPUT);
  pinMode(BEEPPIN,OUTPUT);
  Serial.println("Setup done");
}

void loop() {
  loops++;
  // put your main code here, to run repeatedly:
 switch(check_button()){
    case 0:
      break;
    case 1:
      //mode_static_color(black);
      return;
    case 2:
    color++;
      Serial.print("Color:");
      Serial.println(color);
      beep(0);
      break;  
    case 4: 
      mode++;
      Serial.print("Mode:");
      Serial.println(mode);
    beep(0);
      break; 

  }
  
  
  switch(mode%5){
  case 0:
      mode_static_color(colors[color%colcount]);
      break;
  case 1:
      mode_ring_strobo(colors[color%colcount]);
      break;
  case 2:
      //blue
      mode_strobo(colors[color%colcount]);
      break;
    case 3:
      //blue
      mode_color_transition(colors[(color+loops)%colcount],colors[(loops)%colcount]);
      //mode_color_flow(colors[color%colcount]);
      break;
  case 4:
      mode_static_color(black);
      enter_sleep();
  }



}

void beep(int ms){
    pinMode(BEEPPIN,OUTPUT);
        for(int x=0;x<5;x++){
          digitalWrite(BEEPPIN,255);
          delay(ms);
          digitalWrite(BEEPPIN,0);
      }
        pinMode(BEEPPIN,INPUT);
}

/**
 * Read button and debounce
 * return
 * 1=button pressed but not released yet
 * 2=button pressed <2sec and released 
 * 3=button pressed >2sec and released 
 * 4=button pressed >xsec and not released / will repeat every threshold sec
 */
int check_button(){
  int button=0;
  int ret=0;
  int threshold=1500;
  
  
   button = digitalRead(TASTERPIN);
   if(button == LOW  && button_released == 0){
        button_released=1;
        pressed_time=millis();
        Serial.println("press ");
        ret=1;
   }
  if(button == LOW  && button_released == 1){
     int lptime = millis() - pressed_time;
     if(lptime > threshold){
          Serial.println("Button Long press ");
          pressed_time=millis();
          longpress=true;
          ret=4;
    }
  }
  if(button_released == 1 && button == HIGH){
    button_released=0;
    int lptime = millis() - pressed_time;
    if(lptime > threshold | longpress){
          Serial.println("Button Long press & released");
          longpress=false;
          ret=3;
    }else{
          Serial.println("Button press & released");
          ret=2;
    }
    delay(100);
  }
   return ret;
}

void mode_static_color(int col[]){
 for(int i=0; i<=40; i++){
    pixels.setPixelColor(i, pixels.Color(col[0],col[1],col[2])); // Pixel1 leuchtet in der Farbe Grün
     pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  
}

void mode_ring_strobo(int col[]){
   for(int i=0; i<=40; i++){
    pixels.setPixelColor(i, pixels.Color(col[0],col[1],col[2])); // Pixel1 leuchtet in der Farbe Grün
    delay(3);
     pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
   for(int i=0; i<=40; i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Pixel1 leuchtet in der Farbe Grün
     pixels.show(); // Durchführen der Pixel-Ansteuerung
  }
  }

  
void mode_strobo(int col[]){
   for(int i=0; i<=40; i++){
    pixels.setPixelColor(i, pixels.Color(col[0],col[1],col[2])); // Pixel1 leuchtet in der Farbe Grün
     
  }
  pixels.show(); // Durchführen der Pixel-Ansteuerung
  delay(50);
   for(int i=0; i<=40; i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Pixel1 leuchtet in der Farbe Grün
     
  }
  pixels.show(); // Durchführen der Pixel-Ansteuerung
    delay(50);
  }



void mode_color_transition(int colfrom[], int colto[]){
  
  int r = colfrom[0];
  int g = colfrom[1];
  int b = colfrom[2];
  int i = 0;
  int red =colto[0];
  int green=colto[1];
  int blue=colto[2];

  int rdiff = red -r;
  //0 255
  int gdiff = green -g ;
  int bdiff = blue -b;


   //Color with animation
   for(int i=0; i<40;i++) {
    r=r+(rdiff/40);
    g=g+(gdiff/40);
    b=b+(bdiff/40);
    //interupt
    
    pixels.setPixelColor(i, pixels.Color(r,g,b)); 
    pixels.show();
    delay(pause);
    
    if( i%10 == 9 && digitalRead(TASTERPIN) == LOW) {
      Serial.println("button interupt");
      return;
    }
 }

 
}

void mode_color_flow(int col[]) 
{


  for(int y=0; y<=6; y++){

int r = col[0]-y*10;
int g = y*col[1];
int b = y*col[2];
    
  
  for(int i=0; i<=40; i++){
pixels.setPixelColor(i, pixels.Color(100-i*y,g,2*i+y)); // Pixel1 leuchtet in der Farbe Grün

  }

pixels.show(); // Durchführen der Pixel-Ansteuerung
   delay (pause); // Pause, in dieser Zeit wird nichts verändert. 
   

}
 for(int i=1; i<=40; i++){
pixels.setPixelColor(i, pixels.Color(0,0,0)); // Pixel1 leuchtet in der Farbe Grün

  }
  pixels.show(); // Durchführen der Pixel-Ansteuerung
delay (pause); // Pause, in dieser Zeit wird nichts verändert.

}


void enter_sleep(void)  
  { 
  attachInterrupt(0, INT_PINisr, LOW); 
  delay(10);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
  Serial.println("Enter sleep in 2 sec");
  delay(2000);
  sleep_enable();  
  sleep_mode();  
  /** Das Programm läuft ab hier nach dem Aufwachen weiter. **/  
  /** Es wird immer zuerst der Schlafmodus disabled.        **/  
  sleep_disable();   
  detachInterrupt(0);
  Serial.println("Exit sleep");
  //Avoid color change after wake up
  button_released=0;
  longpress=true;
  mode=0;
  color=0;
  }  
