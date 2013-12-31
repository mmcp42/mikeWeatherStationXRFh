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
  // initialise XRF hardware
  //========================
  pinMode(XRF_SLEEP_PIN, OUTPUT);
  pinMode(XRF_CTS_PIN, INPUT_PULLUP);
  xrfWake();
  
  // enter command mode
  //===================
  Serial.println();
  delay(1100);
  Serial.print("+++");
  delay(1100);
  xrfWaitReply();

  // set sleep mode
  //===============
  Serial.println("ATSM2");
  xrfWaitReply();
  
  // exit command mode
  //==================
  Serial.println("ATDN");
  xrfWaitReply();  
}

void xrfSleep(void)
{
  // set XRF module asleep
  //======================
  digitalWrite(XRF_SLEEP_PIN, HIGH);
}

void xrfWake(void)
{
  // wake up XRF module
  //===================
  digitalWrite(XRF_SLEEP_PIN, LOW);
  
  // wait for ready to send
  //=======================
  while (digitalRead(XRF_CTS_PIN)==HIGH);
  //delay(500);
}

void xrfWaitReply(void)
{
  char charCom;
  
  while(true)
  {
    if (Serial.available())
    {
      charCom = Serial.read();
    }
    if (charCom == '\n' || charCom == '\r')
      break;
  }
}
