//
//  Client.cpp
//  WebSocketChatRoom
//
//  Created by Leonard on 14-7-16.
//  Copyright (c) 2014Äê Leonard. All rights reserved.
//

#include "http_client.h"
#include <vector>
#include <istream>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/uuid/sha1.hpp>
//#include <arpa/inet.h>

using namespace boost::asio;

std::set<boost::shared_ptr<HttpClient> > HttpClient::_clients;

HttpClient::HttpClient(io_service &service)
	:_socket(service), _hand_shake(false)
{
}

HttpClient::~HttpClient()
{
	ip::tcp::endpoint remote = _socket.remote_endpoint();
	std::cout << "End Connection with: " << remote.address().to_string() << ":" << remote.port() << std::endl;
	std::cout << "There are total " << _clients.size() << " client" << std::endl;
}

void HttpClient::start()
{
	_socket.async_read_some(null_buffers(), boost::bind(&HttpClient::read_handler, shared_from_this(), placeholders::error));
}

void HttpClient::read_handler(const boost::system::error_code &ec)
{
	if (!ec)
	{
		size_t size = _socket.available();
		if (size != 0)
		{
			bool flag = true;

			if (!_hand_shake)
				do_hand_shake();
			else
				flag = do_receive();

			if (flag)
				start();
			else
				del_client(shared_from_this());
		}
		else
			del_client(shared_from_this());
	}
}

void HttpClient::do_hand_shake()
{
	streambuf content;
	boost::system::error_code ec;
	std::size_t length = read_until(_socket, content, "\r\n\r\n", ec);
	if (ec)
		return;

	streambuf::const_buffers_type bufs = content.data();
	std::string lines(buffers_begin(bufs), buffers_begin(bufs) + length);
	//std::cout<<lines<<std::endl;

	std::string response, key, encrypted_key;

	//find the Sec-WebSocket-Key
	size_t pos = lines.find("Sec-WebSocket-Key");
	if (pos == lines.npos)
		return;
	size_t end = lines.find("\r\n", pos);
	key = lines.substr(pos + 19, end - pos - 19) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	//get the base64 encode string with sha1
	boost::uuids::detail::sha1 sha1;
	sha1.process_bytes(key.c_str(), key.size());
	unsigned int digest[5];
	sha1.get_digest(digest);
	key.clear();
	for (int i = 0; i<5; i++)
	{
		const char* tmp = reinterpret_cast<char*>(digest);
		key[i * 4] = tmp[i * 4 + 3];
		key[i * 4 + 1] = tmp[i * 4 + 2];
		key[i * 4 + 2] = tmp[i * 4 + 1];
		key[i * 4 + 3] = tmp[i * 4];
	}
	std::string first;
	for (int i = 0; i<20; i++)
	{
		first.append(1, key[i]);
	}
	base64_encode(first, encrypted_key);

	//set the response text
	response.append("HTTP/1.1 101 WebSocket Protocol Handshake\r\n");
	response.append("Upgrade: websocket\r\n");
	response.append("Connection: Upgrade\r\n");
	response.append("Sec-WebSocket-Accept: " + encrypted_key + "\r\n");
	response.append("Sec-WebSocket-Version: 13\r\n\r\n");

	_socket.send(buffer(response));
	_hand_shake = true;
}

bool HttpClient::do_receive()
{
	std::string result;
	retrieve_message(result);

	if (result.size() == 0)
	{
		//std::cout<<"Get Error from client"<<std::endl;
		return false;
	}
	broadcast(result);
	std::cout << "Get Message from client:" << result << std::endl;
	return true;
}

void HttpClient::do_send(std::string &content)
{
	if (!_socket.is_open())
		return;
	std::vector<char> data;

	//fin opcode
	data.push_back(0x81);

	//length
	if (content.length() >= 126)
	{
		data.push_back(126);
		unsigned short length = htons(content.length());
		data.push_back(length & 0xff);
		data.push_back(length >> 8 & 0xff);
	}
	else
		data.push_back(content.length());

	//set mask
	data[1] = data[1] & 0xff;

	for (int i = 0; i<content.size(); i++)
	{
		data.push_back(content[i]);
	}

	_socket.send(buffer(data));
}

void HttpClient::retrieve_message(std::string &output)
{
	//receive fin and opcode
	unsigned char fin_opcode;
	_socket.receive(buffer(&fin_opcode, sizeof(fin_opcode)));
	unsigned char fin = fin_opcode >> 7;
	unsigned char opcode = fin_opcode & 0x7f;
	if (fin == char(0x00))
	{
		printf("%02x\n", fin);
		return;
	}
	switch (opcode)
	{
	case 0x01:
	case 0x02:
		break;
	case 0x08://client send close
			  //_socket.close();
		return;
	case 0x09://client send ping
		return;
	case 0x0A://client send pong
		return;
	default:
		return;
	}

	//get payload_len
	unsigned char payload_len;
	_socket.receive(buffer(&payload_len, sizeof(payload_len)));
	unsigned char is_mask = payload_len >> 7;
	unsigned short length = 0;
	uint64_t tmp = payload_len & 0x7f;
	if (tmp == 126)
	{
		uint16_t len;
		_socket.receive(buffer(&len, sizeof(len)));
		length = ntohs(len);
	}
	else if (tmp == 127)
	{
		_socket.receive(buffer(&tmp, sizeof(tmp)));
		length = ntohs(tmp);
	}
	else
		length = static_cast<unsigned short>(tmp);

	//get mask if exists
	char mask[4];
	if (is_mask)
	{
		_socket.receive(buffer(mask, sizeof(mask)));
	}

	//get data
	if (length == 0)
		return;
	char *data = new char[length];
	_socket.receive(buffer(data, length));
	if (is_mask)
	{
		for (uint64_t i = 0; i<length; i++)
		{
			data[i] = data[i] ^ mask[i % 4];
		}
	}

	output.append(data, length);
}

void HttpClient::add_client(boost::shared_ptr<HttpClient> client)
{
	_clients.insert(client);
}

void HttpClient::del_client(boost::shared_ptr<HttpClient> client)
{
	_clients.erase(client);
}

void HttpClient::broadcast(std::string &content)
{
	for (std::set<boost::shared_ptr<HttpClient> >::const_iterator iter = _clients.begin(); iter != _clients.end(); iter++)
	{
		(*iter)->do_send(content);
	}
}

bool HttpClient::base64_encode(std::string &input, std::string &output)
{
	typedef boost::archive::iterators::base64_from_binary<boost::archive::iterators::transform_width<std::string::const_iterator, 6, 8> > Base64EncodeIterator;
	std::stringstream result;
	copy(Base64EncodeIterator(input.begin()), Base64EncodeIterator(input.end()), std::ostream_iterator<char>(result));
	size_t equal_count = (3 - input.length() % 3) % 3;
	for (size_t i = 0; i < equal_count; i++)
	{
		result.put('=');
	}
	output = result.str();

	return output.empty() == false;
}