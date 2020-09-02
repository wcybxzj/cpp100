#ifndef _I_NET_EVENT_HPP_
#define _I_NET_EVENT_HPP_

#include "CELL.hpp"
#include "CELLClient.hpp"

class CellServer;

class INetEvent {
public:
	virtual void OnNetLeave(CellClientPtr& pClient) = 0;

	virtual void OnNetMsg(CellServer* pCellServer, CellClientPtr& pClient, netmsg_DataHeader* header) = 0;

	virtual void OnNetJoin(CellClientPtr& pClient) = 0;

	virtual void OnNetRecv(CellClientPtr& pClient) = 0;
};


#endif
