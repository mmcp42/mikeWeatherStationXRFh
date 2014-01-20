//==============================================================
//  mikeWeatherStation
//
//  data flash module
//
//  Mike McPherson
//  December 2013
//==============================================================

//=================================
// data record for writing results to flash
// and uploading to server
//=================================
#include "DataRecord.h"
#include <Sodaq_dataflash.h>
#define DF_SELECT   10
#define DF_MOSI     11
#define DF_MISO     12
#define DF_SPICLOCK 13

myRecord dataRecord;
int curPage;      // current page for writing
int curByte;      // byte address within page
int uploadPage;   // start page for upload

extern char gMagic[8];

//=====================================================================================
// erase one page of flash
//=====================================================================================
void dataFlashErase(int page)
{
  // erase a page of flash
  //======================
  dflash.pageErase(page);
}

//==============================================================
// flush one page from its internal buffer to flash
//==============================================================
void dataFlashFlushPage(int page)
{
  // write a page from internal buffer to flash
  //===========================================
  dflash.writeBuf1ToPage(page);
}

//=====================================================================================
// inialise data flash
//=====================================================================================
void dataFlashInit(void)
{
  // establish communication with flash
  //===================================
  dflash.init(DF_MISO, DF_MOSI, DF_SPICLOCK, DF_SELECT);
  
  // initialise page pointers
  // use epoch as random seed if needed
  //===================================
  findCurAndUploadPage(&curPage, &uploadPage, rtc.now().getEpoch());

  // show diagnostics
  //=================
  DIAGPRINT(F("initCurPage: ")); DIAGPRINT(curPage); DIAGPRINT("."); DIAGPRINTLN(curByte);
  DIAGPRINT(F(" uploadPage: ")); DIAGPRINTLN(uploadPage);
  if (uploadPage >= 0 && uploadPage == curPage) 
  {
    // Data flash is totally filled up
    // Forget about oldest upload page
    //================================
    uploadPage = getNextPage(uploadPage);
    DIAGPRINT(F("uploadPage: ")); DIAGPRINTLN(uploadPage);
    // No need to verify validity
  }
  dataFlashInitNewPage(curPage);
}

//=====================================================================================
// prepare flash page for data
//=====================================================================================
void dataFlashInitNewPage(int page)
{
  dataFlashErase(curPage);
  dflash.readPageToBuf1(curPage);
  curByte = 0;
  int i;
  
  // Write new header
  //=================
  myHeader hdr;
  hdr.ts = dataRecord.ts;
  hdr.version = DATA_VERSION;
  for (i=0; i<sizeof(hdr.magic); i++)
    hdr.magic[i] = gMagic[i];
  hdr.page = page;
  dflash.writeStrBuf1(curByte, (uint8_t *)&hdr, sizeof(hdr));
  curByte += sizeof(hdr);

  // Write flash internal buffer to the actual flash memory
  // The reason is that we don't want to lose the info if the software crashes
  //==========================================================================
  dflash.writeBuf1ToPage(curPage);
}

//=====================================================================================
// write data record to flash
//=====================================================================================
void dataRecordWriteToFlash(uint32_t epoch)
{   
  // write data record to flash
  //===========================

  // Is there enough room for a new record in the current page?
  //===========================================================
  if (curPage < 0 || curByte >= (int)(DF_PAGE_SIZE - sizeof(dataRecord))) 
  {
    // No, so start using the next page
    //=================================
    dataRecordCloseCurPage();
  }

  // Write the record to the page
  //=============================
  DIAGPRINT(F("wdr curPage: ")); DIAGPRINT(curPage); DIAGPRINT("."); DIAGPRINTLN(curByte);
  dataRecord.page = curPage;
  dflash.writeStrBuf1(curByte, (unsigned char *)&dataRecord, sizeof(dataRecord));
  curByte += sizeof(dataRecord);

  // Write flash internal buffer to the actual flash memory
  // The reason is that we don't want to lose the info if the software crashes
  //==========================================================================
  dflash.writeBuf1ToPage(curPage);

  if (uploadPage < 0) 
  {
    // Remember this as the first page to upload
    //==========================================
    uploadPage = curPage;
  }
  DIAGPRINT(F("wdr uplPage: ")); DIAGPRINTLN(uploadPage);
}

//=====================================================================================
// brief Close the current page and go to the next
//
// This function also takes care of making sure we always have
// an empty page between curPage and uploadPage.
// If fact uploadPage must be at least 2 ahead because the
// next page for curPage is going to be filled in shortly after
// this.
//=====================================================================================
void dataRecordCloseCurPage()
{
  DIAGPRINT(F("ccp curPage: ")); DIAGPRINT(curPage); DIAGPRINT("."); DIAGPRINTLN(curByte);
  
  dataRecordNewCurPage();
  if (uploadPage >= 0 && getNextPage(curPage) == uploadPage) 
  {
    // This triggers when the flash is completely full. It will
    // be very rare, but still...
    // The newCurPage did shift uploadPage one further, but
    // we always need an empty page and curPage will be
    // filled in shortly with its first record.
    //=========================================================
    uploadPage = getNextPage(uploadPage);
    dataFlashErase(getNextPage(curPage));
  }
}

//=====================================================================================
// brief Set curPage to the next page and make sure uploadPage is moved too
//=====================================================================================
void dataRecordNewCurPage()
{
  curPage = getNextPage(curPage);
  if (curPage == uploadPage) {
    uploadPage = getNextPage(uploadPage);
    if (!isValidUploadPage(uploadPage)) {
      uploadPage = -1;
    }
    DIAGPRINT(F("ncp uplPage: ")); DIAGPRINTLN(uploadPage);
  }

  dataFlashInitNewPage(curPage);
  DIAGPRINT(F("ncp curPage: ")); DIAGPRINT(curPage); DIAGPRINT("."); DIAGPRINTLN(curByte);
}


