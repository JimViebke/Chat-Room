#include <iostream>
#include <sstream>
#include <string>

#include "client.h"
#include "connection_window.h"

int main()
{
	std::string ip_address;
	std::string error;
	unsigned port;

	for (;;)
	{
		for (;;)
		{
			ConnectionWindow connection_window(4, 70, error);
			std::string ip_and_port = connection_window.run();

			if (ip_and_port.find("::") == std::string::npos)
			{
				error = "error: No '::' separator between ipaddress and port.";
				continue;
			}

			ip_address = ip_and_port.substr(0, ip_and_port.find("::"));
			std::string p = ip_and_port.substr(ip_and_port.find("::") + 2, ip_and_port.size());
			port = stoi(p);

			break;
		}

		try
		{
			Client client(50, 100, ip_address, port);
			client.run();
		}
		catch (std::exception ex)
		{
			error = ex.what();
			continue;
		}

		break;
	}
}
