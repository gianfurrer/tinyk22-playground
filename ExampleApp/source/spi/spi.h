/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         SPI driver
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          22.05.2020
 *
 *--------------------------------------------------------------------
 */
#ifndef SOURCES_SPI_SPI_H_
#define SOURCES_SPI_SPI_H_

typedef enum
{
  spiPcsNone = 0x00,
  spiPcs0 = 0x01,
  spiPcs1 = 0x02,
  spiPcs2 = 0x04,
  spiPcs3 = 0x08,
  spiPcs4 = 0x10,
  spiPcs5 = 0x20
} tSpiPcs;

typedef enum
{
  spiPcsPinPTA14,     // PCS0, Alt:2
  spiPcsPinPTB23,     // PCS5, Alt:3
  spiPcsPinPTC0,      // PCS4, Alt:2
  spiPcsPinPTC1,      // PCS3, ALT:2
  spiPcsPinPTC2,      // PCS2, ALT:2
  spiPcsPinPTC3,      // PCS1, Alt:2
  spiPcsPinPTC4,      // PCS0, ALT:2
  spiPcsPinPTD0,      // PCS0, ALT:2
  spiPcsPinPTD4,      // PCS1, ALT:2
  spiPcsPinPTD5,      // PCS2, ALT:2
  spiPcsPinPTD6,      // PCS3, ALT:2
} tSpiPcsasdf;


uint8_t spiWriteReadByte(uint8_t data);
void spiWriteReadBuf(uint8_t *txBuf, uint16_t length, uint8_t *rxBuf);

void spiSetPcsPin(tSpiPcs pcs);
void spiSetSpeed(bool dbr, uint8_t pbr, uint8_t br, uint8_t pcssck, uint8_t cssck);

void spiDisable(void);
void spiEnable(void);
void spiInit(void);


#endif /* SOURCES_SPI_SPI_H_ */
