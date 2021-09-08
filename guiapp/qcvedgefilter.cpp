//
// Created by mk on 07.09.21.
//

#include "qcvedgefilter.h"
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QQmlContext>
#include <iostream>
QImage QVideoFrameToQImage( const QVideoFrame& videoFrame )
{
    if ( videoFrame.handleType() == QAbstractVideoBuffer::GLTextureHandle )
    {
        QImage image( videoFrame.width(), videoFrame.height(), QImage::Format_ARGB32 );
        GLuint textureId = static_cast<GLuint>( videoFrame.handle().toInt() );
        QOpenGLContext* ctx = QOpenGLContext::currentContext();
        QOpenGLFunctions* f = ctx->functions();
        GLuint fbo;
        f->glGenFramebuffers( 1, &fbo );
        GLint prevFbo;
        f->glGetIntegerv( GL_FRAMEBUFFER_BINDING, &prevFbo );
        f->glBindFramebuffer( GL_FRAMEBUFFER, fbo );
        f->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0 );
        f->glReadPixels( 0, 0,  videoFrame.width(),  videoFrame.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.bits() );
        f->glBindFramebuffer( GL_FRAMEBUFFER, static_cast<GLuint>( prevFbo ) );
        return image.rgbSwapped();
    } else if ( videoFrame.handleType() == QAbstractVideoBuffer::NoHandle){
        return QImage(videoFrame.image());
    } else {
        std::cout << "test unsupported handle" << std::endl;
    }

    return QImage();
}

QImage Mat2QImage(const cv::Mat & src)
{

    cv::Mat temp;
    cv::cvtColor(src, temp,cv::COLOR_BGR2RGB);
    QImage dest(temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    return dest.copy();
}

cv::Mat QImage2Mat(const QImage & src)
{
    cv::Mat tmp(src.height(),src.width(),CV_8UC4,(uchar*)src.bits(),src.bytesPerLine());
    cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
    cv::cvtColor(tmp, result,cv::COLOR_RGB2BGR);
    return result;
}

QVideoFilterRunnable *qcvedgefilter::createFilterRunnable() {
    return new qcvedgefilter_runnable(this);
}

qcvedgefilter::qcvedgefilter(QObject* parent) : QAbstractVideoFilter(parent), _threshold1(50), _threshold2(30) {
}

qcvedgefilter_runnable::qcvedgefilter_runnable(qcvedgefilter* filter) : _filter(filter), _threshold1(50), _threshold2(30) {
    QObject::connect(_filter,SIGNAL(configChanged()),this,SLOT(configChanged()));
}

QVideoFrame qcvedgefilter_runnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat,
                                        QVideoFilterRunnable::RunFlags flags) {
    Q_UNUSED(flags);
    Q_UNUSED(surfaceFormat);
    if (!input) {
        std::cout << "test null" << std::endl;
        return QVideoFrame{};
    }
    input->map(QAbstractVideoBuffer::ReadOnly);
    QImage image = QVideoFrameToQImage(*input);
    cv::Mat img;
    cv::Mat greyimg, blurredimg;
    cv::Mat edges;
    cv::Mat flipped;
    auto img_frmt = image.format();
    img = QImage2Mat(image);
    int t1 = ( float(_threshold1) * 400.0f ) / 100.0f;
    int t2 = ( float(_threshold2) * 600.0f ) / 100.0f;
    cv::cvtColor(img, greyimg,cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(greyimg,blurredimg,cv::Size(5,5),0);
    cv::Canny(blurredimg,edges,t1,t2,3,false);
    cv::flip(edges,flipped,1);
    QImage image_res;
    image_res = Mat2QImage(flipped);
    image_res = image_res.convertToFormat(img_frmt);
    input->unmap();
    return QVideoFrame{image_res};
}

void qcvedgefilter_runnable::configChanged() {
    _threshold1 = _filter->_threshold1;
    _threshold2 = _filter->_threshold2;
    float t1 = ( float(_threshold1) * 400.0f ) / 100.0f;
    float t2 = ( float(_threshold2) * 600.0f ) / 100.0f;
    std::cout << "config changed [" << _threshold1 << " | " << _threshold2 << "] => using (" << t1 << "|" << t2 << ")" << std::endl;
}
