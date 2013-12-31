//=====================================================================
// commands.ino
//
//  Created on: 25 Jul 2011
//      Author: Mike McPherson
//=====================================================================

// define some characters
//=======================
#define null (char)0
#define CR '\r'
#define LF '\n'

static int freeRAM(void);
void listSettings(void);

// command interface when connected to PC
//=======================================
void checkCommands(void)
{
  char charCom;
  char parameter;
  long value;
  int i, j, row, col;

  charCom = getChar();
  while (charCom != null)
  {
    switch (charCom)
    {
    case '?':
      showVersion();
      showCommands();
      break;
      
    case 'e':
      // EEPROM functions
      // next character says which action
      //====================================
      parameter = skipChars(' ');
      if (parameter==CR || parameter==LF)
      {
        Serial.println(F("eeprom?"));
        break;
      }
      switch (parameter)
      {
        // clear eeprom
        //==================
        case 'c':
        Serial.println("EEEPROM clear");
        for (i=0; i<256; i=i+1)
          eepromWriteByte(i, 0xFF);
        Serial.println(F("EEPROM cleared"));
        break;

        case 'd':
        // dump EEPROM
        //============
        Serial.println("EEEPROM dump");
        eepromDump();
        break;
        
        case 'r':
        // read all from EEPROM
        //=====================
        Serial.println("EEEPROM read settings");
        eepromReadAll();
        Serial.println(F("settings read from EEPROM"));
        listSettings();
        break;

        case 's':
        // saveall to EEPROM
        //====================
        Serial.println("EEEPROM save settings");
        eepromSaveAll();
        Serial.println(F("settings saved to EEPROM"));
        break;
      }
      break;
      
    case 'h':
      // set epoch
      //==========
      setEpoch();
      break;
        
    case 's':
      // list settings
      //==============
      listSettings();
      break;

    // parameter setting
    //==================
    case 'p':
      // set parameter
      // next character says which parameter
      // followed by value to set
      //====================================
      parameter = skipChars(' ');
      if (parameter==CR || parameter==LF)
      {
        Serial.println(F("parameter?"));
        break;
      }

      switch (parameter)
      {
      case 'm':
      // set Magic
      //============
      setString(&gMagic[0], sizeof(gMagic));
      listSettings();
      break;

      case 'w':
      // set WxId
      //=========
      setString(&gWxId[0], sizeof(gWxId));
      listSettings();
      break;

      default:
        Serial.print(parameter);
        Serial.println(F("??"));
        break;
      }
      break;

    case '\r':
      break;

    case '\n':
      break;

    default:
      Serial.print(charCom);
      Serial.println("??");
      break;
    }
    charCom = getChar();
  }
}

void listSettings(void)
{
  // show version to serial monitor
  //===============================
  showVersion();
  
  // list settings
  //==============
  Serial.print("    wxID: "); Serial.println((char*) gWxId);
  Serial.print("   magic: "); Serial.println((char*) gMagic);
}

void resetFactoryDefaults(void)
{
  // set and save initial values
  //============================
}

void showVersion(void)
{
  Serial.println();
  Serial.print(F("dataflashTest XRF"));
  Serial.print(' ');
  Serial.println(F("v1.02"));
  
  timestampShow(true);

  Serial.print(F("free RAM: ")); 
  Serial.print(freeRAM()); 
  Serial.println(F(" bytes"));
  Serial.println();
}

void showCommands(void)
{
  Serial.println(F("ec    = eeprom clear"));
  Serial.println(F("ed    = eeprom hex dump"));
  Serial.println(F("er    = eeprom read all settings"));
  Serial.println(F("es    = eeprom save all settings"));
  Serial.println(F("fc    = flash clear entire chip"));
  Serial.println(F("h xxx = set epoch"));
  Serial.println(F("s     = list settings"));
  Serial.println(F("p m   = set magic header prefix"));
  Serial.println(F("p w   = set wx station id"));
}

