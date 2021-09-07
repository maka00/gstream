//
// Created by mk on 07.09.21.
//

#ifndef STREAMING_PROJECT_QCVEDGEFILTER_H
#define STREAMING_PROJECT_QCVEDGEFILTER_H

#include <QAbstractVideoFilter>
#include <QVideoFilterRunnable>

class qcvedgefilter : public QAbstractVideoFilter{
    Q_OBJECT
public:
    qcvedgefilter(QObject* parent = nullptr);
    QVideoFilterRunnable *createFilterRunnable() override;
};

class qcvedgefilter_runnable : public QVideoFilterRunnable {
public:
    qcvedgefilter_runnable();
    QVideoFrame run(QVideoFrame* input, const QVideoSurfaceFormat& surfaceFormat, RunFlags flags) override;
};
#endif //STREAMING_PROJECT_QCVEDGEFILTER_H
