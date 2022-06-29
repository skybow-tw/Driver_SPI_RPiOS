#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include "SPISet.h"

static const char *spiDev0 = "/dev/spidev0.0";
static const char *spiDev1 = "/dev/spidev0.1";
static uint8_t spiBPW = 8; //Bits Per Word of SPI
static uint16_t spiDelay = 0;

static uint32_t spiSpeeds[2]; //Different Speed for 3 CS pin(0,1,2)
static int spiFds[2];         //Fd=File descriptor

//--------------------------------
//Setup SPI  channel/speed/Mode
int SPI_Setup(int channel, int speed, int mode)
{
  int fd; //file descriptor

  if ((fd = open(channel == 0 ? spiDev0 : spiDev1, O_RDWR)) < 0)
  {
    printf("Unable to open SPI device: %s\n", strerror(errno));
    return -1;
  }

  spiSpeeds[channel] = speed;
  spiFds[channel] = fd;

  /*
  * SPI MODE:
  *  Mode 0： CPOL=0, CPHA=0
  *  Mode 1： CPOL=0, CPHA=1
  *  Mode 2： CPOL=1, CPHA=0
  *  Mode 3： CPOL=1, CPHA=1
  *  default =Mode 0
  */

  if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0)
  {
    printf("Can't set spi mode: %s\n", strerror(errno));
    return -1;
  }

  if (ioctl(fd, SPI_IOC_RD_MODE, &mode) < 0)
  {
    printf("Can't get spi mode: %s\n", strerror(errno));
    return -1;
  }

  //Set Bits Per Word= 8 bits
  if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0)
  {
    printf("Can't set bits per word: %s\n", strerror(errno));
    return -1;
  }

  if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &spiBPW) < 0)
  {
    printf("Can't get bits per word: %s\n", strerror(errno));
    return -1;
  }

  //Set Read/Write Speed
  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
  {
    printf("Can't set max speed hz: %s\n", strerror(errno));
    return -1;
  }

  if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0)
  {
    printf("Can't get max speed hz: %s\n", strerror(errno));
    return -1;
  }

  return fd; //return file descriptor
}

//------------------------------------------------
//Write and Read a block of data over the SPI bus.
int SPI_ReadWrite(int channel, unsigned char *tx_data, unsigned char *rx_data, int len)
{
  int i = 0;
  struct spi_ioc_transfer spi; //declare a "spi_ioc_transfer"type variable, where this type is a structure defined in linux/spidev.h

  channel &= 1;

  memset(&spi, 0, sizeof(spi));

  spi.tx_buf = (unsigned long)tx_data;
  spi.rx_buf = (unsigned long)rx_data;
  spi.len = len;
  spi.delay_usecs = spiDelay;
  spi.speed_hz = spiSpeeds[channel];
  spi.bits_per_word = spiBPW;
  //Note the data ia being read into the transmit buffer, so will overwrite it!
  //This is also a full-duplex operation.

  return ioctl(spiFds[channel], SPI_IOC_MESSAGE(1), &spi); //SPI_IOC_MESSAGE(1)的1表示spi_ioc_transfer的数量
}
