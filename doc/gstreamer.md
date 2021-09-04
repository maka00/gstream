# gstreamer 

## webcam
```
gst-launch-1.0 -e --gst-debug-level=2 v4l2src do-timestamp=TRUE device=/dev/video0 ! videoconvert !  glimagesink
```
