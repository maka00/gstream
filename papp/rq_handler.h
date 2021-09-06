//
// Created by mk on 06.09.21.
//

#ifndef STREAMING_PROJECT_RQ_HANDLER_H
#define STREAMING_PROJECT_RQ_HANDLER_H
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <opencv2/videoio.hpp>

class rq_handler : public Poco::Net::HTTPRequestHandler {
public:
    virtual void handleRequest(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp);

private:
    int count;
};

class rq_handler_opencv : public Poco::Net::HTTPRequestHandler {
public:
    virtual void handleRequest(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp);

private:
    cv::VideoCapture cam;
};

class rq_handler_error: public Poco::Net::HTTPRequestHandler {
public:
    virtual void handleRequest(Poco::Net::HTTPServerRequest& req, Poco::Net::HTTPServerResponse &resp);

private:
    int count;
};

class rq_factory : public Poco::Net::HTTPRequestHandlerFactory {
public:
    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& rq) {
        if (rq.getURI() == "/")
            return new rq_handler;
        else if (rq.getURI() == "/video")
            return new rq_handler_opencv;
        else
            return new rq_handler_error;
    }
};
#endif //STREAMING_PROJECT_RQ_HANDLER_H
