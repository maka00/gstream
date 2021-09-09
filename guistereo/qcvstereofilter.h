//
// Created by mk on 07.09.21.
//

#ifndef STREAMING_PROJECT_QCVSTEREOFILTER_H
#define STREAMING_PROJECT_QCVSTEREOFILTER_H

#include <QAbstractVideoFilter>
#include <QVideoFilterRunnable>
#include <opencv4/opencv2/opencv.hpp>

class qcvstereofilter : public QAbstractVideoFilter{
    friend class qcvedgefilter_runnable;
    Q_OBJECT
public:
    qcvstereofilter(QObject* parent = nullptr);
    QVideoFilterRunnable *createFilterRunnable() override;
private:
public:
    signals:
};

class qcvstereofilter_runnable : public QObject, public QVideoFilterRunnable {
    Q_OBJECT
public:
    qcvstereofilter_runnable(qcvstereofilter* filter);
    QVideoFrame run(QVideoFrame* input, const QVideoSurfaceFormat& surfaceFormat, RunFlags flags) override;
private:
    qcvstereofilter* _filter;
    cv::Mat _prev_img;
};
#endif //STREAMING_PROJECT_QCVSTEREOFILTER_H
