
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
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
#endif

	std::unique_ptr<pipedat::Connection> connection = ConnectionWindow(4, 70).run();

	if (connection == nullptr)
		return EXIT_SUCCESS;

	Client client(50, 100, std::move(connection));
	client.run();
}
