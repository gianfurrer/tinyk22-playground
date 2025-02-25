/*
 * sd.c
 *
 *  Created on: 12.03.2019
 *      Author: zajost
 */
#if 0
#include "platform.h"
#include "ff.h"
#include "diskio.h"
#include "sd.h"
#include "spi.h"
#include "wait.h"

//#define elog(...)       //  debug(DB_SDCARD, __VA_ARGS__)
//#define dlog(...)       //  debug(DB_SDCARD, __VA_ARGS__)

#define  sdDebug(...)         debug(DB_SDCARD, __VA_ARGS__)


#define SD_CMD_TIMEOUT        5000

// Card Switch
#define SW_CARD_DETECTED()    (GPIOC_PDIR & (1<<0))
#define SW_SET_MUX()          PORTC_PCR0 = PORT_PCR_MUX(1) | PORT_PCR_PE(1)  // PTC0 GPIO with Pull-Down

// Chip select
#define SS_SELECT_SLAVE()     GPIOC_PCOR = (1<<1)           // PTC1 (low active)
#define SS_UNSELECT_SLAVE()   GPIOC_PSOR = (1<<1)           // PTC1 (low active)
#define SS_SET_MUX()          PORTC_PCR1 = PORT_PCR_MUX(1)  // PTC1 Mux as GPIO
#define SS_SET_PDDR()         GPIOC_PDDR |= (1<<1)          // PTC1 as output


#define ALENGTH(x)  (sizeof(x) / sizeof((x)[0]))

// R1 Response Format
#define R1_NO_RESPONSE          (0xFF)
#define R1_RESPONSE_RECV        (0x80)
#define R1_IDLE_STATE           (1 << 0)
#define R1_ERASE_RESET          (1 << 1)
#define R1_ILLEGAL_COMMAND      (1 << 2)
#define R1_COM_CRC_ERROR        (1 << 3)
#define R1_ERASE_SEQUENCE_ERROR (1 << 4)
#define R1_ADDRESS_ERROR        (1 << 5)
#define R1_PARAMETER_ERROR      (1 << 6)



typedef enum
{
  comSpeedModeNone,
  comSpeedModeSlow,
  comSpeedModeFast
} tComSpeedMode;

tComSpeedMode speedMode = comSpeedModeNone;


/**
 *
 * @param drv     Physical drive nmuber to identify the drive
 * @return
 */
DSTATUS disk_initialize (BYTE drv)
{


  return 0;
}




/**
 *
 * @param drv     Physical drive nmuber to identify the drive
 * @return
 */
DSTATUS disk_status (BYTE pdrv)
{

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


}



/**
 *
 * @param drv     Physical drive nmuber to identify the drive
 * @param buff    Data to be written
 * @param sector  Start sector in LBA
 * @param count   Number of sectors to write
 * @return
 */
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{

}


/**
 *
 * @param drv     Physical drive nmuber to identify the drive
 * @param cmd     Control code
 * @param buff    Data to be written
 * @return
 */
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{

}




void sdActivate(void)
{

  SS_SELECT_SLAVE();
}

void sdDeactivate(void)
{

  SS_UNSELECT_SLAVE();
}

/*
void sdSetComSpeedModeSlow(void)
{
  if (speedMode != comSpeedModeSlow)
  {
    spiSetBaudrate(400000);
//    spiDisable();
//    spiSetIdleClockPolarity(0);
//    spiSetShiftClockPolarity(0);
//    spiSetSpeed();
    speedMode = comSpeedModeSlow;
//    spiEnable();
  }
}

void sdSetComSpeedModeFast(void)
{
  if (speedMode != comSpeedModeFast)
  {
    spiSetBaudrate(10000000);
//    spiDisable();
//    spiSetIdleClockPolarity(0);
//    spiSetShiftClockPolarity(0);
//    spiSetSpeed();
    speedMode = comSpeedModeFast;
//    spiEnable();
  }
}
*/

/*


#define SD1_SPI_WRITE(write)            \
   { \
     while(SM1_SendChar(write)!=ERR_OK) {} \
     while(SM1_RecvChar(&rxDummy)!=ERR_OK) {} \
   }




uint8_t SD1_WaitReady(void)
{
  uint8_t tmp;
  TMOUT1_CounterHandle timeout;

  SD1_Activate();
  SD1_SPI_WRITE(SD1_DUMMY);
  timeout = TMOUT1_GetCounter(SD1_TIMEOUT_READY_MS/TMOUT1_TICK_PERIOD_MS); // set up timeout counter
  for (;;) {                            // will timeout
    SD1_SPI_WRITE_READ(SD1_DUMMY, &tmp); // write dummy value, read status
    if (tmp==0xff) {
      break;
    }
    if (TMOUT1_CounterExpired(timeout)) {
      break;
    }
  } // for
  TMOUT1_LeaveCounter(timeout);
  SD1_Deactivate();
  if (tmp==0xff) {
    return ERR_OK; // device is ready
  } else {
    return ERR_BUSY;
  }
}

uint8_t sdSendCommand(uint8_t cmd, uint32_t arg)
{
  uint8_t res;

  if (cmd & 0x80) {                       // ACMD<n> is the command sequence of CMD55-CMD<n>
      cmd &= 0x7F;
      res = SD1_SendCmd(SD1_CMD55, 0);
      if (res > 1) {
        return res;
      }
    }




}

*/


void sdSetLowSpeed(void)
{
  spiSetSpeed(TRUE, 2, 6, 2, 3); // 375 kHz
}


void sdSetHighSpeed(void)
{
  spiSetSpeed(TRUE, 1, 0, 0, 0); // 20 MHz
}


tError sdGetResponse(uint8_t response)
{
  uint8_t i;
  for (i=0; i<10; i++)
  {
    uint8_t rec = spiReadByte(TRUE);
    if (rec == response){
      return EC_SUCCESS;
    }
  }
  return EC_TIMEOUT;
}



uint8_t sdSendCmd(uint8_t cmd, uint32_t arg)
{
  uint8_t i;
  uint8_t buf[6];
  uint8_t result;

  // ACMD<n> is the command sequense of CMD55-CMD<n>
  if (cmd & 0x80)
  {
    cmd &= 0x7F;
    result = sdSendCmd(SD_CMD55, 0);
    if (result > 0) return result;
  }

  buf[0] = cmd;
  buf[1] = arg >> 24;
  buf[2] = arg >> 16;
  buf[3] = arg >> 8;
  buf[4] = arg >> 0;

  // default checksum is 0xFF
  buf[5] = 0;

  // CMD0 and CMD8 needs a checksum. Using precalculated checksum values
  if (cmd == SD_CMD0) buf[5] = 0x95;
  if (cmd == SD_CMD8) buf[5] = 0x87;

  SS_SELECT_SLAVE();
  spiWriteReadBuf(buf, ALENGTH(buf), FALSE);


  // Wait for response. The response is sent back within
  // the command response time (NCR): 0 to 8 bytes
  uint8_t rec;
  for (i=0; i<10; i++)
  {
    rec = spiReadByte(TRUE);
    if ((rec & 0x80) == 0) break;
  }

  SS_UNSELECT_SLAVE();
  spiReadByte(FALSE);

  return rec;


//  // wait for 0x01 from the sd card.
//  if (sdGetResponse(0x01) != EC_SUCCESS)
//  {
//    SS_UNSELECT_SLAVE();
//    spiReadByte(FALSE);
//    return EC_TIMEOUT;
//  }
//
//
//  SS_UNSELECT_SLAVE();
//  spiReadByte(FALSE);
}

void sdWriteBlock()
{

}


uint8_t sdSendCmdSpi(uint8_t cmd, uint32_t arg)
{
  uint8_t i;
  uint8_t buf[6];
  uint8_t result;

  buf[0] = cmd;
  buf[1] = arg >> 24;
  buf[2] = arg >> 16;
  buf[3] = arg >> 8;
  buf[4] = arg >> 0;

  // default checksum is 0xFF
  buf[5] = 0;

  // CMD0 and CMD8 needs a checksum. Using precalculated checksum values
  if (cmd == SD_CMD0) buf[5] = 0x95;
  if (cmd == SD_CMD8) buf[5] = 0x87;

  spiWriteReadBuf(buf, ALENGTH(buf), FALSE);

  // Wait for response. The response is sent back within
  // the command response time (NCR): 0 to 8 bytes
  uint8_t rec;
  for (i=0; i<10; i++)
  {
    rec = spiReadByte(TRUE);
    if ((rec & 0x80) == 0) break;
  }

  return rec;
}

bool sdWaitReady(uint16_t ms)
{
  uint8_t response;
  uint32_t startTime = waitStartWatch();
  // Wait until MISO goes high (0xFF received)
  do
  {
    if (spiReadByte(TRUE) == 0xFF) return TRUE;
  } while (waitGetTimeMs(startTime) < ms);

  return FALSE;
}

static void sdUnselectSlave(void)
{
  SS_UNSELECT_SLAVE();
  spiReadByte(FALSE);
}


uint8_t sdSendCmdEx(uint8_t cmd, uint32_t arg, uint32_t *resp)
{
  uint8_t response;
  uint8_t buf[4];

  SS_SELECT_SLAVE();

  // ACMD<n> is the command sequense of CMD55-CMD<n>
  if (cmd & 0x80)
  {
    cmd &= 0x7F;
    response = sdSendCmdSpi(SD_CMD55, 0x00);
    if (!sdWaitReady(SD_CMD_TIMEOUT))
    {
      dlog("No response CMD:%d response: 0x%x\r\n", cmd, response);
      return EC_NO_ANSWER;
    }
  }

  response = sdSendCmdSpi(SD_CMD55, arg);

  if (resp != NULL) *resp = response;

  if (response == R1_NO_RESPONSE)
  {
    sdUnselectSlave();
    dlog("No response CMD:%d response: 0x%x\r\n", cmd, response);
    return EC_NO_ANSWER;
  }

  if (response & R1_COM_CRC_ERROR)
  {
    sdUnselectSlave();
    dlog("CRC error CMD:%d response: 0x%x\r\n", cmd, response);
    return EC_CRC_ERROR;
  }

  if (response & R1_ILLEGAL_COMMAND)
  {
    sdUnselectSlave();
    dlog("Illegal command CMD:%d response: 0x%x\r\n", cmd, response);
    return EC_INVALID_CMD;;
  }

  dlog("CMD:%d \t arg:0x%x \t Response:0x%x \n", cmd, arg, response);





  switch(cmd)
  {
    case SD_CMD8:
      dlog("V2-Version Card\r\n");

    case SD_CMD58:
      spiRead((uint8_t*) resp, 4, TRUE);
      dlog("R3/R7: 0x%x \n", response);
      break;

  }


  sdUnselectSlave();

  return 0;
}




//DSTATUS McuSDCard_disk_initialize (
//        uint8_t drv                     /* Physical drive number (0..) */
//)
//{
//  uint8_t n, cmd, ty, ocr[4];
//
//  (void)drv; /* not used */
//  if (Stat&STA_NODISK) {
//    return Stat;                        /* No card in the socket */
//  }
//  if (McuSDCard_Init(NULL) != ERR_OK) {
//    return STA_NOINIT;
//  }
//  ty = 0;
//  if (McuSDCard_SendCmd(McuSDCard_CMD0, 0) == 1) { /* Enter Idle state */
//    if (McuSDCard_SendCmd(McuSDCard_CMD8, 0x1AA) == 1) { /* SDHC */
//      for (n = 0; n < 4; n++) {
//        ocr[n] = McuSDCard_ReceiveByte(); /* Get trailing return value of R7 resp */
//      }
//      if (ocr[2] == 0x01 && ocr[3] == 0xAA) { /* The card can work at vdd range of 2.7-3.6V */
//        while (McuSDCard_SendCmd(McuSDCard_ACMD41, 1UL << 30)) {
//          /* Wait for leaving idle state (ACMD41 with HCS bit) */
//        }
//        if (McuSDCard_SendCmd(McuSDCard_CMD58, 0) == 0) { /* Check CCS bit in the OCR */
//          for (n = 0; n < 4; n++) {
//            ocr[n] = McuSDCard_ReceiveByte();
//          }
//          ty = (uint8_t)((ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2); /* SDv2 */
//        }
//      }
//    } else {                            /* SDSC or MMC */
//      if (McuSDCard_SendCmd(McuSDCard_ACMD41, 0) <= 1) {
//        ty = CT_SD1; cmd = McuSDCard_ACMD41; /* SDv1 */
//      } else {
//        ty = CT_MMC; cmd = McuSDCard_CMD1; /* MMCv3 */
//      }
//      while (McuSDCard_SendCmd(cmd, 0)) {
//        /* Wait for leaving idle state */
//      }
//      if (McuSDCard_SendCmd(McuSDCard_CMD16, McuSDCard_BLOCK_SIZE) != 0) { /* Set R/W block length  */
//        ty = 0;
//      }
//    }
//    McuSDCard_SetFastMode();
//  }
//  CardType = ty;
//  Stat &= ~STA_NOINIT;                  /* Clear STA_NOINIT */
//  return Stat;
//}




tError sdInitCard(void)
{
  uint32_t response;
  uint8_t result;
  uint8_t data[10] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  if (!SW_CARD_DETECTED())
  {
    dlog("no card present!");
    return EC_NO_CARD;
  }

  dlog("start init sd card...");

  // After supply voltage reached above 2.2 volts, wait for one millisecond at least.
  // Set SPI clock rate between 100 kHz and 400 kHz.
  sdSetLowSpeed();

  // Set DI and CS high and apply 74 or more clock pulses to SCLK.
  // The card will enter its native operating mode and go ready to accept native command.
  spiWriteReadBuf(data, ALENGTH(data), FALSE);

  // send CMD0 (Reset or GoIdleState) command. All arguments are 0x00 for the reset command.
  // Reseting the MCU SPI master may not reset the on-board SDCard, in which case when
  // MCU power-on occurs the SDCard will resume operations as though there was no reset.
  // In this scenario the first CMD0 will not be interpreted as a command and get lost.
  // For some cards retrying the command overcomes this situation
  for (int i=0; i<3; i++)
  {
    result = sdSendCmdEx(SD_CMD0, 0, &response);
    if (response == R1_IDLE_STATE) break;
    waitMs(1);
  }

  if (response != R1_IDLE_STATE)
  {
    dlog("No sd card, or could not put SD card in to SPI idle state");
    return EC_NO_ANSWER;
  }

  // Send CMD8, if the card rejects the command then it's probably using the
  // legacy protocol, or is a MMC, or just flat-out broken
  result = sdSendCmd(SD_CMD8, 0x1AA);







  return EC_SUCCESS;

  result = sdSendCmd(SD_CMD0, 0);
  if (result != 0x01) return EC_NO_ANSWER;


  result = sdSendCmd(SD_CMD8, 0x1AA);



  return EC_SUCCESS;

  // send CMD1 (SEND_OP_COND) to bring out of idle state. All arguments are 0x00 for this command.
  result = sdSendCmd(SD_CMD1, 0);
  if (result != 0x01) return EC_TIMEOUT;

  // send CMD16 (SET_CLOCKLEN) to set the block length
  // LSB ... MSB
  // 00 00 00 10 => set to 16 bytes
  // 00 00 02 00 => set to 512 bytes
  result = sdSendCmd(SD_CMD16, 0x00000200);
  if (result != 0x01) return EC_TIMEOUT;

  sdSetHighSpeed();
  return EC_SUCCESS;
}



void sdInit(void)
{
  spiSetPcsPin(spiPcsNone);

  SS_UNSELECT_SLAVE();  // Unselect SS-Pin while set it to high level (is low active)
  SS_SET_MUX();         // Set MUX of SlaveSelect to GPIO
  SS_SET_PDDR();        // Set PortDataDirection of SlaveSelect to Output

  SW_SET_MUX();         // Set Card-Switch MUX to GPIO with Pull-Down
}


#endif
