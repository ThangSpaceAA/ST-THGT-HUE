// #include "Serial.h"
// #include "DEV_Config.h"

// pthread_mutex_t* send_struct_mutex_lock;

// void serialize8(uint8_t a)
// {
//     UART_Write_Byte(a);
//     checksum ^= a;
// }

// //Truyen 2 byte
// void serialize16(int16_t a)
// {
//     static uint8_t t;
//     t = a;
//     serialize8(t);
//     checksum ^= t;
//     t = (a >> 8) & 0xff;
//     serialize8(t);
//     checksum ^= t;
// }

// void serializeWriteNByte(uint8_t* buffer, int size) {
//     UART_Write_nByte(buffer, size);
// }


// //Truyen 4 byte
// void serialize32(uint32_t a)
// {
//     static uint8_t t;
//     t = a;
//     serialize8(t);
//     checksum ^= t;
//     t = a >> 8;
//     serialize8(t);
//     checksum ^= t;
//     t = a >> 16;
//     serialize8(t);
//     checksum ^= t;
//     t = a >> 24;
//     serialize8(t);
//     checksum ^= t;
// }

// //
// void headSerialResponse(uint8_t err, uint8_t s)
// {
//     serialize8('$');
//     serialize8('M');
//     serialize8(err ? '!' : '>');
//     checksum = 0;
//     serialize8(s);
//     serialize8(cmdMSP);
// }

// void headSerialReply(uint8_t s)
// {
//     headSerialResponse(0, s);
// }

// void headSerialError(uint8_t s)
// {
//     headSerialResponse(1, s);
// }

// //
// void tailSerialReply(void)
// {
//     serialize8(checksum);
// }

// //
// void serializeNames(const char *s)
// {
//     const char *c;
//     for (c = s; *c; c++)
//         serialize8(*c);
// }

// uint8_t read8(void)
// {
//     return inBuf[indRX++] & 0xff;
// }
// //
// uint16_t read16(void)
// {
//     uint16_t t = read8();
//     t += (uint16_t)read8() << 8;
//     return t;
// }

// int16_t readint16(void) //
// {
//     int16_t temp = (inBuf[indRX++]);
//     temp = temp + ((inBuf[indRX++]) << 8);
//     return temp;
// }
// //
// uint32_t read32(void)
// {
//     uint32_t t = read16();
//     t += (uint32_t)read16() << 16;
//     return t;
// }

// void send_struct(uint8_t cmd, uint8_t *cb, uint8_t siz)
// {
//     int _size = siz;
//     // printf("\r\nsend struct size :%03d\r\n", siz);
//     // cmdMSP = cmd;
//     // headSerialReply(siz);
//     // uint8_t* buffer_send = cb;
//     // while (siz--)
//     // {
//     //     serialize8(*buffer_send);
//     //     buffer_send++;
//     // }    
//     // tailSerialReply();
//     printf("\r\nsend struct size :%03d\r\n", siz);
//     cmdMSP = cmd;
//     headSerialReply(siz);
//     uint8_t* buffer_send = cb;
//     if (_size>0)
//     {
//         serialize8(*buffer_send);
//         buffer_send++;
//         _size --;

//     }    
//     tailSerialReply();
// }

// //
// void send_byte(uint8_t cmd, uint8_t data)
// {
//     cmdMSP = cmd;
//     headSerialReply(1);
//     serialize8(data);
//     tailSerialReply();
//     printf("\r\nCMD send: %d\r\n", cmd);
//     serialFlush(fd);
// }

// void readstruct(uint8_t *pt, uint8_t size)
// {
//     uint16_t i = 0;
//     for (i = 0; i < size; i++)
//     {
//         *pt = inBuf[indRX++];
//         pt++;
//     }
// }


// // void serial_get_buffer(void)
// // {
// //     uint8_t c = 0;
// //     // if (serialDataAvail(fd))
// //     // {
// //         c = (uint8_t)(UART_Read_Byte());
// //         // cout << "This is check: " << c <<endl;
// //         if (c_state == IDLE)
// //         {
// //             c_state = (c == '$') ? HEADER_START : IDLE;
// //             if (c_state == IDLE)
// //             {
// //             }
// //         }
// //         else if (c_state == HEADER_START)
// //         {
// //             c_state = (c == 'M') ? HEADER_M : IDLE;
// //         }
// //         else if (c_state == HEADER_M)
// //         {
// //             c_state = (c == '>') ? HEADER_ARROW : IDLE;
// //         }
// //         else if (c_state == HEADER_ARROW)
// //         {
// //             if (c > 255)
// //             {
// //                 c_state = IDLE;
// //             }
// //             else
// //             {
// //                 dataSize = c;
// //                 offset = 0;
// //                 checksum = 0;
// //                 indRX = 0;
// //                 checksum ^= c;
// //                 c_state = HEADER_SIZE;
// //             }
// //         }
// //         else if (c_state == HEADER_SIZE)
// //         {
// //             cmdMSP = c;
// //             checksum ^= c;
// //             c_state = HEADER_CMD;
// //         }
// //         else if (c_state == HEADER_CMD && offset < dataSize)
// //         {
// //             checksum ^= c;
// //             inBuf[offset++] = c;
// //         }
// //         else if (c_state == HEADER_CMD && offset >= dataSize)
// //         {
// //             if (checksum == c)
// //             {
// //                 get_data_finish();
// //             }
// //             c_state = IDLE;
// //         }
// //     // }
// // }
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Serial.h"

uint8_t checksum;
uint8_t indRX;
uint8_t inBuf[255];
uint8_t offset;
uint8_t dataSize;
uint8_t cmdMSP;

int fd;
_serial_state c_state;
// #include "../POE_lib/DEV_Config.h"

//extern int fd;
void serialize8(uint8_t a)
{
    serialPutchar(fd,a);
    // UART_Write_Byte(a);
    checksum ^= a;
}

//Truyen 2 byte
void serialize16(int16_t a)
{
    static uint8_t t;
    t = a;
    serialize8(t);
    checksum ^= t;
    t = (a >> 8) & 0xff;
    serialize8(t);
    checksum ^= t;
}

// void serializeWriteNByte(uint8_t* buffer, int size) {
//     // UART_Write_nByte(buffer, size);
//     serialPuts(fd, (uint8_t *)buffer);
// }


//Truyen 4 byte
void serialize32(uint32_t a)
{
    static uint8_t t;
    t = a;
    serialize8(t);
    checksum ^= t;
    t = a >> 8;
    serialize8(t);
    checksum ^= t;
    t = a >> 16;
    serialize8(t);
    checksum ^= t;
    t = a >> 24;
    serialize8(t);
    checksum ^= t;
}

//
void headSerialResponse(uint8_t err, uint8_t s)
{
    serialize8('$');
    serialize8('M');
    serialize8(err ? '!' : '>');
    checksum = 0;
    serialize8(s);
    serialize8(cmdMSP);
}

void headSerialReply(uint8_t s)
{
    headSerialResponse(0, s);
}

void headSerialError(uint8_t s)
{
    headSerialResponse(1, s);
}

//
void tailSerialReply(void)
{
    serialize8(checksum);
}

//
void serializeNames(const char *s)
{
    const char *c;
    for (c = s; *c; c++)
        serialize8(*c);
}

uint8_t read8(void)
{
    return inBuf[indRX++] & 0xff;
}
//
uint16_t read16(void)
{
    uint16_t t = read8();
    t += (uint16_t)read8() << 8;
    return t;
}

int16_t readint16(void) //
{
    int16_t temp = (inBuf[indRX++]);
    temp = temp + ((inBuf[indRX++]) << 8);
    return temp;
}
//
uint32_t read32(void)
{
    uint32_t t = read16();
    t += (uint32_t)read16() << 16;
    return t;
}

void send_struct(uint8_t cmd, uint8_t *cb, uint8_t siz)
{
    cmdMSP = cmd;
    headSerialReply(siz);
    while (siz--)
    {
        serialize8(*cb++);
    }
    tailSerialReply();
}


void send_byte(uint8_t cmd, uint8_t data)
{
    cmdMSP = cmd;
    headSerialReply(1);
    serialize8(data);
    tailSerialReply();
    // printf("\r\nCMD send: %d\r\n", cmd);
    // serialFlush(fd);
}

void readstruct(uint8_t *pt, uint8_t size)
{
    uint16_t i = 0;
    for (i = 0; i < size; i++)
    {
        *pt = inBuf[indRX++];
        pt++;
    }
}


// void serial_get_buffer(void)
// {
//     uint8_t c = 0;
//     // if (serialDataAvail(fd))
//     // {
//         c = (uint8_t)(UART_Read_Byte());
//         // cout << "This is check: " << c <<endl;
//         if (c_state == IDLE)
//         {
//             c_state = (c == '$') ? HEADER_START : IDLE;
//             if (c_state == IDLE)
//             {
//             }
//         }
//         else if (c_state == HEADER_START)
//         {
//             c_state = (c == 'M') ? HEADER_M : IDLE;
//         }
//         else if (c_state == HEADER_M)
//         {
//             c_state = (c == '>') ? HEADER_ARROW : IDLE;
//         }
//         else if (c_state == HEADER_ARROW)
//         {
//             if (c > 255)
//             {
//                 c_state = IDLE;
//             }
//             else
//             {
//                 dataSize = c;
//                 offset = 0;
//                 checksum = 0;
//                 indRX = 0;
//                 checksum ^= c;
//                 c_state = HEADER_SIZE;
//             }
//         }
//         else if (c_state == HEADER_SIZE)
//         {
//             cmdMSP = c;
//             checksum ^= c;
//             c_state = HEADER_CMD;
//         }
//         else if (c_state == HEADER_CMD && offset < dataSize)
//         {
//             checksum ^= c;
//             inBuf[offset++] = c;
//         }
//         else if (c_state == HEADER_CMD && offset >= dataSize)
//         {
//             if (checksum == c)
//             {
//                 get_data_finish();
//             }
//             c_state = IDLE;
//         }
//     // }
// }