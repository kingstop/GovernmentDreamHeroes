#include "http_server.h"

using namespace boost::asio;

HttpServer::HttpServer(io_service &service, unsigned short port)
	:_acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), port))
{

}

void HttpServer::start()
{
	ClientPtr client(new HttpClient(_acceptor.get_io_service()));
	_acceptor.async_accept(client->get_socket(), boost::bind(&HttpServer::accept_handler, this, client, placeholders::error));
}

void HttpServer::accept_handler(ClientPtr client, const boost::system::error_code &ec)
{
	if (!ec)
	{
		//print connection information
		ip::tcp::endpoint remote = client->get_socket().remote_endpoint();
		std::cout << "Get Connection from:" << remote.address().to_string() << " : " << remote.port() << std::endl;

		HttpClient::add_client(client->shared_from_this());
		client->start();
	}

	start();
}

/*
#include <iostream>
#include "Server.h"

int main(int argc, const char * argv[])
{
boost::asio::io_service service;
Server server(service,2000);

server.start();
service.run();

return 0;
}
*/