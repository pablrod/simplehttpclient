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

using namespace std;

void ParseCommandLine(int argc, const char *argv[], string &host, int &port);

int main(int argc, const char *argv[]) {
	int port;
	string host;
	ParseCommandLine(argc, argv, host, port);

	return 0;
}

void ParseCommandLine(int argc, const char *argv[], string &host, int &port) {

	using namespace boost::program_options;
	using boost::program_options::error;

	options_description command_line_description("Arguments");
	command_line_description.add_options()
	("port", value<int>(&port)->default_value(80), "Port number")
	("host", value<string>(&host)->required(), "Host name: www.example.com");

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
