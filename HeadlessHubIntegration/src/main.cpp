#include "HeadlessHubIntegration.hpp"

#include <bb/Application>
//#include <QtCore/QCoreApplication.h>

using namespace bb;

void myMessageOutput(QtMsgType type, const char* msg) {
    // setup shared logs folder
    QString logFilePath = (new QDir())->absoluteFilePath("shared/documents/logs");
    QDir sharedDir(logFilePath);

    if (!sharedDir.exists()) {
    sharedDir.mkpath (logFilePath);
    }

    QFileInfo fileInfo((new QDir())->absoluteFilePath(logFilePath + "/HeadlessHubIntegration.txt"));
    QFile file((new QDir())->absoluteFilePath(logFilePath + "/HeadlessHubIntegration.txt"));

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

int main(int argc, char **argv)
{
    qInstallMsgHandler(myMessageOutput);

	Application app(argc, argv);

	qDebug() << "HeadlessHubIntegration: started";

    HeadlessHubIntegration headless(&app);

    return Application::exec();
}
