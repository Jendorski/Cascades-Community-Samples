/*
 * Copyright (c) 2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "HeadlessHubIntegration.hpp"

#include <errno.h>
#include <malloc.h>
#include <spawn.h>
#include <unistd.h>
#include <QDebug>
#include <QStringList>
#include <bb/data/JsonDataAccess>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeTargetReply>

using namespace bb::data;

HeadlessHubIntegration::HeadlessHubIntegration(Application* app) :
    _testAccount(NULL),
    _hubCache(NULL),
    _udsUtil(NULL),
    _settings(NULL),
    _itemCounter(0),
	_app(app),
	_invokeManager(new InvokeManager())
{
	qDebug() << "HeadlessHubIntegration: HeadlessHubIntegration";

    bool started = false;
	bool connectResult;

	_invokeManager->setParent(this);

    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning.
    Q_UNUSED(connectResult);
	connectResult = connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoked(const bb::system::InvokeRequest&)));
	qDebug() << "HeadlessHubIntegration: started and connected to invokeManager";

    // This is only available in Debug builds.
    Q_ASSERT(connectResult);

	switch(_invokeManager->startupMode()) {
	case ApplicationStartupMode::LaunchApplication:
		qDebug() << "HeadlessHubIntegration: Regular application launch";
		break;
	case ApplicationStartupMode::InvokeApplication:
		qDebug() << "HeadlessHubIntegration: Launching app from invoke";
		break;
	case ApplicationStartupMode::InvokeCard:
		qDebug() << "HeadlessHubIntegration: Launching card from invoke";
		break;
    // enable when 10.3 beta is released
    //case ApplicationStartupMode::InvokeHeadless:
    case 4:
        qDebug() << "HeadlessHubIntegration: Launching headless from invoke";
        started = true;
        break;
	default:
		qDebug() << "HeadlessHubIntegration: other launch: " << _invokeManager->startupMode();
		break;
	}

    //Create an instance of our PushManager class. This class will handle all push interactions.
    _pushManager = new PushManager(_invokeManager);

    // enable when 10.3 beta is released
    //if (_invokeManager->startupMode() == ApplicationStartupMode::InvokeHeadless) {
    if (started) {
        initialize();
    }
}

HeadlessHubIntegration::~HeadlessHubIntegration()
{
	qDebug() << "HeadlessHubIntegration: ~HeadlessHubIntegration";

	// don't need to delete _invokeManager since this is its parent it will be
	// killed appropriately by Qt

    if (_testAccount) {
        delete _testAccount;
    }
    if (_hubCache) {
        delete _hubCache;
    }
    if (_settings) {
        delete _settings;
    }
    if (_udsUtil) {
        delete _udsUtil;
    }
}

void HeadlessHubIntegration::initialize()
{
    qDebug() << "HeadlessHubIntegration: initialize: " << (_udsUtil != NULL);

    _initMutex.lock();

    // initialize UDS
    if (!_udsUtil) {
        _udsUtil = new UDSUtil(QString("exampleHubService"), QString("hubassets"));
        _udsUtil->initialize();
        _settings = new QSettings("Example", "Hub Integration");
        _hubCache = new HubCache(_settings);
        _testAccount = new TestAccount(_udsUtil, _hubCache);

       qDebug() << "HeadlessHubIntegration: initialize: initialized " << (_udsUtil != NULL);
    }

    _initMutex.unlock();
}

void HeadlessHubIntegration::onInvoked(const bb::system::InvokeRequest& request)
{
    qDebug() << "HeadlessHubIntegration: onInvoked: HeadlessHubIntegration was invoked";

    // Sometimes this method gets invoked before the constructor initializes fully so the following code should handle this scenario.
    initialize();

	if(request.action().compare("bb.action.system.STARTED") == 0) {
		qDebug() << "HeadlessHubIntegration: onInvoked: HeadlessHubIntegration : auto started";
	} else if(request.action().compare("bb.action.START") == 0) {
		qDebug() << "HeadlessHubIntegration: onInvoked: HeadlessHubIntegration : start";
	} else if(request.action().compare("bb.action.STOP") == 0) {
		qDebug() << "HeadlessHubIntegration: onInvoked: HeadlessHubIntegration : stop";
		//_app->quit();
		_app->requestExit();

    } else if(request.action().compare("bb.action.MARKREAD") == 0) {
        qDebug() << "HeadlessHubIntegration: onInvoked: mark read" << request.data();
        JsonDataAccess jda;

        QVariantMap objectMap = (jda.loadFromBuffer(request.data())).toMap();
        QVariantMap attributesMap = objectMap["attributes"].toMap();

        markHubItemRead(attributesMap);

    } else if(request.action().compare("bb.action.MARKUNREAD") == 0) {
        qDebug() << "HeadlessHubIntegration: onInvoked: mark unread" << request.data();
        JsonDataAccess jda;

        QVariantMap objectMap = (jda.loadFromBuffer(request.data())).toMap();
        QVariantMap attributesMap = objectMap["attributes"].toMap();

        markHubItemUnread(attributesMap);

    } else if(request.action().compare("bb.action.MARKPRIORREAD") == 0) {
        JsonDataAccess jda;

        qint64 timestamp = (jda.loadFromBuffer(request.data())).toLongLong();
        QDateTime date = QDateTime::fromMSecsSinceEpoch(timestamp);

        qDebug() << "HeadlessHubIntegration: onInvoked: mark prior read : " << timestamp << " : " << request.data();

        _testAccount->markHubItemsReadBefore(_testAccount->categoryId(), timestamp);

    } else if(request.action().compare("bb.action.DELETE") == 0) {
        qDebug() << "HeadlessHubIntegration: onInvoked: HeadlessHubIntegration : delete" << request.data();
        JsonDataAccess jda;

        QVariantMap objectMap = (jda.loadFromBuffer(request.data())).toMap();
        QVariantMap attributesMap = objectMap["attributes"].toMap();

        removeHubItem(attributesMap);

    } else if(request.action().compare("bb.action.DELETEPRIOR") == 0) {
        JsonDataAccess jda;

        qint64 timestamp = (jda.loadFromBuffer(request.data())).toLongLong();
        QDateTime date = QDateTime::fromMSecsSinceEpoch(timestamp);

        qDebug() << "HeadlessHubIntegration: onInvoked: mark prior delete : " << timestamp << " : " << request.data();

        _testAccount->removeHubItemsBefore(_testAccount->categoryId(), timestamp);

    } else if(request.action().compare("bb.action.PUSH") == 0) {
        qDebug() << "HeadlessHubIntegration: onInvoked: HeadlessHubIntegration : push";
        QByteArray message = _pushManager->getPayload(request);
        processNewMessage(message);
    } else {
        qDebug() << "HeadlessHubIntegration: onInvoked: unknown service request " << request.action() << " : " << request.data();
    }
}

void HeadlessHubIntegration::markHubItemRead(QVariantMap itemProperties)
{
    qDebug()  << "HeadlessHubIntegration::markHubItemRead: item: " << itemProperties;

    qDebug()  << "HeadlessHubIntegration::markHubItemRead: item src Id: " << itemProperties["sourceId"].toString();
    qDebug()  << "HeadlessHubIntegration::markHubItemRead: item message Id: " << itemProperties["messageid"].toString();

    qint64 itemId;

    if (itemProperties["sourceId"].toString().length() > 0) {
        itemId = itemProperties["sourceId"].toLongLong();
    } else if (itemProperties["messageid"].toString().length() > 0) {
        itemId = itemProperties["messageid"].toLongLong();
    }

    _testAccount->markHubItemRead(_testAccount->categoryId(), itemId);
}

void HeadlessHubIntegration::markHubItemUnread(QVariantMap itemProperties)
{
    qDebug()  << "HeadlessHubIntegration::markHubItemUnread: item: " << itemProperties;

    qDebug()  << "HeadlessHubIntegration::markHubItemUnread: item src Id: " << itemProperties["sourceId"].toString();
    qDebug()  << "HeadlessHubIntegration::markHubItemUnread: item message Id: " << itemProperties["messageid"].toString();

    qint64 itemId;

    if (itemProperties["sourceId"].toString().length() > 0) {
        itemId = itemProperties["sourceId"].toLongLong();
    } else if (itemProperties["messageid"].toString().length() > 0) {
        itemId = itemProperties["messageid"].toLongLong();
    }

    _testAccount->markHubItemUnread(_testAccount->categoryId(), itemId);
}

void HeadlessHubIntegration::removeHubItem(QVariantMap itemProperties)
{
    qDebug()  << "HeadlessHubIntegration::removeHubItem: item: " << itemProperties;

    qDebug()  << "HeadlessHubIntegration::removeHubItem: item src Id: " << itemProperties["sourceId"].toString();
    qDebug()  << "HeadlessHubIntegration::removeHubItem: item message Id: " << itemProperties["messageid"].toString();

    qint64 itemId;
    if (itemProperties["sourceId"].toString().length() > 0) {
        itemId = itemProperties["sourceId"].toLongLong();
    } else if (itemProperties["messageid"].toString().length() > 0) {
        itemId = itemProperties["messageid"].toLongLong();
    }

    _testAccount->removeHubItem(_testAccount->categoryId(), itemId);
}

void HeadlessHubIntegration::processNewMessage(QByteArray message) {

	qDebug()  << "HeadlessHubIntegration::processNewMessage: message: " << message;

    bool notify = false;
    QString priority("");
    QString from("Test");
    QString title("");
    QString body("");

    do {
    	int index = message.indexOf('|');
		qDebug() << "HeadlessHubIntegration::processNewMessage: index: " << index << "\n";
    	if (index > 0) {
    		if (priority.size() == 0) {
    			priority = message.left(index);
    			message = message.right(message.size() - index - 1);

    			qDebug() << "HeadlessHubIntegration::processNewMessage: priority: " << priority << "\n";
    			qDebug() << "HeadlessHubIntegration::processNewMessage: message: " << message << "\n";

    		} else
    		if (title.size() == 0) {
    			title = message.left(index);
    			message = message.right(message.size() - index - 1);

    			qDebug() << "HeadlessHubIntegration::processNewMessage: title: " << title << "\n";
    			qDebug() << "HeadlessHubIntegration::processNewMessage: message: " << message << "\n";
    		}
    	} else {
    		if (body.size() == 0) {
    			body += message;

    			qDebug() << "HeadlessHubIntegration::processNewMessage: body: " << body << "\n";
    		}

    		message.clear();
    	}
    } while (message.size() > 0);

	//The first part of the push denotes the priority of the message
	switch (priority.toInt()) {
	case PushManager::Low:
		break;
	case PushManager::Medium:
		break;
	case PushManager::High:
		notify = true;
		break;
	default:
		break;
	}

    QVariantMap* itemMap = new QVariantMap();
    (*itemMap)["body"] = body;

    _itemCounter++;
    bool retVal = _testAccount->addHubItem(_testAccount->categoryId(), *itemMap, from, title, QDateTime::currentDateTime().toMSecsSinceEpoch(), QString::number(_itemCounter),"", "",  notify);
}
