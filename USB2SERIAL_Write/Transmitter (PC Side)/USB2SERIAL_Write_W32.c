
	//====================================================================================================//
	// Serial Port Programming using Win32 API in C                                                       //
	// (Writes data to serial port)                                                                      //
	//====================================================================================================//
	
	//====================================================================================================//
	// www.xanthium.in										                                              //
	// Copyright (C) 2014 Rahul.S                                                                         //
	//====================================================================================================//
	
	//====================================================================================================//
	// The Program runs on the PC side and uses Win32 API to communicate with the serial port or          //
	// USB2SERIAL board and writes the data to it.                                                       //    
	//----------------------------------------------------------------------------------------------------//
	// Program runs on the PC side (Windows) and transmits a single character.                            //            
	// Program uses CreateFile() function to open a connection serial port(COMxx).                        //
	// Program then sets the parameters of Serial Comm like Baudrate,Parity,Stop bits in the DCB struct.  //
	// After setting the Time outs,the Program writes a character to COMxx using WriteFile().             //
    //----------------------------------------------------------------------------------------------------// 
	// BaudRate     -> 9600                                                                               //
	// Data formt   -> 8 databits,No parity,1 Stop bit (8N1)                                              //
	// Flow Control -> None                                                                               //
	//====================================================================================================//

	
	//====================================================================================================//
	// Compiler/IDE  :	Microsoft Visual Studio Express 2013 for Windows Desktop(Version 12.0)            //
	//               :  gcc 4.8.1 (MinGW)                                                                 //
	// Library       :  Win32 API,windows.h,                                                              //
	// Commands      :  gcc -o USB2SERIAL_Write_W32 USB2SERIAL_Write_W32.c                                //
	// OS            :	Windows(Windows 7)                                                                //
	// Programmer    :	Rahul.S                                                                           //
	// Date	         :	30-November-2014                                                                  //
	//====================================================================================================//

	//====================================================================================================//
	// Sellecting the COM port Number                                                                     //
    //----------------------------------------------------------------------------------------------------//
    // Use "Device Manager" in Windows to find out the COM Port number allotted to USB2SERIAL converter-  // 
    // -in your Computer and substitute in the  "ComPortName[]" array.                                    //
	//                                                                                                    //
	// for eg:-                                                                                           //
	// If your COM port number is COM32 in device manager(will change according to system)                //
	// then                                                                                               //
	//			char   ComPortName[] = "\\\\.\\COM32";                                                    //
	//====================================================================================================//

	#include <Windows.h>
	#include <stdio.h>
	#include <string.h>
	#include <time.h>
 
	void sleep(unsigned int mseconds)
	{
	    clock_t goal = mseconds + clock();
	    while (goal > clock());
	}

	void main(void)
	{
		
		HANDLE hComm;                          // Handle to the Serial port
		char   ComPortName[] = "\\\\.\\COM4"; // Name of the Serial port(May Change) to be opened,
		BOOL   Status;
		
		printf("\n\n +==========================================+");
		printf("\n |  Serial Transmission (Win32 API)         |");
		printf("\n +==========================================+\n");
		/*----------------------------------- Opening the Serial Port --------------------------------------------*/
			int port, baudrate;
			printf("\n Please enter Com port number: ");
			scanf("%d", &port);
			if(port > 256) {
				printf("\n No such port!");
				return;
			}
			sprintf(ComPortName,"\\\\.\\COM%d", port);
			printf("\n Please enter BaudRate: ");
			scanf("%d", &baudrate);

		hComm = CreateFile( ComPortName,                       // Name of the Port to be Opened
							GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
							0,                                 // No Sharing, ports cant be shared
							NULL,                              // No Security
							OPEN_EXISTING,                     // Open existing port only
							0,                                 // Non Overlapped I/O
							NULL);                             // Null for Comm Devices

		if (hComm == INVALID_HANDLE_VALUE)
			printf("\n   Error! - Port %s can't be opened", ComPortName);
		else 
			printf("\n   Port %s Opened\n ", ComPortName);

		/*----------------------------------- Opening log file --------------------------------------------------*/
		HANDLE hFile;
		char filename[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.xxx";
		printf("\n Please enter log filename to be read:");
		scanf("%s", &filename);
		hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile != INVALID_HANDLE_VALUE) {
			printf ("\r\n   Read file: %s success!", filename);
		} else {
			printf("open log file fail!!");
			getchar();
			return;
		}

		/*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/

		DCB dcbSerialParams = { 0 };                        // Initializing DCB structure
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		
		Status = GetCommState(hComm, &dcbSerialParams);     //retreives  the current settings

		if (Status == FALSE)
			printf("\n   Error! in GetCommState()");

		dcbSerialParams.BaudRate = baudrate;      // Setting BaudRate = 9600
		dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
		dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
		dcbSerialParams.Parity   = NOPARITY;      // Setting Parity = None 

		Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

		if (Status == FALSE)
			{
				printf("\n   Error! in Setting DCB Structure");
			}
		else
			{
				printf("\n   Setting DCB Structure Successfull\n");
				printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
				printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
				printf("\n       StopBits = %d", dcbSerialParams.StopBits);
				printf("\n       Parity   = %d", dcbSerialParams.Parity);
			}
		
		/*------------------------------------ Setting Timeouts --------------------------------------------------*/
		
		COMMTIMEOUTS timeouts = { 0 };

		timeouts.ReadIntervalTimeout         = 50;
		timeouts.ReadTotalTimeoutConstant    = 50;
		timeouts.ReadTotalTimeoutMultiplier  = 10;
		timeouts.WriteTotalTimeoutConstant   = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;

		if (SetCommTimeouts(hComm, &timeouts) == FALSE)
			printf("\n   Error! in Setting Time Outs");
		else
			printf("\n\n   Setting Serial Port Timeouts Successfull");

		/*----------------------------- Reading  Data from log file----------------------------------------*/
		int datarate = 128;
		int DataSent = 0;
		char Databuf[datarate];
		DWORD dwRead;
		DWORD dwFileSize;
		dwFileSize = GetFileSize(hFile, NULL);
		printf("\r\n file size: %d byte",dwFileSize);
	while(1) {
		sleep(100);

		DataSent += datarate;


		ReadFile(hFile, Databuf, datarate, &dwRead, NULL);

		/*----------------------------- Writing a Character to Serial Port----------------------------------------*/
		// char   lpBuffer[] = "A";		       // lpBuffer should be  char or byte array, otherwise write wil fail
		DWORD  dNoOFBytestoWrite ;              // No of bytes to write into the port
		DWORD  dNoOfBytesWritten = 0;          // No of bytes written to the port
		
		dNoOFBytestoWrite = sizeof(Databuf); // Calculating the no of bytes to write into the port
		Status = WriteFile(hComm,               // Handle to the Serialport
						   Databuf,            // Data to be written to the port 
						   dNoOFBytestoWrite,   // No of bytes to write into the port
						   &dNoOfBytesWritten,  // No of bytes written to the port
						   NULL);

		printf("\r\n %d byte data has been sent",DataSent);
		if (DataSent >= dwFileSize) //end of file
			break;
	}

		printf("\r\n The file has been sent!!");
		CloseHandle(hComm);//Closing the Serial Port
		CloseHandle(hFile);//Closing the Serial Port
		getchar();
		getchar();
}
