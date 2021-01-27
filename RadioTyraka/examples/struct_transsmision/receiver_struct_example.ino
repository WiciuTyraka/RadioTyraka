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

//------ example of struct message
struct structMessage
{
    char type[5] = "TEST";
    char message[12] = "RadioTyraka";
    int number = 420;
    float float_number = 420.420;
    int counter = 0;
};

void print_message(PacketInfo *info, struct structMessage *msg);

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
        struct structMessage my_struct_message;
        uint8_t len = RH_E32_MAX_MESSAGE_LEN;
        /*Receive message, message stored in buf, len of message stored in len.
          as a result we got info.
          info content:
            Status   <- it's transmission success status,
            MSG_LEN  <- lenght of actual tranmited message + header,
            ID_TX    <- transmiter ID.
            FLAGS    <- user flags.
            MSG_TYPE <-  string or struct.
        */
        PacketInfo info = LoRa.receiveMessage(&my_struct_message, &len); //    <----------- recevie message

        Serial.print("Status: ");
        Serial.println(getStatusDescription(info.status));

        if (info.status == L_SUCCESS) // check if Status of transmition is successful.
        {
            print_message(&info, &my_struct_message);
        }
        else //fail if we receiv nothing - something goes wrong
        {
            Serial.println("recv failed");
        }
    }
}

void print_message(PacketInfo *info, struct structMessage *msg)
{
    // Serial.println("Buffer:"); // Print content of buffer in hex
    // for (int i = 0; i < info.MSG_LEN; i++)
    // {
    //     char b[2];
    //     sprintf(b, "%02X", buf[i]);
    //     Serial.print(b);
    //     Serial.print(" ");
    // }
    Serial.print("odebrano wiadomosc numer: ");
    Serial.println(msg->counter); // Print acctual message
    Serial.print("czas od ostatniej wiadomosci: ");
    Serial.print(millis() - temp_recv);
    Serial.println(" [ms]");
    Serial.println();

    Serial.println("STRUCT:");
    Serial.print("type         : ");
    Serial.println(msg->type);
    Serial.print("message      : ");
    Serial.println(msg->message);
    Serial.print("number       : ");
    Serial.println(msg->number);
    Serial.print("float_number : ");
    Serial.println(msg->float_number);
    Serial.println();

    Serial.println("HEADER INFO");
    Serial.print("message type   : ");
    Serial.println(info->MSG_TYPE);
    Serial.println("0  - string");
    Serial.println("1  - struct");
    Serial.print("message lenght : ");
    Serial.println(info->MSG_LEN); //print length of message + header
    Serial.println("FLAGS");
    Serial.print("FLAG1         : ");
    Serial.println(info->FLAGS.FLAG1);
    Serial.print("FLAG2         : ");
    Serial.println(info->FLAGS.FLAG2);
    Serial.print("FLAG3         : ");
    Serial.println(info->FLAGS.FLAG3);
    Serial.print("FLAG4         : ");
    Serial.println(info->FLAGS.FLAG4);
    Serial.print("MISSION_STATE : ");
    Serial.println(getflightStateDescription(info->FLAGS.MISSION_STATE));

    Serial.println("-------------------------");
    Serial.println("-------------------------");
    Serial.println();

    temp_recv = millis();
}