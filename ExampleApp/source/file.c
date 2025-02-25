/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         FAT File handling
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          25.05.2021
 *
 *--------------------------------------------------------------------
 */

#include "platform.h"
#include "file.h"
#include "util.h"
#include "ff.h"
#include "string.h"

#if SOLUTION == 0

static FIL file;
static FATFS fs;

void fileMount(void)
{
  FRESULT result;
  ilog("mounting card");
  result = f_mount(&fs, "0", 1);
  if (result != FR_OK)
  {
    elog("Could not mount file system");
  }
  else
  {
    ilog("mount ok");
  }
}

void fileOpen(char *filename)
{
  FRESULT result;
  // Create/open file
  // Create file
  result = f_open(&file, filename, FA_OPEN_APPEND|FA_WRITE);
  if (result != FR_OK)
  {
    elog("failed creating file");
    return;
  }
  else
  {
    ilog("file opened for writing: %s", filename);
  }
}

void fileClose(void)
{
  FRESULT result;
  // Closing File
  result =  f_close(&file);
  if (result != FR_OK)
  {
    elog("failed closing file");
    return;
  }
  else
  {
    ilog("file closed");
  }
}


void fileWrite(char *text)
{
  UINT bw;
  FRESULT result;
  result = f_write(&file, text, strlen(text), &bw);
  if (result != FR_OK)
  {
    elog("failed writing file %s", text);
  }
  else
  {
    ilog("file write: %s", text);
  }
}

void fileSync(void)
{
  FRESULT result;

  result = f_sync(&file);
  if (result != FR_OK)
  {
    elog("failed writing file");
  }
  else
  {
    ilog("file syncronized");
  }
}

void fileWriteData(uint32_t time, uint16_t value1, uint16_t value2)
{
  char buf[64];

  // todo 13.05

  buf[0] = '\0';
  utilStrcatNum32u(buf, sizeof(buf), time);
  utilStrcat(buf, sizeof(buf), ";");

  // ...

  if (time % 10 == 0) fileSync();
  //if (time % 600 == 0) fileSync();
}



// todo 13.02 implement spiBeforeReadWrite and spiAfterReadWrite
void spiBeforeReadWrite(void)
{

}

void spiAfterReadWrite(void)
{

}
#endif
