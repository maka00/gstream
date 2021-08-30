#include <iostream>
#include <boost/scope_exit.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <gstreamer.h>

void test_pattern() {
    gstreamer gs("test-pipeline");
    gs.add_element("one", "videotestsrc", "source");
    gs.add_element("one", "vertigotv", "filter");
    gs.add_element("one", "videoconvert", "convert");
    gs.add_element("one", "autovideosink", "sink");
    gs.link_pipeline();
    gs.set_property("source", "pattern",0);
    gs.run();
}

void stream_from_uri() {
    gstreamer gs("test-pipeline");
    gs.add_element("unbound", "uridecodebin", "source");
    gs.add_element("audio", "audioconvert", "convert");
    gs.add_element("audio", "audioresample", "resample");
    gs.add_element("audio", "autoaudiosink", "audiosink");
    gs.add_element("video", "videoconvert", "vconvert");
    gs.add_element("video", "autovideosink", "videosink");
    gs.link_pipeline();
    gs.set_property("source", "uri","https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm");
    gs.set_pad_added_handler("source",[](const std::string& pattern) {
        std::optional<std::string> result;
        if (pattern.find("audio/x-raw") != std::string::npos)
            result = std::string("convert");
        else if (pattern.find("video/x-raw") != std::string::npos)
            result = std::string("vconvert");
        return result;
    });
    gs.run();
}

int main(int argc, char *argv[]) {
    static auto console = spdlog::stdout_color_mt("console");
    spdlog::set_pattern("[%H:%M:%S %z][%s][%!][%#] %v");
    stream_from_uri();
    return 0;
}