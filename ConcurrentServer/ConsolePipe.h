#pragma once
#include <map>

DWORD WINAPI ConsolePipe(LPVOID pPrm)
{
	std::cout << "ConsolePipe запущен;\n";
	DWORD rc = 0;
	HANDLE hPipe;
	try
	{
		BOOL fSuccess;
		std::map<int, std::pair<HANDLE, long>> thread_map;

		SECURITY_ATTRIBUTES SecurityAttributes;
		SECURITY_DESCRIPTOR SecurityDescriptor;

		fSuccess = InitializeSecurityDescriptor(
			&SecurityDescriptor,
			SECURITY_DESCRIPTOR_REVISION);

		if (!fSuccess) {
			throw new std::string("InitializeSecurityDescriptor():");
		}

		fSuccess = SetSecurityDescriptorDacl(
			&SecurityDescriptor,
			TRUE,
			NULL,
			FALSE);

		if (!fSuccess) {
			throw new std::string("SetSecurityDescriptorDacl():");
		}

		SecurityAttributes.nLength = sizeof(SecurityAttributes);
		SecurityAttributes.lpSecurityDescriptor = &SecurityDescriptor;
		SecurityAttributes.bInheritHandle = FALSE;

		char rnpname[50];
		strcpy_s(rnpname, "\\\\.\\pipe\\");
		strcat_s(rnpname, npname);
		if ((hPipe = CreateNamedPipe(rnpname, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL, INFINITE, &SecurityAttributes)) == INVALID_HANDLE_VALUE)
			throw SetErrorMsgText("Create:", GetLastError());

		while (*((TalkersCmd*)pPrm) != Exit)
		{
			if (!ConnectNamedPipe(hPipe, NULL))
				throw SetErrorMsgText("Connect:", GetLastError());
			char ReadBuf[50], WriteBuf[50];
			DWORD nBytesRead = 0, nBytesWrite = 0;
			TalkersCmd SetCommand;
			bool Check;
			while (*((TalkersCmd*)pPrm) != Exit)
			{
				if (*((TalkersCmd*)pPrm) == Getcommand) {

					if (!ReadFile(hPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL))
						break;
					if (nBytesRead > 0)
					{
						Check = true;
						int n = atoi(ReadBuf);
						switch (n)
						{
						case 0:
							sprintf_s(WriteBuf, "%s", "Start");
							SetCommand = TalkersCmd::Start;
							break;
						case 1:
							sprintf_s(WriteBuf, "%s", "Stop");
							SetCommand = TalkersCmd::Stop;
							break;
						case 2:
							sprintf_s(WriteBuf, "%s", "Exit");
							SetCommand = TalkersCmd::Exit;
							break;
						case 3:
							sprintf_s(WriteBuf, "\nПодключено.: \t%i\nНеудачно.: \t%i\nЗавершено.: \t%i\n", Accept, Fail, Finished);
							Check = false;
							break;
						case 4:
							sprintf_s(WriteBuf, "%s", "Wait");
							SetCommand = TalkersCmd::Wait;
							break;
						case 5:
							sprintf_s(WriteBuf, "%s", "Shutdown");
							SetCommand = TalkersCmd::Shutdown;
							break;
						default:
							sprintf_s(WriteBuf, "%s", "NoCmd");
							if (n > 0)
							{
								port = n;
								TalkersCmd* tc = new TalkersCmd(Start);
								thread_map.insert(std::pair<int, std::pair < HANDLE, long>>(n, std::pair< HANDLE, long>(CreateThread(NULL, NULL, AcceptServer, (LPVOID)tc, NULL, NULL), (long)tc)));
								sprintf_s(WriteBuf, "Открыт порт \t%i", n);
							}
							else
							{
								n *= -1;
								auto f = thread_map.find(n);
								*((TalkersCmd*)(thread_map[n].second)) = Exit;
								CloseHandle(thread_map[n].first);
								delete (TalkersCmd*)(thread_map[n].second);
								thread_map.erase(f);
								sprintf_s(WriteBuf, "Закрыт порт \t%i", n);
							}
							Check = false;
							break;
						}
						if (Check == true)
						{
							*((TalkersCmd*)pPrm) = SetCommand;
							std::cout << std::format("ConsolePipe: команда {}", WriteBuf) << std::endl;
						}

						if (!WriteFile(hPipe, WriteBuf, sizeof(WriteBuf), &nBytesRead, NULL))
							throw new std::string("CP WRITE ERROR");
					}
				}
				else Sleep(1000);
			}
			if (!DisconnectNamedPipe(hPipe))
				throw SetErrorMsgText("disconnect:", GetLastError());
		}
		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);
		std::cout << "ConsolePipe остановлен;\n";
	}
	catch (std::string ErrorPipeText)
	{
		std::cout << ErrorPipeText << std::endl;
	}
	ExitThread(rc);
}