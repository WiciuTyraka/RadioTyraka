# RadioTyraka

## About

- [Rocket Localization System](#rocket-localization-system)
  - [About](#about)
  - [LoRa Library](#lora-library)
    - [Detailed Description](#detailed-description)
    - [Types](#types)
    - [Public Member Functions](#public-member-function)
    - [Protected Member Function](#protected-member-function)
    - [Private Member Functions](#private-member-function)
    - [Additional Inherited Members](#additional-inherited-members)
    - [Library Member Documentation](#library-member-documentation)

# LoRa Library

## Detailed Description

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

## Library Member Documentation

### Types

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

## </details>

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

## </details>

---

### Public Member Functions

### Protected Member Functions

### Private Member Functions

### Constructor

<details>
 You can have multiple instances of Contructor, but each instance must have its own serial connection, M0 M1 and AUX connections. Initialises the mode of the referenced pins Does NOT set the baud rate of the serial connection to the radio.

    RadioTyraka( Stream *s = &Serial,
                 uint8_t m0_pin = 4,
                 uint8_t m1_pin = 5,
                 uint8_t aux_pin = 8,
                 uint8_t ID = 0
                 );

**Parameters**

- `*s` - pointer to Stream class object. Used for UART communication between the microcontroler and LoRa device. HardwareSerial or SoftwareSerial object shouldbe used in constructor invoke.
- `m0_pin` and `m1_pin` - pins used for changing operation mode.
- `aux_pin` - is a pin that check the operation, transmission and receiving status.
- `ID` - a unique identifier in the range 1-255. enables identification of the transmitting device. It is not needed for correct operation, the default value is 0.

</details>

### Init

<details>

Itialise the device transport hardware and software. Make sure the RadioTyraka is properly, including setting the serial port baud rate and parity to that configured in the radio (typically 9600 baud, 8N1) before calling init(). Sets the module to default transmition values (these setting can be changed after initialisation with the various set\* functions):

- transmition frequency - `443MHz`,
- transmision power - `21dBm`,
- data rate - `5kpbs`.

This function may not return if the AUX pin is not connected. Initialisation failure can be caused by: Electrical connections to the radio, incorrect or incomplete Radio configured to use a different baud rate to the one configured to the microcontoler serial port, incorrect radio module connected to the serial port. Other serial communicaitons problems between the microcontroler and the radio device.

**Returns**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; true if initialisation succeeded.

&nbsp;

inherited from RadioHead.

</details>
