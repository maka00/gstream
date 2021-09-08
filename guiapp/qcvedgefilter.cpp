//
// Created by mk on 07.09.21.
//

#include "qcvedgefilter.h"
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

QVideoFilterRunnable *qcvedgefilter::createFilterRunnable() {
    return new qcvedgefilter_runnable();
}

qcvedgefilter::qcvedgefilter(QObject* parent) : QAbstractVideoFilter(parent) {

}

qcvedgefilter_runnable::qcvedgefilter_runnable() {

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
    QTransform myTransform;
    myTransform.rotate(180);
    image = image.transformed(myTransform);
    input->unmap();
    return QVideoFrame(image);
}
