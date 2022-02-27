#pragma once

DWORD WINAPI DispatchServer(LPVOID pPrm)
{
	std::cout << "DispatchServer запущен;\n";
	DWORD rc = 0;
	try
	{
		while (*((TalkersCmd*)pPrm) != Exit)
		{
			try
			{
				if (WaitForSingleObject(Event, 300) == WAIT_OBJECT_0)
				{

					if (&Work > 0)
					{
						Contact* client = NULL;
						int libuf = 1;
						char CallBuf[50] = "", SendError[50] = "ErrorInquiry";
						EnterCriticalSection(&scListContact);
						for (ListContact::iterator p = Contacts.begin(); p != Contacts.end(); p++)
						{
							if (p->type == Contact::ACCEPT)
							{
								client = &(*p);

								bool Check = false;
								while (Check == false)
								{
									if ((libuf = recv(client->s, CallBuf, sizeof(CallBuf), NULL)) == SOCKET_ERROR)
									{
										switch (WSAGetLastError())
										{
										case WSAEWOULDBLOCK: Sleep(100); break;
										default: throw  SetErrorMsgText("Recv:", WSAGetLastError());
										}
									}
									else 
										Check = true;
								}

								if (strcmp(CallBuf, "Rand") == 0 || strcmp(CallBuf, "Echo") == 0 || strcmp(CallBuf, "Time") == 0)
									Check == true;
								else 
									Check == false;

								if (Check == true)
								{
									client->type = Contact::CONTACT; //меняем статус обработки
									strcpy_s(client->srvname, CallBuf);
									client->htimer = CreateWaitableTimer(NULL, false, NULL); //создание ожидающего таймера
									_int64 time = -150000000; //установка времени (15 секунд)
									SetWaitableTimer(client->htimer, (LARGE_INTEGER*)&time, 0, ASWTimer, client, false);

									if ((libuf = send(client->s, CallBuf, sizeof(CallBuf), NULL)) == SOCKET_ERROR)
										throw SetErrorMsgText("Send:", WSAGetLastError());
									client->hthread = ts1(CallBuf, client);
								}
								else
								{
									if ((libuf = send(client->s, SendError, sizeof(SendError) + 1, NULL)) == SOCKET_ERROR)
										throw SetErrorMsgText("Send:", WSAGetLastError());
									closesocket(client->s);
									client->sthread = Contact::ABORT;
									CancelWaitableTimer(client->htimer);
									InterlockedIncrement(&Fail);
								}
							}
						}
						LeaveCriticalSection(&scListContact);
					}
					SleepEx(0, true);
				}
				SleepEx(0, true);
			}
			catch (std::string errorMsgText)
			{
				std::cout << errorMsgText << std::endl;
			}
		}
	}
	catch (std::string errorMsgText)
	{
		std::cout << errorMsgText << std::endl;
	}
	std::cout << "DispatchServer остановлен;\n";
	ExitThread(rc);
}