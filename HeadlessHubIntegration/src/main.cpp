#include "HeadlessHubIntegration.hpp"

#include <bb/Application>
//#include <QtCore/QCoreApplication.h>

using namespace bb;

int main(int argc, char **argv)
{
	 Application app(argc, argv);

	qDebug() << "HeadlessHubIntegration: started";

    HeadlessHubIntegration headless(&app);

    return Application::exec();
}
