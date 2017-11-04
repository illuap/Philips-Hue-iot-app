// MyResource.h: Taken from the helper site on OWL and modified to implement a RESTful API with Wt
// Nicole Chow	nchow24		250725575
// Oct 3, 2017


#pragma once
#ifndef MYRESOURCE_H_
#define MYRESOURCE_H_

#include <Wt/WResource>

class MyResource : public Wt::WResource {
public:
	MyResource();
	virtual ~MyResource();
protected:
	virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};

#endif /* MYPRESOURCE_H_ */
