#include<gst/gst.h>
#include <iostream>
#include <boost/scope_exit.hpp>

int main_threading(int argc, char *argv[]) {
    GstElement *pipeline, *audio_source, *tee, *audio_queue, *audio_convert, *audio_resample, *audio_sink;
    GstElement *video_queue, *visual, *video_convert, *video_sink;
    GstBus *bus;
    GstMessage *msg;
    GstPad *tee_audio_pad, *tee_video_pad;
    GstPad *queue_audio_pad, *queue_video_pad;

    gst_init(&argc, &argv);

    audio_source = gst_element_factory_make("audiotestsrc", "audio_source");
    tee = gst_element_factory_make("tee", "tee");
    audio_queue = gst_element_factory_make("queue", "audio_queue");
    audio_convert = gst_element_factory_make("audioconvert", "audio_convert");
    audio_resample = gst_element_factory_make("audioresample", "audio_resample");
    audio_sink = gst_element_factory_make("autoaudiosink", "audio_sink");
    video_queue = gst_element_factory_make("queue", "video_queue");
    visual = gst_element_factory_make("wavescope", "visual");
    video_convert = gst_element_factory_make("videoconvert", "video_convert");
    video_sink = gst_element_factory_make("autovideosink", "video_sink");

    pipeline = gst_pipeline_new("queue-pipeline");

    if (!pipeline || !audio_source || !tee || !audio_queue || !audio_convert || !audio_resample || !audio_sink ||
        !video_queue || !visual || !video_convert || !video_sink) {
        g_printerr("Not all elements could be created!\n");
        return -1;
    }

    g_object_set(audio_source, "freq", 215.0f, nullptr);
    g_object_set(visual, "shader",0, "style", 1, nullptr);

    gst_bin_add_many(GST_BIN(pipeline), audio_source, tee, audio_queue, audio_convert, audio_resample, audio_sink,
                     video_queue, visual, video_convert, video_sink, nullptr);
    if(gst_element_link_many(audio_source, tee, nullptr) != true ||
            gst_element_link_many(audio_queue, audio_convert, audio_resample, audio_sink, nullptr) != true ||
            gst_element_link_many(video_queue, visual, video_convert, video_sink, nullptr) != true) {
        g_printerr("Tee could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    tee_audio_pad = gst_element_get_request_pad(tee, "src_%u");
    g_print("Obtained request pad %s for audio branch.\n", gst_pad_get_name(tee_audio_pad));
    queue_audio_pad = gst_element_get_static_pad(audio_queue, "sink");
    tee_video_pad = gst_element_get_request_pad(tee,"src_%u");
    g_print("Obtained request pad %s for video branch.\n", gst_pad_get_name(tee_video_pad));
    queue_video_pad = gst_element_get_static_pad(video_queue, "sink");
    if(gst_pad_link(tee_audio_pad, queue_audio_pad) != GST_PAD_LINK_OK ||
       gst_pad_link(tee_video_pad, queue_video_pad) != GST_PAD_LINK_OK ) {
        g_printerr("Tee could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    gst_object_unref(queue_audio_pad);
    gst_object_unref(queue_video_pad);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    bus = gst_element_get_bus(pipeline);

    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                     static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (pipeline), GST_DEBUG_GRAPH_SHOW_ALL,"pipeline");

    gst_element_release_request_pad(tee, tee_audio_pad);
    gst_element_release_request_pad(tee, tee_video_pad);
    gst_object_unref(tee_audio_pad);
    gst_object_unref(tee_video_pad);

    if (msg != nullptr)
        gst_message_unref(msg);

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}
struct CustomData {
    GstElement *pipeline;
    GstElement *uri_decode;
    GstElement *tee;
    GstElement *audio_queue;
    GstElement *audio_convert;
    GstElement *audio_resample;
    GstElement *audio_sink;
    GstElement *video_queue;
    //GstElement *visual;
    GstElement *video_convert;
    GstElement *video_sink;

};

void parse_message(GstMessage *msg);

/* Handler for the pad-added signal */
static void pad_added_handler(GstElement *src, GstPad *new_pad, CustomData *data) {
    GstPad *sink_pad = nullptr; //gst_element_get_static_pad(data->convert, "sink");
    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = nullptr;
    GstStructure *new_pad_struct = nullptr;
    const gchar *new_pad_type = nullptr;
    BOOST_SCOPE_EXIT_ALL(new_pad_caps, sink_pad) {
                                                     if (new_pad_caps != nullptr)
                                                         gst_caps_unref(new_pad_caps);

                                                     /* Unreference the sink pad */
                                                     if (sink_pad != nullptr)
                                                         gst_object_unref(sink_pad);
                                                 };

    g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

    const std::string audio_id{"audio/x-raw"};
    const std::string video_id{"video/x-raw"};

    /* Check the new pad's type */
    new_pad_caps = gst_pad_get_current_caps(new_pad);
    new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    new_pad_type = gst_structure_get_name(new_pad_struct);
    if (g_str_has_prefix(new_pad_type, audio_id.c_str())) {
        // sink_pad = gst_element_get_static_pad(data->tee, "sink");
        return;
    } else if (g_str_has_prefix(new_pad_type, video_id.c_str())) {
        sink_pad = gst_element_get_static_pad(data->tee, "sink");
    } else {
        g_print("It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type);
        return;

    }
    /* If our converter is already linked, we have nothing to do here */
    if (gst_pad_is_linked(sink_pad)) {
        g_print("We are already linked. Ignoring.\n");
        return;
    }

    /* Attempt the link */
    ret = gst_pad_link(new_pad, sink_pad);
    if (GST_PAD_LINK_FAILED (ret)) {
        g_print("Type is '%s' but link failed.\n", new_pad_type);
    } else {
        g_print("Link succeeded (type '%s').\n", new_pad_type);
        GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (data->pipeline), GST_DEBUG_GRAPH_SHOW_ALL,"pipeline");

    }
}

int main_video_threading(int argc, char *argv[]) {
    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    GstPad *tee_audio_pad, *tee_video_pad;
    GstPad *queue_audio_pad, *queue_video_pad;

    gst_init(&argc, &argv);

    data.uri_decode = gst_element_factory_make("uridecodebin", "source");
    data.tee = gst_element_factory_make("tee", "tee");
    data.audio_queue = gst_element_factory_make("queue", "audio_queue");
    data.audio_convert = gst_element_factory_make("audioconvert", "audio_convert");
    data.audio_resample = gst_element_factory_make("audioresample", "audio_resample");
    data.audio_sink = gst_element_factory_make("autoaudiosink", "audio_sink");
    data.video_queue = gst_element_factory_make("queue", "video_queue");
    //data.visual = gst_element_factory_make("wavescope", "visual");
    data.video_convert = gst_element_factory_make("videoconvert", "video_convert");
    data.video_sink = gst_element_factory_make("autovideosink", "video_sink");

    data.pipeline = gst_pipeline_new("queue-pipeline");

    if (!data.pipeline || !data.tee || !data.audio_queue || !data.audio_convert || !data.audio_resample || !data.audio_sink ||
        !data.video_queue || !data.video_convert || !data.video_sink) {
        g_printerr("Not all elements could be created!\n");
        return -1;
    }

    gst_bin_add_many(GST_BIN(data.pipeline), data.uri_decode, data.tee, data.audio_queue, data.audio_convert, data.audio_resample, data.audio_sink,
                     data.video_queue, data.video_convert, data.video_sink, nullptr);
    /*
    if(gst_element_link_many(data.uri_decode, data.tee, nullptr) != true) {
        g_printerr("Tee input parts could not be linked.\n");
        gst_object_unref(data.pipeline);
        return -1;
    } */
    /*
    if(gst_element_link_many(data.audio_queue, data.audio_convert, data.audio_resample, data.audio_sink, nullptr) != true) {
        g_printerr("Tee audio parts could not be linked.\n");
        gst_object_unref(data.pipeline);
        return -1;
    }
     */
    if(gst_element_link_many(data.video_queue, data.video_convert, data.video_sink, nullptr) != true) {
        g_printerr("Tee video parts could not be linked.\n");
        gst_object_unref(data.pipeline);
        return -1;
    }
    g_object_set(data.uri_decode,
                 "uri",
                 "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
                 nullptr);

    /* Connect to the pad-added signal */
    g_signal_connect (data.uri_decode,
                      "pad-added",
                      G_CALLBACK(pad_added_handler),
                      &data);

    tee_audio_pad = gst_element_get_request_pad(data.tee, "src_%u");
    g_print("Obtained request pad %s for audio branch.\n", gst_pad_get_name(tee_audio_pad));
    queue_audio_pad = gst_element_get_static_pad(data.audio_queue, "sink");
    tee_video_pad = gst_element_get_request_pad(data.tee,"src_%u");
    g_print("Obtained request pad %s for video branch.\n", gst_pad_get_name(tee_video_pad));
    queue_video_pad = gst_element_get_static_pad(data.video_queue, "sink");
    /*
    if(gst_pad_link(tee_audio_pad, queue_audio_pad) != GST_PAD_LINK_OK) {
        g_printerr("Audio Tee could not be linked.\n");
        gst_object_unref(data.pipeline);
        return -1;
    }
     */
    if(gst_pad_link(tee_video_pad, queue_video_pad) != GST_PAD_LINK_OK ) {
        g_printerr("Video Tee could not be linked.\n");
        gst_object_unref(data.pipeline);
        return -1;
    }

    gst_object_unref(queue_audio_pad);
    gst_object_unref(queue_video_pad);

    gst_element_set_state(data.pipeline, GST_STATE_PLAYING);

    bus = gst_element_get_bus(data.pipeline);
    bool terminate{false};
    do {
        msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                         static_cast<GstMessageType>(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR |
                                                                     GST_MESSAGE_EOS));
        parse_message(msg);
    } while (!terminate);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                     static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN (data.pipeline), GST_DEBUG_GRAPH_SHOW_ALL,"pipeline");

    gst_element_release_request_pad(data.tee, tee_audio_pad);
    gst_element_release_request_pad(data.tee, tee_video_pad);
    gst_object_unref(tee_audio_pad);
    gst_object_unref(tee_video_pad);

    if (msg != nullptr)
        gst_message_unref(msg);

    gst_object_unref(bus);
    gst_element_set_state(data.pipeline, GST_STATE_NULL);
    gst_object_unref(data.pipeline);
    return 0;
}

void parse_message(GstMessage *msg) {
    if (msg != nullptr) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE (msg)) {
            case GST_MESSAGE_ERROR: {
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error received from element %s: %s\n",
                           GST_OBJECT_NAME (msg->src), err->message);
                g_printerr("Debugging information: %s\n",
                           debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
            }
                break;
            case GST_MESSAGE_EOS:
                g_print("End-Of-Stream reached.\n");
                break;
            default:
                /* We should not reach here because we only asked for ERRORs and EOS */
                g_printerr("Unexpected message received.\n");
                break;
        }
        gst_message_unref(msg);
    }
}

int main(int argc, char *argv[]) {
    return main_video_threading(argc, argv);
}