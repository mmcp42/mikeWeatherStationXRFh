//==============================================================
//  mikeWeatherStation
//
//  grove module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=================================
// Grove power
//=================================
#define GROVEPOWERPIN    6

//=====================================================================================
// initialise grove pin
//=====================================================================================
void groveInit(void)
{
  pinMode(GROVEPOWERPIN, OUTPUT);     
}

//=====================================================================================
// power down switchable Grove sockets
//=====================================================================================
void groveOff(void)
{
  // set Grove power off
  //====================
  digitalWrite(GROVEPOWERPIN, LOW);
  DIAGPRINT('g');
  DIAGFLUSH();
}

//=====================================================================================
// power up switchable Grove sockets
//=====================================================================================
void groveOn(void)
{
  // set Grove power on
  //===================
  if (!digitalRead(GROVEPOWERPIN))
  {
    digitalWrite(GROVEPOWERPIN, HIGH);
    DIAGPRINT('G');
    DIAGFLUSH();
  }
}



