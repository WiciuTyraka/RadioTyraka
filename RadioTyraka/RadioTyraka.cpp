#include <RH_E32.h>
#include <RadioTyraka.h>

RadioTyraka::RadioTyraka(Stream *s, uint8_t m0_pin, uint8_t m1_pin, uint8_t aux_pin, uint8_t ID)
    : RH_E32{s, m0_pin, m1_pin, aux_pin},
      _device_id(ID)
{
}

LoRaSinglePacket *RadioTyraka::init_stack(int m)
{
  LoRaSinglePacket *st = (LoRaSinglePacket *)malloc(sizeof(LoRaSinglePacket) + m * sizeof(uint8_t));
  return st;
}

/*
--------------------------------------------------------------------------
------------------            Transmiter              --------------------
------------------  methodes used to sending message  --------------------
--------------------------------------------------------------------------
*/

Status RadioTyraka::sendString(const uint8_t *message, const uint8_t size, const Flags *flags)
{
  Status status = L_SUCCESS;
  //That was used for dynamic memory allocation
  LoRaSinglePacket *LoRaPacket = init_stack(size);

  LoRaPacket->MSG_LEN = size + RH_E32_HEADER_LEN;
  LoRaPacket->ID_RX = 0xff; //must be 0xff
  LoRaPacket->ID_TX = _device_id;
  LoRaPacket->FLAGS = packFlagsToByte(flags);
  LoRaPacket->FLAGS &= ~(1 << 7); // set last bit to 0 cause its string message

  memcpy(LoRaPacket->message, message, size);

  status = tyrakaSend((uint8_t *)LoRaPacket, size + RH_E32_HEADER_LEN);

  //That was used for dynamic memory allocation
  free(LoRaPacket);

  return status;
}

Status RadioTyraka::sendStruct(const void *message, const uint8_t size, const Flags *flags)
{
  Status status = L_SUCCESS;
  //That was used for dynamic memory allocation
  LoRaSinglePacket *LoRaPacket = init_stack(size);

  LoRaPacket->MSG_LEN = size + RH_E32_HEADER_LEN;
  LoRaPacket->ID_RX = 0xff; //must be 0xff
  LoRaPacket->ID_TX = _device_id;
  LoRaPacket->FLAGS = packFlagsToByte(flags);
  LoRaPacket->FLAGS |= 1 << 7; // set last bit to 1 cause its struct message

  memcpy(LoRaPacket->message, (unsigned char *)message, size);

  status = tyrakaSend((uint8_t *)LoRaPacket, size + RH_E32_HEADER_LEN);

  //That was used for dynamic memory allocation
  free(LoRaPacket);

  return status;
}

uint8_t RadioTyraka::packFlagsToByte(const Flags *struct_flags)
{
  uint8_t byte_flags = 0;

  if (struct_flags != NULL)
  {
    //set first bit of byte_flag
    byte_flags |= struct_flags->FLAG1 << 0;

    //set second bit of byte_flag
    byte_flags |= struct_flags->FLAG2 << 1;

    //set 3th bit of byte_flag
    byte_flags |= struct_flags->FLAG3 << 2;

    //set 4th bit of byte_flag
    byte_flags |= struct_flags->FLAG4 << 3;

    //set 5-7th bit of byte_flag
    byte_flags |= struct_flags->MISSION_STATE << 4;
  }

  return byte_flags;
}

Status RadioTyraka::tyrakaSend(const uint8_t *packet, uint8_t size_)
{
  if (size_ > RH_E32_MAX_PAYLOAD_LEN)
    return ERR_PACKET_TOO_BIG;

  Status result = L_SUCCESS;

  waitPacketSent(); // Make sure we dont collide with previous message

  uint8_t len = _s->write(packet, size_);

  if (len != size_)
  {
    if (len == 0)
    {
      result = ERR_NO_RESPONSE_FROM_DEVICE;
    }
    else
    {
      result = ERR_DATA_SIZE_NOT_MATCH;
    }
  }

  setMode(RHModeTx);
  // Aux will return high when the TX buffer is empty

  return result;
}

/*
--------------------------------------------------------------------------
------------------             Reseiver               --------------------
------------------  methodes used to reseive message  --------------------
--------------------------------------------------------------------------
// */
PacketInfo RadioTyraka::receiveMessage(void *packet, uint8_t *size_)
{
  Status status = tyrakaReceive();

  if (packet && size_)
  {
    // Skip the 4 headers that are at the beginning of the rxBuf
    if (*size_ > _bufLen - RH_E32_HEADER_LEN)
      *size_ = _bufLen - RH_E32_HEADER_LEN;

    memcpy(packet, _buf + RH_E32_HEADER_LEN, *size_);
  }

  PacketInfo info;
  info.status = status;
  info.MSG_LEN = _buf[0];
  info.ID_TX = _buf[2];
  info.FLAGS = unpackFlagsFromByte(_buf[3]);
  info.MSG_TYPE |= _buf[3] >> 7;

  clearRxBuf();

  return info;
}

Flags RadioTyraka::unpackFlagsFromByte(const uint8_t byte_flags)
{
  Flags struct_flags;

  //set FLAG1
  struct_flags.FLAG1 = byte_flags & 1;

  //set FLAG2
  struct_flags.FLAG2 = byte_flags & 1 << 1;

  //set FLAG3
  struct_flags.FLAG3 = byte_flags & 1 << 2;

  //set FLAG4
  struct_flags.FLAG4 = byte_flags & 1 << 3;

  //set MISSION_STATE - 3bit
  struct_flags.MISSION_STATE = (FlightState)((byte_flags >> 4) & 7);

  return struct_flags;
}

Status RadioTyraka::tyrakaReceive()
{
  if (!_rxBufValid)
  {
    if (_mode == RHModeTx)
      return ERR_MODE_TX_ACTIVE;

    if (!_s->available())
      return ERR_NO_RESPONSE_FROM_DEVICE;

    //-------------------------------------------------------------------
    //  Moja wersja wrazie problemow zakomentowac i odkomentowac klasyczna
    uint8_t data;
    _s->readBytes((char *)&data, 1);
    if (data <= RH_E32_MAX_MESSAGE_LEN && data > 0)
    {
      _buf[_bufLen++] = data;

      while (_bufLen < _buf[0])
      {
        if (_s->readBytes((char *)&data, 1))
          _buf[_bufLen++] = data;
      }
    }
    else
    {
      while (_s->readBytes((char *)&data, 1) == 1) // Not read timeout (suck all bites)
      {
        _buf[_bufLen++] = data;
      }
    }
    // TODO
    // hendling for second case from if - STATUS VALUE
    //---------------------------------------------------------------------
    // Klasyczna wersja
    // uint8_t data;
    // while (_s->readBytes((char *)&data, 1) == 1) // Not read timeout
    // {
    //   _buf[_bufLen++] = data;
    // }
    //----------------------------------------------------------------------
    if (_bufLen < RH_E32_HEADER_LEN)
    {
      return ERR_INCOMPLETE_HEADER;
    }
    else if (_bufLen < _buf[0])
    {
      return ERR_DATA_SIZE_NOT_MATCH;
    }
    else if (_bufLen > _buf[0] || _bufLen > RH_E32_MAX_PAYLOAD_LEN)
    {
      clearRxBuf();
      return ERR_PACKET_TOO_BIG;
    }

    validateRxBuf();
  }

  return L_SUCCESS;
}

/*
--------------------------------------------------------------------------
------------------         Seters and geters       -----------------------
------------------  methodes used to set and get   -----------------------
--------------------------------------------------------------------------
 */

void RadioTyraka::setDeviceID(uint8_t ID)
{
  _device_id = ID;
}

uint8_t RadioTyraka::getDeviceID()
{
  return _device_id;
}

// bool RadioTyraka::setFlagsByte(uint8_t flag) // set 2-8 bit in flags
// {
//   if (flag > 127)
//   {
//     return false;
//   }
//   else
//   {
//     _flags |= flag;
//     return true;
//   }
// }

// void RadioTyraka::setFlags2Bit(bool _2bit) // set second bit in flags
// {
//   _flags |= _2bit << 6;
// }
// void RadioTyraka::setFlags3Bit(bool _3bit) // set 3th bit in flags
// {
//   _flags |= _3bit << 5;
// }
// void RadioTyraka::setFlags4Bit(bool _4bit) // set 4th bit in flags
// {
//   _flags |= _4bit << 4;
// }
// void RadioTyraka::setFlags5Bit(bool _5bit) // set 5th bit in flags
// {
//   _flags |= _5bit << 3;
// }
// void RadioTyraka::setFlags6Bit(bool _6bit) // set 6th bit in flags
// {
//   _flags |= _6bit << 2;
// }
// void RadioTyraka::setFlags7Bit(bool _7bit) // set 7th bit in flags
// {
//   _flags |= _7bit << 1;
// }
// void RadioTyraka::setFlags8Bit(bool _8bit) // set 8th bit in flags
// {
//   _flags |= _8bit << 0;
// }

// uint8_t RadioTyraka::getFlagsByte()
// {
//   return _flags;
// }