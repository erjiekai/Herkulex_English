#include "serial_stream.hpp"

SerialStream::SerialStream(bool use_overlapped)
{
	SerialStreamHandle = INVALID_HANDLE_VALUE;

	this->use_overlapped = use_overlapped;

	memset(&dcb, 0, sizeof(dcb));

	dcb.DCBlength = sizeof(dcb);
	dcb.BaudRate = CBR_115200; //CBR_115200
	dcb.Parity = NOPARITY;
	dcb.fParity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.ByteSize = 8;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
}

SerialStream::~SerialStream()
{
	if (SerialStreamHandle != INVALID_HANDLE_VALUE)
		CloseHandle(SerialStreamHandle);

	SerialStreamHandle = INVALID_HANDLE_VALUE;
}

void SerialStream::configurePort(int baudrate, int charsize, int parity, int stopbit, int flowcontrol)
{
	dcb.BaudRate = baudrate; //CBR_115200
	dcb.Parity = parity;
	dcb.fParity = parity;
	dcb.StopBits = stopbit;
	dcb.ByteSize = charsize;
	dcb.fDtrControl = flowcontrol;
}

/** @brief Opens a COM port in windows
*
* @param[in] device the name of the port (eg: COM1 or COM24)
*
* @return returns flag code [0 = everything successful, 1 = INVALID_HANDLE_VALUE, 2 = SetCommState failed ]
*/
int SerialStream::Open(const char* device)
{
	int flag = 0;

	strcpy(comport, "\\\\.\\COM");
	strcat(comport, device);


	if (use_overlapped == true)
	{
		SerialStreamHandle = CreateFile(comport,
			GENERIC_READ | GENERIC_WRITE, // access ( read and write)
			0,                            // (share) 0:cannot share the COM port
			0,                            // security  (None)
			OPEN_EXISTING,                // creation : open_existing
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,         // FILE_ATTRIBUTE_NORMAL use "FILE_FLAG_OVERLAPPED" for overlapped operation (event trigger/callback)
			0                             // no templates file for COM port...
		);
	}
	else
	{
		SerialStreamHandle = CreateFile(comport,
			GENERIC_READ | GENERIC_WRITE,  // access ( read and write)
			0, // (share) 0:cannot share the COM port
			0,                           // security  (None)
			OPEN_EXISTING,               // creation : open_existing
			0,        // use "FILE_FLAG_OVERLAPPED" for overlapped operation (event trigger/callback)
			NULL                            // no templates file for COM port...
		);
	}

	// if handle successfully created, configure port, create event handle and get device number
	if (SerialStreamHandle != INVALID_HANDLE_VALUE)
	{
		if (!SetCommState(SerialStreamHandle, &dcb)) //configure the port comm protocol according to dcb (dcb configured in constructor)
			flag = 2; // set flag to 2 if SetCommState returns a false (0)

		SetCommMask(SerialStreamHandle, EV_DSR); // create event handle. EV_RXCHAR
		if (use_overlapped == true)
		{
			memset(&ov, 0, sizeof(ov));
			ov.hEvent = CreateEvent(
				NULL,  // default security attributes
				TRUE,  // manual-reset event
				FALSE, // not signaled
				NULL   // no name
			);
			// Initialize the rest of the OVERLAPPED structure to zero.
			ov.Internal = 0;
			ov.InternalHigh = 0;
			ov.Offset = 0;
			ov.OffsetHigh = 0;

			if (ov.hEvent == NULL)
				printf("flag creating overlapped event! abort now");
			//        assert(ov.hEvent);
		}
	}
	else
	{
		flag = 1;
	}

	if (flag != 0)
	{
		Close();
	}
	else
	{
		clear();
	}

	return flag;
}

void SerialStream::Close(void)
{
	CloseHandle(SerialStreamHandle);
	SerialStreamHandle = INVALID_HANDLE_VALUE;

	//printf("Port 1 has been CLOSED and %d is the file descriptionn", fileDescriptor);
}

bool SerialStream::good()
{
	//return WaitNamedPipe(comport, 1); //wait for max 1ms 
	if (SerialStreamHandle != INVALID_HANDLE_VALUE)
		return true;
	else
		return false;
}

void SerialStream::write(char* buffer, int len)
{
	if (use_overlapped == true)
	{
		//Also Block other operations till writefile has finished, but can be rewrote to allow multithreading

		for (int i = 0; i < len; i++) {
			DWORD error_cc = WriteFile(SerialStreamHandle, buffer + i, 1, NULL, &(ov));
			if (error_cc == FALSE && GetLastError() != ERROR_IO_PENDING)
			{
				printf("Cannot write to USB\n");
				i--;
			}
		}
	}
	else
	{
		//Block other operations till writefile has finished
		unsigned long result;

		if (SerialStreamHandle != INVALID_HANDLE_VALUE)
			WriteFile(SerialStreamHandle, buffer, len, &result, NULL);
		//printf("Written %d bytes\n", result);
		//return result;
	}
}

int SerialStream::read(char* buffer)
{
	if (use_overlapped == true)
	{

		unsigned long nbr = 0; //number of bytes that is read out

		//for (int i = 0; i < len; i++)//clear the output buffer
		//	buffer[i] = 0;

		BOOL fWaitingOnRead = FALSE;

		if (!ReadFile(SerialStreamHandle, buffer, 1, &nbr, &(ov))) //try to read one byte and fail
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				printf("IO Pending");
				fWaitingOnRead = TRUE;
			}
			else
			{
				printf("Error in WaitCommEvent, abort");
			}
		}
		else //success reading one byte. can continue reading the rest of the buffer
		{
			/// code to read out 400 bytes at one go. This will stop the program until 400 bytes have been read. not useful.
			// need to start if else with "if (!WaitCommEvent (sp[0].SerialStreamHandle, &event_flag, &(sp[0].ov)))"
			//                printf("WaitCommEvent returned immediatly\n");
			//                unsigned long nbr = 0; //number of bytes read
			//                ReadFile(sp[0].SerialStreamHandle, output, 400, &nbr, &(sp[0].ov));

			/// code to read the rest of the buffer after 1 byte has been read
			DWORD errorcode = 0;
			COMSTAT mycomstat;
			ClearCommError(SerialStreamHandle, &errorcode, &mycomstat); //use to obtain the number of bytes left over in the buffer
			if (mycomstat.cbInQue > 0)
			{
				ReadFile(SerialStreamHandle, buffer + 1, mycomstat.cbInQue, &nbr, &(ov));
				std::string str(buffer); //convert the char array to string
				//printf("bytes: %d output: %s \n", nbr + 1, str.c_str());
			}
			else
			{
				PurgeComm(SerialStreamHandle, PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR);
			}

			///this code tries to read the buffer until no bytes is left. Doesnt work properly with overlapped I/O
			//                int counter = 0;
			//                unsigned long nbr = 0; //number of bytes read
			//                char curr=0;
			//                do
			//                {
			//                    ReadFile(sp[0].SerialStreamHandle, &curr, 1, &nbr, &(sp[0].ov));
			//                    if (nbr == 1)
			//                    {
			//                        output[counter]=curr;
			//                        counter++;
			//                    }
			//
			//                }while(nbr==1);
			//                std::string str(output);
			//                printf("bytes: %d output: %s",counter,str.c_str());
			//
		}
		//printf("\n");


		return ((int)nbr);
	}
	else
	{
		unsigned long nbr = 0; //number of bytes that is read out

		//for (int i = 0; i < len; i++)//clear the output buffer
		//	buffer[i] = 0;

		ReadFile(SerialStreamHandle, buffer, 1, &nbr, NULL);
		if (nbr > 0)
		{
			DWORD errorcode = 0;
			COMSTAT mycomstat;
			ClearCommError(SerialStreamHandle, &errorcode, &mycomstat); //use to obtain the number of bytes left over in the buffer
			if (mycomstat.cbInQue > 0)
			{
				if (SerialStreamHandle != INVALID_HANDLE_VALUE)
				{
					ReadFile(SerialStreamHandle, buffer + 1, mycomstat.cbInQue, &nbr, NULL);
				}
				//std::string str(buffer); //convert the char array to string
				//printf("bytes: %d output: %s\n", nbr + 1, str.c_str());
			}
		}
		return ((int)nbr);
	}
}

void SerialStream::read(char* buffer, int len)
{
	unsigned long nbr = 0; //number of bytes that is read out
	ReadFile(SerialStreamHandle, buffer, len, &nbr, NULL);
}

int SerialStream::get(char& buffer)
{
	unsigned long nbr = 0; //number of bytes that is read out
	ReadFile(SerialStreamHandle, &buffer, 1, &nbr, NULL);
	return nbr;
}

void SerialStream::clear()
{
	PurgeComm(SerialStreamHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
}


/*
// main used for testing
void main()
{
	int numUSB = 1;
	SerialStream* serial_port;
	serial_port = new SerialStream[numUSB];


	for (int i = 0; i < numUSB; i++)
	{
		std::string comport = "/dev/ttyACM";
		comport = comport + std::to_string(i);

		printf("Connecting to %s\n", comport.c_str());
		serial_port[i].Open(comport.c_str());
		if (!serial_port[i].good())
		{
			std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
				<< "Error: Could not open serial port: " << comport
				<< std::endl;
			exit(1);
		}

	}

	for (int i = 0; i < numUSB; i++)
		serial_port[i].configurePort(BAUD_115200, 8, PARITY_NONE, 1, 0);

	// Write
	const int num_commd = 4;
	char output[num_commd];
	output[0] = 0xf7;
	output[1] = 0x7b;
	serial_port[0].write(output, num_commd);

	//Read
	int datasize = 44; //example
	char* data = new char[datasize]; // add 1 for button state
	for (int i = 0; i < numUSB; i++)
		serial_port[i].read(data, datasize); // all read data will be put into "data"
}

*/