#pragma once

static void WaitClients()
{
	bool ListEmpty = false;
	while (!ListEmpty)
	{
		// проверяем наличие клиентов
		EnterCriticalSection(&scListContact);
		ListEmpty = Contacts.empty();
		LeaveCriticalSection(&scListContact);
		SleepEx(0, TRUE);
	}
}

bool AcceptCycle(int squirt, SOCKET* s)
{
	bool rc = false;
	// Создаем новый Contact
	Contact c(Contact::ACCEPT, "AcceptServer");
	c.hAcceptServer = GetCurrentThread();

	if ((c.s = accept(*s, (sockaddr*)&c.prms, &c.lprms)) == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
			throw  SetErrorMsgText("Accept:", WSAGetLastError());
	}
	else
	{
		rc = true;
		InterlockedIncrement(&Accept);// Увеличиваем кол-во общих подключений
		InterlockedIncrement(&Work);  // Увеличиваем кол-во текущих подключений
		EnterCriticalSection(&scListContact);
		Contacts.push_front(c);
		LeaveCriticalSection(&scListContact);
		SetEvent(Event); // Переводим событие в сигнальное состояние

	}
	return rc;
};

void CommandsCycle(TalkersCmd& cmd, SOCKET* s)
{
	int squirt = 0;
	while (cmd != Exit) 
	{
		switch (cmd)
		{
		case Start:
			cmd = Getcommand;
			squirt = AS_SQUIRT;
			break;
		case Stop:
			cmd = Getcommand;
			squirt = 0;
			break;
		case Wait:
			WaitClients();
			cmd = Getcommand;
			squirt = AS_SQUIRT;
			break;
		case Shutdown:  //обработать текущих, а после завершить сервер
			WaitClients();
			cmd = Exit;
			break;
		};

		if (cmd != Exit && squirt > Work)
		{
			if (AcceptCycle(squirt, s))
			{
				cmd = Getcommand;
			}

			SleepEx(0, TRUE);
		}

	}
};

DWORD WINAPI AcceptServer(LPVOID pPrm)
{
	std::cout << "AcceptServer запущен;\n";
	DWORD rc = 0; //код возврата  
	SOCKET  ServerSocket;
	WSADATA wsaData;

	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup:", WSAGetLastError());

		if ((ServerSocket = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw SetErrorMsgText("Socket:", WSAGetLastError());

		SOCKADDR_IN Server_IN;
		Server_IN.sin_family = AF_INET;
		Server_IN.sin_port = htons(port);
		Server_IN.sin_addr.s_addr = ADDR_ANY;
		if (bind(ServerSocket, (LPSOCKADDR)&Server_IN, sizeof(Server_IN)) == SOCKET_ERROR)
			throw SetErrorMsgText("Bind:", WSAGetLastError());

		if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR)
			throw SetErrorMsgText("Listen:", WSAGetLastError());

		u_long nonblk;
		if (ioctlsocket(ServerSocket, FIONBIO, &(nonblk = 1)) == SOCKET_ERROR)
			throw SetErrorMsgText("Ioctlsocket:", WSAGetLastError());

		TalkersCmd* command = (TalkersCmd*)pPrm;
		CommandsCycle(*((TalkersCmd*)command), &ServerSocket);

		if (closesocket(ServerSocket) == SOCKET_ERROR)
			throw SetErrorMsgText("Сlosesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (std::string errorMsgText)
	{
		std::cout << errorMsgText << std::endl;
	}
	std::cout << "AcceptServer остановлен;\n";
	ExitThread(rc);
}