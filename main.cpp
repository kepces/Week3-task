//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Declaration and initialization of public global objects]===============

//DigitalIn enterButton(BUTTON1);
DigitalIn toggleGasAlarmS1(D2);
DigitalIn requestGasStateS2(D3);
DigitalIn requestTemperatureAlarmS3(D4);
DigitalIn toggleTemperatureAlarmS4(D5);
DigitalIn resetAlarmsS5(D6);
DigitalIn toggleContinousModeS6(D7);

DigitalOut gasStateLed(LED1);
DigitalOut temperatureStateLed(LED2);
DigitalOut continousModeLed(LED3);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

//=====[Declaration and initialization of public global variables]=============

bool DeleteThis = OFF;
bool gasAlarm = OFF;
bool overTemperatureAlarm = OFF;
bool continousModeEnabled = OFF;

//=====[Declarations (prototypes) of public functions]=========================

void inputsInit();
void outputsInit();

void alarmActivationUpdate();

void sendToUart();
void availableCommands();

void readButtons();

void continousOutputTask();

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    inputsInit();
    outputsInit();
    while (true) {
        readButtons();
        continousOutputTask();
        delay(200);
    }
}

//=====[Implementations of public functions]===================================

void inputsInit()
{
    toggleGasAlarmS1.mode(PullDown);
    requestGasStateS2.mode(PullDown);
    requestTemperatureAlarmS3.mode(PullDown);
    toggleTemperatureAlarmS4.mode(PullDown);
    resetAlarmsS5.mode(PullDown);
    toggleContinousModeS6.mode(PullDown);
}

void outputsInit()
{
    gasStateLed = OFF;
    temperatureStateLed = OFF;
    continousModeLed = OFF;
}

void readButtons()
{
    if ( toggleGasAlarmS1 ) {
        gasAlarm = !gasAlarm;
        gasStateLed = !gasStateLed;

        if ( gasAlarm ) {
            uartUsb.write("WARNING: GAS DETECTED\r\n", 23);
        } else {
            uartUsb.write("GAS SET TO NOT DETECTED\r\n", 25);
        } 
        delay(200);
    }

    if ( requestGasStateS2 ) {
        if ( gasAlarm ) {
            uartUsb.write("GAS ALARM ACTIVE\r\n", 20);
        } else {
            uartUsb.write("GAS ALARM CLEAR\r\n", 19);
        }
    }

    if ( requestTemperatureAlarmS3 ) {
        if ( overTemperatureAlarm ) {
            uartUsb.write("TEMP ALARM ACTIVE\r\n", 21);
        } else {
            uartUsb.write("TEMP ALARM CLEAR\r\n", 20);
        }
    }

    if ( toggleTemperatureAlarmS4 ) {
        overTemperatureAlarm = !overTemperatureAlarm;
        temperatureStateLed = !temperatureStateLed;

        if ( overTemperatureAlarm ) {
            uartUsb.write("WARNING: TEMPERATURE TOO HIGH\r\n", 31);
        } else {
            uartUsb.write("TEMPERATURE SET TO NOT HIGH\r\n", 29);
        } 
        delay(200);
    }
    
    if ( resetAlarmsS5 ) {
        gasAlarm = gasStateLed = overTemperatureAlarm = temperatureStateLed = OFF;
        uartUsb.write("ALARMS RESET\r\n", 14);
    }

    if ( toggleContinousModeS6 ) {
        continousModeEnabled = !continousModeEnabled;

        if ( continousModeEnabled ) {
            uartUsb.write("Continous mode enabled\r\n", 24);
        }
    }
    delay(2000);
}

void continousOutputTask()
{
    if ( continousModeEnabled ) {
        int buffer_size = 23;  // length of string without the on or off letters
        const char* gasStateString;
        const char* temperatureStateString;

        if ( gasAlarm )  {
            gasStateString = "ON";
            buffer_size += 2;
        } else {
            gasStateString = "OFF";
            buffer_size += 3;
        }
        if ( overTemperatureAlarm )  {
            temperatureStateString = "ON";
            buffer_size += 2;
        } else {
            temperatureStateString = "OFF";
            buffer_size += 3;
        }

        char buffer[buffer_size];
        sprintf(buffer, "GAS: %s\t Temperature: %s\r\n", gasStateString, temperatureStateString);
        uartUsb.write(buffer, buffer_size);
        delay(2000);
    }
}

void availableCommands()
{
    uartUsb.write( "Available commands:\r\n", 23 );
    uartUsb.write( "Press '1' to toggle the gas alarm state\r\n", 43 );
    uartUsb.write( "Press '2' to get the gas detector state\r\n", 43 );
    uartUsb.write( "Press '3' to get the over temperature detector state\r\n", 56 );
    uartUsb.write( "Press '4' to toggle the over-temperature\r\n", 44 );
    uartUsb.write( "Press '5' to reset the both alarms\r\n\r\n", 42 );
    uartUsb.write( "Press '6' enter or exit continous monitoring mode\r\n\r\n", 57 );
}
