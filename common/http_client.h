//
//  Client.h
//  WebSocketChatRoom
//
//  Created by Leonard on 14-7-16.
//  Copyright (c) 2014Äê Leonard. All rights reserved.
//

#ifndef __WebSocketChatRoom__Client__
#define __WebSocketChatRoom__Client__

#include <iostream>
#include <set>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <string>

class HttpClient
	:public boost::enable_shared_from_this<HttpClient>
{
public:
	HttpClient(boost::asio::io_service &service);
	~HttpClient();

	void start();
	void read_handler(const boost::system::error_code &ec);

	void do_hand_shake();
	bool do_receive();
	void do_send(std::string &content);

	boost::asio::ip::tcp::socket &get_socket() { return _socket; }
protected:
	void retrieve_message(std::string &out);
public:
	static void add_client(boost::shared_ptr<HttpClient> client);
	static void del_client(boost::shared_ptr<HttpClient> client);

	static void broadcast(std::string &content);

	static bool base64_encode(std::string &input, std::string &output);
protected:
	boost::asio::ip::tcp::socket _socket;
	bool _hand_shake;
protected:
	static std::set<boost::shared_ptr<HttpClient> > _clients;
};

#endif /* defined(__WebSocketChatRoom__Client__) */