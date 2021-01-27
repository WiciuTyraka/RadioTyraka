#ifndef RadioTyraka_h
#define RadioTyraka_h

#include <RH_E32.h>
#include <statesNaming.h> //includs enums

/*  define alldata struct */

typedef struct LoRaSinglePacket
{
    /*
    Header of packet
    8bits - message len
    */
    byte MSG_LEN = 0;
    /*
    1-8bits - message reciver ID
    it have to be 0xff
    */
    byte ID_RX = 0xff;
    /*
    1-8bits - message transmiter ID
    you can use it as you want
    */
    byte ID_TX = 0;
    /*
    1bit    - String or struct message (1 or 0)
    2-8bits - For user flags
    you can use it as you like
    */
    byte FLAGS = 0;
    /*
    message payload  - RH_E32_MAX_MESSAGE_LEN = RH_E32_MAX_PAYLOAD_LEN - RH_E32_HEADER_LEN
    message payload  - 54 = 58 - 4
    */
    char message[];

} LoRaSinglePacket;

typedef struct flags
{
    /*
    7 bits for flags
    */
    bool FLAG1 = 0;
    bool FLAG2 = 0;
    bool FLAG3 = 0;
    bool FLAG4 = 0;
    FlightState MISSION_STATE = LAUNCHPAD; // 3 bit flag.

} Flags;

typedef struct packetInfo
{
    /*
    Status of transmition 
    */
    Status status;
    /* 
    Header of packet:
    8bits - message len
    */
    byte MSG_LEN = 0;
    /*
    1-8bits - message transmiter ID
    */
    byte ID_TX = 0;
    /*
    1bit    - Serial or struct message (1 or 0)
    2-8bits - For user flags
    */
    Flags FLAGS;
    /*
    1bit    - string or struct message (0 or 1)
    2-8bits - For user flags
    */
    bool MSG_TYPE = 0;

} PacketInfo;

/* main class definition */
class RadioTyraka : public RH_E32
{

public:
    //RadioTyraka(Stream *s = &Serial, uint8_t m0_pin = 4, uint8_t m1_pin = 5, uint8_t aux_pin = 8);
    RadioTyraka(Stream *s = &Serial, uint8_t m0_pin = 4, uint8_t m1_pin = 5, uint8_t aux_pin = 8, uint8_t ID = 0);

    Status sendString(const uint8_t *message, const uint8_t size, const Flags *flags = NULL);
    Status sendStruct(const void *message, const uint8_t size, const Flags *flags = NULL);

    PacketInfo receiveMessage(void *packet, uint8_t *size_);

    //------ geters and seter ------
    void setDeviceID(uint8_t ID);
    uint8_t getDeviceID();

    // bool setFlagsByte(uint8_t flag); // set 7 bytes of flags
    // void setFlags2Bit(bool _2bit);   // set second bit in flags
    // void setFlags3Bit(bool _3bit);   // set 3th bit in flags
    // void setFlags4Bit(bool _4bit);   // set 4th bit in flags
    // void setFlags5Bit(bool _5bit);   // set 5th bit in flags
    // void setFlags6Bit(bool _6bit);   // set 6th bit in flags
    // void setFlags7Bit(bool _7bit);   // set 7th bit in flags
    // void setFlags8Bit(bool _8bit);   // set 8th bit in flags
    // uint8_t getFlagsByte();

private:
    Status tyrakaSend(const uint8_t *packet, uint8_t size_);

    Status tyrakaReceive();

    uint8_t packFlagsToByte(const Flags *struct_flags);
    Flags unpackFlagsFromByte(const uint8_t byte_flags);

    LoRaSinglePacket *init_stack(int m);

    /// ID of LoRa device
    uint8_t _device_id = 0;

    // /// flags for message
    //uint8_t _flags = 0;
};

#endif
