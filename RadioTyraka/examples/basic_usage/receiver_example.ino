//example sketch for Receiver,
//with RadioTyraka library
//Board STM32

#include <RadioTyraka.h>

//------ Defines LoRa pins----------
#define loraRx 17
#define loraTx 16
#define m0 4
#define m1 0
#define aux 2
//------ Defines leds pins----------
#define led1 26
#define led2 27
#define led3 12
#define led4 14

//--------constructor for LoRa device------
RadioTyraka LoRa(&Serial2, m0, m1, aux); //arguments -> (&Serial,M0,M1,AUX)

long int temp_recv = 0; //time from last message

void setup()
{
    //---- set all leds pins -----------
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(led3, OUTPUT);
    pinMode(led4, OUTPUT);

    Serial.begin(9600); //Start log Serial - from receiver
    while (!Serial)
        ; // Wait for serial port to be available

    // Init the serial connection to the E32 module
    // which is assumned to be running at 9600baud.
    // If your E32 has been configured to another baud rate, change this:
    Serial.println("Wait for lora to be available");
    Serial2.begin(9600);
    while (!Serial2)
        ;

    Serial.println("initing"); //Init LoRa
    if (!LoRa.init())
    {
        Serial.println("init failed"); //init faild, check wiring
        digitalWrite(led2, HIGH);
        while (1)
            ;
    }
    Serial.println("init over"); //init pass successfully, LoRa rready to use
    digitalWrite(led1, HIGH);

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
    if (LoRa.available()) //Is any message avaliable?
    {

        //max message length
        // MAX_MESSAGE_LEN = MAX_PAYLOAD_LEN - HEADER_LEN [53 = 58 - 5]
        // header is added automatically, so that means we can use 55 bytes.
        uint8_t buf[RH_E32_MAX_MESSAGE_LEN]; //buffer for message
        uint8_t len = sizeof(buf);           //number of characters

        /*Receive message, message stored in buf, len of message stored in len.
          as a result we got info.
          info content:
            Status   <- it's transmission success status,
            MSG_LEN  <- lenght of actual tranmited message + header,
            ID_TX    <- transmiter ID.
            FLAGS    <- user flags.
            MSG_TYPE <-  string or struct.
        */
        PacketInfo info = LoRa.receiveMessage(buf, &len); //    <----------- recevie message

        if (info.status == L_SUCCESS) // check if Status of transmition is successful.
        {

            Serial.println("Buffer:"); // Print content of buffer in hex
            for (int i = 0; i < info.MSG_LEN; i++)
            {
                char b[2];
                sprintf(b, "%02X", buf[i]);
                Serial.print(b);
                Serial.print(" ");
            }
            Serial.println();
            Serial.print("odebrano wiadomosc : ");
            Serial.println((char *)buf); // Print acctual message

            Serial.print("o rozmiarze : ");
            Serial.println(info.MSG_LEN); //print length of message + header

            Serial.print("czas od ostatniej wiadomosci: ");
            Serial.print(millis() - temp_recv);
            Serial.println(" [ms]");
            Serial.println();

            temp_recv = millis();
        }
        else //fail if we receiv nothing - something goes wrong
        {
            Serial.println("recv failed");
        }
    }
}