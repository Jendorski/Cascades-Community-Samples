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

#ifndef HEADLESS_HUB_INTEGRATION_HPP_
#define HEADLESS_HUB_INTEGRATION_HPP_

#include <bb/Application>
#include <QObject>
#include <QSettings>
#include "UDSUtil.hpp"
#include <bb/pim/unified/unified_data_source.h>

#include "UDSUtil.hpp"
#include "HubCache.hpp"
#include "TestAccount.hpp"
#include "PushManager.hpp"

namespace bb {
    namespace system {
        class ApplicationStartupMode;
        class InvokeManager;
        class InvokeRequest;
        class InvokeTargetReply;
    }
}

using bb::Application;
using bb::system::ApplicationStartupMode;
using bb::system::InvokeManager;
using bb::system::InvokeRequest;
using bb::system::InvokeTargetReply;

class HeadlessHubIntegration : public QObject {
	Q_OBJECT

public:
	HeadlessHubIntegration(Application* app);
	virtual ~HeadlessHubIntegration();

	Q_SLOT void onInvoked(const bb::system::InvokeRequest& request);

    void initialize();

private slots:
	void processNewMessage(QByteArray message);
    void markHubItemRead(QVariantMap itemProperties);
    void markHubItemUnread(QVariantMap itemProperties);
    void removeHubItem(QVariantMap itemProperties);

private:
	PushManager *_pushManager; //Handle all push interactions

    TestAccount* _testAccount;
    HubCache*     _hubCache;
    UDSUtil*      _udsUtil;
    QSettings*    _settings;

    int  _itemCounter;
    Application* _app;
    InvokeManager* _invokeManager;
    QMutex _initMutex;
};

#endif /* HEADLESS_HUB_INTEGRATION_HPP_ */
