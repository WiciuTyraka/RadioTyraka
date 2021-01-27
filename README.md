# RadioTyraka

## About

- [Rocket Localization System](#rocket-localization-system)
  - [About](#about)
  - [LoRa Library](#lora-library)
    - [Detailed Description](#detailed-description)
    - [Types](#types)
  - [Library Member Documentation](#library-member-documentation)
    - [Public Member Functions](#public-member-function)
    - [Protected Member Function](#protected-member-function)
    - [Private Member Functions](#private-member-function)
    - [Additional Inherited Members](#additional-inherited-members)

# LoRa Library

## Detailed Description

This library provides basic functions for sending and receiving unaddressed, unreliable datagrams of arbitrary length to 53 octets per packet. Manager classes may use this class to implement reliable, addressed datagrams and streams, mesh routers, repeaters, translators etc.
Naturally, for any 2 radios to communicate that must be configured to use the same frequency and modulation scheme.

This library provides an object-oriented interface for sending and receiving data messages with EBYTE RFM95/96/97/98(W), Semtech SX1276/77/78/7E32-TTL-1W9 and compatible radio modules. These modules implement long range LORA transcivers with a transparent serial interface.
This driver provides methods for sending and receiving messages of up to 53 octets on any frequency supported by the radio, in a range of data rates and power outputs. Frequency can be set with 1MHz precision to any frequency from 410 to 441MHz.

### Pinout

<details>

| Pin no. | Pin name |         Pin direction | Pin application                                                                                                                                                                                                                                                                                                                               |
| ------- | -------: | --------------------: | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 1       |       M0 | Input（weak pull-up） | Work with M1 & decide the four operating modes.Floating is not allowed, can be ground.                                                                                                                                                                                                                                                        |
| 2       |       M1 | Input（weak pull-up） | Work with M0 & decide the four operating modes.Floating is not allowed, can be ground.                                                                                                                                                                                                                                                        |
| 3       |      RXD |                 Input | TTL UART inputs, connects to external (MCU, PC) TXD outputpin. Can be configured as open-drain or pull-up input.                                                                                                                                                                                                                              |
| 4       |      TXD |                Output | TTL UART outputs, connects to external RXD (MCU, PC) inputpin. Can be configured as open-drain or push-pull output                                                                                                                                                                                                                            |
| 5       |      AUX |                Output | To indicate module’s working status & wakes up the external MCU. During the procedure of self-check initialization, the pin outputs low level. Can be configured as open-drain output orpush-pull output (floating is allowed). If you have trouble like freeze device, you must put a pull-up 4.7k resistor or better connect to the device. |
| 6       |      VCC |                       | Power supply 2.3V~5.5V DC                                                                                                                                                                                                                                                                                                                     |
| 7       |      GND |                       | Ground                                                                                                                                                                                                                                                                                                                                        |

&nbsp;

Various modes can be set via M0 and M1 pins.

| Mode         |  M1 |  M0 | Explanation                                                                                                                                                    |
| ------------ | --: | --: | -------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Normal       |   0 |   0 | UART and wireless channel is good to go                                                                                                                        |
| Wake-up      |   0 |   1 | Same as normal but a preamble code is added to transmitted data for waking-up the receiver.                                                                    |
| Power-Saving |   0 |   0 | UART is disable and wireless is on WOR(wake on radio) mode which means the device will turn on when there is data to be received. Transmission is not allowed. |
| Sleep        |   0 |   1 | Used in setting parameters. Transmitting and receiving disabled.                                                                                               |

</details>

### Packet Format

All messages sent and received by this Driver conform to this packet format:

| header fileds | bytes |                       Description |
| ------------- | ----: | --------------------------------: |
| MSG_LEN       |     1 |                 length of message |
| ID_RX         |     1 |            ID of receiver message |
| ID_TX         |     1 | ID of transmiter - message sender |
| FLAGS         |     1 |   user flags packed into one byte |
| message[]     |  0-53 |                           message |

`NOTE: ID_RX mus be 0xff - workaround`

### Performance

This radio supports a range of different data rates and powers. The lowest speeds are the most reliable, however you should note that at 1kbps and with an 13 octet payload, the transmission time for one packet approaches 5 seconds. Therefore you should be cautious about trying to send too many or too long messages per unit of time, lest you monopolise the airwaves. Be a good neighbour and use the lowest power and fastest speed that you can.

When running with a power output of 1W and at the slowest speed of 1kbps, this module has an impressive range. Device was tested with: E32-TTL-1W (1 W power, 1kbps data rate), transmiter was equpied with sormal wire antenna, receiver was equiped with directional antenna Yagi-Uda, both antennas was in line of sight.

The picture below shows a map with the receiver and transmitter on it.

![map_pic](map.PNG)

# Library Members Documentation

## Types

---

**DataRate**

Values to be passed to [setDataRate()](#setdatarate) to control the on-air data rate.
This is NOT to be used to control the baud rate of the serial connection to the radio

<details>

```cpp
enum DataRate
{
    DataRate1kbps = RH_E32_PARAM_SPED_DATARATE_1KBPS,
    DataRate2kbps = RH_E32_PARAM_SPED_DATARATE_2KBPS,
    DataRate5kbps = RH_E32_PARAM_SPED_DATARATE_5KBPS,
    DataRate8kbps = RH_E32_PARAM_SPED_DATARATE_8KBPS,
    DataRate10kbps = RH_E32_PARAM_SPED_DATARATE_10KBPS,
    DataRate15kbps = RH_E32_PARAM_SPED_DATARATE_15KBPS,
    DataRate20kbps = RH_E32_PARAM_SPED_DATARATE_20KBPS,
    DataRate25kbps = RH_E32_PARAM_SPED_DATARATE_25KBPS
}
```

</details>

---

**PowerLevel**

Values to be passed to [setPower()](#setpower) to control the transmitter power.

<details>

```cpp
enum PowerLevel
{
    Power30dBm = RH_E32_PARAM_OPTION_POWER_30DBM,
    Power27dBm = RH_E32_PARAM_OPTION_POWER_27DBM,
    Power24dBm = RH_E32_PARAM_OPTION_POWER_24DBM,
    Power21dBm = RH_E32_PARAM_OPTION_POWER_21DBM
}
```

</details>

---

**BaudRate**

Values to be passed to [setBaudRate()](#setbaudrate) to control the radio serial connection baud rate.
This is NOT to be used to control the on-air data rate the radio transmits and receives at

<details>

```cpp
enum BaudRate
{
    BaudRate1200 = RH_E32_PARAM_SPED_UART_BAUD_1200,
    BaudRate2400 = RH_E32_PARAM_SPED_UART_BAUD_2400,
    BaudRate4800 = RH_E32_PARAM_SPED_UART_BAUD_4800,
    BaudRate9600 = RH_E32_PARAM_SPED_UART_BAUD_9600,
    BaudRate19200 = RH_E32_PARAM_SPED_UART_BAUD_19200,
    BaudRate38400 = RH_E32_PARAM_SPED_UART_BAUD_38400,
    BaudRate57600 = RH_E32_PARAM_SPED_UART_BAUD_57600,
    BaudRate115200 = RH_E32_PARAM_SPED_UART_BAUD_115200
}
```

</details>

---

**OperatingMode**

Defines values to be passed to [setOperatinMode](#setoperationmode).

For internal driver user only.

<details>

```cpp
 typedef enum
  {
    ModeNormal = 0,  ///< Normal mode for sending and receiving messages
    ModeWakeUp,      ///< Adds a long preamble to transmission to allow destination receivers to wake up
    ModePowerSaving, ///< Receiver sleeps until a message is received
    ModeSleep        ///< Use during parameter setting
  } OperatingMode;
```

</details>

---

**Parameters**

Structure for reading and writing radio control parameters.

For internal driver user only.

<details>

```cpp
 typedef struct
  {
    uint8_t head;   ///< 0xc2 (no save) or 0xc0 (save)
    uint8_t addh;   ///< High address byte (not used by this driver)
    uint8_t addl;   ///< Low address byte (not used by this driver)
    uint8_t sped;   ///< Data and baud rate parameters
    uint8_t chan;   ///< Radio channel
    uint8_t option; ///< Various control options

  } Parameters;
```

</details>

---

**ResponseStatus**

Value which provide information about transmision succes.
Thanks to the value the user can check exactly what went wrong during the transmission.
Very useful during debugging.
returned by [sendString()](#sendstring), [sendStruct()](#sendstruct) and indirectly by [receiveMessage()](#receivemessage).

<details>

```cpp
typedef enum RESPONSE_STATUS
{
	L_SUCCESS = 1,
	ERR_UNKNOWN, /* something shouldn't happened */
	ERR_NOT_SUPPORT,
	ERR_NOT_IMPLEMENT,
	ERR_NOT_INITIAL,
	ERR_INVALID_PARAM,
	ERR_DATA_SIZE_NOT_MATCH,
	ERR_BUF_TOO_SMALL,
	ERR_TIMEOUT,
	ERR_HARDWARE,
	ERR_HEAD_NOT_RECOGNIZED,
	ERR_NO_RESPONSE_FROM_DEVICE,
	ERR_WRONG_UART_CONFIG,
	ERR_PACKET_TOO_BIG,
	ERR_MODE_TX_ACTIVE,
	ERR_INCOMPLETE_HEADER
} Status;
```

</details>

---

**FlightState**

Value used to convey information about the current status of the mission.
Used as one of transmission flag.

`NOTE: useful in Rockets projects`

<details>

```cpp
typedef enum flightState
{
	LAUNCHPAD = 0,
	PROPELLED_FLIGHT,
	COAST,
	DROGUE_DEPLOYMENT,
	DROGUE_DESCENT,
	MAIN_DEPLOYMENT,
	MAIN_DESCENT

} FlightState;
```

</details>

---

**LoRaSinglePacket**

This is struct used for wraping message into packet.
Library automatically wrap user message in [sendString()](#sendstring) and [sendStruct()](#sendstruct).
Each field contains information about the packet.

`NOTE: ID_RX must be 0xff, its broadcast value.`

<details>

```cpp
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
```

</details>

---

**Flags**

Data container for user flags.
These flags can be used by the user in any way, depend of user needs.
used as one field in message packet.

<details>

```cpp
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
```

</details>

---

**PacketInfo**

Result of [receiveMessage()](#receivemessage).
This container contain transmission metadata.

<details>

```cpp
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
```

</details>

---

## Public Member Functions

### Constructor

User can have multiple instances of Contructor, but each instance must have its own serial connection, M0 M1 and AUX connections. Initialises the mode of the referenced pins Does NOT set the baud rate of the serial connection to the radio.

<details>

```cpp
RadioTyraka( Stream *s = &Serial,
             uint8_t m0_pin = 4,
             uint8_t m1_pin = 5,
             uint8_t aux_pin = 8,
             uint8_t ID = 0
             );
```

**Parameters**

- `*s` - pointer to Stream class object. Used for UART communication between the microcontroler and LoRa device. HardwareSerial or SoftwareSerial object shouldbe used in constructor invoke.
- `m0_pin` and `m1_pin` - pins used for changing operation mode.
- `aux_pin` - is a pin that check the operation, transmission and receiving status.
- `ID` - a unique identifier in the range 1-255. enables identification of the transmitting device. It is not needed for correct operation, the default value is 0.

</details>

---

### Init

Method used for LoRa device initialization.

<details>
Initialize the device transport hardware and software. Make sure the RadioTyraka is properly, including setting the serial port baud rate and parity to that configured in the radio (typically 9600 baud, 8N1) before calling init(). Sets the module to default transmition values (these setting can be changed after initialisation with the various set\* functions):

- transmition frequency - `443MHz`,
- transmision power - `21dBm`,
- data rate - `5kpbs`.

This function may not return if the AUX pin is not connected. Initialisation failure can be caused by: Electrical connections to the radio, incorrect or incomplete Radio configured to use a different baud rate to the one configured to the microcontoler serial port, incorrect radio module connected to the serial port. Other serial communicaitons problems between the microcontroler and the radio device.

**Returns**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; true if initialisation succeeded.

&nbsp;

inherited from RadioHead.

</details>

---

### waitPacketSent

Waits for any currently transmitting packet to be completely sent

<details>

```cpp
bool 	waitPacketSent ()
```

**Returns**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;true if successful

&nbsp;

inherited from RadioHead.

</details>

---

### setPower

Sets the transmitter power output

<details>

```cpp
bool  setPower (PowerLevel level)
```

**Parameters**

- `level` - level of valid power setting from the Power enum

&nbsp;

**Returns**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;true if successful

&nbsp;

inherited from RadioHead.

</details>

---

### setDataRate

Sets the on-air data rate to be used by the transmitter and receiver.

<details>

```cpp
bool 	setDataRate (DataRate rate)
```

**Parameters**

- `rate` - level of valid data rate from the DataRate enum

&nbsp;

**Returns**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;true if successful

&nbsp;

inherited from RadioHead.

</details>

---

### setFrequency

Sets the tarnsmitter and receiver frequency.

<details>

```cpp
bool 	setFrequency (uint16_t frequency)
```

**Parameters**

- `frequency` - level of valid frequency in range from 410 to 441 MHz.

&nbsp;

**Returns**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;true if successful

&nbsp;

inherited from RadioHead.

</details>

---

### sendString

Send string message and wrap message into packet.

<details>

```cpp
Status sendString(const uint8_t *message, const uint8_t size, const Flags *flags = NULL);
```

**Parameters**

- `message` - pointer to the memory area where the message is stored,
- `size` - number of bytes of data to send,
- `flags` - pointer to Flags struct which stored all flags. This parameter is optional, the message can be sent without flags.

&nbsp;

**Returns**

- `Status` - status stored information about transmission success.

</details>

---

### sendStruct

Send data from struct and wrap message into packet.

<details>

```cpp
Status sendStruct(const void *message, const uint8_t size, const Flags *flags = NULL);
```

**Parameters**

- `message` - pointer to the memory area where the message is stored,
- `size` - number of bytes of data to send,
- `flags` - pointer to Flags struct which stored all flags. This parameter is optional, the message can be sent without flags.

&nbsp;

**Returns**

- `Status` - status stored information about transmission success.

</details>

---

### receiveMessage

Method used to receive message, can be used for both, struct and string messages.
Message type can be distinguished by type field in Packet Info struct.

<details>

```cpp
PacketInfo receiveMessage(void *packet, uint8_t *size_);
```

**Parameters**

- `packet` - pointer to the memory area where the message will be stored.
- `size` - pointer to variable which stored number of bytes of recived data.

&nbsp;

**Returns**

- `PacketInfo` - struct which stored metadeta of transmission.

| PacketInfo fileds | bit no. |                                 Description |
| ----------------- | ------: | ------------------------------------------: |
| status            |       8 |                       Status of transmition |
| MSG_LEN           |       8 |                   length of recived message |
| ID_TX             |       8 |                       message transmiter ID |
| FLAGS             |       7 |                                  user flags |
| MSG_TYPE          |       1 | type of message - string or struct (0 or 1) |

</details>

---

### getDeviceID

get the transmiter device ID which was set by [constructor](#constructor) or [setDeviceId](#setdeviceid).

<details>

```cpp
uint8_t getDeviceID();
```

**Returns**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Device ID.

</details>

---

### setDeviceID

Set new ID of the device.

<details>

```cpp
void setDeviceID(uint8_t ID);
```

**Parameters**

- `ID` - new unique identificator number.

</details>

---

## Protected Member Functions

### setOperatingMode

Sets the operating mode of the radio. For internal use only

<details>

```cpp
void 	setOperatingMode (OperatingMode mode)
```

**Parameters**

- `mode` - value of valid mode from the OperatingMode enum

&nbsp;
inherited from RadioHead.

</details>

---

### waitAuxHigh

Waits for the AUX pin to go high For internal use only.

<details>

```cpp
void 	waitAuxHigh ()
```

&nbsp;
inherited from RadioHead.

</details>

---

### waitAuxLow

Waits for the AUX pin to go low For internal use only

<details>

```cpp
void 	waitAuxLow ()
```

&nbsp;
inherited from RadioHead.

</details>

---

### readParameters

Read the radio configuration parameters from LoRa device into local memory.

<details>

```cpp
bool 	readParameters (Parameters &params)
```

**Parameters**

- `params` - reference to a Parameter structure which will be filled if successful.

&nbsp;

**Returns**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;true if successful

&nbsp;

inherited from RadioHead.

</details>

---

### writeParameters

Write radio configuration parameters from local memory to the radio. You can choose whether the parameter will be saved across power down or not.

<details>

```cpp
bool 	writeParameters (Parameters &params, bool save=false)
```

**Parameters**

- `params` - Reference to a Parameter structure containing the radio configuration parameters to be written to the radio.
- `save` - If true, the parameters will be saved across power down in the radio

&nbsp;

**Returns**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;true if successful

&nbsp;

inherited from RadioHead.

</details>

---

### validateRxBuf

Examine the receive buffer to determine whether the message is for this node.
Check whether the latest received message is complete and uncorrupted.
For internal use only.

<details>

```cpp
void 	validateRxBuf ()
```

inherited from RadioHead.

</details>

---

### clearRxBuf

Clear local receive buffer. For internal use only.

<details>

```cpp
void 	clearRxBuf ()
```

&nbsp;

inherited from RadioHead.

</details>

---

## Private Member Functions

### tyrakaSend

Method used internaly by [sendString](#sendstring) and [sendStruct](#sendstruct) to send wrapped packet.
This method directly drive UART port.

For internal driver user only.

<details>

```cpp
Status tyrakaSend(const uint8_t *packet, uint8_t size_);
```

**Parameters**

- `message` - pointer to the memory area where the packet is stored,
- `size` - number of bytes of data to send,

&nbsp;

**Returns**

- `Status` - status stored information about transmission success.

</details>

---

### tyrakaReceive

Method used internaly by [receiveMessage](#receivemessage) to recive wrapped packet.
This method directly drive UART port.

For internal driver user only.

<details>

```cpp
Status tyrakaReceive();
```

**Returns**

- `Status` - status stored information about transmission success.

</details>

---

### packFlagsToByte

Method used internaly by [sendString](#sendstring) and [sendStruct](#sendstruct) to pack all fields from Flags struct into one byte of packet.

For internal driver user only.

<details>

```cpp
uint8_t packFlagsToByte(const Flags *struct_flags);
```

**Parameters**

- `struct_flags` - pointer to the memory area where the flags are stored.

&nbsp;

**Returns**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; byte which store all flags bits.

</details>

---

### unpackFlagsFromByte

Method used internaly by [receiveMessage](#receivemessage) to unpack flags byte into Flags struct.

For internal driver user only.

<details>

```cpp
Flags unpackFlagsFromByte(const uint8_t byte_flags);
```

**Parameters**

- `byte_flags` - byte which store all flags bits.

&nbsp;

**Returns**

- `Flags` - Struct with unpacked flags.

</details>

---

### init_stack

Method used internaly by [sendString](#sendstring) and [sendStruct](#sendstruct) to allocate memory for string or struct data.

For internal driver user only.

<details>

```cpp
LoRaSinglePacket *init_stack(int m);
```

**Parameters**

- `m` - size of message.

&nbsp;

**Returns**

- `LoRaSinglePacket` - conteiner for packet with allocated memory for header and message.

</details>

---
