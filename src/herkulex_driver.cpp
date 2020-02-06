#include "herkulex_driver.hpp"
#include "enumser.h" //find valid comport

/** @brief Connect to the USB serial device and configure the port settings.
*
* This function will search all connected comport for a matching comport name and connect to it.
*
* @param[in] valid_com_port the com port name to match
*
* @return returns nothing
*/
HerkulexDriver::HerkulexDriver(std::string valid_com_name)
{

	int valid_port_num = getValidComPort(valid_com_name);

	printf("Connecting to COM%d\n", valid_port_num);
	sp.Open(std::to_string(valid_port_num).c_str());
	if (!sp.good())
	{
		std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
			<< "Error: Could not open serial port: COM" << std::to_string(valid_port_num)
			<< std::endl;
		exit(1);
	}

	sp.configurePort(BAUD_115200, 8, PARITY_NONE, 1, 0);
}

/** @brief Get all the connected IMU on windows machine
*
* This function is only necessary on windows.\n
* The name of the comport device must match the stated name of "USB Serial Device" to be considered an IMU
*
* @param[out] valid_com_port a pointer to store all the discovered comport
*
* @return returns the number of valid comports discovered
*/
int HerkulexDriver::getValidComPort(std::string valid_com_name)
{
	CEnumerateSerial::CPortAndNamesArray portAndNames;
	int count = 0;
	int valid_com_port = 0;
	if (CEnumerateSerial::UsingSetupAPI1(portAndNames))
	{
		for (const auto& port : portAndNames)
		{
#pragma warning(suppress: 26489)
			//_tprintf(_T("COM%u <%s>\n"), port.first, port.second.c_str());
			if (!strcmp(port.second.c_str(), valid_com_name.c_str())) //strcmp returns 0 when contents of both strings are equal
			{
				valid_com_port = port.first;
				//printf("Valid com port = COM%d\n", valid_com_port[count]);
				count++;
				break; //stop after finding one identical comport name
			}
		}
	}
	else
		_tprintf(_T("CEnumerateSerial::UsingSetupAPI1 failed, Error:%u\n"), GetLastError()); //NOLINT(clang-diagnostic-format)

	return valid_com_port;
}

void HerkulexDriver::send(char pID, HerkulexCmd cmd, char* data, char datalen, bool printCommand)
{
	char packetsize = 7 + datalen;
	char* command = new char[packetsize];

	//checksum 1
	char checksum1 = packetsize ^ pID ^ cmd;
	for (int i = 0; i < datalen; i++)
		checksum1 = checksum1 ^ data[i];
	checksum1 = checksum1 & 0xFE;

	//checksum 2
	char checksum2 = ~checksum1 & 0xFE;

	command[0] = 0xFF;
	command[1] = 0xFF;
	command[2] = packetsize;
	command[3] = pID;
	command[4] = cmd;
	command[5] = checksum1;
	command[6] = checksum2;
	for (int i = 0; i < datalen; i++)
		command[7 + i] = data[i];
	
	if(printCommand ==  true)
		printHexCommand(command, packetsize);

	sp.write(command, packetsize);

	delete[] command;
}

void HerkulexDriver::read(char* buffer)
{
	sp.read(buffer);
}

void HerkulexDriver::printHexCommand(char* data, char len)
{
	printf("send command: ");
	for (int i = 0; i < len; i++)
	{
		unsigned char temp = data[i];
		if (temp < 16)
			printf("0%x ", temp);
		else
			printf("%x ", temp);
	}
	printf("\n");
}

void HerkulexDriver::setLEDColour(char pID, LEDColour colour)
{
	char data[] = { 53, 1, colour };
	send(pID, kRAM_WRITE, data, 3);
}

/** @brief set the acknowledge policy
*
* Sets ACK Packet reply policy(1) when Request Packet is received. \n
* ACK Policy(RAM Register Address 1)\n
* 0 : No reply to any Request Packet\n
* 1 : Only reply to Read CMD\n (default)
* 2 : Reply to all Request Packet\n
*
* @param[in] pID id of the motor
* @param[in] policy the acknowledge policy
*
* @return return nothing
*/
void HerkulexDriver::setAcknowledgePolicy(char pID, int policy)
{
	char data[] = { 1, 1, policy };
	send(pID, kRAM_WRITE, data, 3);
}

/** @brief set the control mode
*
* Sets control mode (56): \n
* 0 : Position control (default)
* 1 : Turn/Velocity Control(Continuous rotation)
*
* @note Torque control must be set to "Torque ON" (0x60) for motor to move
*
* @param[in] pID id of the motor
* @param[in] controlmode the control mode
*
* @return return nothing
*/
void HerkulexDriver::setControlMode(char pID, int controlmode)
{
	char data[] = { 56, 1, controlmode };
	send(pID, kRAM_WRITE, data, 3);
}

/** @brief set the Torque Control
*
* Sets torque control (52): \n
* 0 (0x00) : Torque free
* 1 (0x40) : Brake on
* 2 (0x60) : Torque On
*
* @param[in] pID id of the motor
* @param[in] controlmode the control mode
*
* @return return nothing
*/
void HerkulexDriver::setTorqueControl(char pID, int mode)
{
	char torque_control_mode = 0;
	if (mode == 0)
		torque_control_mode = 0x00;
	else if (mode == 1)
		torque_control_mode = 0x40;
	else if (mode == 2)
		torque_control_mode = 0x60;

	char data[] = { 52, 1, torque_control_mode };
	send(pID, kRAM_WRITE, data, 3);
}


/** gets the absolute angle
* Absolute position Raw Data (60)
* Angle = r(Absolute Position) X 0.325
*
* @param[in] pID id of the motor
*
* @return return the absolute angle of the motor
*/
float HerkulexDriver::getAbsoluteAngle(char pID)
{
	const int return_bytes = 2;
	char data[] = { 60, return_bytes };
	send(pID, kRAM_READ, data, 2);
	char buffer[11 + return_bytes];//2 for header, 1 for packet size, 1 for pID, 1 for cmd, 2 for checksum, 1 for add, 1 for length, return_bytes for data, 1 for status error. 1 for status detail
	read(buffer);
	//printf("Return Buffer: ");
	//for (int i = 9; i < 11 + return_bytes; i++)
	//	printf("%x ", (unsigned char)buffer[i]);
	//printf("\n");
	unsigned short absolute_position = varc.Char2Short(buffer + 9);
	//printf("Angle = %hd\n", absolute_position);
	float absolute_angle = absolute_position * 0.325;
	return absolute_angle;
}

/** gets the calibrated angle
* Calibrated position Data (58)
*
* @param[in] pID id of the motor
*
* @return return the calibrated angle of the motor
*/
float HerkulexDriver::getCalibratedAngle(char pID)
{
	const int return_bytes = 2;
	char data[] = { 58, return_bytes };
	send(pID, kRAM_READ, data, 2);
	char buffer[11 + return_bytes] = { 0 };//2 for header, 1 for packet size, 1 for pID, 1 for cmd, 2 for checksum, 1 for add, 1 for length, return_bytes for data, 1 for status error. 1 for status detail
	read(buffer);
	//printf("Return Buffer: ");
	//for (int i = 9; i < 11 + return_bytes; i++)
	//	printf("%x ", (unsigned char)buffer[i]);
	//printf("\n");
	unsigned short calibrated_position = varc.Char2Short(buffer + 9);
	calibrated_position = calibrated_position & 0b000001111111111;
	//float absolute_angle = absolute_position * 0.325;
	return calibrated_position;
}

/** gets the status error and status detail
* 
* Status Error:
* Bit Value Comment
* 0   0X01  Exceed Input Voltage limit
* 1   0X02  Exceed allowed POT limit
* 2   0X04  Exceed Temperature limit
* 3   0X08  Invalid Packet
* 4   0X10  Overload detected
* 5   0X20  Driver fault detected
* 6   0X40  EEP REG distorted
* 7   0X80  reserved
*
* Status Detail:
* Bit Value Comment
* 0   0X01  Moving flag
* 1   0X02  Inposition flag
* 2   0X04  Checksum Error
* 3   0X08  Unknown Command
* 4   0X10  Exceed REG range
* 5   0X20  Garbage detected
* 6   0X40  MOTOR_ON flag
* 7   0X80  reserved
*
* @param[in] pID id of the motor
*
* return integer of combined status_error<<8 & status_detail
*/
int HerkulexDriver::getError(char pID)
{
	send(pID, kSTAT, NULL, 0);
	char buffer[9];//2 for header, 1 for packet size, 1 for pID, 1 for cmd, 2 for checksum, 1 for status error. 1 for status detail
	read(buffer);

	char status_error = buffer[7];
	if (status_error % 10 == 0x01)
		printf("pID[%d] Status Error[%u]: Exceed input voltage limit\t", pID, status_error);
	if (status_error % 10 == 0x02)
		printf("pID[%d] Status Error[%u]: Exceed allowed potentiometer limit. (Angle exceeds safety threshold)\t", pID, status_error);
	if (status_error % 10 == 0x04)
		printf("pID[%d] Status Error[%u]: Exceed temperature limit\t", pID, status_error);
	if (status_error % 10 == 0x08)
		printf("pID[%d] Status Error[%u]: Invalid Packet\t", pID, status_error);
	if (floor(status_error / 10.0) * 10 == 0x10)
		printf("pID[%d] Status Error[%u]: Overload detected\t", pID, status_error);
	if (floor(status_error / 10.0) * 10 == 0x20)
		printf("pID[%d] Status Error[%u]: Driver fault detected\t", pID, status_error);
	if (floor(status_error / 10.0) * 10 == 0x40)
		printf("pID[%d] Status Error[%u]: EEP register distorted\t", pID, status_error);
	if (floor(status_error / 10.0) * 10 == 0x80)
		printf("pID[%d] Status Error[%u]: Reserved. (not used for error code)\t", pID, status_error);

	char status_detail = buffer[8];
	if (status_detail % 10 == 0x01)
		printf("Status Detail[%u]: Moving flag\t", status_detail);
	if (status_detail % 10 == 0x02)
		printf("Status Detail[%u]: Inposition flag\t", status_detail);
	if (status_detail % 10 == 0x04)
		printf("Status Detail[%u]: Checksum error\t", status_detail);
	if (status_detail % 10 == 0x08)
		printf("Status Detail[%u]: Unkown command\t", status_detail);
	if (floor(status_detail / 10.0) * 10 == 0x10)
		printf("Status Detail[%u]: Exceed register range\t", status_detail);
	if (floor(status_detail / 10.0) * 10 == 0x20)
		printf("Status Detail[%u]: Garbage detected\t", status_detail);
	if (floor(status_detail / 10.0) * 10 == 0x40)
		printf("Status Detail[%u]: MOTOR_ON flag\t", status_detail);
	if (floor(status_detail / 10.0) * 10 == 0x80)
		printf("Status Detail[%u]: Reserved. (not used for error code)\t", status_detail);

	int error = status_error << 8 + status_detail;
	//printf("pid[%d] error code = %d %u %u ", pID, error, status_error, status_detail);
	if (error>0)
	{
		printf("Error[%d] = %d\n", pID, error);
		printf("Clear error? (y/n)");
		char ch;
		std::cin >> ch;
		if (ch == 'y')
			clearError(pID);
	}
	return error;
}

/** clear the status error and status detail
*
* @param[in] pID id of the motor
*
* return returns nothing
*/
int HerkulexDriver::clearError(char pID)
{
	char data[] = { 0x30, 2, 0,0 };// set register 0x30 & 0x31 (status error and status detail) to 0x00 and 0x00
	send(pID, kRAM_WRITE, data, 4);

	return getError(pID);
}

/** runs the Motor using S_JOG protocol
*
* @note S_JOG is suppose to allow multiple motors to be commanded simultaneously. However, only the specied motor via data[3] (pID element) will move. Hence each motor must be commanded individually now.
*
* @param[in] *sjog array of S_JOG_TAG structure
* @param[in] num_sjog number of sjog elements in sjog array
*
* return returns nothing
*/
void HerkulexDriver::runMotor(S_JOG_TAG* sjog, char num_sjog)
{

	char* data = new char[4 * num_sjog + 1];
	data[0] = sjog[0].time;
	for (int i = 0; i < num_sjog; i++)
	{
		char pos[2];
		varc.Short2Char(sjog[i].pos, pos);
		data[i * 4 + 1] = pos[1];
		data[i * 4 + 2] = pos[0];
		data[i * 4 + 3] = (sjog[0].mode << 1) + (sjog[0].led << 2);
		data[i * 4 + 4] = sjog[i].pID;
	}

	send(sjog[0].pID, kS_JOG, data, 4 * num_sjog + 1, true);

	delete[] data;
}
