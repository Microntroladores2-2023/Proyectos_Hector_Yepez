#include <Arduino.h>

typedef struct
{
    uint8_t b0 : 1;
    uint8_t b1 : 1;
    uint8_t b2 : 1;
    uint8_t b3 : 1;
    uint8_t b4 : 1;
    uint8_t b5 : 1;
    uint8_t b6 : 1;
    uint8_t b7 : 1;

} UINT8_BITS;

typedef struct
{
    uint16_t b0 : 1;
    uint16_t b1 : 1;
    uint16_t b2 : 1;
    uint16_t b3 : 1;
    uint16_t b4 : 1;
    uint16_t b5 : 1;
    uint16_t b6 : 1;
    uint16_t b7 : 1;
    uint16_t b8 : 1;
    uint16_t b9 : 1;
    uint16_t b10 : 1;
    uint16_t b11 : 1;
    uint16_t b12 : 1;
    uint16_t b13 : 1;
    uint16_t b14 : 1;
    uint16_t b15 : 1;
} UINT16_BITS;

typedef union _BYTE_VAL
{
    UINT8_BITS bits;
    uint8_t Val;
} UINT8_VAL;

typedef union _WORD_VAL
{
    uint16_t Val;
    UINT16_BITS bits;
    struct
    {
        uint8_t LB;
        uint8_t HB;
    } byte;

} UINT16_VAL;

typedef union _DWORD_VAL
{
    uint32_t Val;
    struct
    {
        uint8_t LB;
        uint8_t HB;
        uint8_t UB;
        uint8_t MB;
    } byte;
    struct
    {
        uint16_t LW;
        uint16_t HW;
    } word;
    struct
    {
        UINT16_VAL low;
        UINT16_VAL high;
    } wordUnion;
    struct
    {
        UINT8_VAL lowLSB;
        UINT8_VAL lowMSB;
        UINT8_VAL highLSB;
        UINT8_VAL highMSB;
    } byteUnion;
    uint8_t v[4];
    uint16_t w[2];
} UINT32_VAL;

typedef union _INT_VAL
{
    unsigned int Val;
    struct
    {
        uint8_t LB;
        uint8_t HB;
        uint8_t UB;
        uint8_t MB;
    } byte;
    struct
    {
        uint16_t LW;
        uint16_t HW;
    } word;
    struct
    {
        UINT16_VAL low;
        UINT16_VAL high;
    } wordUnion;
    struct
    {
        UINT8_VAL lowLSB;
        UINT8_VAL lowMSB;
        UINT8_VAL highLSB;
        UINT8_VAL highMSB;
    } byteUnion;
    uint8_t v[4];
    uint16_t w[2];
} UINT_VAL;

#define LSB(a) ((a).v[0])
#define MSB(a) ((a).v[1])

#define LOWER_LSB(a) ((a).v[0])
#define LOWER_MSB(a) ((a).v[1])
#define UPPER_LSB(a) ((a).v[2])
#define UPPER_MSB(a) ((a).v[3])


typedef union 
{
    float Val;
    struct
    {
        uint8_t Byte0;
        uint8_t Byte1;
        uint8_t Byte2;
        uint8_t Byte3;
    } byte;
}float_VAL;
