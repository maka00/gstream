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

gst-launch-1.0 -v v4l2src device=/dev/video0 ! \
    video/x-raw,format=YUY2,width=640,height=480,framerate=15/1 ! \
    tee name=t \
    t. ! queue ! xvimagesink sync=false \
    t. ! queue ! \
    videoconvert ! vp8enc ! \
    matroskamux ! filesink location='raw_dual.mkv' sync=false
    
gst-launch-1.0 -v v4l2src device=/dev/video0 \
    ! video/x-raw,format=YUY2,width=640,height=480,framerate=15/1 \
    ! tee name=t \
    t. ! queue ! xvimagesink sync=false \
    t. ! queue ! \
    videoconvert ! x264enc tune=zerolatency ! h264parse ! \
    matroskamux ! filesink location='raw_dual.mkv' sync=false    
```
