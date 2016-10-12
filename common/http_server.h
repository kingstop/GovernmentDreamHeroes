#ifndef __WebSocketChatRoom__Server__
#define __WebSocketChatRoom__Server__

#include <iostream>
#include <boost/asio.hpp>
#include "http_client.h"

typedef boost::shared_ptr<HttpClient>  ClientPtr;

class HttpServer
{
public:
	HttpServer(boost::asio::io_service &service, unsigned short port);

	void start();
	void accept_handler(ClientPtr client, const boost::system::error_code &ec);
protected:
	boost::asio::ip::tcp::acceptor _acceptor;
};

#endif /* defined(__WebSocketChatRoom__Server__) */