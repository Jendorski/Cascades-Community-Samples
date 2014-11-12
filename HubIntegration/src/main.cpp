#include <bb/cascades/Application>

#include <QLocale>
#include <QTranslator>
#include <bb/system/InvokeManager>
#include "applicationui.hpp"

#include <Qt/qdeclarativedebug.h>

using namespace bb::cascades;
using namespace bb::system;

void myMessageOutput(QtMsgType type, const char* msg) {
    // setup shared logs folder
    QString logFilePath = (new QDir())->absoluteFilePath("shared/documents/logs");
    QDir sharedDir(logFilePath);

    if (!sharedDir.exists()) {
    sharedDir.mkpath (logFilePath);
    }

    QFileInfo fileInfo((new QDir())->absoluteFilePath(logFilePath + "/HubIntegration.txt"));
    QFile file((new QDir())->absoluteFilePath(logFilePath + "/HubIntegration.txt"));

    // Open the file that was created
    if (!file.exists()) {
        QDir().mkpath (fileInfo.dir().path());
        // Open the file that was created
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            file.write(msg);
            file.write("\n");
            file.flush();
            file.close();
        }
    } else {
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            file.write(msg);
            file.write("\n");
            file.flush();
            file.close();
        }
    }
}

Q_DECL_EXPORT int main(int argc, char **argv)
{
    qInstallMsgHandler(myMessageOutput);

    Application app(argc, argv);

    // Create the Application UI object, this is where the main.qml file
    // is loaded and the application scene is set.
    new ApplicationUI(&app);

    // Enter the application main event loop.
    return Application::exec();
}
