#include<gst/gst.h>
#include <iostream>
#include <boost/scope_exit.hpp>

int main_playbin(int argc, char *argv[]) {
    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;
    GError *err = nullptr;

    gst_init(&argc, &argv);
    pipeline = gst_parse_launch(
            "playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
            &err);

    if (err != nullptr) {
        g_printerr("Error: %s\n", err->message);
        g_error_free(err);
        return 1;
    }
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                     static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    if (msg != nullptr)
        gst_message_unref(msg);

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
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

int main_pipeline(int argc, char *argv[]) {
    GstElement *pipeline, *source, *sink;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
    source = gst_element_factory_make("videotestsrc", "source");
    sink = gst_element_factory_make("autovideosink", "sink");
    if (!gst_element_factory_find("vertigotv")) {
        std::cout << "unable to finde vertigotv";
        return -1;
    }
    auto filter = gst_element_factory_make("vertigotv", "filter");
    auto colorspc = gst_element_factory_make("videoconvert", "color");
    /* Create the empty pipeline */
    pipeline = gst_pipeline_new("test-pipeline");

    if (!pipeline || !source || !sink || !filter) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many(GST_BIN (pipeline), source, filter, colorspc, sink, nullptr);
    if (!gst_element_link_many(source, filter, colorspc, sink, nullptr)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Modify the source's properties */
    g_object_set(source, "pattern", 0, nullptr);

    /* Start playing */
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                     static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Parse message */
    parse_message(msg);

    /* Free resources */
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}

/* Structure to contain all our information, so we can pass it to callbacks */
typedef struct _CustomData {
    GstElement *pipeline;
    GstElement *source;
    GstElement *convert;
    GstElement *resample;
    GstElement *sink;
    GstElement *vconvert;
    GstElement *vsink;
} CustomData;

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
        sink_pad = gst_element_get_static_pad(data->convert, "sink");
    } else if (g_str_has_prefix(new_pad_type, video_id.c_str())) {
        sink_pad = gst_element_get_static_pad(data->vconvert, "sink");
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

int main_dyn_pipeline(int argc, char *argv[]) {
    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    gboolean terminate = false;
    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
    data.source = gst_element_factory_make("uridecodebin", "source");
    data.convert = gst_element_factory_make("audioconvert", "convert");
    data.resample = gst_element_factory_make("audioresample", "resample");
    data.sink = gst_element_factory_make("autoaudiosink", "sink");
    data.vconvert = gst_element_factory_make("videoconvert", "vconvert");
    data.vsink = gst_element_factory_make("autovideosink", "vsink");

    /* Create the empty pipeline */
    data.pipeline = gst_pipeline_new("test-pipeline");

    if (!data.pipeline || !data.source || !data.convert || !data.resample || !data.sink || !data.vconvert || !data.vsink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    /* Build the pipeline. Note that we are NOT linking the source at this
     * point. We will do it later. */
    gst_bin_add_many(GST_BIN (data.pipeline), data.source, data.convert, data.resample, data.sink, data.vconvert, data.vsink, nullptr);
    if (!gst_element_link_many(data.convert, data.resample, data.sink, nullptr)) {
        g_printerr("Audio elements could not be linked.\n");
        gst_object_unref(data.pipeline);
        return -1;
    }
    if (!gst_element_link_many(data.vconvert, data.vsink, nullptr)) {
        g_printerr("Video elements could not be linked.\n");
        gst_object_unref(data.pipeline);
        return -1;
    }

    /* Set the URI to play */
    g_object_set(data.source,
                 "uri",
                 "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
                 //"https://192.168.1.50:5001/vs/sharing/SLd8G42P#!dHZzaG93X2VwaXNvZGUtMjgy",
                 nullptr);

    /* Connect to the pad-added signal */
    g_signal_connect (data.source,
                      "pad-added",
                      G_CALLBACK(pad_added_handler),
                      &data);

    /* Start playing */
    ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(data.pipeline);
        return -1;
    }

    /* Listen to the bus */
    bus = gst_element_get_bus(data.pipeline);
    do {
        msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                         static_cast<GstMessageType>(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR |
                                                                     GST_MESSAGE_EOS));
        parse_message(msg);
    } while (!terminate);
    /* Free resources */
    gst_object_unref(bus);
    gst_element_set_state(data.pipeline, GST_STATE_NULL);
    gst_object_unref(data.pipeline);
    return 0;
}


int main(int argc, char *argv[]) {
    //return main_playbin(argc, argv);
    //return main_pipeline(argc, argv);
    return main_dyn_pipeline(argc, argv);
}