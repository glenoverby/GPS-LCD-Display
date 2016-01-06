# GPS-LCD-Display
An Arduino sketch to display data from a GPS on a Hitachi HD44780 compatible display

It uses (and is based on) the Arduno LiquidCrystalSerial library, which
interfaces to Hitachi HD44780 compatible displays.  See: 
[LiquidCrystalSerial](https://www.arduino.cc/en/Tutorial/LiquidCrystalSerialDisplay?from=Tutorial.LiquidCrystalSerial)

The display format assumes a line by 20 character LCD display.
 Format:
```
   Latitude  Longitude
   Altitude   GridSquare
   Time Date
   Lock-mode Satellites
```

![Image of display](https://raw.githubusercontent.com/glenoverby/GPS-LCD-Display/master/doc/LCDgps-front.JPG)

 
* Lock comes from $GPGSA sentence
* Date, Lat, Lon comes from $GPRMC sentence
* Altitude and number of satellites comes from $GPGGA sentence

The wiring of an Arduino Mini to the display looks like:

![Image of wiring](https://raw.githubusercontent.com/glenoverby/GPS-LCD-Display/master/doc/LCDgps-back.JPG)


