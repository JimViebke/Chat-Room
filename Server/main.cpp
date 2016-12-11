
#if defined(_DEBUG)
#include <stdlib.h>
#include <crtdbg.h>
#define CRTDBG_MAP_ALLOC
#endif

#include "server.h"

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

	Server server(20, 40);
	server.run();
}
