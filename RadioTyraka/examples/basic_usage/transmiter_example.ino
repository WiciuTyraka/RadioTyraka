//example sketch for GPD Tracker,
//with RadioTyraka library
//uC STM32f411

#include <RadioTyraka.h>
//#include "SoftwareSerial.h"

//------ Defines LoRa pins----------
#define tx PA9
#define rx PA10
#define m0 PA11
#define m1 PA12
#define aux PC13

//harware serial for log - from device
HardwareSerial logSerial(PC7, PC6); // RX, TX

//Define Serial for LoRa as hardware serial
HardwareSerial loraSerial(rx, tx);
//SoftwareSerial mySerial(rx,tx);      //TX,RX
RadioTyraka LoRa(&loraSerial, m0, m1, aux, 127); //arguments -> (&Serial,M0,M1,AUX,ID)

int i = 0; //variable for incr number of send message

void setup()
{

    logSerial.begin(9600); //Start log Serial - from receiver

    // Init the serial connection to the E32 module
    // which is assumned to be running at 9600baud.
    // If your E32 has been configured to another baud rate, change this:
    loraSerial.begin(9600);
    while (!loraSerial)
        ;

    if (!LoRa.init())
    {
        logSerial.println("init faild!!");
        while (1)
            ;
    }
    else
        logSerial.println("init over");

    // Defaults after initialising are:
    // 433MHz, 21dBm, 5kbps
    // You can change these as below

    //  /----------------------------------------------------------------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------  settings ---------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------------------------------------------
    if (!LoRa.setDataRate(RH_E32::DataRate5kbps)) //ustawiamy opcje : RH_E32::DataRate25kbps  |   RH_E32::DataRate10kbps  |   RH_E32::DataRate1kbps
        Serial.println("setDataRate failed");
    //  if (!LoRa.setPower(RH_E32::Power21dBm))             //ustawiamy opcje : RH_E32::Power30dBm      |   RH_E32::Power21dBm
    // //    Serial.println("setPower failed");

    if (!LoRa.setFrequency(433))
        Serial.println("setFrequency failed");

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------------------------------------------------------------------------
}

void loop()
{
    // Send a message to Receiver
    char t[42];
    sprintf(t, "hello world #%d", i++);

    /*Transmit message stored in t.
      as a result we got status.
      status stored information about transmission success.
    */
    Status status = LoRa.sendString((uint8_t *)t, sizeof(t));

    logSerial.println("wysyalem widomosc:");
    logSerial.println(t);
    logSerial.println(status);

    ////  //LoRa.waitPacketSent();    // waiting for pacekt fully send

    delay(5);
}