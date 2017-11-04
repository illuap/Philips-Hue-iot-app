// MyResource.cpp : Taken from the helper site on OWL and modified to implement a RESTful API with Wt

#include <iostream>
#include <fstream>
#include <string>
#include <Wt/Http/Response>
#include "MyResource.h"

using namespace std;
using namespace Wt::Http;

MyResource::MyResource() {
}

MyResource::~MyResource() {
}

void MyResource::handleRequest(const Request& request, Response& response) {

	//get the requested ID from the URL (if it exists) and give full details about a specific person
	//if there is no ID specified, give general information about all Persons
	//if ID is out of bounds, give error message
	string path = request.pathInfo();							//ID parameter
	string list = "{ <br/> \"people\": [ <br />";												//information to be displayed
	string title;												

	//give general information about Persons
	if (path.empty()) {
		title = "ADDRESS BOOK - GENERAL INFORMATION:";
	}
	else {
		//get the Id from the URL
		string param = path.substr(1, path.size() - 1);
		int id = atoi(param.c_str());

		//get full Person details from the Id
		if (id > 5) {
			title = "ADDRESS BOOK - SPECIFIC PERSON INFORMATION:";

		} else {
			//error message for nonexistent ID
			list = "Person with ID: " + param + " does not exist";
		}
	}

	//taken from the helper link on OWL
	int contentLength = request.contentLength();
	char* buffer = new char[contentLength + 1];
	request.in().read(buffer, contentLength);
	buffer[contentLength] = 0;
	response.setMimeType("application/xml");
	ostream& out = response.out();

	//display on the webpage
	out << "<?xml version='1.0' encoding='utf-8' ?>" << endl;
	out << "<?xml-stylesheet href='my-style.css' ?>" << endl;
	out << "<reply>" << endl;
	out << "<method>" << title << "</method>" << endl;
	out << "<string>" << list << "</string>" << endl;
	out << "</reply>";

	delete[] buffer;
}
