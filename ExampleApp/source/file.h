/**
 *--------------------------------------------------------------------\n
 *          HSLU T&A Hochschule Luzern Technik+Architektur            \n
 *--------------------------------------------------------------------\n
 *
 * \brief         FAT File handling
 * \file
 * \author        Christian Jost, christian.jost@hslu.ch
 * \date          22.05.2020
 *
 *--------------------------------------------------------------------
 */
#ifndef SOURCES_FILE_H_
#define SOURCES_FILE_H_


void fileMount(void);

void fileOpen(char *filename);

void fileClose(void);

void fileWrite(char *text);

void fileSync(void);

void fileWriteData(uint32_t time, uint16_t value1, uint16_t value2);

#endif /* SOURCES_FILE_H_ */
