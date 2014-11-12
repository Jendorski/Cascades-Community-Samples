#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/Page>
#include <bb/cascades/TextArea>
#include <bb/system/CardResizeMessage>
#include <bb/system/CardDoneMessage>

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app),
    	_settings("Example", "Hub Integration"),
    	_invokeManager(new InvokeManager(this)),
    	_app(app)
{
	bool connectResult;


    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning.
    Q_UNUSED(connectResult);
	connectResult = connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoked(const bb::system::InvokeRequest&)));
	qDebug() << "HubIntegration: started and connected to invokeManager";

    // This is only available in Debug builds.
    Q_ASSERT(connectResult);

    // prepare the localization
    _pTranslator = new QTranslator(this);
    _pLocaleHandler = new LocaleHandler(this);
    if(!QObject::connect(_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()))) {
        // This is an abnormal situation! Something went wrong!
        // Add own code to recover here
        qWarning() << "Recovering from a failed connect()";
    }
    // initial load
    onSystemLanguageChanged();


    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    _qml = QmlDocument::create("asset:///main.qml").parent(this);

	_qml->setContextProperty("_app", this);

    // Create root object for the UI
    _root = _qml->createRootObject<NavigationPane>();

    // Set created root object as the application scene
    app->setScene(_root);

	switch(_invokeManager->startupMode()) {
	case ApplicationStartupMode::LaunchApplication:
		qDebug() << "HubIntegration: Regular application launch";
		break;

	case ApplicationStartupMode::InvokeApplication:
		qDebug() << "HubIntegration: Launching app from invoke";

	    _isCard = false;
		break;

	case ApplicationStartupMode::InvokeCard:
		qDebug() << "HubIntegration: Launching card from invoke";

	    _isCard = true;
		break;

	default:
		qDebug() << "HubIntegration: other launch";
		break;
	}
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("HubIntegration_%1").arg(locale_string);
    if (_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(_pTranslator);
    }
}

void ApplicationUI::onInvoked(const bb::system::InvokeRequest& request)
{
	qDebug() << "HubIntegration: onInvoked: Hub Integration was invoked";

	if(request.action().compare("bb.action.COMPOSE") == 0) {
        qDebug() << "HubIntegration: onInvoked: compose item: " << request.data();

	    // Create scene document from main.qml asset, the parent is set
	    // to ensure the document gets destroyed properly at shut down.
	    _pageQml = QmlDocument::create("asset:///compose.qml").parent(this);

	    // Create root object for the UI
	    _page = _pageQml->createRootObject<Page>();

	    _root->push(_page);

	} else
	if(request.action().compare("bb.action.VIEW") == 0) {
		qDebug() << "HubIntegration: onInvoked: view item: " << request.data();

	    // Create scene document from main.qml asset, the parent is set
	    // to ensure the document gets destroyed properly at shut down.
	    _pageQml = QmlDocument::create("asset:///message.qml").parent(this);

	    // Create root object for the UI
	    _page = _pageQml->createRootObject<Page>();

	    JsonDataAccess jda;

		QVariantMap objectMap = (jda.loadFromBuffer(request.data())).toMap();
		QVariantMap itemMap = objectMap["attributes"].toMap();

		QVariantList items = _settings.value("hub/items").toList();

		for(int index = 0; index < items.size(); index++) {
			QVariantMap item = items.at(index).toMap();
			QString sourceId = item["messageid"].toString();
			qDebug() << "HubIntegration: onInvoked: found item" << item;

			if (item["sourceId"].toString() == itemMap["messageid"].toString() ||
				item["sourceId"].toString() == itemMap["sourceId"].toString()) {
				qDebug() << "HubIntegration: onInvoked: page " << _page;
				Container *container = _page->findChild<Container*>("container");
				qDebug() << "HubIntegration: onInvoked: container " << container;
				Container *fromContainer = container->findChild<Container*>("fromContainer");
				qDebug() << "HubIntegration: onInvoked: fromContainer " << fromContainer;
				Label *fromName = fromContainer->findChild<Label*>("fromName");
				qDebug() << "HubIntegration: onInvoked: fromName " << fromName;
				fromName->setText(item["name"].toString());
				Container *subjectContainer = container->findChild<Container*>("subjectContainer");
				qDebug() << "HubIntegration: onInvoked: fromContainer " << subjectContainer;
				TextArea *subject = subjectContainer->findChild<TextArea*>("subject");
				qDebug() << "HubIntegration: onInvoked: subject " << subject;
				subject->setText(item["description"].toString());
				Container *bodyContainer = container->findChild<Container*>("bodyContainer");
				qDebug() << "HubIntegration: onInvoked: fromContainer " << bodyContainer;
				TextArea *body = bodyContainer->findChild<TextArea*>("body");
				qDebug() << "HubIntegration: onInvoked: body " << body;
				qDebug() << "HubIntegration: onInvoked: body data " << item["body"].toString();
				body->setText(item["body"].toString());

				InvokeRequest request;
				request.setTarget("com.example.HeadlessHubIntegration");
				request.setAction("bb.action.MARKREAD");
				request.setMimeType("hub/item");
				request.setUri(QUrl("pim:"));

				QByteArray bytes;
				jda.saveToBuffer(objectMap, &bytes);
				request.setData(bytes);

				InvokeTargetReply *reply = _invokeManager->invoke(request);
				Q_UNUSED(reply);

				break;
			}
		}

	    _root->push(_page);

	} else {
		qDebug() << "ApplicationUI: onInvoked: unknown service request " << request.action() << " : " << request.data();
	}
}

void ApplicationUI::cardResizeRequested(const bb::system::CardResizeMessage& resizeMessage)
{
    // Respond after resizing is done.
    _invokeManager->cardResized(resizeMessage);
}

void ApplicationUI::closeCard()
{
	_app->requestExit();

	if (_isCard) {
		// Assemble response message
		CardDoneMessage message;
		message.setData(tr(""));
		message.setDataType("text/plain");
		message.setReason(tr("Success!"));

		// Send message
		_invokeManager->sendCardDone(message);
	}
}
