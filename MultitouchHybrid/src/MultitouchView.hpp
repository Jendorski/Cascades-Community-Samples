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


#ifndef MULTITOUCHVIEW_HPP
#define MULTITOUCHVIEW_HPP

#include <assert.h>
#include <screen/screen.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <bb/cascades/TouchEvent>

#include <QtCore/QObject>
#include <QtCore/QString>

#include "OpenGLView.hpp"
#include "OpenGLThread.hpp"

class MultitouchView : public OpenGLView {

Q_OBJECT

public:
	MultitouchView();

	virtual ~MultitouchView();

public Q_SLOTS:

	// action slots
	void reset(bool skipColour);

public:

	// touch handler
	void multitouch(MultitouchEvent *event);

	// overriden methods from OpenGLView
	int initialize();
	int regenerate();
	void cleanup();

	void update();
	void render();

private:
	// view transform functions
	void setupView();
	float touch_coords[10];
	float square_fade[5];
	float square_size[5];
    float square_vertices[5*8];
};

#endif /* MULTITOUCHVIEW_HPP */

