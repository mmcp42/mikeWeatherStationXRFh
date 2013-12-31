//=====================================================================
// functions.ino
//
//  Created on: 25 Jul 2011
//      Author: Mike McPherson
//=====================================================================

// define some characters
//=======================
#define null (char)0
#define CR '\r'
#define LF '\n'

char getChar(void)
{
  // function to get the next char from the COM port
  // returns null if nothing in yet
  //================================================

  int charCount;
  char charCom;

  charCom = null;
  charCount = Serial.available();

  if (charCount > 0)
  {
    charCom = Serial.read();
    // Serial.print(charCom);
  }
  return charCom;
}

long getNumber(char *terminator)
{
  char charCom;
  long P1;
  boolean isHex;

  // read a number from COM
  // skips leading spaces
  // returns character that terminated the number
  // uses the terminator passed as the first possible character
  // returns -1 by default
  //===========================================================
  P1 = -1;
  isHex = 0;

  // start with character passed
  //============================
  charCom = *terminator;

  // skip leading spaces
  //====================
  if (charCom==' ')
    charCom = skipChars(' ');

  // end of command?
  //================
  if (charCom==CR || charCom==LF)
  {
    // yup, so nothing to do!
    //=======================
    *terminator = charCom;
    return P1;
  }

  if (charCom=='x' || charCom=='X')
  {
    isHex = 1;
    charCom = getChar();
  }

  // process up to end of line
  //==========================
  P1 = 0;
  while ((charCom != CR) && (charCom != LF)&& charCom != ' ')
  {
    if (isHex==1)
    {
      // read hex digit
      //================
      while ( (charCom>='0' && charCom<='9') || (charCom >= 'A' && charCom <= 'F') )
      {
        if (charCom>='0' && charCom<='9')
          P1 = P1 * 16 + charCom - '0';
        else
          P1 = P1 * 16 + charCom - 'A' + 10;
        charCom = getChar();
      }
    }
    else
    {
      // read P1 as a decimal number
      //============================
      while ((charCom>='0') && (charCom<='9'))
      {
        P1 = P1 * 10 + charCom - '0';
        charCom = getChar();
      }
    }
    charCom = getChar();
  }
  *terminator = charCom;
  return P1;
}

void setString(char *parameter, int size)
{
  int i;
  char charCom;
  
  // initialise empty
  //=================
  for (i=0; i<size; i++)
    parameter[i] = NULL;
  
  // read from command to end of line or 32 chars
  //=============================================
  charCom = skipChars(' ');
  i = 0;
  while ((charCom != CR) && (charCom != LF) && (i<32))
  {
    // process next character
    //=======================
    parameter[i] = charCom;
    charCom = skipChars(' ');
    i++;
  }
}

char skipChars(char toSkip)
{
  char charCom;

  // wait for non-NULL character from COM port
  // returns 1st non-NULL character
  //===========================================
  charCom = toSkip;
  while ((charCom == toSkip) || (charCom == null))
    charCom = getChar();
  return charCom;
}




