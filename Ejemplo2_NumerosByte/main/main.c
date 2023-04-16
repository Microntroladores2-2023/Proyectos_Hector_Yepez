#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void ver(const char *tag, int l)
{
    printf("%s\t %d \n", tag, l);
}

void app_main()
{
    printf("\n");
    printf("Progroma para ver el numero de byte de las variables\n");
    printf("\n");

    ver(" bool", sizeof(bool));
    // ver(" boolean", sizeof(boolean));
    // ver(" byte", sizeof(byte));
    ver(" char", sizeof(char));
    ver(" unsigned char", sizeof(unsigned char));
    ver(" uint8_t", sizeof(uint8_t));
    ver(" int8_t", sizeof(int8_t));
    ver(" short", sizeof(short));
    ver(" uint16_t", sizeof(uint16_t));
    ver(" int16_t", sizeof(int16_t));
    // ver(" word", sizeof(word));
    ver(" int", sizeof(int));
    ver(" unsigned int", sizeof(unsigned int));
    ver(" size_t", sizeof(size_t));
    ver(" float", sizeof(float));
    ver(" long", sizeof(long));
    ver(" unsigned long", sizeof(unsigned long));
    ver(" uint32_t", sizeof(uint32_t));
    ver(" int32_t", sizeof(int32_t));
    ver(" double", sizeof(double));
    ver(" long long", sizeof(long long));
    ver("unsigned long long", sizeof(unsigned long long));
    ver(" uint64_t", sizeof(uint64_t));
    ver(" int64_t", sizeof(int64_t));
}