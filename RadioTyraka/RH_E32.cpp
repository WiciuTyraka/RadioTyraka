// RH_E32.cpp
//
// Copyright (C) 2017 Mike McCauley
// $Id: RH_E32.cpp,v 1.4 2018/11/15 01:10:48 mikem Exp $

#include <RadioHead.h>
#ifdef RH_HAVE_SERIAL // No serial

#include <RH_E32.h>
#include <Stream.h>

RH_E32::RH_E32(Stream *s, uint8_t m0_pin, uint8_t m1_pin, uint8_t aux_pin)
    : _s(s),
      _m0_pin(m0_pin),
      _m1_pin(m1_pin),
      _aux_pin(aux_pin)
{
  // Prevent glitches at startup
  pinMode(_aux_pin, INPUT);
  digitalWrite(_m0_pin, true);
  digitalWrite(_m1_pin, true);
  pinMode(_m0_pin, OUTPUT);
  pinMode(_m1_pin, OUTPUT);
}

bool RH_E32::init()
{
  // When a message is available, Aux will go low 5 msec before the first character is output
  // So if we ever wait more than this period of time after Aux low, can conclude there will be no data
  _s->setTimeout(10);

  delay(200);
  // Wait until the module is connected
  waitAuxHigh();

  if (!getVersion())
    return false; // Could not communicate with module or wrong type of module
  setMode(RHModeRx);
  clearRxBuf();

  if (!setDataRate(DataRate8kbps))
    return false;

  if (!setPower(Power21dBm))
    return false;
  //  if (!setBaudRate(BaudRate9600, Parity8N1))
  //  return false;

  if (!setFrequency(411))
    return false;

  return true;
}

bool RH_E32::reset()
{
  setOperatingMode(ModeSleep);
  uint8_t resetCommand[] = {RH_E32_COMMAND_RESET, RH_E32_COMMAND_RESET, RH_E32_COMMAND_RESET};
  size_t result = _s->write(resetCommand, sizeof(resetCommand));
  setOperatingMode(ModeNormal);
  return (result == sizeof(resetCommand));
}

bool RH_E32::readParameters(Parameters &params)
{
  setOperatingMode(ModeSleep);
  uint8_t readParamsCommand[] = {RH_E32_COMMAND_READ_PARAMS, RH_E32_COMMAND_READ_PARAMS, RH_E32_COMMAND_READ_PARAMS};
  _s->write(readParamsCommand, sizeof(readParamsCommand));
  delay(10);
  size_t result = _s->readBytes((char *)&params, sizeof(params)); // default 1 sec timeout
  setOperatingMode(ModeNormal);
  return (result == sizeof(Parameters));
}

bool RH_E32::writeParameters(Parameters &params, bool save)
{
  setOperatingMode(ModeSleep);
  params.head = save ? RH_E32_COMMAND_WRITE_PARAMS_SAVE : RH_E32_COMMAND_WRITE_PARAMS_NOSAVE;
  //  printBuffer("writing now", (uint8_t*)&params, sizeof(params));
  size_t result = _s->write((uint8_t *)&params, sizeof(params));
  if (result != sizeof(params))
    return false;
  delay(10); // MAGICZNY DELAY
  // Now we expect to get the same data back
  result = _s->readBytes((char *)&params, sizeof(params));
  if (result != sizeof(params))
    return false;
  //    printBuffer("additional read", (uint8_t*)&params, sizeof(params));
  // Without a little delay here, writing params often fails
  delay(100);

  setOperatingMode(ModeNormal);
  return result == sizeof(params);
}

void RH_E32::setOperatingMode(OperatingMode mode)
{
  waitAuxHigh();
  switch (mode)
  {
  case ModeNormal:
    digitalWrite(_m0_pin, false);
    digitalWrite(_m1_pin, false);
    break;

  case ModeWakeUp:
    digitalWrite(_m0_pin, true);
    digitalWrite(_m1_pin, false);
    break;

  case ModePowerSaving:
    digitalWrite(_m0_pin, false);
    digitalWrite(_m1_pin, true);
    break;

  case ModeSleep:
    digitalWrite(_m0_pin, true);
    digitalWrite(_m1_pin, true);
    break;
  }
  delay(20); // Takes a little while to start its response
  waitAuxHigh();
}

bool RH_E32::getVersion()
{
  setOperatingMode(ModeSleep);
  uint8_t readVersionCommand[] = {RH_E32_COMMAND_READ_VERSION, RH_E32_COMMAND_READ_VERSION, RH_E32_COMMAND_READ_VERSION};
  _s->write(readVersionCommand, sizeof(readVersionCommand));
  uint8_t version[4];
  delay(10);
  size_t result = _s->readBytes((char *)version, sizeof(version)); // default 1 sec timeout
  setOperatingMode(ModeNormal);
  if (result == 4)
  {
    // Successful read
    //      printBuffer("read version", version, sizeof(version));
    if (version[0] != 0xc3 || version[1] != 0x32)
    {
      // Not an E32
      return false;
    }
    else
    {
      // REVISIT: do something with it?
    }
  }
  else
  {
    // Read failed: no module? Wrong baud?
    return false;
  }
  return true;
}

void RH_E32::waitAuxHigh()
{
  // REVISIT: timeout needed?
  while (digitalRead(_aux_pin) == false)
    ;
}

void RH_E32::waitAuxLow()
{
  while (digitalRead(_aux_pin) == true)
    ;
}

// Check whether the latest received message is complete and uncorrupted
void RH_E32::validateRxBuf()
{
  if (_bufLen < RH_E32_HEADER_LEN)
    return; // Too short to be a real message
  if (_bufLen != _buf[0])
    return; // Do we have all the message?

  // Extract the 4 headers
  _rxHeaderTo = _buf[1];
  _rxHeaderFrom = _buf[2];
  _rxHeaderId = _buf[3];
  _rxHeaderFlags = _buf[4];
  if (_promiscuous ||
      _rxHeaderTo == _thisAddress ||
      _rxHeaderTo == RH_BROADCAST_ADDRESS)
  {
    _rxGood++;
    _rxBufValid = true;
  }
}

void RH_E32::validateRxBufTelemetry()
{
  const char headerLength = 2;
  if (_bufLen < headerLength)
    return; // Too short to be a real message
  if (_bufLen != (_buf[0] & 0b00111111))
    return; // Do we have all the message?

  _rxGood++;
  _rxBufValid = true;
}

void RH_E32::clearRxBuf()
{
  _rxBufValid = false;
  _bufLen = 0;
}

bool RH_E32::available()
{
  // Caution: long packets could be sent in several bursts
  if (!_rxBufValid)
  {
    if (_mode == RHModeTx)
      return false;

    if (!_s->available())
      return false;

    // Suck up all the characters we can
    uint8_t data;
    while (_s->readBytes((char *)&data, 1) == 1) // Not read timeout
    {
      _buf[_bufLen++] = data;
    }
    // Now assess what we have
    if (_bufLen < RH_E32_HEADER_LEN)
    {
      //	    Serial.println("Incomplete header");
      return false;
    }

    else if (_bufLen < _buf[0])
    {
      //	    Serial.println("Incomplete message");
      return false;
    }

    else if (_bufLen > _buf[0] || _bufLen > RH_E32_MAX_PAYLOAD_LEN)
    {
      //	    Serial.println("Overrun");
      clearRxBuf();
      _rxBad++;
      return false;
    }

    // Else it a partial or complete message, test it
    //	printBuffer("read success", _buf, _bufLen);
    validateRxBuf();
  }
  return _rxBufValid;
}

bool RH_E32::availableTelemetry()
{
  const char headerLength = 2;
  // Caution: long packets could be sent in several bursts
  if (!_rxBufValid)
  {
    if (_mode == RHModeTx)
      return false;

    if (!_s->available())
      return false;

    // Suck up all the characters we can
    uint8_t data;
    while (_s->readBytes((char *)&data, 1) == 1) // Not read timeout
    {

      _buf[_bufLen++] = data;
    }
    // Now assess what we have
    if (_bufLen < headerLength)
    {
      return false;
    }

    else if (_bufLen < (_buf[0] & 0b00111111))
    {
      return false;
    }

    else if (_bufLen > (_buf[0] & 0b00111111) || _bufLen > RH_E32_MAX_PAYLOAD_LEN)
    {
      clearRxBuf();
      _rxBad++;
      return false;
    }

    // Else it a partial or complete message, test it
    //	printBuffer("read success", _buf, _bufLen);
    validateRxBufTelemetry();
  }
  return _rxBufValid;
}

bool RH_E32::recv(uint8_t *buf, uint8_t *len)
{
  if (!available())
    return false;
  if (buf && len)
  {
    // Skip the 4 headers that are at the beginning of the rxBuf
    if (*len > _bufLen - RH_E32_HEADER_LEN)
      *len = _bufLen - RH_E32_HEADER_LEN;
    memcpy(buf, _buf, *len);
  }
  clearRxBuf(); // This message accepted and cleared
  return true;
}

bool RH_E32::recvTelemetry(uint8_t *buf, uint8_t *len, uint8_t *headerFirstByte, uint8_t *crc)
{
  const char headerLength = 2;
  if (!availableTelemetry())
    return false;
  if (buf && len)
  {
    // Skip the 4 headers that are at the beginning of the rxBuf
    if (*len > _bufLen - headerLength)
      *len = _bufLen - headerLength;
    memcpy(buf, _buf + headerLength, *len);
    *headerFirstByte = _buf[0];
    *crc = _buf[1];
  }
  clearRxBuf(); // This message accepted and cleared
  return true;
}

bool RH_E32::send(const uint8_t *data, uint8_t len)
{
  if (len > RH_E32_MAX_MESSAGE_LEN)
    return false;

  waitPacketSent(); // Make sure we dont collide with previous message

  // Set up the headers
  _buf[0] = len + RH_E32_HEADER_LEN; // Number of octets in teh whole message
  _buf[1] = _txHeaderTo;             // do usuniecia
  _buf[2] = _txHeaderFrom;           // do usuniecia
  _buf[3] = _txHeaderId;
  _buf[4] = _txHeaderFlags;

  // REVISIT: do we really have to do this? perhaps just write it after writing the header?
  memcpy(_buf + RH_E32_HEADER_LEN, data, len);

  _s->write(_buf, len + RH_E32_HEADER_LEN);
  setMode(RHModeTx);
  _txGood++;
  // Aux will return high when the TX buffer is empty

  return true;
}

uint8_t RH_E32::calcCRC(char *str, uint8_t len)
{
  uint8_t crc = 0x00;
  char extract;
  uint8_t sum;
  for (int i = 0; i < len; i++)
  {
    extract = *str;
    for (char tempI = 8; tempI; tempI--)
    {
      sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum)
        crc ^= 0x8C;
      extract >>= 1;
    }
    str++;
  }
  return crc;
}

bool RH_E32::sendBinaryStruct(const uint8_t *data, uint8_t len, bool isTelemetry)
{
  const uint8_t headerLength = 2;
  const uint8_t maxMessageSize = 58 - headerLength;

  if (len > maxMessageSize)
    return false;

  waitPacketSent(); // Make sure we dont collide with previous message

  // Set up the headers
  // For binary message: 00xxxxxx , x = length
  _buf[0] = len + headerLength; // Number of octets in teh whole message

  if (!isTelemetry)
  {
    _buf[0] = _buf[0] | 0b01000000; // This is message from GPS tracker
  }

  _buf[1] = 0x00; //calcCRC((char *)data, len); // mby for crc?

  // REVISIT: do we really have to do this? perhaps just write it after writing the header?
  memcpy(_buf + headerLength, data, len);

  _s->write(_buf, len + headerLength);
  setMode(RHModeTx);
  _txGood++;
  // Aux will return high when the TX buffer is empty

  return true;
}

bool RH_E32::sendCharArray(const uint8_t *data, uint8_t len, bool isTelemetry)
{
  const uint8_t headerLength = 2; // 3B
  const uint8_t maxMessageSize = 58 - headerLength;

  if (len > maxMessageSize)
    return false;

  waitPacketSent(); // Make sure we dont collide with previous message

  // Set up the headers
  // For char array message: 10xxxxxx , x = length
  _buf[0] = len + headerLength; // Number of octets in teh whole message
  _buf[0] = _buf[0] | 0b10000000;

  if (!isTelemetry)
  {
    _buf[0] = _buf[0] | 0b01000000; // This is message from GPS tracker
  }

  _buf[1] = 0x00; // calcCRC((char *)data, len); // mby for crc?

  // REVISIT: do we really have to do this? perhaps just write it after writing the header?
  memcpy(_buf + headerLength, data, len);

  _s->write(_buf, len + headerLength);
  setMode(RHModeTx);
  _txGood++;
  // Aux will return high when the TX buffer is empty

  return true;
}

uint8_t RH_E32::maxMessageLength()
{
  return RH_E32_MAX_MESSAGE_LEN;
}

bool RH_E32::waitPacketSent()
{
  if (_mode == RHModeTx)
    waitAuxHigh();
  setMode(RHModeRx);
  return true;
}

bool RH_E32::setDataRate(DataRate rate)
{
  Parameters params;
  if (!readParameters(params))
    return false;
  // The DataRate enums are the same values as the register bitmasks
  params.sped &= ~RH_E32_PARAM_SPED_DATARATE_MASK;
  params.sped |= (rate & RH_E32_PARAM_SPED_DATARATE_MASK);
  return writeParameters(params);
}

bool RH_E32::setPower(PowerLevel level)
{
  Parameters params;
  if (!readParameters(params))
    return false;
  // The DataRate enums are the same values as the register bitmasks
  params.option &= ~RH_E32_PARAM_OPTION_POWER_MASK;
  params.option |= (level & RH_E32_PARAM_OPTION_POWER_MASK);
  return writeParameters(params);
}

bool RH_E32::setBaudRate(BaudRate rate, Parity parity)
{
  Parameters params;
  if (!readParameters(params))
    return false;
  // The DataRate enums are the same values as the register bitmasks
  params.sped &= ~RH_E32_PARAM_SPED_UART_BAUD_MASK;
  params.sped |= (rate & RH_E32_PARAM_SPED_UART_BAUD_MASK);

  // Also set the parity
  params.sped &= ~RH_E32_PARAM_SPED_UART_MODE_MASK;
  params.sped |= (parity & RH_E32_PARAM_SPED_UART_MODE_MASK);

  return writeParameters(params);
}

bool RH_E32::setFrequency(uint16_t frequency)
{
  if (frequency < 410 || frequency > 441)
    return false;

  Parameters params;
  if (!readParameters(params))
    return false;
  params.chan = frequency - 410;
  return writeParameters(params);
}

#endif // RH_HAVE_SERIAL
