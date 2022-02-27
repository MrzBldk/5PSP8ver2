#pragma once
#include <ws2tcpip.h>

DWORD WINAPI GarbageCleaner(LPVOID pPrm)
{
	std::cout << "GarbageCleaner запущен;\n";
	DWORD rc = 0;
	try
	{
		while (*((TalkersCmd*)pPrm) != Exit && &Work > 0)
		{
			EnterCriticalSection(&scListContact);

			for (std::list<Contact>::iterator first = Contacts.begin(); first != Contacts.end();)
			{
				if (first->sthread == Contact::FINISH || first->sthread == Contact::TIMEOUT || first->sthread == Contact::ABORT || first->type == Contact::EMPTY)
				{
					char ipbuff[INET_ADDRSTRLEN];
					InetNtop(AF_INET, &(((struct sockaddr_in*)&(first->prms))->sin_addr), ipbuff, INET_ADDRSTRLEN);
					std::cout << std::format("IP удаленного клиента: {}", ipbuff);
					std::cout << " с кодом " << first->sthread << ";" << std::endl;
					if (first->type == Contact::EMPTY) InterlockedIncrement(&Fail);
					else
					{
						if (first->sthread == Contact::FINISH) InterlockedIncrement(&Finished);
						if (first->sthread == Contact::TIMEOUT) InterlockedIncrement(&Fail);
						if (first->sthread == Contact::ABORT) InterlockedIncrement(&Fail);
						CloseHandle(first->hthread);
						CloseHandle(first->htimer);
					}
					closesocket(first->s);
					first = Contacts.erase(first);
					InterlockedDecrement(&Work);
				}

				else first++;
			}
			LeaveCriticalSection(&scListContact);
			Sleep(1000);
		}
	}
	catch (std::string errorMsgText)
	{
		std::cout << errorMsgText << std::endl;
	}
	std::cout << "GarbageCleaner остановлен;\n";
	ExitThread(rc);
}