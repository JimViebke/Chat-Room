
#if defined(_DEBUG)
#include <stdlib.h>
#include <crtdbg.h>
#define CRTDBG_MAP_ALLOC
#endif

#include "server.h"

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

	Server server;
	server.run();

	_CrtDumpMemoryLeaks();
}