
#include <iostream>

#include "client.h"

int main()
{
	Client client("fake.ip", 8050);
	client.run();

	std::cin.ignore();
}
