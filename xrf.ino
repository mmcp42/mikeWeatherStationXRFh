//==============================================================
//  mikeWeatherStation
//
//  XRF module
//
//  Mike McPherson
//  December 2013
//==============================================================

#define XRF_SLEEP_PIN 7
#define XRF_CTS_PIN 8

void xrfInit(void)
{
  boolean reply;
  
  // initialise XRF hardware
  //========================
  pinMode(XRF_SLEEP_PIN, OUTPUT);
  pinMode(XRF_CTS_PIN, INPUT_PULLUP);
  xrfWake();
  
  reply = false;
  while (reply == false)
  {
    // enter command mode
    //===================
    XRFPRINTLN();
    delay(1100);
    XRFPRINT("+++");
    delay(1100);
    reply = xrfWaitReply();
  
    // set sleep mode
    //===============
    XRFPRINTLN("ATSM2");
    reply = xrfWaitReply();
  
    // exit command mode
    //==================
    XRFPRINTLN("ATDN");
    reply = xrfWaitReply();  
  }
}

void xrfSleep(void)
{
  // if already asleep
  // quietly go about business
  //===========================
  if (digitalRead(XRF_CTS_PIN)==HIGH)
    return;

#ifdef DEBUG
  DIAGPRINTLN('x');
  delay(100);
#endif
  DIAGFLUSH();

  // set XRF module asleep
  //======================
  digitalWrite(XRF_SLEEP_PIN, HIGH);
}

void xrfWake(void)
{
  int looper;
  
  // if already awake
  // quietly go about business
  //===========================
  if (digitalRead(XRF_CTS_PIN)==LOW)
    return;
  
  // wake up XRF module
  //===================
  digitalWrite(XRF_SLEEP_PIN, LOW);
  
  // wait for ready to send
  //=======================
  for (looper = 0; looper < 1000; looper++)
  {
    if(digitalRead(XRF_CTS_PIN)==LOW)
      break;
    delayMicroseconds(100);
  }
  
  if(digitalRead(XRF_CTS_PIN)==HIGH)
    DIAGPRINT(F("xrfWake fail"));

#ifdef DEBUG
  DIAGPRINT('X');
#endif
}

boolean xrfWaitReply(void)
{
  char charCom;
  int i;
  
  for (i=0; i<15; i++)
  {
    if (xrfport.available())
    {
      charCom = xrfport.read();
      if (charCom == '\n' || charCom == '\r')
      {
        return true;
        break;
      }
    }
    delay(100);
  }
  return false;
}
