//
// Created by mk on 06.09.21.
//

#include "rq_handler.h"
#include <iostream>
#include <Poco/Util/Application.h>
#include <Poco/Base64Encoder.h>
#include <Poco/StreamCopier.h>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <fmt/format.h>
#include <strstream>

void rq_handler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp) {
    resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    resp.setContentType("text/html");
    std::ostream &out = resp.send();
    out << "<body>";
    out << "<div class=\"container\">";
    out << "<div class=\"row\">";
    out << "<div class=\"col-lg-8  offset-lg-2\">";
    out << "<h3 class=\"mt-5\">Live Streaming</h3>";
    out << "<img src=\"" << "/video" << "\" width=\"100%\">";
    out << "</div> </div> </div> </body>";
    out.flush();
}

void rq_handler_opencv::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp) {
    resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    resp.setContentType("image/jpeg");

    Poco::Util::Application::instance().logger().information("get video call");

    if (!cam.isOpened()) {
        cam.open(0);
    }
    cv::Mat img;
    if (cam.read(img)) {
        std::vector<unsigned char> buf;
        cv::imwrite("test.jpg", img);
        cv::imencode(".jpg",img,buf);
        resp.sendBuffer(buf.data(),buf.size());
        return;
    }
    resp.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
    resp.setContentType("image/jpeg");

}

void rq_handler_error::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp) {
    resp.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
    resp.setContentType("text/html");
    std::ostream &out = resp.send();
    out << "<h1>Error</h1>";
    out.flush();
}
