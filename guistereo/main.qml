import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtMultimedia 5.8
import com.maka00.classes 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "Minimal Qml"
    menuBar: MenuBar {
        Menu {
            title: qsTr("&Cam")
            Action {
                text: qsTr("&Connect..")
            }
            MenuSeparator {}
            Action {
                text: qsTr("&Quit")
                onTriggered: Qt.quit()
            }
        }
        Menu {
            title: qsTr("&Edit")
            Action {
                text: qsTr("Cu&t")
            }
            Action {
                text: qsTr("&Copy")
            }
            Action {
                text: qsTr("&Paste")
            }
        }
        Menu {
            title: qsTr("&Help")
            Action {
                text: qsTr("&About")
            }
        }
    }
    Camera {
        id: camera
    }

    StereoVideoFilter {
        id: stereoVideoFilter
    }

    GroupBox {
        anchors.fill: parent
        padding: 5
        ColumnLayout {
            anchors.fill: parent
            RowLayout {
            Rectangle {
                //Layout.alignment: Qt.AlignHCenter
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: 50
                color: "red"
                visible: false
            }
            VideoOutput {
                id: video
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: 300
                source: camera
                autoOrientation: false
                filters: [stereoVideoFilter]
            }
            }
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Label {
                    text: "Camera"
                }
                ComboBox {
                    Layout.fillWidth: true
                    model: QtMultimedia.availableCameras
                    textRole: "displayName"
                    onActivated: {
                        camera.stop()
                        camera.deviceId = model[currentIndex].deviceId
                        cameraStartTimer.start()
                    }

                    Timer {
                        id: cameraStartTimer
                        interval: 500
                        running: false
                        repeat: false
                        onTriggered: camera.start()
                    }

                    onAccepted: {
                        console.log("aaa")
                    }
                }
            }
        }
    }
}
