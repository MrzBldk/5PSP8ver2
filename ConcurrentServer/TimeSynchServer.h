#pragma once
#include <ws2tcpip.h>

DWORD WINAPI TimeSynchServer(LPVOID lParam)
{
	DWORD rc = 0;
	int Cs = 0, Cc = 0, i = 0, correction = 0, sumCorrection = 0, sCorrection = 0;
	Contact* client = (Contact*)lParam;

	int d = 613608 * 3600; // секyнд между 1900 и 1970
	SOCKADDR_IN sntpopt;
	SOCKET sntp;
	sntpopt.sin_family = AF_INET;
	InetPton(AF_INET, "88.147.254.232", &(sntpopt.sin_addr));
	sntpopt.sin_port = htons(123);
	NTP_Packet out_buf, in_buf;
	ZeroMemory(&out_buf, sizeof(out_buf));
	ZeroMemory(&in_buf, sizeof(in_buf));
	out_buf.head[0] = 0x1B;
	out_buf.head[1] = 0x00;
	out_buf.head[2] = 0x04;
	out_buf.head[3] = 0xEC;

	QueueUserAPC(ASStartMessage, client->hAcceptServer, (DWORD)client);
	try
	{
		sntp = socket(AF_INET, SOCK_DGRAM, NULL);
		int lenout = 0, lenin = 0, lensockaddr = sizeof(sntpopt);
		time_t sec = time(0);

		client->sthread = Contact::WORK;
		int  bytes = 1;
		char ibuf[50], obuf[50] = "Close: finish;";

		while (client->TimerOff == false)
		{
			Cs = clock() + sCorrection;
			if ((bytes = recv(client->s, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR)
			{
				switch (WSAGetLastError())
				{
				case WSAEWOULDBLOCK: Sleep(100); break;
				default: throw SetErrorMsgText("Recv:", WSAGetLastError());
				}
			}
			else
			{
				if (strcmp(ibuf, "exit") != 0)
				{
					if (client->TimerOff != false)
						break;
					Cc = atoi(ibuf);
					correction = Cs - Cc;
					_itoa_s(correction, ibuf, 10);
					if ((send(client->s, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR)
						throw  SetErrorMsgText("Send:", WSAGetLastError());
					sumCorrection += correction;
					std::cout << std::format("Текущее значение счётчика: {}, Значение счётчика клиента: {}, Коррекция для клиента: {}, Среднее значение коррекции: {}", Cs, Cc, correction, sumCorrection / ++i) << std::endl;
					if (i % 10 == 0)
					{
						if (lenout = sendto(sntp, (char*)&out_buf, sizeof(out_buf), NULL, (sockaddr*)&sntpopt, sizeof(sntpopt)) == SOCKET_ERROR)
							throw SetErrorMsgText("sendto:", WSAGetLastError());
						if (lenin = recvfrom(sntp, (char*)&in_buf, sizeof(in_buf), NULL, (sockaddr*)&sntpopt, &lensockaddr) == SOCKET_ERROR)
							throw SetErrorMsgText("recvfrom:", WSAGetLastError());
						in_buf.ReferenceTimestamp[0] = ntohl(in_buf.ReferenceTimestamp[0]) - d;
						in_buf.TransmiTimestamp[0] = ntohl(in_buf.TransmiTimestamp[0]) - d;
						in_buf.TransmiTimestamp[1] = ntohl(in_buf.TransmiTimestamp[1]);
						int s = in_buf.TransmiTimestamp[0] - sec;
						int ms = 1000.0 * ((double)(in_buf.TransmiTimestamp[1]) / (double)0xffffffff);
						int newCs = s * 1000 + ms;
						std::cout << std::format("Значение счётчика: {}, Полученное от NTP-сервера значение счётчика: {}, Коррекция: {}", Cs, newCs, (newCs - Cs)) << std::endl;
						sCorrection += newCs - Cs;
					}
				}
				else
				{
					break;
				}
			}
		}

		closesocket(sntp);
		
		if (client->TimerOff == false)
		{
			CancelWaitableTimer(client->htimer);
			if ((send(client->s, obuf, sizeof(obuf) + 1, NULL)) == SOCKET_ERROR)
				throw  SetErrorMsgText("Send:", WSAGetLastError());
			client->sthread = Contact::FINISH;
			QueueUserAPC(ASFinishMessage, client->hAcceptServer, (DWORD)client);
		}
	}
	catch (std::string ex)
	{
		std::cout << ex << std::endl;
		CancelWaitableTimer(client->htimer);
		client->sthread = Contact::ABORT;
	}
	ExitThread(rc);
}