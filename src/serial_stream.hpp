#ifndef SERIAL_STREAM_HPP_
#define SERIAL_STREAM_HPP_

#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <string>
#include <ctime> // for timer

/** Reads data from a USB port for WINDOWS
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
class SerialStream
{
private:
	DCB dcb;
	char comport[15];
	OVERLAPPED ov;
	bool use_overlapped = false;

public:
	HANDLE SerialStreamHandle;

	SerialStream(bool use_overlapped = false);
	~SerialStream();

	int Open(const char* device);
	void Close(void);

	void write(char* buffer, int len); //uses overlapped 
	int read(char* buffer); //uses overlapped
	void read(char* buffer, int len);
	int get(char& buffer); //uses overlapped
	void configurePort(int baudrate, int charsize, int parity, int stopbit, int flowcontrol);
	bool good();
	void clear();
};

#endif /*SERIAL_STREAM_HPP_*/