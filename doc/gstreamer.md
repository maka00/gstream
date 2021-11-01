# gstreamer 

## webcam
```
[use opengl]
gst-launch-1.0 -e --gst-debug-level=2 v4l2src do-timestamp=TRUE device=/dev/video0 ! videoconvert !  glimagesink

[use xv]
gst-launch-1.0 -e --gst-debug-level=2 v4l2src do-timestamp=TRUE device=/dev/video0 ! videoconvert !  xvimagesink

[use autovideo]
gst-launch-1.0 -e --gst-debug-level=2 v4l2src do-timestamp=TRUE device=/dev/video0 ! videoconvert !  autovideosink
```

## display and write to file (encoded)
```
gst-launch-1.0 -v v4l2src device=/dev/video0 \
    ! video/x-raw,format=YUY2,width=640,height=480,framerate=15/1 \
    ! xvimagesink
```

```
gst-launch-1.0 -v v4l2src device=/dev/video0 ! \
    video/x-raw,format=YUY2,width=640,height=480,framerate=15/1 ! \
    tee name=t \
    t. ! queue ! xvimagesink sync=false \
    t. ! queue ! \
    videoconvert ! vp8enc ! \
    matroskamux ! filesink location='raw_dual.mkv' sync=false
```

```
gst-launch-1.0 -v v4l2src device=/dev/video0 \
    ! video/x-raw,format=YUY2,width=640,height=480,framerate=15/1 \
    ! tee name=t \
    t. ! queue ! xvimagesink sync=false \
    t. ! queue ! \
    videoconvert ! x264enc tune=zerolatency ! h264parse ! \
    matroskamux ! filesink location='raw_dual.mkv' sync=false    
```

### Stream WebCAM via UDP and RTP

VP8 encoding:
```
gst-launch-1.0 v4l2src device=/dev/video0 \
    ! video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 \
    ! videoconvert \
    ! vp8enc threads=1 deadline=1 resize-allowed=true \
    ! rtpvp8pay \
    ! udpsink host=127.0.0.1 port=1234

--- or ----

gst-launch-1.0 v4l2src device=/dev/video0 \
    ! video/x-raw,format=YUY2 \
    ! videoconvert \
    ! vp8enc threads=1 deadline=1 resize-allowed=true \
    ! rtpvp8pay \
    ! udpsink host=127.0.0.1 port=1234
     
```
### playback:
```
gst-launch-1.0 udpsrc port=1234 \
    ! 'application/x-rtp, payload=96' \
    ! rtpvp8depay \
    ! vp8dec threads=4 \
    ! videoflip method=rotate-180 \
    ! xvimagesink sync=false

```

### x264 encoding:

```
gst-launch-1.0 -v v4l2src  device=/dev/video0 \
  ! video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 \
  ! videoconvert \
  ! x264enc pass=cbr tune=zerolatency quantizer=50 \
  ! rtph264pay \
  ! udpsink host=127.0.0.1 port=1234

```

```
### gst-launch-1.0 -v udpsrc port=1234 \
    ! 'application/x-rtp, payload=(int)96' \
    ! rtph264depay \
    ! avdec_h264 \
    ! videoconvert \
    ! xvimagesink sync=false

```
## VLC playback [test.spd]
```
c=IN IP4 127.0.0.1
m=video 1234 RTP/AVP 96
a=rtpmap:96 H264/90000
```
also: disable automatic HW acceleration

## NVidia Plugins
```
gst-launch-1.0 -v videotestsrc pattern=ball is-live=true \
        ! video/x-raw,width=2896,height=2896,framerate=1/1 \
        ! queue \
        ! nvvidconv ! 'video/x-raw(memory:NVMM),format=(string)I420' \
        ! nvv4l2h264enc !  'video/x-h264, stream-format=(string)byte-stream' \
        ! h264parse ! \
        filesink location='raw_dual.h264' sync=false   
```

## TS format

### Recording in TS format (using splitmuxsink)
```
gst-launch-1.0 videotestsrc ! x264enc ! h264parse ! queue ! splitmuxsink muxer=mpegtsmux location=./test%02d.ts max-size-time=10000000000
```


### Sender
```
gst-launch-1.0 -v v4l2src ! "video/x-raw,width=800,height=600,framerate=15/1" ! queue ! videoconvert ! queue ! x264enc tune=zerolatency bitrate=3096 byte-stream=true threads=4 key-int-max=15 intra-refresh=true  ! queue ! mpegtsmux name=mux  ! queue ! udpsink host=127.0.0.1 port=5432
```

### Receiver
```
gst-launch-1.0 -v udpsrc  port=5432 ! tsdemux name=demux demux. ! queue !  h264parse ! queue !   avdec_h264 ! queue ! videoconvert ! queue ! autovideosink
```

### Playing TS files with GStreamer
```
gst-launch-1.0 filesrc location=test.ts ! tsdemux  !  h264parse ! avdec_h264 ! videoconvert ! ximagesink
```
