//
// Created by mk on 07.09.21.
//

#ifndef STREAMING_PROJECT_QCVEDGEFILTER_H
#define STREAMING_PROJECT_QCVEDGEFILTER_H

#include <QAbstractVideoFilter>
#include <QVideoFilterRunnable>

class qcvedgefilter : public QAbstractVideoFilter{
    friend class qcvedgefilter_runnable;
    Q_OBJECT
    Q_PROPERTY(int threshold1 READ threshold1 WRITE setthreshold1 NOTIFY configChanged )
    Q_PROPERTY(int threshold2 READ threshold2 WRITE setthreshold2 NOTIFY configChanged )
public:
    qcvedgefilter(QObject* parent = nullptr);
    QVideoFilterRunnable *createFilterRunnable() override;
private:
    int _threshold1;
    int _threshold2;
public:
    int threshold1() const {
        return _threshold1;
    }

    void setthreshold1(int threshold1) {
        _threshold1 = threshold1;
        emit configChanged();
    }

    int threshold2() const {
        return _threshold2;
    }

    void setthreshold2(int threshold2) {
        _threshold2 = threshold2;
        emit configChanged();
    }

    signals:
    void configChanged();
};

class qcvedgefilter_runnable : public QObject, public QVideoFilterRunnable {
    Q_OBJECT
public:
    qcvedgefilter_runnable(qcvedgefilter* filter);
    QVideoFrame run(QVideoFrame* input, const QVideoSurfaceFormat& surfaceFormat, RunFlags flags) override;
public slots:
    void configChanged();
private:
    qcvedgefilter* _filter;
    int _threshold1;
    int _threshold2;
};
#endif //STREAMING_PROJECT_QCVEDGEFILTER_H
