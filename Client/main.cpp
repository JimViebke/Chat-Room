#include <iostream>
#include <sstream>
#include <string>

#include "client.h"
#include "connection_window.h"

int main()
{
	const std::string ip_and_port =  ConnectionWindow(4, 70).run();

	if (ip_and_port == "") return EXIT_FAILURE;

	const std::string ip_address = ip_and_port.substr(0, ip_and_port.find("::"));
	const std::string p = ip_and_port.substr(ip_and_port.find("::") + 2, ip_and_port.size());
	const unsigned port = stoi(p);

	Client client(50, 100, ip_address, port);
	client.run();
}
