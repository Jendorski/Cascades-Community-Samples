/* Copyright (c) 2012-2013 Research In Motion Limited.
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

#include "PushManager.hpp"
#include <bb/network/PushPayload>
#include <bb/system/ApplicationStartupMode>
#include <bb/IconBadge>

using namespace bb::network;
using namespace bb::system;

const QString PushManager::BLACKBERRY_PUSH_APPLICATION_ID =
        "Application ID";
const QString PushManager::BLACKBERRY_PUSH_URL =
        "http://cp###.pushapi.eval.blackberry.com/";


//Update this value as well as the invoke-target id in the bar-descriptor.xml file
const QString PushManager::BLACKBERRY_INVOKE_TARGET_ID = "com.example.HeadlessHubIntegration";

PushManager::PushManager(InvokeManager* invokeManager) {

	//Create a push service so the device knows how to invoke our app when a push arrives
	m_pushService = new PushService(BLACKBERRY_PUSH_APPLICATION_ID,
			BLACKBERRY_INVOKE_TARGET_ID);

	//The InvokeManager will handle the invocation where the push data is received
	this->invokeManager = invokeManager;

	bool connectResult;

	//createSession is asynchronous so once completed we can proceed with creating the channel
    Q_UNUSED(connectResult);
	connectResult = connect(m_pushService,
							SIGNAL(createSessionCompleted (const bb::network::PushStatus &)),
							this,
							SLOT(createSessionCompleted(const bb::network::PushStatus&)));

    // This is only available in Debug builds.
    Q_ASSERT(connectResult);

	//createChannel is asynchronous so once completed can reister to launch
    Q_UNUSED(connectResult);
	connectResult = connect(m_pushService,
							SIGNAL(createChannelCompleted (const bb::network::PushStatus&,
											const QString &)), this,
							SLOT(createChannelCompleted(const bb::network::PushStatus&,
											const QString &)));

    // This is only available in Debug builds.
    Q_ASSERT(connectResult);

    Q_UNUSED(connectResult);
	connectResult = connect(m_pushService, SIGNAL(pushTransportReady(const bb::network::PushCommand::Type)),
							this, SLOT(onPushTransportReady(bb::network::PushCommand::Type)));

    // This is only available in Debug builds.
    Q_ASSERT(connectResult);

	//Initiate the session creation so we can start reciving push messages
	m_pushService->createSession();
}

PushManager::~PushManager() {
}

/**
 * Once the session is created successfully we can then create the channel (register
 * with the BlackBerry Push Service)
 */
void PushManager::createSessionCompleted(
		const bb::network::PushStatus& status) {
	if (!status.isError() && m_pushService) {
		log("Session creation completed successfully");

		//If the app was launched by a push incoming then we know the channel is already created
		if (appWasAutoStarted()) {
			m_pushService->createChannel(
					QUrl(BLACKBERRY_PUSH_URL));
		}
	} else {
		log("Session creation failed: " + status.errorDescription());
	}
}

/**
 * If the channel was created successfully we can then register to launch the app
 * so the app opens when a push arrives.
 */
void PushManager::createChannelCompleted(const bb::network::PushStatus& status,
		const QString& token) {
	Q_UNUSED(token);
	if (!status.isError() && m_pushService) {
		log("Channel creation completed successfully");
		m_pushService->registerToLaunch();
	} else {
		log("Channel creation failed: " + status.errorDescription());

		switch(status.code()){
		case  PushErrorCode::NoError:
		  break;
		case  PushErrorCode::TransportFailure:
		  log("Create channel failed as the push transport is unavailable. \
					Verify your mobile network and/or Wi-Fi are turned on. \
					If they are on, you will be notified when the push \
					transport is available again.");
		  break;
		case PushErrorCode::SubscriptionContentNotAvailable:
		  log("Create channel failed as the PPG is currently \
					returning a server error. \
					You will be notified when the PPG is available \
					again.");
		  break;
		}

	}
}
void PushManager::onPushTransportReady(const bb::network::PushCommand::Type command) {
	if (command == bb::network::PushCommand::CreateChannel) {
		m_pushService->createChannel(
				QUrl(BLACKBERRY_PUSH_URL));
	}
	else if (command == bb::network::PushCommand::DestroyChannel) {
		m_pushService->destroyChannel();
	}
}

/**
 * When the app gets invoked this method will be called and passed the InvokeRequest
 */
QByteArray PushManager::getPayload(const InvokeRequest& request) {
	QByteArray message;

	//If the app is here we know the InvokeRequest contains push data
	PushPayload payload(request);
	if (payload.isValid()) {
		log("payload is valid, processing now");
		if (payload.isAckRequired()) { //This section is only useful for Push Plus
			log("ACK required, sending");
			m_pushService->acceptPush(payload.id());
		}
		//Read all the push data from the payload
		message = payload.data();

		qDebug() << "payload data: " << payload.data() << " : " << message;

		//Pass it to our method which will store the push in a nice format
		logPush(message);
	}

	return message;
}

//Check whether the app was launched by the user of via invocation
bool PushManager::appWasAutoStarted() {
	// If the application was launched after receiving a push then we know the channel
	// is already created and our application is already registered to launch, otherwise
	// we should likely create these.
	return (invokeManager->startupMode()
			!= ApplicationStartupMode::InvokeApplication);
}

// Split the push into its various parts then store to QSettings
void PushManager::logPush(const QString & pushMessage) {
	pushList = settings.value("pushList").toList();
	QString pushMessageWithDate = pushMessage;
	pushMessageWithDate = pushMessageWithDate.append(
			"|" + QDateTime::currentDateTime().toString("h:mm:ss ap"));
	pushList.append(pushMessageWithDate);
	settings.setValue("pushList", pushList);

	//This 'emit' lets anyone listening know that the DataModel has been updated
	emit pushListChanged(pushList);
}

//Wipe out the data store erasing all existing push messages
void PushManager::clearPushList() {
	log("Clearing push list from cache");
	settings.clear();
	pushList = settings.value("pushList").toList();
	emit pushListChanged(pushList);
}

//This method retrieves all push messages, updating the list, then notifies
// all listeners of the new list.
void PushManager::updatePushList() {
	pushList = settings.value("pushList").toList();
	emit pushListChanged(pushList);
}

//A log method to output to the console
void PushManager::log(const QString &toLog) {
	qDebug() << toLog;
}

