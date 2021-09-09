#include <QApplication>
#include <QQmlApplicationEngine>
#include <qcvstereofilter.h>
int main(int argc, char** argv)
{
    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
    QApplication app(argc, argv);
    qmlRegisterType< qcvstereofilter >("com.maka00.classes", 1, 0, "StereoVideoFilter" );
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}