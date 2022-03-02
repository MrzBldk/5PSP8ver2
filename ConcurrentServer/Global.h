#pragma once

#include <iostream>
#include <format>
#include <list>
#include <Winsock2.h>    
#include "Error.h"

#pragma comment(lib,"WS2_32.lib")

INT port = 2000; 
INT uport = 2000; 
LPCSTR dllname = "ServiceLibrary";
LPCSTR npname = "NOS";
LPCSTR ucall = "NOS";
INT AS_SQUIRT = 10;
HANDLE(*ts1)(char*, LPVOID);
HMODULE st1;
HANDLE Event = CreateEvent(NULL, FALSE, FALSE, NULL);

struct NTP_Packet
{
	CHAR head[4];
	DWORD32 RootDelay;
	DWORD32 RootDispersion;
	CHAR ReferenceIdentifier[4];
	CHAR ReferenceTimestamp[2];
	DWORD64 OriginateTimestamp;
	DWORD32 TransmiTimestamp[2];
	DWORD32 KeyIdentifier;
	DWORD64 MessageDigest[2];
};

enum TalkersCmd { Start, Stop, Exit, Statistics, Wait, Shutdown, Getcommand };

volatile LONG Accept = 0;  //���������� �����������
volatile LONG Fail = 0;    //��������� �����������
volatile LONG Finished = 0;//����������� ������
volatile LONG Work = 0;    //���������� � ������� ������


CRITICAL_SECTION scListContact; //����������� ������

struct Contact         // ������� ������ �����������       
{
	enum TE {
		EMPTY,              //������ ������� ������ ����������� 
		ACCEPT,             //���������, �� �� �������������
		CONTACT             //������� �������������� �������  
	}    type;

	enum ST {
		WORK,               //���� ����� ������� � ��������
		ABORT,              //������������� ������ ���������� ����������� 
		TIMEOUT,            //������������� ������ ���������� �� ������� 
		FINISH              //������������� ������ ����������  ��������� 
	}      sthread;

	SOCKET      s;
	SOCKADDR_IN prms; 
	int         lprms;
	HANDLE      hthread;
	HANDLE      htimer;
	bool        TimerOff; 
	bool        CloseConn;
	char msg[50];
	char srvname[15];

	HANDLE hAcceptServer;

	Contact(TE t = EMPTY, LPCSTR namesrv = "")
	{
		memset(&prms, 0, sizeof(SOCKADDR_IN));
		lprms = sizeof(SOCKADDR_IN);
		type = t;
		strcpy_s(srvname, namesrv);
		msg[0] = 0;
		CloseConn = false;
		TimerOff = false;
	};

	void SetST(ST sth, LPCSTR m = "")
	{
		sthread = sth;
		strcpy_s(msg, m);
	}
};

typedef std::list<Contact> ListContact;
ListContact Contacts;

void CALLBACK ASWTimer(LPVOID Lprm, DWORD, DWORD)
{
	char obuf[50] = "Close: timeout;";
	Contact* client = (Contact*)Lprm; //����������� ���������� ��������
	EnterCriticalSection(&scListContact); //������ � ����������� ������
	client->TimerOff = true; 	//������ ����� ������������ �������
	client->sthread = Contact::TIMEOUT;
	LeaveCriticalSection(&scListContact); 	//������� �� ����������� ������
	if ((send(client->s, obuf, sizeof(obuf) + 1, NULL)) == SOCKET_ERROR)
		throw  SetErrorMsgText("Send:", WSAGetLastError());
	SYSTEMTIME stt;
	GetLocalTime(&stt);
	std::cout << std::format("{:0>2}.{:0>2}.{} {:0>2}:{:0>2} Timeout ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute);
	std::cout << client->srvname << ";" << std::endl;
}

void CALLBACK ASStartMessage(DWORD Lprm)
{
	Contact* client = (Contact*)Lprm;
	// ������ ����� ������������ �������
	char* sn = client->srvname;
	// ��������� �������
	SYSTEMTIME stt;
	// �������� ������� ����� � ����
	GetLocalTime(&stt);
	// ������� ���������
	std::cout << std::format("{:0>2}.{:0>2}.{} {:0>2}:{:0>2} ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute);
	std::cout << sn << " started;" << std::endl;
}

void CALLBACK ASFinishMessage(DWORD Lprm)
{
	Contact* client = (Contact*)Lprm;
	char* sn = client->srvname;
	SYSTEMTIME stt;
	GetLocalTime(&stt);
	std::cout << std::format("{:0>2}.{:0>2}.{} {:0>2}:{:0>2} ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute);
	std::cout << sn << " stoped;" << std::endl;
}