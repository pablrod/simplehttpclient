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
#include <boost/algorithm/string.hpp>

using namespace std;

class HTTPGetRequest {
public:
	HTTPGetRequest(const string &host, const string &path) :
			host(host), path(path) {

	}

	const string& Host() const {
		return host;
	}

	const string& Path() const {
		return path;
	}

private:
	string host;
	string path;
};

std::ostream & operator<<(std::ostream &os, const HTTPGetRequest &request) {
	os << "GET " << request.Path() << " HTTP/1.0\r\n";
	os << "Host: " << request.Host() << "\r\n";
	os << "Accept: */*\r\n";
	os << "Connection: close\r\n\r\n";
	return os;
}

class HTTPResponse {
public:
	HTTPResponse(istream &response_stream) {
		response_stream >> http_version;
		response_stream >> status_code;
		getline(response_stream, status_message); // Extra space at beginning
		string header;
		using namespace boost;
		while (getline(response_stream, header)) {
			auto separator_position = header.find_first_of(":");
			if (separator_position != string::npos) {
				headers[header.substr(0, separator_position)] = header.substr(
						separator_position + 2, string::npos);
			}
		}
	}

	const map<string, string>& Headers() const {
		return headers;
	}

	const string& HttpVersion() const {
		return http_version;
	}

	unsigned int StatusCode() const {
		return status_code;
	}

	const string& StatusMessage() const {
		return status_message;
	}

private:
	string http_version;
	unsigned int status_code;
	string status_message;
	map<string, string> headers;
};

std::ostream & operator<<(std::ostream &os, const HTTPResponse &response) {
	os << "HTTPResponse: " << endl;
	os << response.HttpVersion() << " ";
	os << response.StatusCode() << " ";
	os << response.StatusMessage() << endl;
	auto headers = response.Headers();
	for_each(headers.begin(), headers.end(),
			[&os](const pair<string, string> &pair) {
				os << pair.first << ": " << pair.second << endl;
			});
	return os;
}

void ParseCommandLine(int argc, const char *argv[], string &host, int &port,
		string &path);

int main(int argc, const char *argv[]) {
	int port;
	string host;
	string path;
	ParseCommandLine(argc, argv, host, port, path);

	try {
		using namespace boost::asio;
		using namespace boost::asio::ip;
		using boost::asio::streambuf;

		io_service communication_service;
		auto server = tcp::resolver(communication_service).resolve(
				boost::asio::ip::tcp::resolver::query(host, to_string(port)));

		tcp::socket connection(communication_service);
		connect(connection, server);
		streambuf request;
		ostream request_stream(&request);
		HTTPGetRequest http_get(host, path);
		request_stream << http_get;
		write(connection, request);
		streambuf response;
		read_until(connection, response, "\r\n");

		istream response_stream(&response);
		HTTPResponse http_response(response_stream);
		cout << http_response;

	} catch (exception &e) {
		cerr << "An exception has ocurred: " << e.what() << endl;
	}

	return 0;
}

void ParseCommandLine(int argc, const char *argv[], string &host, int &port,
		string &path) {

	using namespace boost::program_options;
	using boost::program_options::error;

	options_description command_line_description("Arguments");
	command_line_description.add_options()("port",
			value<int>(&port)->default_value(80), "Port number")("host",
			value<string>(&host)->required(), "Host name: www.example.com")(
			"path", value<string>(&path)->default_value("/", "Server path"));

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
