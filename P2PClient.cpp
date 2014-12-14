#include "P2PClient.h"

P2PClient::P2PClient()
{
	this->sentNatPunchthroughRequest = false;

	this->rakPeer = RakNet::RakPeerInterface::GetInstance();
	this->rakPeer->AttachPlugin(&(this->natPunchthroughClient));
	this->natPunchServerAddress = new RakNet::SystemAddress("natpunch.jenkinssoftware.com",61111);
	
	// Use the default socket descriptor; this will make the OS assign us a
	// random port.
	RakNet::SocketDescriptor socketDescriptor;
	// Allow 2 connections: one for the other peer, one for the NAT server.
	this->rakPeer->Startup(2, &socketDescriptor, 1);
	
	std::cout << "* Your GUID is:" << std::endl <<
		this->rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString()
		<< std::endl;
	
	// Start the thread for packet receiving
	this->isListening = true;
	this->listenLoopThread = new std::thread(&P2PClient::listenLoop, this);

	// Connect to the NAT Punchthrough server
	std::cout << "* Connecting to the NAT punchthrough server..." << std::endl;
	rakPeer->Connect(this->natPunchServerAddress->ToString(false),
							this->natPunchServerAddress->GetPort(), 0, 0);
}

P2PClient::~P2PClient()
{
	// Stops all networking threads of the RakNet, blocks for a maximum 1000ms
	this->rakPeer->Shutdown(1000);
	
	// Terminate the thread for packet receiving
	this->isListening = false;
	this->listenLoopThread->join();
	delete this->listenLoopThread;
	
	// Cleanup...
	RakNet::RakPeerInterface::DestroyInstance(this->rakPeer);
	delete this->packet;
	delete this->natPunchServerAddress;
}

void P2PClient::listen()
{
	std::cout << "* Started listening for a connection..." << std::endl;
	this->rakPeer->SetMaximumIncomingConnections(1);
}

void P2PClient::attemptNatPunchthrough(std::string remote_guid_)
{
	this->sentNatPunchthroughRequest = true;
	RakNet::RakNetGUID remote;
	remote.FromString(remote_guid_.c_str());
	this->natPunchthroughClient.OpenNAT(remote, *(this->natPunchServerAddress));
}

void P2PClient::sendMessage(std::string message_)
{
	// Just trim the message, just in case.
	message_ = message_.substr(0, this->MAX_USER_MESSAGE_LENGTH);

	RakNet::BitStream bs;
	
	bs.Write(static_cast<unsigned char>(ID_USER_PACKET_ENUM));
	bs.Write(static_cast<unsigned int>(message_.size()));
	bs.Write(message_.c_str(), static_cast<unsigned int>(message_.size()));

	this->rakPeer->Send(&bs, MEDIUM_PRIORITY, RELIABLE, 1,
										this->remotePeer, false);
}

void P2PClient::listenLoop()
{
	// Allocate the buffer for the incoming message string
	char* message = new char[this->MAX_USER_MESSAGE_LENGTH];

	while(this->isListening)
	{
		for (this->packet = this->rakPeer->Receive();
			this->packet;
			this->rakPeer->DeallocatePacket(this->packet),
			this->packet = this->rakPeer->Receive())
		{
			RakNet::BitStream bts(this->packet->data,
									this->packet->length,
									false);

			// Check the packet identifier
			switch(this->packet->data[0])
			{
				case ID_CONNECTION_REQUEST_ACCEPTED:
					if(this->packet->systemAddress == 
												*(this->natPunchServerAddress))
					{
						std::cout << "* Successfully connected to the NAT " <<
											"punchthrough server." << std::endl;
					}
					else if(this->packet->systemAddress == this->remotePeer)
					{
						std::cout << "* Connection to the remote peer " <<
									"successfully established." << std::endl;
					}
				break;
				case ID_NEW_INCOMING_CONNECTION:
					std::cout << "* A peer has connected." << std::endl;
				break;
				case ID_NAT_PUNCHTHROUGH_SUCCEEDED:
					std::cout << "* NAT punchthrough succeeded!" << std::endl;
					this->remotePeer = this->packet->systemAddress;
					
					if(this->sentNatPunchthroughRequest)
					{
						this->sentNatPunchthroughRequest = false;
						std::cout << "* Connecting to the peer..." << std::endl;
						this->rakPeer->Connect(this->remotePeer.ToString(false),
											this->remotePeer.GetPort(), 0, 0);
					}
				break;
				case ID_USER_PACKET_ENUM:
					unsigned char rcv_id;
					bts.Read(rcv_id);
					unsigned int length;
					bts.Read(length);
					bts.Read(message, length);
					std::cout << "* Message received:" << std::endl <<
								message << std::endl;
				break;
				case ID_REMOTE_DISCONNECTION_NOTIFICATION:
					std::cout << "* Remote peer has disconnected." << std::endl;
				break;
				case ID_REMOTE_CONNECTION_LOST:
					std::cout << "* Remote peer lost connection." << std::endl;
				break;
				case ID_DISCONNECTION_NOTIFICATION:
					std::cout << "* A peer has disconnected." << std::endl;
				break;
				case ID_CONNECTION_LOST:
					std::cout << "* A connection was lost." << std::endl;
				break;
				default:
					std::cout << "* Received a packet with unspecified " <<
									"message identifier." << std::endl;
				break;
			} // check package identifier
		} // package receive loop
	} // listening loop
	
	delete[] message;
}

