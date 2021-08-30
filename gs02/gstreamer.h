//
// Created by mk on 30.08.21.
//

#ifndef STREAMING_PROJECT_GSTREAMER_H
#define STREAMING_PROJECT_GSTREAMER_H


#include <vector>
#include <string>
#include <map>
#include <variant>
#include <gst/gst.h>
#include <functional>
#include <optional>

class gstreamer {
public:
    explicit gstreamer(const std::string &pipeline_name);

    virtual ~gstreamer();

    void add_element(const std::string &bin_name, const std::string &plugin_name, const std::string &element_name);

    void link_pipeline();

    void run();

    void stop();

    void
    set_property(const std::string &element, const std::string &property, std::variant<int, float, std::string> value);

    void set_pad_added_handler(const std::string &element,
                               const std::function<std::optional<std::string>(const std::string &)>& matcher);

    static void pad_added_handler(GstElement *src, GstPad *new_pad,
                                  std::tuple<std::map<std::string, GstElement *>, std::function<std::optional<std::string>(
                                          const std::string &)>> *data);

private:
    void post_fix();

    void parse_message(GstMessage *msg);

    std::map<std::string, std::vector<std::pair<std::string, GstElement *>>> _pipelines;
    std::map<std::string, GstElement *> _elements;
    std::tuple<std::map<std::string, GstElement *>, std::function<std::optional<std::string>(
            const std::string &)>> _matcher;
    std::string _element_to_postfix;
    bool _stopped;
    GstElement *_pipeline;

};


#endif //STREAMING_PROJECT_GSTREAMER_H
