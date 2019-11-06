//https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/

#include <stdio.h>
#include <string.h>
typedef unsigned char DIGIT[7];

// display with 4 digits and two dots
struct display {
    DIGIT digits[4];
    char dots[2];
};

// define digits
DIGIT numbers[10] =
{
    {1,1,1,1,1,1,0},    //0
    {0,1,1,0,0,0,0},    //1
    {1,1,0,1,1,0,1},    //2
    {1,1,1,1,0,0,1},    //3
    {0,1,1,0,0,1,1},    //4
    {1,0,1,1,0,1,1},    //5
    {0,0,1,1,1,1,1},    //6
    {1,1,1,0,0,0,0},    //7
    {1,1,1,1,1,1,1},    //8
    {1,1,1,1,0,1,1}     //9
};

int main()
{
    struct display myDisplay;
    memcpy(myDisplay.digits[0], numbers[1], sizeof(DIGIT));
    memcpy(myDisplay.digits[1], numbers[4], sizeof(DIGIT));
    memcpy(myDisplay.digits[2], numbers[0], sizeof(DIGIT));
    memcpy(myDisplay.digits[3], numbers[4], sizeof(DIGIT));
}