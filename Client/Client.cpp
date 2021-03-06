#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>
#include <format>
#include <Winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "WS2_32.lib")

bool SystemMessage(char* ch)
{
	char Timeout[50] = "Close: timeout;", Close[50] = "Close: finish;", Abort[50] = "Close: Abort;";
	return (!strcmp(ch, Timeout) || !strcmp(ch, Abort) || !strcmp(ch, Close)) ? true : false;
}

std::string  GetErrorMsgText(int code)
{
	std::string msgText;
	switch (code)
	{
	case WSAEINTR:                  msgText = "WSAEINTR";               break;    //Работа функции прервана
	case WSAEACCES:                 msgText = "WSAEACCES";              break;    //Разрешение отвергнуто
	case WSAEFAULT:                 msgText = "WSAEFAULT";              break;    //Ошибочный адрес
	case WSAEINVAL:                 msgText = "WSAEINVAL";              break;    //Ошибка в аргументе
	case WSAEMFILE:                 msgText = "WSAEMFILE";              break;    //Слишком много файлов открыто
	case WSAEWOULDBLOCK:            msgText = "WSAEWOULDBLOCK";         break;    //Ресурс временно недоступен
	case WSAEINPROGRESS:            msgText = "WSAEINPROGRESS";         break;    //Операция в процессе развития
	case WSAEALREADY:               msgText = "WSAEALREADY";            break;    //Операция уже выполняется
	case WSAENOTSOCK:               msgText = "WSAENOTSOCK";            break;    //Сокет задан неправильно
	case WSAEDESTADDRREQ:           msgText = "WSAEDESTADDRREQ";        break;    //Требуется адрес расположения
	case WSAEMSGSIZE:               msgText = "WSAEMSGSIZE";            break;    //Сообщение слишком длинное
	case WSAEPROTOTYPE:             msgText = "WSAEPROTOTYPE";          break;    //Неправильный тип протокола для сокета
	case WSAENOPROTOOPT:            msgText = "WSAENOPROTOOPT";         break;    //Ошибка в опции протокола
	case WSAEPROTONOSUPPORT:        msgText = "WSAEPROTONOSUPPORT";     break;    //Протокол не поддерживается
	case WSAESOCKTNOSUPPORT:        msgText = "WSAESOCKTNOSUPPORT";     break;    //Тип сокета не поддерживается
	case WSAEOPNOTSUPP:             msgText = "WSAEOPNOTSUPP";          break;    //Операция не поддерживается
	case WSAEPFNOSUPPORT:           msgText = "WSAEPFNOSUPPORT";        break;    //Тип протоколов не поддерживается
	case WSAEAFNOSUPPORT:           msgText = "WSAEAFNOSUPPORT";        break;    //Тип адресов не поддерживается протоколом
	case WSAEADDRINUSE:             msgText = "WSAEADDRINUSE";          break;    //Адрес уже используется
	case WSAEADDRNOTAVAIL:          msgText = "WSAEADDRNOTAVAIL";       break;    //Запрошенный адрес не может быть использован
	case WSAENETDOWN:               msgText = "WSAENETDOWN";            break;    //Сеть отключена
	case WSAENETUNREACH:            msgText = "WSAENETUNREACH";         break;    //Сеть не достижима
	case WSAENETRESET:              msgText = "WSAENETRESET";           break;    //Сеть разорвала соединение
	case WSAECONNABORTED:           msgText = "WSAECONNABORTED";        break;    //Программный отказ связи
	case WSAECONNRESET:             msgText = "WSAECONNRESET";          break;    //Связь восстановлена
	case WSAENOBUFS:                msgText = "WSAENOBUFS";             break;    //Не хватает памяти для буферов
	case WSAEISCONN:                msgText = "WSAEISCONN";             break;    //Сокет уже подключен
	case WSAENOTCONN:               msgText = "WSAENOTCONN";            break;    //Сокет не подключен
	case WSAESHUTDOWN:              msgText = "WSAESHUTDOWN";           break;    //Нельзя выполнить send: сокет завершил работу
	case WSAETIMEDOUT:              msgText = "WSAETIMEDOUT";           break;    //Закончился отведенный интервал  времени
	case WSAECONNREFUSED:           msgText = "WSAECONNREFUSED";        break;    //Соединение отклонено
	case WSAEHOSTDOWN:              msgText = "WSAEHOSTDOWN";           break;    //Хост в неработоспособном состоянии
	case WSAEHOSTUNREACH:           msgText = "WSAEHOSTUNREACH";        break;    //Нет маршрута для хоста
	case WSAEPROCLIM:               msgText = "WSAEPROCLIM";            break;    //Слишком много процессов
	case WSASYSNOTREADY:            msgText = "WSASYSNOTREADY";         break;    //Сеть не доступна
	case WSAVERNOTSUPPORTED:        msgText = "WSAVERNOTSUPPORTED";     break;    //Данная версия недоступна
	case WSANOTINITIALISED:         msgText = "WSANOTINITIALISED";      break;    //Не выполнена инициализация WS2_32.DLL
	case WSAEDISCON:                msgText = "WSAEDISCON";             break;    //Выполняется отключение
	case WSATYPE_NOT_FOUND:         msgText = "WSATYPE_NOT_FOUND";      break;    //Класс не найден
	case WSAHOST_NOT_FOUND:         msgText = "WSAHOST_NOT_FOUND";      break;    //Хост не найден
	case WSATRY_AGAIN:              msgText = "WSATRY_AGAIN";           break;    //Неавторизированный хост не найден
	case WSANO_RECOVERY:            msgText = "WSANO_RECOVERY";         break;    //Неопределенная  ошибка
	case WSANO_DATA:                msgText = "WSANO_DATA";             break;    //Нет записи запрошенного типа
	case WSA_INVALID_HANDLE:      msgText = "WSA_INVALID_HANDLE";     break;    //Указанный дескриптор события  с ошибкой
	case WSA_INVALID_PARAMETER:   msgText = "WSA_INVALID_PARAMETER";  break;    //Один или более параметров с ошибкой
	case WSA_IO_INCOMPLETE:       msgText = "WSA_IO_INCOMPLETE";      break;    //Объект ввода-вывода не в сигнальном состоянии
	case WSA_IO_PENDING:          msgText = "WSA_IO_PENDING";         break;    //Операция завершится позже
	case WSA_NOT_ENOUGH_MEMORY:   msgText = "WSA_NOT_ENOUGH_MEMORY";  break;    //Не достаточно памяти
	case WSA_OPERATION_ABORTED:   msgText = "WSA_OPERATION_ABORTED";  break;    //Операция отвергнута
	case WSASYSCALLFAILURE:         msgText = "WSASYSCALLFAILURE";      break;
	default:                msgText = "***ERROR***";      break;
	};
	return msgText;
}

std::string  SetErrorMsgText(std::string msgText, int code)
{
	return  msgText + GetErrorMsgText(code) + " " + std::to_string(code);
}

bool  GetServer(char* call, SOCKADDR_IN* from, int* flen, SOCKET* cC, SOCKADDR_IN* all)
{
	char ibuf[50], obuf[50];
	int  libuf = 0, lobuf = 0;

	if ((lobuf = sendto(*cC, call, strlen(call) + 1, NULL, (sockaddr*)all, sizeof(*all))) == SOCKET_ERROR)
		throw  SetErrorMsgText("Sendto:", WSAGetLastError());
	if ((libuf = recvfrom(*cC, ibuf, sizeof(ibuf), NULL, (sockaddr*)from, flen)) == SOCKET_ERROR)
		if (WSAGetLastError() == WSAETIMEDOUT)
			return false;
		else
			throw  SetErrorMsgText("Recv:", WSAGetLastError());
	return !strcmp(call, ibuf);
}

int main()
{
	setlocale(LC_ALL, "Russian");
	SetConsoleTitle("Client");
	int port = 0;
	SOCKET  ClientSocket;
	WSADATA wsaData;
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup:", WSAGetLastError());
		if ((ClientSocket = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("Socket:", WSAGetLastError());

		int ch = 0;
		bool fin = false;
		int cCall = 0;

		int max = 100, lobuf = 1;
		char obuf[50] = "";
		char ibuf[50] = "";
		int bport = 2000;

		char Call[50];
		char Name[50];
		char Calls[50];

		SOCKADDR_IN Server = { 0 };
		Server.sin_family = AF_INET;
		Server.sin_port = htons(port);

		SOCKADDR_IN Server_IN;
		int Flen = sizeof(Server);
		int chs = 0;
		std::cout << "1 - Имя сервера" << std::endl << "2 - Позывной" << std::endl << "> ";
		std::cin >> chs;
		if (chs == 1)
		{
			std::cout << "Соединение с сервером (по имени и порту)" << std::endl << "> ";
			std::cout << "Введите имя сервера: ";
			std::cin >> Name;

			hostent* s = gethostbyname(Name);
			if (s == NULL)
				throw "Сервер не найден;";
			std::cout << "Введите порт сервера: ";
			std::cin >> port;
			Server_IN.sin_addr = *(struct in_addr*)s->h_addr_list[0];

		}
		else if (chs == 2)
		{
			std::cout << "Введите позывной: ";
			std::cin >> Calls;
			SOCKET cC;
			if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
				throw SetErrorMsgText("Socket:", WSAGetLastError());

			int optval = 1;
			if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST,
				(char*)&optval, sizeof(int)) == SOCKET_ERROR)
				throw SetErrorMsgText("Opt:", WSAGetLastError());

			SOCKADDR_IN all;
			all.sin_family = AF_INET; 
			all.sin_port = htons(bport);
			all.sin_addr.s_addr = INADDR_BROADCAST;
			SOCKADDR_IN clnt;
			memset(&clnt, 0, sizeof(clnt));
			int lc = sizeof(clnt);

			bool bsr = GetServer(Calls, &clnt, &lc, &cC, &all);
			if (bsr == false)
				throw "Server not found;";
			else
			{
				Server_IN.sin_addr.s_addr = clnt.sin_addr.s_addr;
				if (closesocket(cC) == SOCKET_ERROR)
					throw SetErrorMsgText("Closesocket:", WSAGetLastError());
				std::cout << "Введите порт: ";
				std::cin >> port;
			}

		}
		else throw "Wrong code";

		Server_IN.sin_family = AF_INET;
		Server_IN.sin_port = htons(port);

		if ((connect(ClientSocket, (sockaddr*)&Server_IN, sizeof(Server_IN))) == SOCKET_ERROR)
			throw "Connection Error";

		while (!fin)
		{
			std::cout << "Запрос: " << std::endl << "1 - Rand" << std::endl << "2 - Time" << std::endl << "3 - Echo" << std::endl << "> ";
			std::cin >> cCall;
			if (cCall == 1 || cCall == 2 || cCall == 3)
			{
				fin = true;
				break;
			}

			else
			{
				if (closesocket(ClientSocket) == SOCKET_ERROR)
					throw SetErrorMsgText("Closesocket:", WSAGetLastError());
				throw "Введен не верный код;";
			}
		}

		switch (cCall)
		{
		case 1:
			strcpy_s(Call, "Rand");
			std::cout << "Введите команду Rand или другое слово для выхода:" << std::endl;
			break;
		case 2:
			strcpy_s(Call, "Time");
			std::cout << "Введите команду Time или другое слово для выхода:" << std::endl;
			break;
		case 3:
			strcpy_s(Call, "Echo");
			std::cout << "Введите строку для отправки или команду exit для выхода:" << std::endl;
			break;
		default:
			strcpy_s(Call, "Echo");
		}

		if ((lobuf = send(ClientSocket, Call, sizeof(Call), NULL)) == SOCKET_ERROR)
			throw SetErrorMsgText("Send:", WSAGetLastError());
		char rCall[50];
		if ((lobuf = recv(ClientSocket, rCall, sizeof(rCall), NULL)) == SOCKET_ERROR)
			throw SetErrorMsgText("Recv:", WSAGetLastError());

		if (strcmp(Call, rCall) != 0)
			throw "Ошибка сервера";
		else
		{
			bool Check = true;
			fin = false;

			u_long nonblk = 1;
			if (SOCKET_ERROR == ioctlsocket(ClientSocket, FIONBIO, &nonblk))
				throw SetErrorMsgText("Ioctlsocket:", WSAGetLastError());
			clock_t StartTime = clock();
			bool rcv = true;

			char iib[50];
			std::cout << "->";
			std::cin >> iib;

			while (!fin)
			{
				if (rcv)
				{
					if ((lobuf = send(ClientSocket, iib, sizeof(iib), NULL)) == SOCKET_ERROR)
						throw SetErrorMsgText("Send:", WSAGetLastError());
					rcv = false;
				}

				if ((recv(ClientSocket, obuf, sizeof(obuf), NULL)) == SOCKET_ERROR)
				{
					switch (WSAGetLastError())
					{
					case WSAEWOULDBLOCK: Sleep(100); break;
					default: throw SetErrorMsgText("Recv:", WSAGetLastError());
					}
				}
				else
				{
					if (SystemMessage(obuf))
					{
						std::cout << std::format("Сервер завершил соединение: {}", obuf) << std::endl;
						break;
					}
					else
						std::cout << std::format("Полученное сообщение:[{}]", obuf) << std::endl;
					rcv = true;
				}
			}
			clock_t FinishTime = clock();
			std::cout << std::format("Время: {} сек.", (double)(FinishTime - StartTime) / CLOCKS_PER_SEC) << std::endl;
		}
		if (closesocket(ClientSocket) == SOCKET_ERROR)
			throw SetErrorMsgText("Closesocket:", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (char* errorMsgText)
	{
		std::cout << errorMsgText << std::endl;
	}
	system("pause");
	return 0;
}