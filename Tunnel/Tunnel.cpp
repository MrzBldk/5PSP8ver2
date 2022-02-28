#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>
#include <Winsock2.h>
#pragma comment(lib, "WS2_32.lib")

std::string  GetErrorMsgText(int code);
std::string  SetErrorMsgText(std::string msgText, int code);

bool SystemMessage(char* ch)
{
	char Timeout[50] = "Close: timeout;", Close[50] = "Close: finish;", Abort[50] = "Close: Abort;";
	return (!strcmp(ch, Timeout) || !strcmp(ch, Abort) || !strcmp(ch, Close)) ? true : false;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	SetConsoleTitle("Tunnel");
	int port = 0;
	char Name[50];
	SOCKET  TunnelClientSocket;
	SOCKET  TunnelServerSocket;
	WSADATA wsaData;
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup:", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2001);
		serv.sin_addr.s_addr = INADDR_ANY;
		std::cout << "Введите имя сервера: ";
		std::cin >> Name;
		hostent* s = gethostbyname(Name);
		if (s == NULL)
			throw "Сервер не найден;";
		std::cout << "Введите порт сервера: ";
		std::cin >> port;
		SOCKADDR_IN Server_IN;
		Server_IN.sin_addr = *(struct in_addr*)s->h_addr_list[0];
		Server_IN.sin_family = AF_INET;
		Server_IN.sin_port = htons(port);
		while (true) {
			if ((TunnelClientSocket = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
				throw  SetErrorMsgText("Socket:", WSAGetLastError());
			if ((TunnelServerSocket = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
				throw SetErrorMsgText("socket:", WSAGetLastError());
			if ((connect(TunnelClientSocket, (sockaddr*)&Server_IN, sizeof(Server_IN))) == SOCKET_ERROR)
				throw "Ошибка соединения;";
			char Call[50];
			char rCall[50];
			int lobuf;
			if (bind(TunnelServerSocket, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
				throw SetErrorMsgText("bind:", WSAGetLastError());
			if (listen(TunnelServerSocket, SOMAXCONN) == SOCKET_ERROR)
				throw SetErrorMsgText("listen:", WSAGetLastError());
			SOCKET cS;
			SOCKADDR_IN clnt;
			memset(&clnt, 0, sizeof(clnt));
			int lclnt = sizeof(clnt);
			if ((cS = accept(TunnelServerSocket, (sockaddr*)&clnt, &lclnt)) == INVALID_SOCKET)
				throw SetErrorMsgText("accept:", WSAGetLastError());
			if ((lobuf = recv(cS, Call, sizeof(Call), NULL)) == SOCKET_ERROR)
				throw SetErrorMsgText("recv:", WSAGetLastError());
			std::cout << "Получил от клиента сообщение: " << Call << ". Пересылаю серверу." << std::endl;
			if ((lobuf = send(TunnelClientSocket, Call, sizeof(Call), NULL)) == SOCKET_ERROR)
				throw SetErrorMsgText("Send:", WSAGetLastError());
			if ((lobuf = recv(TunnelClientSocket, rCall, sizeof(rCall), NULL)) == SOCKET_ERROR)
				throw SetErrorMsgText("Recv:", WSAGetLastError());
			std::cout << "Получил от сервера сообщение: " << rCall << ". Пересылаю клиенту." << std::endl;
			if ((lobuf = send(cS, rCall, strlen(rCall) + 1, NULL)) == SOCKET_ERROR)
				throw SetErrorMsgText("send:", WSAGetLastError());
			u_long nonblk = 1;
			if (SOCKET_ERROR == ioctlsocket(TunnelClientSocket, FIONBIO, &nonblk))
				throw SetErrorMsgText("Ioctlsocket:", WSAGetLastError());
			char iib[50];
			char obuf[50] = "";
			bool rcv = true;
			while (true)
			{
				if (rcv)
				{
					if ((lobuf = recv(cS, iib, sizeof(iib), NULL)) == SOCKET_ERROR)
						throw SetErrorMsgText("recv:", WSAGetLastError());
					std::cout << "Получил от клиента сообщение: " << iib << ". Пересылаю серверу." << std::endl;
					if ((lobuf = send(TunnelClientSocket, iib, sizeof(iib), NULL)) == SOCKET_ERROR)
						throw "Ошибка error;";
					rcv = false;
				}
				if ((recv(TunnelClientSocket, obuf, sizeof(obuf), NULL)) == SOCKET_ERROR)
				{
					switch (WSAGetLastError())
					{
					case WSAEWOULDBLOCK: Sleep(100); break;
					default: throw  SetErrorMsgText("Recv:", WSAGetLastError());
					}
				}
				else
				{
					std::cout << "Получил от cервера сообщение: " << obuf << ". Пересылаю клиенту." << std::endl;
					if (SystemMessage(obuf))
					{
						if ((lobuf = send(cS, obuf, strlen(obuf) + 1, NULL)) == SOCKET_ERROR)
							throw SetErrorMsgText("send:", WSAGetLastError());
						break;
					}
					else
					{
						if ((lobuf = send(cS, obuf, strlen(obuf) + 1, NULL)) == SOCKET_ERROR)
							throw SetErrorMsgText("send:", WSAGetLastError());
					}
					rcv = true;
				}
			}
			if (closesocket(TunnelServerSocket) == SOCKET_ERROR)
				throw SetErrorMsgText("Closesocket:", WSAGetLastError());
			if (closesocket(cS) == SOCKET_ERROR)
				throw SetErrorMsgText("Closesocket:", WSAGetLastError());
		}
		if (closesocket(TunnelClientSocket) == SOCKET_ERROR)
			throw SetErrorMsgText("Closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup:", WSAGetLastError());
		system("Pause");
	}
	catch (std::string errorMsgText)
	{
		std::cout << errorMsgText << std::endl;
		system("Pause");
	}
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