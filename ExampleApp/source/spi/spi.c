/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         SPI driver
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          12.05.2020
 *
 *--------------------------------------------------------------------
 */
#include "platform.h"
#include "spi.h"

//https://github.com/xxxajk/spi4teensy3/blob/master/spi4teensy3.cpp


#define SPI       SPI0

#define SPI_CLK_POLARITY_LOW      0
#define SPI_CLK_POLARITY_HIGH     1
const uint16_t BR_VALUES[16] = {2, 4, 6, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};
const uint8_t ASC_VALUES[16] = {0, 0, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

#define ASIZE(x)  (sizeof(x) / sizeof((x)[0]))

typedef struct
{
  SPI_Type *spiBase;
  PORT_Type *spiClk;
  PORT_Type *spiMiSo;
  PORT_Type *spiMoSi;

} tSpi;

static tSpiPcs peripheralchipSelect;



void spiDisable(void)
{
  SPI->MCR |= SPI_MCR_HALT_MASK;          // Set Halt Mode
  SPI->MCR |= SPI_MCR_MDIS_MASK;          // Disable Module
  while (SPI->SR & SPI_SR_TXRXS_MASK) {}  // wait until modul is stopped
}


void spiEnable(void)
{
  SPI->MCR &= ~SPI_MCR_MDIS_MASK;         // Enable Module
  SPI->MCR |= SPI_MCR_CLR_TXF_MASK;       // Clear Tx FIFO
  SPI->MCR |= SPI_MCR_CLR_RXF_MASK;       // Clear Rx FIFO
  SPI->SR = SPI_SR_TCF_MASK               // Clear all interrupts 0x9A0A0000U
      | SPI_SR_EOQF_MASK
      | SPI_SR_TFUF_MASK
      | SPI_SR_TFFF_MASK
      | SPI_SR_RFOF_MASK
      | SPI_SR_RFDF_MASK;
  SPI->MCR &= ~SPI_MCR_HALT_MASK;          // Disable Halt Mode

  while (SPI->MCR & (SPI_MCR_HALT_MASK)){}
}


void spiSetSpeed(bool dbr, uint8_t pbr, uint8_t br, uint8_t pcssck, uint8_t cssck)
{
  spiDisable();
  uint32_t ctar = SPI_CTAR_DBR(dbr)
    | SPI_CTAR_CPOL(0)                // The inactive stat value of SCK is low
    | SPI_CTAR_CPHA(0)                // Data is captured on the leading edge of SCK and changed on the following edge
    | SPI_CTAR_PBR(pbr)               // Baud Rate Prescaler: 0..3 => /2, /3, /5, /7
    | SPI_CTAR_PASC(pcssck)
    | SPI_CTAR_ASC(cssck)
    | SPI_CTAR_PCSSCK(pcssck)         //
    | SPI_CTAR_CSSCK(cssck)  //
    //| SPI_CTAR_CSSCK(ASC_VALUES[br])  //
    | SPI_CTAR_BR(br);                // Baud Rate Scaler 0..15 => 2..32768

  SPI->CTAR[0] = ctar | SPI_CTAR_FMSZ(7);   // 8 Bit Data Frame (n+1)
  SPI->CTAR[1] = ctar | SPI_CTAR_FMSZ(15);  // 16 Bit Data Frame (n+1)
  spiEnable();
}


void spiSetBaudrate(int32_t freq)
{
  uint8_t i;
  for (i=0; i<ASIZE(BR_VALUES); i++)
  {
    if (BUSCLOCK / BR_VALUES[i] <= freq)
    {
      spiDisable();
      // Baudrate: (BUSCLOCK/PBR) x ((1+DBR)/BR)
      SPI->CTAR[0] =
          SPI_CTAR_DBR(1)         // Double Data Rate
          | SPI_CTAR_FMSZ(7)      // 8 Bit Data Frame (n+1)
          | SPI_CTAR_CPOL(0)      // The inactive stat value of SCK is low
          | SPI_CTAR_CPHA(0)      // Data is captured on the leading edge of SCK and changed on the following edge
          | SPI_CTAR_PBR(0)       // Baud Rate Prescaler: 0 => /2

          | SPI_CTAR_PASC(0)
          //| SPI_CTAR_ASC(ASC_VALUES[i])

          | SPI_CTAR_PCSSCK(0)
          | SPI_CTAR_CSSCK(ASC_VALUES[i])

          | SPI_CTAR_BR(i);       // Baud Rate Scaler 0..15 => 2..32768

      SPI->CTAR[1] =
          SPI_CTAR_DBR(1)         // Double Data Rate
          | SPI_CTAR_FMSZ(15)     // 16 Bit Data Frame (n+1)
          | SPI_CTAR_CPOL(0)      // The inactive stat value of SCK is low
          | SPI_CTAR_CPHA(0)      // Data is captured on the leading edge of SCK and changed on the following edge
          | SPI_CTAR_PBR(0)       // Baud Rate Prescaler: 0 => /2

          | SPI_CTAR_PASC(0)
        //  | SPI_CTAR_ASC(ASC_VALUES[i])

          | SPI_CTAR_PCSSCK(0)
          | SPI_CTAR_CSSCK(ASC_VALUES[i]) //

          | SPI_CTAR_BR(i);       // Baud Rate Scaler 0..15 => 2..32768

      //SPI_CTAR_REG(SPI, 0) = (SPI_CTAR_REG(SPI, 0) & ~SPI_CTAR_BR_MASK) | SPI_CTAR_BR(i);
      spiEnable();
      break;
    }
  }
}


uint8_t spiWriteReadByte(uint8_t data)
{
  uint8_t rxByte;
  spiWriteReadBuf(&data, 1, &rxByte);
  return rxByte;
}

void spiDummy(void) {}

void spiBeforeReadWrite(void) __attribute__ ((weak, alias("spiDummy")));

void spiAfterReadWrite(void) __attribute__ ((weak, alias("spiDummy")));

void spiWriteReadBuf(uint8_t *txBuf, uint16_t length, uint8_t *rxBuf)
{
  bool has8Bit = 0;
  uint16_t rxCount = length;

  spiBeforeReadWrite();

  SPI->MCR |= SPI_MCR_CLR_TXF(1) | SPI_MCR_CLR_RXF(1);
  SPI->TCR = 0;

  SPI->SR = SPI_SR_EOQF(1);   // Clear End Of Queue Flag (EOQF)

  if (length & 1) // Send 8 bit
  {
    has8Bit = TRUE;
    length--;
    if (txBuf)
    {
      SPI->PUSHR = SPI_PUSHR_CONT(0)//length != 0)
        | SPI_PUSHR_CTAS(0)
        | SPI_PUSHR_EOQ(length == 0)
        | SPI_PUSHR_CTCNT(0)  // Clear the TCNT field
     // | SPI_PUSHR_PCS(peripheralchipSelect)
        | SPI_PUSHR_TXDATA(*txBuf++);
    }
    else
    {
      SPI->PUSHR = SPI_PUSHR_CONT(0)//length != 0)
        | SPI_PUSHR_CTAS(0)
        | SPI_PUSHR_EOQ(length == 0)
        | SPI_PUSHR_CTCNT(0)  // Clear the TCNT field
     // | SPI_PUSHR_PCS(peripheralchipSelect)
        | SPI_PUSHR_TXDATA(0xFF);
    }
  }

  while (TRUE) // Send 16 bit
  {
    // Check if Transmit FIFO is not full (TFFF==1)
    //if (SPI_SR_REG(SPI) & SPI_SR_TFFF_MASK && length) funktioniert nicht!!!!
    if (((SPI->SR & SPI_SR_TXCTR_MASK) < SPI_SR_TXCTR(3)) && length)
    {
      length -= 2;
      if (txBuf)
      {
        uint16_t data = *txBuf++ << 8;
        data |= *txBuf++;
        SPI->PUSHR = SPI_PUSHR_CONT(1)//length != 0)
          | SPI_PUSHR_CTAS(1)
          | SPI_PUSHR_EOQ(length == 0)
          | SPI_PUSHR_CTCNT(0)  // Clear the TCNT field
        //| SPI_PUSHR_PCS(peripheralchipSelect)
          | SPI_PUSHR_TXDATA(data);
      }
      else
      {
        SPI->PUSHR = SPI_PUSHR_CONT(1)//length != 0)
         | SPI_PUSHR_CTAS(1)
         | SPI_PUSHR_EOQ(length == 0)
         | SPI_PUSHR_CTCNT(0)  // Clear the TCNT field
       //| SPI_PUSHR_PCS(peripheralchipSelect)
         | SPI_PUSHR_TXDATA(0xFFFF);
      }
    }

    // Check if RX FIFO is not empty (RFDF == 1)
    //if (SPI_SR_REG(SPI) & SPI_SR_RFDF_MASK)
    if (SPI->SR & SPI_SR_RXCTR_MASK)
    {
      uint16_t tmp = SPI->POPR;
      if (has8Bit)
      {
        has8Bit = FALSE;
        rxCount--;
        if (rxBuf) *rxBuf++ = (uint8_t)tmp;
      }
      else
      {
        rxCount -= 2;
        if (rxBuf)
        {
          *rxBuf++ = (uint8_t)(tmp >> 8);
          *rxBuf++ = (uint8_t)(tmp);
        }
      }
      if (rxCount == 0)
      {
        while (SPI->SR & SPI_SR_TXRXS_MASK){}
        break;
      }
    }
  }
  spiAfterReadWrite();
}


void spiSetPcsPin(tSpiPcs pcs)
{
  peripheralchipSelect = pcs;
}


void spiInit(void)
{
  SIM->SCGC6 |= SIM_SCGC6_SPI0(1);

  PORTC->PCR[6] = PORT_PCR_MUX(2); // MOSI on PTC6, ALT2
  PORTC->PCR[7] = PORT_PCR_MUX(2) | PORT_PCR_PS(1) | PORT_PCR_PE(1);   // MISO on PTC7, ALT2
  PORTC->PCR[5] = PORT_PCR_MUX(2);   //CLK on PTC5, ALT2
  PORTC->PCR[1] = PORT_PCR_MUX(2); // PCS3 on PTC1, ALT2

  SPI->MCR =
      SPI_MCR_MSTR(1)         // Master Mode
      | SPI_MCR_ROOE(0)       // On Rx FIFO Overflow: Incomming data is shifted into the register
      | SPI_MCR_PCSIS(0x3F)   // The inactive state of PCSx is high
      | SPI_MCR_DIS_TXF(0)    // Enable Transmit FIFO
      | SPI_MCR_DIS_RXF(0)    // Enable Receive FIFO
      | SPI_MCR_CLR_TXF(1)    // Flush Transmit FIFO
      | SPI_MCR_CLR_RXF(1)    // Flush Receive FIFO
      | SPI_MCR_HALT(1);      // Halt Module during init!

  SPI->MCR &= ~SPI_MCR_HALT(1); // Disable Halt

  spiSetSpeed(TRUE, 1, 0, 0, 0); // 20 MHz
}


