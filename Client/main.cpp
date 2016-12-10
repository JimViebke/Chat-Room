
#if defined(_DEBUG)
#include <stdlib.h>
#include <crtdbg.h>
#define CRTDBG_MAP_ALLOC
#endif

#include <iostream>
#include <sstream>
#include <string>

#include "client.h"
#include "connection_window.h"

int main()
{
#if defined(_DEBUG)
	#define DEBUG_NEW new(__FILE__, __LINE__)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
#endif

	const std::string ip_and_port =  ConnectionWindow(4, 70).run();

	if (ip_and_port == "") return EXIT_FAILURE;

	const std::string ip_address = ip_and_port.substr(0, ip_and_port.find("::"));
	const std::string p = ip_and_port.substr(ip_and_port.find("::") + 2, ip_and_port.size());
	const unsigned port = stoi(p);

	Client client(50, 100, ip_address, port);
	client.run();

	_CrtDumpMemoryLeaks();
}
