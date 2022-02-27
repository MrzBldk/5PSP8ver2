#include "Global.h"
#include "AcceptServer.h"
#include "DispatchServer.h"
#include "GarbageCleaner.h"
#include "ResponseServer.h"
#include "ConsolePipe.h"

void main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	SetConsoleTitle("Concurrent Server");
	std::cout << "Параллельный сервер запущен" << std::endl;

	#pragma region console arguments
	if (argc > 1)
	{
		int tmp = atoi(argv[1]);
		if (tmp >= 0 && tmp <= 65535)
		{
			port = atoi(argv[1]);
			std::cout << std::format("Используемый TCP-порт: {}", port) << std::endl;
		}
		else
		{
			std::cout << "Выбран неверный TCP-порт" << std::endl;
		}
	}
	else
	{
		std::cout << std::format("Используемый TCP-порт: {}(по умолчанию)", port) << std::endl;

	}

	if (argc > 2)
	{
		int tmp = atoi(argv[2]);
		if (tmp >= 0 && tmp <= 65535)
		{
			uport = atoi(argv[2]);
			std::cout << std::format("Используемый UDP-порт: {}", uport) << std::endl;


		}
		else
		{
			std::cout << "Выбран неверный UDP-порт" << std::endl;
		}
	}
	else
	{
		std::cout << std::format("Используемый UDP-порт: {}(по умолчанию)", uport) << std::endl;
	}

	if (argc > 3)
	{
		dllname = argv[3];
	}

	if (argc > 4)
	{
		npname = argv[4];
		std::cout << std::format("Имя именованного канала: {}", npname) << std::endl;

	}
	else
	{
		std::cout << std::format("Имя именованного канала: {}(по умолчанию)", npname) << std::endl;
	}

	if (argc > 5)
	{
		ucall = argv[5];
		std::cout << std::format("Позывной:  {}", ucall) << std::endl;
	}
	else
	{
		std::cout << std::format("Позывной:  {}(по умолчанию)", ucall) << std::endl;
	}
	#pragma endregion

	srand(time(0));

	volatile TalkersCmd  cmd = Start;

	InitializeCriticalSection(&scListContact);

	st1 = LoadLibrary(dllname);
	ts1 = (HANDLE(*)(char*, LPVOID))GetProcAddress(st1, "SSS");
	(st1 == NULL) ? std::cout << "Ошибка загрузки DLL" << std::endl : std::cout << std::format("Загруженная DLL: {}", dllname) << std::endl;


	HANDLE hAcceptServer = CreateThread(NULL, NULL, AcceptServer, (LPVOID)&cmd, NULL, NULL);
	HANDLE hDispatchServer = CreateThread(NULL, NULL, DispatchServer, (LPVOID)&cmd, NULL, NULL);
	HANDLE hGarbageCleaner = CreateThread(NULL, NULL, GarbageCleaner, (LPVOID)&cmd, NULL, NULL);
	HANDLE hConsolePipe = CreateThread(NULL, NULL, ConsolePipe, (LPVOID)&cmd, NULL, NULL);
	HANDLE hResponseServer = CreateThread(NULL, NULL, ResponseServer, (LPVOID)&cmd, NULL, NULL);

	SetThreadPriority(hGarbageCleaner, THREAD_PRIORITY_BELOW_NORMAL);
	SetThreadPriority(hDispatchServer, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hConsolePipe, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hResponseServer, THREAD_PRIORITY_ABOVE_NORMAL);
	SetThreadPriority(hAcceptServer, THREAD_PRIORITY_HIGHEST);

	WaitForSingleObject(hAcceptServer, INFINITE);
	WaitForSingleObject(hDispatchServer, INFINITE);
	WaitForSingleObject(hGarbageCleaner, INFINITE);
	WaitForSingleObject(hConsolePipe, INFINITE);
	WaitForSingleObject(hResponseServer, INFINITE);

	CloseHandle(hAcceptServer);
	CloseHandle(hDispatchServer);
	CloseHandle(hGarbageCleaner);
	CloseHandle(hConsolePipe);
	CloseHandle(hResponseServer);

	DeleteCriticalSection(&scListContact);

	FreeLibrary(st1);
}
