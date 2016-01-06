/*
 GPS Data display by Glen Overby 01 August 2015
 Assumes a line by 20 character LCD display.
 Format:
   Latitude  Longitude
   Altitude   GridSquare
   Time Date
   Lock-mode Satellites
 
  Lock comes from $GPGSA sentence
  Date, Lat, Lon comes from $GPRMC sentence
  Altitude and number of satellites comes from $GPGGA sentence

 This is based on:

 LiquidCrystal Library - Serial Input

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch displays text sent over the serial port
 (e.g. from the Serial Monitor) on an attached LCD.

 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/LiquidCrystalSerial

 */
 
#define INPUT_BUFSIZE  128
#define OUTPUT_BUFSIZE 81

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

char gps_lock;
char gps_sats[3];
char gps_alt[6];

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  lcd.clear();
  lcd.write("Hello");
  // initialize the serial communications:
  Serial.begin(9600);
  gps_lock = '0';
  gps_alt[0] = '\0';
  gps_sats[0] = '0';
  gps_sats[1] = '0';
}

void loop()
{
  char input[INPUT_BUFSIZE];
  //char output[OUTPUT_BUFSIZE];
  char line1[21], line2[21], line3[21], line4[21], grid[7];
  int count, nstrings;
  char *array[16];
  char *in, *o, *s;
  
  count = Serial.readBytesUntil('\n', input, INPUT_BUFSIZE);
  // when characters arrive over the serial port...
  if (count > 0) {
    input[count] = '\0';
    Serial.write(input);
    Serial.flush();
    //lcd.write(input);
    in = input;
    if (*in == '\r')
      in++;
 
    if(!strncmp(in, "$GPGSA", 6)) {
      nstrings = split(in, array, 16, ',');
      Serial.write("GPS GSA!");
      Serial.write(array[2]);
      Serial.write("\n");
      Serial.flush();
      gps_lock = *array[2];
    }

    if(!strncmp(in, "$GPGGA", 6)) {
      nstrings = split(in, array, 16, ',');
      Serial.write("GPS GGA!");
      Serial.write(nstrings);
      Serial.write("\n");
      Serial.flush();
      /* copy the integer part of the altitude */
      s = array[9];
      o = gps_alt;
      while (*s != '\0' && *s != '.') {
        *o++ = *s++;
      }
      *o = '\0';
      /* Copy the number of satellites in use */
      s = array[7];
      o = gps_sats;
      while (*s != '\0') {
        *o++ = *s++;
      }
      *o = '\0';
    }
    if(!strncmp(in, "$GPRMC", 6)) {
      nstrings = split(in, array, 16, ',');
      Serial.write("GPS RMC!");
      Serial.write("\n");
      Serial.flush();
      
      if (nstrings > 10 && *array[2] == 'A') {
        int lat_degrees=0, lat_minutes=0, lat_seconds=0;
        int lon_degrees, lon_minutes, lon_seconds;
        char mh1,mh2,mh5,mh6;
        unsigned char mh3,mh4;
        int i;
        
        sscanf(array[3], "%2d%2d.%2d", &lat_degrees, &lat_minutes,
          &lat_seconds);
        sscanf(array[5], "%3d%2d.%2d", &lon_degrees, &lon_minutes,
          &lon_seconds);
        sprintf(line1, "%d %d %d%c  %d %d %d%c", 
          lat_degrees, lat_minutes, lat_seconds, *array[4],
          lon_degrees, lon_minutes, lon_seconds, *array[6]);
        
        /* Convert latitude & longitude to Maidenhead Grid */
        {
          float minute, second, lat, lon, I;
          int C1, C2, C3, C4, C5, C6;
          char L1, L2, L5, L6;
          
          minute=1.0/60.0;
          second=1.0/3600.0;

          lat = lat_degrees + lat_minutes*minute + lat_seconds * second;
          lon = lon_degrees + lon_minutes*minute + lon_seconds * second;
          
          /* Longitude */
          C1 = (int)((180 - lon) / 20);
          L1 = 'A' + C1;
          
          I = fmod((180 - lon), 20.0);
          C3 = (int)(I / 2);

          I = fmod(I, 2);
          C5 = (int) (I * 12);
          L5 = 'a'+C5;

          /* Latitude */
          C2 = (int)((lat+90) / 10);
          L2 = 'A'+C2;

          I = fmod((lat+90), 10);
          C4 = (int)(I);

          C6 = (int)((I-C4) * 24);
          L6 = 'a'+C6;

          sprintf(grid,"%c%c%d%d%c%c", L1, L2, C3, C4, L5, L6);
        }
        
        sprintf(line2, "%5.5s        %s",
          gps_alt, grid);
        sprintf(line3, "%2.2s:%2.2s:%2.2s  %2.2s/%2.2s/%2.2s  ",
          array[1], array[1]+2, array[1]+4,
          array[9]+2, array[9], array[9]+4);
        sprintf(line4, "%c %2.2s",
          gps_lock, gps_sats);

        lcd.clear();    // clear the screen
        lcd.write(line1);
        lcd.setCursor(0,1);
        lcd.write(line2);
        lcd.setCursor(0,2);
        lcd.write(line3);
        lcd.setCursor(0,3);
        lcd.write(line4);
      } else {
        lcd.clear();
        lcd.write("No Lock");
      }
    }
  }
}

/*
 Split a string into an array based on a separator
 */
int split(char *string, char **array, int count, char sep)
{
  char *s, **a;
  int c;
  s = string;
  a = array;
  *a++ = s;
  c = 1;
  while(count--) {
    while (*s != sep && *s != '\0')
      s++;
    if (*s == '\0')
      return c;
    *s++ = '\0';
    *a++ = s;
    c++;
  }
  return c;
}



