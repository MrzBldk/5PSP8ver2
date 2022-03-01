#pragma once

DWORD WINAPI TimeSynchServer(LPVOID lParam)
{
	DWORD rc = 0;
	int Cs = 0, Cc = 0, i = 0, correction = 0, sumCorrection = 0;
	Contact* client = (Contact*)lParam;
	QueueUserAPC(ASStartMessage, client->hAcceptServer, (DWORD)client);
	try
	{
		client->sthread = Contact::WORK;
		int  bytes = 1;
		char ibuf[50], obuf[50] = "Close: finish;";
		while (client->TimerOff == false)
		{
			Cs = clock();
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
					std::cout << std::format("Текущее значение счётчика: {}, Значение счётчика клиента: {}, Коррекция для клиента: {}, Среднее значение коррекции: {}", Cs, Cc, correction, sumCorrection/++i) << std::endl;
				}
				else
				{
					break;
				}
			}
		}
		
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