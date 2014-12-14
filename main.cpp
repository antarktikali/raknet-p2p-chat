#include "P2PClient.h"
#include <iostream>
#include <string>

int main()
{
	std::cout << "* Usage:" << std::endl <<
					"  [l]isten for a connection" << std::endl <<
					"  [c]onnect to a remote peer" << std::endl <<
					"  [s]end a message to the remote peer" << std::endl <<
					"  [q]uit" << std::endl;
	
	P2PClient *peer = new P2PClient();
	bool isRunning = true;
	std::string input;
	
	while(isRunning)
	{
		std::cin >> input;
		
		if(input ==  "l")
		{
			peer->listen();
		}
		else if("c" == input)
		{
			std::cout << "* Enter the guid of the remote peer:" << std::endl;
			std::cin >> input;
			peer->attemptNatPunchthrough(input);
		}

		else if("s" == input)
		{
			std::cout << "* Enter message:" << std::endl;;
			std::cin >> input;
			peer->sendMessage(input);
		}

		else if(input == "q")
		{
			isRunning = false;
		}

		else
		{
			std::cout << "wha??" << std::endl;
		}
	}

	delete peer;
	return 0;
}
