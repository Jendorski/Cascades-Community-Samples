/*
 * Copyright (c) 2012-2013 BlackBerry
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
#include <bb/cascades/AbsoluteLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Application>
#include <bb/cascades/Window>
#include <bb/cascades/Button>
#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/ForeignWindowControl>
#include <bb/cascades/ImplicitAnimationController>
#include <bb/cascades/Page>
#include <bb/cascades/LayoutUpdateHandler>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/SceneCover>
#include <QtCore/QTimer>
#include <QDebug>


#include "MultitouchHybridApp.hpp"

#include <screen/screen.h>

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <EGL/egl.h>
#include <sys/strm.h>

using namespace bb::cascades;

MultitouchHybridApp::MultitouchHybridApp()
{

	Application::instance()->setAutoExit(false);

	QObject::connect(
			Application::instance(),
			SIGNAL(manualExit()),
			this,
			SLOT(shutdown()));


    QmlDocument *qml = QmlDocument::create("asset:///main.qml");

    if (!qml->hasErrors()) {
    	OpenGLView::setRenderingAPI(GL_ES_1);

		m_pMultitouch = new MultitouchView();

		qml->setContextProperty("_multitouch", m_pMultitouch);

		// The application NavigationPane is created from QML.
		m_navPane = qml->createRootObject<NavigationPane>();
		if (m_navPane) {
        	m_fwBound = false;

         	m_pForeignWindowControl = m_navPane->findChild<ForeignWindowControl*>("multitouchHybridFW");

         	QObject::connect(m_pForeignWindowControl, SIGNAL(controlFrameChanged(const QRectF &)), this, SLOT(onLayoutFrameChanged(const QRectF &)));

    		// connect ForeignWindowControl signals to slots
    		QObject::connect(m_pForeignWindowControl, SIGNAL(touch(bb::cascades::TouchEvent *)),
    						m_pMultitouch,   SLOT(onTouch(bb::cascades::TouchEvent *)) );

            // Finally the main scene for the application is set the Page.
            Application::instance()->setScene(m_navPane);
		}
	}
}

void MultitouchHybridApp::shutdown() {
	qDebug()  << "MultitouchHybridApp::shutdown";

	OpenGLView::shutdown();

	Application::instance()->quit();
}

void MultitouchHybridApp::onLayoutFrameChanged(const QRectF &layoutFrame) {
	qDebug()  << "MultitouchHybridApp::onLayoutFrameChanged: " << m_pForeignWindowControl << ":" << m_fwBound;

	fprintf(stderr, "fw size: %f,%f %fx%f\n", layoutFrame.x(), layoutFrame.y(), layoutFrame.width(), layoutFrame.height());

	if (layoutFrame.width() > 0 && layoutFrame.height() > 0) {
		if (m_fwBound == false) {
			m_fwBound = true;

			QString mainWindowGroupId = Application::instance()->mainWindow()->groupId();

			m_pMultitouch->setWindowGroup(mainWindowGroupId);
			m_pMultitouch->setWindowID("multitouchHybridAppID1");
			m_pMultitouch->setPosition(layoutFrame.x(), layoutFrame.y());
			m_pMultitouch->setSize(layoutFrame.width(), layoutFrame.height());
			m_pMultitouch->add();
			m_pMultitouch->setEnabled(true);

			m_pForeignWindowControl->setVisible(true);

		} else {
			m_pMultitouch->setAngle(OrientationSupport::instance()->displayDirection());
			m_pMultitouch->setPosition(layoutFrame.x(), layoutFrame.y());
			m_pMultitouch->setSize(layoutFrame.width(), layoutFrame.height());
			m_pMultitouch->setAltered(true);
		}
	}
}

