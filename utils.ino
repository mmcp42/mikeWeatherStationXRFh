//==============================================================
//  mikeWeatherStation
//
//  utilitiy routines module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=====================================================================================
// routine to initialise diagnostic port
//=====================================================================================
void diagInit(void)
{
#ifdef ENABLE_DIAG
  // initialise diag port for diagnostics
  //=====================================
  diagport.begin(9600);
#endif
}

//=====================================================================================
// Returns the number of bytes currently free in RAM
//=====================================================================================
static int freeRAM(void) 
{
  extern int  __bss_end;
  extern int* __brkval;
  int free_memory;
  if (reinterpret_cast<int>(__brkval) == 0) 
  {
    // if no heap use from end of bss section
    //=======================================
    free_memory = reinterpret_cast<int>(&free_memory) - reinterpret_cast<int>(&__bss_end);
  } else 
  {
    // use from top of stack to heap
    //==============================
    free_memory = reinterpret_cast<int>(&free_memory) - reinterpret_cast<int>(__brkval);
  }
  return free_memory;
}

//=====================================================================================
// show amount of RAM free
//=====================================================================================
void freeRAMshow(void)
{
  DIAGPRINT(F("   free RAM: "));
  DIAGPRINT(freeRAM()); 
  DIAGPRINTLN(F(" bytes"));
}

//=====================================================================================
// print separator
//=====================================================================================
void separatorPrint(int count)
{
  // print a separator
  //==================
  for (int i = 0; i<count; i++)
  DIAGPRINT("=");
  DIAGPRINTLN();
}


