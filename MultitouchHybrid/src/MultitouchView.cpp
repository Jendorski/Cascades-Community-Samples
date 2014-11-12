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

#include "MultitouchView.hpp"
#include <math.h>

#include <QDebug>

using namespace bb::cascades;

MultitouchView::MultitouchView() : touch_coords( { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } ), square_fade( { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } )
{
		setDisplay(DISPLAY_DEVICE);
}

MultitouchView::~MultitouchView() {
	// TODO Auto-generated destructor stub
}

int MultitouchView::initialize() {

	setZ(5);

	int returnCode = OpenGLView::initGL();
	if (returnCode == EXIT_SUCCESS) {

		reset(false);

		// reset stale flag for initialization only
		setStale(true);

		//Common gl setup
		glShadeModel(GL_SMOOTH);
		glClearColor(0.0f, 0.0f, 0.0f, 0.01f);
	}

	qDebug()  << "MultitouchView::initialize " << ":" << returnCode;

    return returnCode;
}

int MultitouchView::regenerate()
{
	int returnCode = OpenGLView::regenerate();

	reset(true);

	return returnCode;
}

void MultitouchView::cleanup() {
	OpenGLView::cleanup();
}

void MultitouchView::render() {
    int index = 0;

	if (m_display == DISPLAY_DEVICE) {
		//Typical render pass
		glClearColor(0.0f, 0.0f, 0.0f, 0.01f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//First render background and menu if it is enabled
		setupView();

	    //Setup touch location polygon
		for (index = 0; index < 5; index++) {
			if (square_fade[index] == 1.0) {
				square_vertices[index*8+0] = (float)touch_coords[2*index+0];
				square_vertices[index*8+1] = m_surface_height - (float)touch_coords[2*index+1];
				square_vertices[index*8+2] = (float)touch_coords[2*index+0]+square_size[index];
				square_vertices[index*8+3] = m_surface_height - (float)touch_coords[2*index+1];
				square_vertices[index*8+4] = (float)touch_coords[2*index+0];
				square_vertices[index*8+5] = m_surface_height - (float)touch_coords[2*index+1]-square_size[index];
				square_vertices[index*8+6] = (float)touch_coords[2*index+0]+square_size[index];
				square_vertices[index*8+7] = m_surface_height - (float)touch_coords[2*index+1]-square_size[index];

				//fprintf(stderr, "square: %d %f %f,%f %f,%f %f,%f %f,%f \n", index, square_fade[index], square_vertices[index*8+0], square_vertices[index*8+1], square_vertices[index*8+2], square_vertices[index*8+3],
				//		square_vertices[index*8+4], square_vertices[index*8+5], square_vertices[index*8+6], square_vertices[index*8+7]);

				glColor4f(0.0f, 0.f, 1.0f, 1.0f);
				glEnableClientState(GL_VERTEX_ARRAY);

				glVertexPointer(2, GL_FLOAT, 0, square_vertices);

				glDrawArrays(GL_TRIANGLE_STRIP, index*4+0, 4);

				glDisableClientState(GL_VERTEX_ARRAY);

				square_fade[index] /= 1.25;

				setStale(true);

			} else
			if (square_fade[index] > 0.0) {
				square_vertices[index*8+0] -= 2.0;
				square_vertices[index*8+1] += 2.0;
				square_vertices[index*8+2] += 2.0;
				square_vertices[index*8+3] += 2.0;
				square_vertices[index*8+4] -= 2.0;
				square_vertices[index*8+5] -= 2.0;
				square_vertices[index*8+6] += 2.0;
				square_vertices[index*8+7] -= 2.0;

				//fprintf(stderr, "square: %d %f %f,%f %f,%f %f,%f %f,%f \n", index, square_fade[index], square_vertices[index*8+0], square_vertices[index*8+1], square_vertices[index*8+2], square_vertices[index*8+3],
				//		square_vertices[index*8+4], square_vertices[index*8+5], square_vertices[index*8+6], square_vertices[index*8+7]);

				glColor4f(0.0f, 0.0f, 1.0f, square_fade[index]);
				glEnableClientState(GL_VERTEX_ARRAY);

				glVertexPointer(2, GL_FLOAT, 0, square_vertices);

				glDrawArrays(GL_TRIANGLE_STRIP, index*4+0, 4);

				glDisableClientState(GL_VERTEX_ARRAY);

				square_fade[index] /= 1.25;
				if (square_fade[index] < 0.1) {
					square_fade[index] = 0.0;
				}
				setStale(false);
			}
		}

		//fprintf(stderr, "square_fade: %f\n", square_fade);
	}
}

void MultitouchView::setupView() {
    glViewport(0, 0, m_width, m_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrthof(0.0f, (float)m_width / (float)m_height, 0.0f, 1.0f, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0f / (float)m_height, 1.0f / (float)m_height, 1.0f);
}


void MultitouchView::reset(bool skipColour) {
	setStale(true);
}

void MultitouchView::update() {
}

void MultitouchView::multitouch(MultitouchEvent *event) {
	//fprintf(stderr, "multitouch: %d %d %d %d %d %f %f \n", event->type(), event->isDown(), event->isUp(), event->isMove(), event->Id(), event->windowX(), event->windowY() );

	if (event->isDown()) {
		//fprintf(stderr, "multitouch down: %d %d %f %f \n", event->type(), event->Id(), event->windowX(), event->windowY() );
		touch_coords[2*event->Id()+0] = event->windowX();
		touch_coords[2*event->Id()+1] = event->windowY();
		square_fade[event->Id()] = 1.0;
		square_size[event->Id()] = 8.0 + event->pressure();
		setStale(true);
	}
	if (event->isUp()) {
		//fprintf(stderr, "multitouch release: %d %d %f %f \n", event->type(), event->Id(), event->windowX(), event->windowY() );
		square_fade[event->Id()] = 0.0;
	}
	if (event->isMove()) {
		//fprintf(stderr, "multitouch move: %d %d %f %f \n", event->type(), event->Id(), event->windowX(), event->windowY() );
		touch_coords[2*event->Id()+0] = event->windowX();
		touch_coords[2*event->Id()+1] = event->windowY();
		square_fade[event->Id()] = 1.0;
		square_size[event->Id()] = 8.0 + event->pressure();
		setStale(true);
	}
}
