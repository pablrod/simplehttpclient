/**
 * @file simple_http_client.cpp
 *
 * @date 2015-02-03
 *
 * @author Pablo Rodríguez González
 *
 */

#include <boost/program_options.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using namespace std;

void ParseCommandLine(int argc, const char *argv[], string &host, int &port, string &path);

int main(int argc, const char *argv[]) {
	int port;
	string host;
	string path;
	ParseCommandLine(argc, argv, host, port, path);

	try {
		using namespace boost::asio;
		using namespace boost::asio::ip;

		io_service communication_service;
		auto server = tcp::resolver(communication_service).resolve(
				boost::asio::ip::tcp::resolver::query(host, to_string(port)));

		tcp::socket connection(communication_service);
		connect(connection, server);
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "GET " << path << " HTTP/1.0\r\n";
		request_stream << "Host: " << host << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";
		write(connection, request);
		boost::asio::streambuf response;
		read_until(connection, response, "\r\n");

		cout << &response << endl;
	} catch (exception &e) {
		cerr << "An exception has ocurred: " << e.what() << endl;
	}

	return 0;
}

void ParseCommandLine(int argc, const char *argv[], string &host, int &port, string &path) {

	using namespace boost::program_options;
	using boost::program_options::error;

	options_description command_line_description("Arguments");
	command_line_description.add_options()("port",
			value<int>(&port)->default_value(80), "Port number")("host",
			value<string>(&host)->required(), "Host name: www.example.com")
			("path", value<string>(&path)->default_value("/", "Server path"));

	try {
		variables_map arguments;
		store(parse_command_line(argc, argv, command_line_description),
				arguments);
		notify(arguments);

	} catch (error &wrong_arguments) {
		cerr << wrong_arguments.what() << endl;
		cerr << command_line_description << endl;
		exit(EXIT_FAILURE);
	}
}
