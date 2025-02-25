/*
 * diskio.c
 *
 *  Created on: 08.04.2019
 *      Author: zajost
 */
#include "platform.h"
#include "diskio.h"
#include "McuSDCard.h"




/**
 *
 * @param drv     Physical drive nmuber to identify the drive
 * @return
 */
DSTATUS disk_initialize (BYTE drv)
{
  switch (drv)
  {
    case 0:
      return McuSDCard_disk_initialize(drv);
      break;
  }

  return RES_PARERR;
}


/**
 *
 * @param drv     Physical drive nmuber to identify the drive
 * @return
 */
DSTATUS disk_status (BYTE drv)
{
  switch (drv)
  {
    case 0:
      return McuSDCard_disk_status(drv);
      break;
  }

  return RES_PARERR;
}


/**
 *
 * @param drv     Physical drive nmuber to identify the drive
 * @param buff    Data buffer to store read data
 * @param sector  Start sector in LBA
 * @param count   Number of sectors to read
 * @return
 */
DRESULT disk_read (BYTE drv, BYTE* buff, DWORD sector, UINT count)
{
  switch (drv)
  {
    case 0:
      return McuSDCard_disk_read(drv, buff, sector, count);
      break;
  }

  return RES_PARERR;
}


/**
 *
 * @param drv     Physical drive nmuber to identify the drive
 * @param buff    Data to be written
 * @param sector  Start sector in LBA
 * @param count   Number of sectors to write
 * @return
 */
DRESULT disk_write (BYTE drv, const BYTE* buff, DWORD sector, UINT count)
{
  switch (drv)
  {
    case 0:
      return McuSDCard_disk_write(drv, buff, sector, count);
      break;
  }

  return RES_PARERR;
}


/**
 *
 * @param drv     Physical drive nmuber to identify the drive
 * @param cmd     Control code
 * @param buff    Data to be written
 * @return
 */
DRESULT disk_ioctl (BYTE drv, BYTE cmd, void* buff)
{
  switch (drv)
  {
    case 0:
      return McuSDCard_disk_ioctl(drv, cmd, buff);
      break;
  }

  return RES_PARERR;
}













