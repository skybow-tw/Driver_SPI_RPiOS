//This header file only declare function prototype.

//Following Macro would be activated if the header file is included in other C++ program
#ifdef __cplusplus
extern "C"
{
#endif
    int SPI_Setup(int channel, int speed, int mode);
    int SPI_ReadWrite(int channel, unsigned char *tx_data, unsigned char *rx_data, int len);

#ifdef __cplusplus
}
#endif