#ifndef statesNaming_h
#define statesNaming_h

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

/* Enum for representig state of flight.
   This state is changing during flight*/
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

static String getStatusDescription(Status status)
{
	switch (status)
	{
	case L_SUCCESS:
		return F("L_Success");
		break;
	case ERR_UNKNOWN:
		return F("Unknown");
		break;
	case ERR_NOT_SUPPORT:
		return F("Not support!");
		break;
	case ERR_NOT_IMPLEMENT:
		return F("Not implement");
		break;
	case ERR_NOT_INITIAL:
		return F("Not initial!");
		break;
	case ERR_INVALID_PARAM:
		return F("Invalid param!");
		break;
	case ERR_DATA_SIZE_NOT_MATCH:
		return F("Data size not match!");
		break;
	case ERR_BUF_TOO_SMALL:
		return F("Buff too small!");
		break;
	case ERR_TIMEOUT:
		return F("Timeout!!");
		break;
	case ERR_HARDWARE:
		return F("Hardware error!");
		break;
	case ERR_HEAD_NOT_RECOGNIZED:
		return F("Save mode returned not recognized!");
		break;
	case ERR_NO_RESPONSE_FROM_DEVICE:
		return F("No response from device! (Check wiring)");
		break;
	case ERR_WRONG_UART_CONFIG:
		return F("Wrong UART configuration! (BPS must be 9600 for configuration)");
		break;
	case ERR_PACKET_TOO_BIG:
		return F("The device support only 58byte of data transmission! Packet is to big");
		break;
	case ERR_MODE_TX_ACTIVE:
		return F("Device in TX mode!");
		break;
	case ERR_INCOMPLETE_HEADER:
		return F("header incomplate!");
		break;
	default:
		return F("Invalid status!");
	}
}

static String getflightStateDescription(FlightState flightState)
{
	switch (flightState)
	{
	case LAUNCHPAD:
		return F("Rocket on the Launchpad");
		break;
	case PROPELLED_FLIGHT:
		return F("PROPELLED_FLIGHT");
		break;
	case COAST:
		return F(" COAST!");
		break;
	case DROGUE_DEPLOYMENT:
		return F("DROGUE_DEPLOYMENT");
		break;
	case DROGUE_DESCENT:
		return F("DROGUE_DESCENT!");
		break;
	case MAIN_DEPLOYMENT:
		return F("MAIN_DEPLOYMENT");
		break;
	case MAIN_DESCENT:
		return F("MAIN_DESCENT!");
		break;
	default:
		return F("Invalid status!");
	}
}

#endif