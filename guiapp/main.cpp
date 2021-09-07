#include <QApplication>
#include <QQmlApplicationEngine>
#include <qcvedgefilter.h>
int main(int argc, char** argv)
{
    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
    QApplication app(argc, argv);
    qmlRegisterType< qcvedgefilter >("com.maka00.classes", 1, 0, "EdgeVideoFilter" );
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}