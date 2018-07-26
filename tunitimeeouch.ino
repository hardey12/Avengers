#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>
#include <EEPROM.h>
#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

// Calibrate values
#define TS_MINX 965
#define TS_MINY 85
#define TS_MAXX 125
#define TS_MAXY 905

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x3F7D
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define GREY    0x2108
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Meter colour schemes
#define RED2RED 0
#define GREEN2GREEN 1
#define BLUE2BLUE 2
#define BLUE2RED 3
#define GREEN2RED 4
#define RED2GREEN 5
#define BOXSIZE 55
#define BOXSIZE1 120
#define BOXSIZE2 45

  
int coldcolor, ccurrentcolor;
int tcount;
bool cupdata=false;

int address = 50;
int address1 = 51;

int oldcolor, currentcolor;
int pcount;
bool updat=false;
bool updata=false;
int n1=1;
uint32_t runTime = -99999;       // time for next update
int reading = 0; // Value to be displayed
int d = 0; // Variable used for the sinewave test waveform
boolean alert = 0;
int8_t ramp = 1;
int tesmod =0;

bool dupdata;
bool iupdata;

int count1;
int motor = 13;
int slv  = 28;
int ss;
int s;
int m1;
 int m=0;
 int val;
int tempPin = 8;

const int ledPin = 12;// the number of the LED pin

int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 700;   


void setup(void) {
    pinMode(motor, OUTPUT);                    
  pinMode(ledPin, OUTPUT);                    
  pinMode(slv, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("Paint!"));
 
  tft.begin(0x9341); // SDFP5408
pcount = EEPROM.read(address);                  // read the value from the address
   tcount = EEPROM.read(address1);    
  tft.setRotation(0); // Need for the Mega, please changed for your choice or rotation initial

  // Border

  drawBorder();
  
  // Initial screen
  
  tft.setCursor (30, 50);
  tft.setTextSize (3);
  tft.setTextColor(BLUE);
  tft.println("MEDILUTION");
  tft.setCursor (65, 85);
  tft.println("L.L.P.");
  tft.setCursor (45, 150);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);
  tft.println("Light Source");

  tft.setCursor (60, 250);
  tft.setTextSize (1);
  tft.setTextColor(BLACK);
  tft.println("Touch to proceed");

  waitOneTouch();
  tft.fillScreen(BLUE);


tft.setCursor (1,2);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);
  tft.println("PR in mmhg");

tft.setCursor (126,2);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);
  tft.println("TM in min.");

tft.setCursor (1,95);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);
  tft.println("SetP");

tft.setCursor (126,95);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);
  tft.println("SetT");
  


tft.fillRect(0, 90,260, 5, RED);    
tft.fillRect(120, 0,5, 238, RED);    
tft.fillRect(0, 140,260, 5, RED);
tft.fillRect(0, 237,260, 3, RED);

tft.fillCircle(60, 190, BOXSIZE2, YELLOW);
 tft.setCursor (35,180);
  tft.setTextSize (3);
  tft.setTextColor(BLACK);
  tft.println("I.F");
  
  tft.fillCircle(180, 190, BOXSIZE2, GREEN);
tft.setCursor (155,180);
  tft.setTextSize (3);
  tft.setTextColor(BLACK);
  tft.println("D.F");
  

 tft.fillRect(0, 245,BOXSIZE, BOXSIZE+20, WHITE);
 tft.setCursor (5,270);
  tft.setTextSize (4);
  tft.setTextColor(BLACK);
  tft.println("P+");
 
  tft.fillRect(BOXSIZE+5, 245,BOXSIZE, BOXSIZE+20, WHITE);
 tft.setCursor (BOXSIZE+10,270);
  tft.setTextSize (4);
  tft.setTextColor(BLACK);
  tft.println("P-");

  
 tft.fillRect((BOXSIZE+5)*2, 245,BOXSIZE, BOXSIZE+20, WHITE);
  tft.setCursor (((BOXSIZE+5)*2)+5,270);
  tft.setTextSize (4);
  tft.setTextColor(BLACK);
  tft.println("T+");
 
  tft.fillRect((BOXSIZE+5)*3, 245,BOXSIZE, BOXSIZE+20, WHITE);
 tft.setCursor (((BOXSIZE+5)*3)+5,270);
  tft.setTextSize (4);
  tft.setTextColor(BLACK);
  tft.println("T-");
 
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop()
{
  
val = analogRead(tempPin);
float mv = ( val); 
int y = map(mv, 533, 980, 1, 500);
if(y < 0)
      y = 0;

tft.setCursor (0,25);
  tft.setTextSize (5.5);
  tft.setTextColor(BLACK,BLUE);

  tft.print(y);
  tft.println(" ");

   updata = false;
   cupdata = false;
    
  TSPoint p = ts.getPoint();
  

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

    
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    // *** SPFD5408 change -- End
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());;


 
  if (p.y > 190 && p.y <235) {
       coldcolor = ccurrentcolor;

       if (p.x < 100)
       { 
        dupdata = false;
       iupdata = true;
         ccurrentcolor = RED; 
         tft.drawCircle(60, 190, BOXSIZE2, BLACK);
       }
       else
       if (p.x < 240) 
       {
    dupdata = true;
    iupdata = false;
    ccurrentcolor = YELLOW; 
        tft.drawCircle(180, 190, BOXSIZE2, BLACK);
       } 
if (coldcolor != ccurrentcolor) {
          if (coldcolor == RED) tft.drawCircle(60, 190, BOXSIZE2, WHITE);
          if (coldcolor == YELLOW) tft.drawCircle(180, 190, BOXSIZE2, WHITE);
          }
      }    
  
  

    if (p.y > 245) {
       oldcolor = currentcolor;

       if (p.x < BOXSIZE+5) { 
         pcount=pcount+5;

   if(pcount > 500)
      pcount = 500;

         currentcolor = RED; 
         tft.drawRect(0, 245,BOXSIZE, BOXSIZE+20, BLACK);
       } else if (p.x < (BOXSIZE+5)*2) {
 pcount=pcount-5;

   if(pcount < 0)
      pcount = 000;
        
         currentcolor = YELLOW;
        tft.drawRect(BOXSIZE+5, 245,BOXSIZE, BOXSIZE+20, BLACK);
       }
       else
       if (p.x < (BOXSIZE+5)*3) { 
         tcount=tcount+1;

   if(tcount > 90)
     tcount = 90;

         currentcolor = GREEN; 
         tft.drawRect((BOXSIZE+5)*2, 245,BOXSIZE, BOXSIZE+20, BLACK);
       } else if (p.x < (BOXSIZE+5)*4) {
 tcount=tcount-1;

   if(tcount < 0)
      tcount = 000;
        
         currentcolor = CYAN;
         tft.drawRect((BOXSIZE+5)*3, 245,BOXSIZE, BOXSIZE+20, BLACK);
       }
        
if (oldcolor != currentcolor) {
          if (oldcolor == RED) tft.drawRect(0, 245,BOXSIZE, BOXSIZE+20, WHITE);
          if (oldcolor == YELLOW) tft.drawRect(BOXSIZE+5, 245,BOXSIZE, BOXSIZE+20, WHITE);
          if (oldcolor == GREEN) tft.drawRect((BOXSIZE+5)*2, 245,BOXSIZE, BOXSIZE+20, WHITE);
          if (oldcolor == CYAN)  tft.drawRect((BOXSIZE+5)*3, 245,BOXSIZE, BOXSIZE+20, WHITE);

          }
      updata = true;
      }    
  }

  
  

Serial.println(iupdata);

if(iupdata)

  {
tft.setCursor (130,25);
  tft.setTextSize (6);
  tft.setTextColor(BLACK,BLUE);
  tft.print(m1);
tft.println(" ");
 Serial.print(m1);
ss=ss+1;  
if(ss==7)
{ 
 ss=0; 
 s=s+1; 
} 
if(s==60) 
{ 
 s=0; 
 m1=m1+1; 
  
} 
if(m1==100) 
{ 
 m1=1; 
}  


   if( tcount <= m1)               
  {
    unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
if (ledState == 0) {
      ledState = 10;
    } else {
      ledState = 0;
    }
    analogWrite(ledPin, ledState);
  }
  }
 
   if( tcount > m1)               
  {
  digitalWrite(ledPin,LOW);
  }
      
int x1= pcount+8;
int x2= pcount-5;
    
   if( y > x1 )               
  {  
        digitalWrite(slv, LOW);
     delay (300);
  digitalWrite(slv, HIGH);
    delay (20);
  digitalWrite(motor, LOW);
  
  }  
     if( y < x2 )               
  
   {
    analogWrite(motor, 80);
     digitalWrite(slv, LOW);
   
   }     

       else
  {
        digitalWrite(motor, LOW);    // turn the LED off by making the voltage LOW
     digitalWrite(slv, LOW);
   
}      

}


if(dupdata)
{
  if(y  > 4)               
 { 
 digitalWrite(slv, LOW);    // turn the LED off by making the voltage LOW
     delay (600);
  digitalWrite(slv, HIGH);
  delay (20);
  digitalWrite(motor, LOW);
   
      
 }
 if(y  < 4)               
 { 
   // prints the"mV" notation
 digitalWrite(slv, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(motor, LOW);
 }
}


   
     if (updata)
  {

tft.setCursor (45,110);
  tft.setTextSize (3);
  tft.setTextColor(BLACK,BLUE);
  tft.print(pcount);
  tft.println(" ");

tft.setCursor (170,110);
  tft.setTextSize (3);
  tft.setTextColor(BLACK,BLUE);
  tft.print(tcount);
  tft.println(" ");
  }
EEPROM.write(address, (pcount));                 // write the value to the EEPROM address
EEPROM.write(address1, (tcount));                 // write the value to the EEPROM address
}

// Wait one touch

TSPoint waitOneTouch() {

  // wait 1 touch to exit function
  
  TSPoint p;
  
  do {
    p= ts.getPoint(); 
  
    pinMode(XM, OUTPUT); //Pins configures again for TFT control
    pinMode(YP, OUTPUT);
  
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  
  return p;
}


void drawBorder () {

  // Draw a border

  uint16_t width = tft.width() - 1;
  uint16_t height = tft.height() - 1;
  uint8_t border = 10;

  tft.fillScreen(RED);
  tft.fillRect(border, border, (width - border * 2), (height - border * 2), WHITE);
  
}
