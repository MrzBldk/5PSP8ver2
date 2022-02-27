#include <iostream>
#include <string>
#include <Windows.h>

std::string GetErrorMsgText(int code)
{
	std::string msgText;
	switch (code)
	{
	case ERROR_PIPE_CONNECTED: msgText = "ERROR_PIPE_CONNECTED"; break;
	case ERROR_BROKEN_PIPE: msgText = "ERROR_BROKEN_PIPE"; break;
	case ERROR_PIPE_BUSY: msgText = "ERROR_PIPE_BUSY"; break;
	case ERROR_PIPE_LISTENING: msgText = "ERROR_PIPE_LISTENING"; break;
	case ERROR_CANNOT_IMPERSONATE: msgText = "ERROR_CANNOT_IMPERSONATE"; break;
	case ERROR_PIPE_LOCAL: msgText = "ERROR_PIPE_LOCAL"; break;
	case ERROR_BAD_PIPE: msgText = "ERROR_BAD_PIPE"; break;
	case ERROR_NO_DATA: msgText = "ERROR_NO_DATA"; break;
	case ERROR_PIPE_NOT_CONNECTED: msgText = "ERROR_PIPE_NOT_CONNECTED"; break;
	default: msgText = "***ERROR***"; break;
	}
	return msgText;
}
std::string SetPipeError(std::string msgText, int code)
{
	return msgText + GetErrorMsgText(code) + " " + std::to_string(code);
}

int main()
{
	setlocale(LC_ALL, "Rus");
	SetConsoleTitle("Remote Console");

	try
	{
		std::cout<<("\nКоманды управления:\n 1 - Start  \t (запуск сервера)\n 2 - Stop  \t (остановить сервер)\n 3 - Exit  \t (завершить работу сервера)\n 4 - Statistics  (вывод статистики)\n 5 - Wait  \t (приостановить сервер)\n 6 - Shutdown  \t (приостанавлевает и завершает работу сервера)\n 7 - Завершить RConsole\n\n");
		char ReadBuf[50] = "";
		char WriteBuf[2] = "";
		DWORD nBytesRead;
		DWORD nBytesWrite;

		int Code = 0;

		char serverName[256];
		char PipeName[512];
		bool result;

		std::cout << "Введите имя севера: ";
		std::cin >> serverName;
		result = sprintf_s(PipeName, "\\\\%s\\pipe\\NOS", serverName);


		BOOL fSuccess;

		SECURITY_ATTRIBUTES SecurityAttributes;
		SECURITY_DESCRIPTOR SecurityDescriptor;

		fSuccess = InitializeSecurityDescriptor(
			&SecurityDescriptor,
			SECURITY_DESCRIPTOR_REVISION);

		if (!fSuccess)
			throw SetPipeError("InitializeSecurityDescriptor() ", GetLastError());

		fSuccess = SetSecurityDescriptorDacl(
			&SecurityDescriptor,
			TRUE,
			NULL,
			FALSE);

		if (!fSuccess)
			throw SetPipeError("SetSecurityDescriptorDacl() ", GetLastError());

		SecurityAttributes.nLength = sizeof(SecurityAttributes);
		SecurityAttributes.lpSecurityDescriptor = &SecurityDescriptor;
		SecurityAttributes.bInheritHandle = FALSE;

		HANDLE hNamedPipe = CreateFile(PipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, &SecurityAttributes);
		if (hNamedPipe == INVALID_HANDLE_VALUE)
			throw SetPipeError("CrateFile ", GetLastError());

		do
		{
			std::cout << "Команда: ";
			std::cin >> Code;
			if (Code > 0 && Code < 7)
			{
				sprintf_s(WriteBuf, "%d", Code - 1);
				if (!WriteFile(hNamedPipe, WriteBuf, strlen(WriteBuf) + 1, &nBytesWrite, NULL))
					throw SetPipeError("WriteFile ", GetLastError());
				if (ReadFile(hNamedPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL))
					std::cout << ReadBuf << std::endl;
				else
					throw SetPipeError("ReadFile ", GetLastError());
			}
			else if (Code > 7) printf("Неверная команда.\n\n");
		} while (Code != 7 && Code != 3 && Code != 6);
	}
	catch (char* ErrorPipeText)
	{
		std::cout << ErrorPipeText << std::endl;
	}
	std::cout << "Консоль управления завершает работу...";
	Sleep(3000);
	return 0;
}