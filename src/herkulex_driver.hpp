#ifndef HERKULEX_DRIVER_HPP_
#define HERKULEX_DRIVER_HPP_

#include "serial_stream.hpp"
#include "variable_conversion.hpp"

enum LEDColour
{
	kGreen = 0x01,
	kBlue = 0x02,
	kRed = 0x04
};

/** Data structure for programming motor movement using S_JOG
* @param[in] pID id of the motor
* @param[in] pos (1) position of the motor when mode = 0 (0 to 1023). (2) speed of the motor when mode = 1 (0 to 1023)
* @param[in] time operating time to reach position or continue speed (0 to 255 ticks). every tick = 11.2ms
* @param[in] led led colour (green = 0x01, blue = 0x02, red = 0x04)
* @param[in] mode the control mode (position control = 0, continuous rotation = 1)
*/
struct S_JOG_TAG {
	char pID = 0;				//id of the motor
	unsigned short pos = 0;		//[mode = 1] position of the motor (0 to 1023). [mode = ] speed of the motor (0 to 1023)
	char time = 60;				//operating time to reach position or continue speed (0 to 255 ticks). every tick = 11.2ms
	LEDColour led = kGreen;		//led colour (green = 0x01, blue = 0x02, red = 0x04)
	char mode = 0;				//the control mode (position control = 0, continuous rotation = 1)

	S_JOG_TAG::S_JOG_TAG() {}

	S_JOG_TAG::S_JOG_TAG(char pID, unsigned short pos, char time, LEDColour led, char mode)
	{
		set(pID, pos, time, led, mode);
	}

	void S_JOG_TAG::set(char pID, unsigned short pos, char time, LEDColour led, char mode)
	{
		this->pID = pID;
		this->pos = pos;
		this->time = time;
		this->led = led;
		this->mode = mode;
	}
};


/** Uses serial_stream.hpp to send comamnds to herkulex
*
* @note for linux, use LibSerial
* @see http://libserial.sourceforge.net/
*
* This class is written to mimic the SerialStream.h member functions. Hence, the member functions name are all the same, and not following google style guide
*
* basics on usb read write using Win32 API
* http://xanthium.in/Serial-Port-Programming-using-Win32-API
* For using Overlapped (Multithread). i.e. non blocking read/write
* https://www.dreamincode.net/forums/topic/165693-microsoft-working-with-overlapped-io/
*
* Created by:
* @author Er Jie Kai (EJK)
 */
class HerkulexDriver
{
private:
	enum HerkulexCmd
	{
		kEEP_WRITE = 0x01,
		kEEP_READ = 0x02,
		kRAM_WRITE = 0x03,
		kRAM_READ = 0x04,
		kI_JOG = 0x05,
		kS_JOG = 0x06,
		kSTAT = 0x07,
		kROLLBACK = 0x08,
		kREBOOT = 0x09
	};

	int num_motor;
	SerialStream sp;

	VariableConversion varc;

	int getValidComPort(std::string valid_com_name);
	void send(char pID, HerkulexCmd cmd, char* data, char datalen, bool printCommand = false);
	void read(char* buffer);
	void printHexCommand(char* data, char len);

public:
	HerkulexDriver(std::string valid_com_name);
	void setLEDColour(char pID, LEDColour colour);
	void setAcknowledgePolicy(char pID, int policy = 1);
	void setControlMode(char pID, int controlmode = 0); 
	void setTorqueControl(char pID, int mode = 0);

	float getAbsoluteAngle(char pID);
	float getCalibratedAngle(char pID);
	int getError(char pID);

	int clearError(char pID);

	void runMotor(S_JOG_TAG* sjog, char num_sjog);
};

#endif /*HERKULEX_DRIVER_HPP_*/