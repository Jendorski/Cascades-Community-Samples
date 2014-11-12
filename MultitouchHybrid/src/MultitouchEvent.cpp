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

#include "MultitouchEvent.hpp"
#include <math.h>

#include <QDebug>

/*!
 * @brief Constructs a touch event.
 *
 * @param touchType - The type of touch.
 * @param screenX - Touch @c x coordinate relative to the screen.
 * @param screenY - Touch @c x coordinate relative to the screen.
 * @param windowX - Touch @c x coordinate relative to the window.
 * @param windowY - Touch @c x coordinate relative to the window.
 * @param target - The touch target of this event.
 * @return A touch event.
 */
MultitouchEvent::MultitouchEvent(int type, float screenX, float screenY, float windowX, float windowY, int id, int pressure, QObject* target)
{
    _type = type;
    _screenX = screenX;
    _screenY = screenY;
    _windowX = windowX;
    _windowY = windowY;
    _touchId = id;
    _pressure = pressure;
    _target = target;
}

/*!
 * @brief Destructs the touch event.
 */
MultitouchEvent::~MultitouchEvent() {
	// TODO Auto-generated destructor stub
}

/*!
 * @brief Retrieves the @c x coordinate of the touch relative to the screen.
 *
 * @return The @c x coordinate of the touch.
 */
float MultitouchEvent::screenX() const
{
	return _screenX;
}

/*!
 * @brief Retrieves the @c x coordinate of the touch relative to the screen.
 *
 * @return The @c y coordinate of the touch.
 */
float MultitouchEvent::screenY() const
{
	return _screenY;
}

/*!
 * @brief Retrieves the @c x coordinate of the touch relative to the window.
 *
 * @return The @c x coordinate of the touch.
 */
float MultitouchEvent::windowX() const
{
	return _windowX;
}


/*!
 * @brief Retrieves the @c x coordinate of the touch relative to the window.
 *
 * @return The @c y coordinate of the touch.
 */
float MultitouchEvent::windowY() const
{
	return _windowY;
}

/*!
 * @brief  @c pressure of the touch target.
 */
int MultitouchEvent::pressure() const
{
	return _pressure;
}

/*!
 * @brief  @c Id of touch target.
 */
int MultitouchEvent::Id() const
{
	return _touchId;
}

/*!
 * @brief The type of the event (SCREEN_EVENT_MTOUCH_TOUCH, SCREEN_EVENT_MTOUCH_MOVE, or SCREEN_EVENT_MTOUCH_RELEASE).
 *
 * @return The type.
 */
int MultitouchEvent::type() const
{
	return _type;
}

/*!
     * @brief Convenience function for checking if the touch type returned from touchType() is SCREEN_EVENT_MTOUCH_MOVE.
 *
 * @return @c true if the touch type is TouchType::Move.
 */
Q_INVOKABLE bool MultitouchEvent::isMove() const
{
	return (_type == SCREEN_EVENT_MTOUCH_MOVE);
}

/*!
     * @brief Convenience function for checking if the touch type returned from touchType() is SCREEN_EVENT_MTOUCH_RELEASE.
 *
 * @return @c true if the touch type is TouchType::Up.
 */
Q_INVOKABLE bool MultitouchEvent::isUp() const
{
	return (_type == SCREEN_EVENT_MTOUCH_RELEASE);
}

/*!
     * @brief Convenience function for checking if the touch type returned from touchType() is SCREEN_EVENT_MTOUCH_TOUCH.
 *
 * @return @c true if the touch type is TouchType::Down.
 */
Q_INVOKABLE bool MultitouchEvent::isDown() const
{
	return (_type == SCREEN_EVENT_MTOUCH_TOUCH);
}

/*
int MultitouchEvent::initialize() {

	setZ(5);

	int returnCode = OpenGLView::initGL();
	if (returnCode == EXIT_SUCCESS) {

		reset(false);

		// reset stale flag for initialization only
		setStale(true);

		//Common gl setup
		glShadeModel(GL_SMOOTH);
		glClearColor(0.775f, 0.775f, 0.775f, 1.0f);
	}

	qDebug()  << "MultitouchEvent::initialize " << ":" << returnCode;

    return returnCode;
}

int MultitouchEvent::regenerate()
{
	int returnCode = OpenGLView::regenerate();

	reset(true);

	return returnCode;
}

void MultitouchEvent::cleanup() {
	OpenGLView::cleanup();
}

void MultitouchEvent::render() {
    int index = 0;

	if (m_display == DISPLAY_DEVICE) {
		//Typical render pass
		glClearColor(0.775f, 0.775f, 0.775f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//First render background and menu if it is enabled
		setupView();

	    //Setup touch location polygon
		for (index = 0; index < 5; index++) {
			if (square_fade[index] == 1.0) {
				square_vertices[index*8+0] = (float)touch_coords[2*index+0];
				square_vertices[index*8+1] = m_surface_height - (float)touch_coords[2*index+1];
				square_vertices[index*8+2] = (float)touch_coords[2*index+0]+32.0;
				square_vertices[index*8+3] = m_surface_height - (float)touch_coords[2*index+1];
				square_vertices[index*8+4] = (float)touch_coords[2*index+0];
				square_vertices[index*8+5] = m_surface_height - (float)touch_coords[2*index+1]-33.0;
				square_vertices[index*8+6] = (float)touch_coords[2*index+0]+32.0;
				square_vertices[index*8+7] = m_surface_height - (float)touch_coords[2*index+1]-33.0;

				fprintf(stderr, "square: %d %f %f,%f %f,%f %f,%f %f,%f \n", index, square_fade[index], square_vertices[index*8+0], square_vertices[index*8+1], square_vertices[index*8+2], square_vertices[index*8+3],
						square_vertices[index*8+4], square_vertices[index*8+5], square_vertices[index*8+6], square_vertices[index*8+7]);

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
					//setStale(false);
				}
			}
		}

		//fprintf(stderr, "square_fade: %f\n", square_fade);
	}
}

void MultitouchEvent::setupView() {
    glViewport(0, 0, m_width, m_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrthof(0.0f, (float)m_width / (float)m_height, 0.0f, 1.0f, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0f / (float)m_height, 1.0f / (float)m_height, 1.0f);
}


void MultitouchEvent::reset(bool skipColour) {
	setStale(true);
}

void MultitouchEvent::update() {
}

void MultitouchEvent::onTouch(bb::cascades::TouchEvent *event) {
	if (event->isDown()) {
		qDebug() << "on touch down: " << event->screenX() << " : " << event->screenY();
	}
	if (event->isUp()) {
		qDebug() << "on touch up: " << event->screenX() << " : " << event->screenY();
	}
	if (event->isMove()) {
		qDebug() << "on touch move: " << event->screenX() << " : " << event->screenY();
	}
}

void MultitouchEvent::handleScreenEvent(bps_event_t *event) {
    int screenEvent;
    int buttons;
    int position[2];
    int realPosition[2];
    int touchID;
    int touchPressure;

    //static bool mouse_pressed = false;

    screen_event_t screen_event = screen_event_get_event(event);

    //Query type of screen event and its location on the screen
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE,
            &screenEvent);
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION,
    		position);
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_POSITION,
    		realPosition);
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TOUCH_ID,
    		&touchID);
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TOUCH_PRESSURE,
    		&touchPressure);

	switch (screenEvent) {
		case SCREEN_EVENT_MTOUCH_TOUCH:
			touch_coords[2*touchID+0] = (float)position[0];
			touch_coords[2*touchID+1] = (float)position[1];
			square_fade[touchID] = 1.0;
			setStale(true);

			fprintf(stderr, "touch: %d %d,%d  %d,%d \n", touchID, realPosition[0], realPosition[1], position[0], position[1] );

			break;

		case SCREEN_EVENT_MTOUCH_MOVE:
			touch_coords[2*touchID+0] = (float)position[0];
			touch_coords[2*touchID+1] = (float)position[1];
			square_fade[touchID] = 1.0;
			setStale(true);

			fprintf(stderr, "touch move: %d %d,%d  %d,%d \n", touchID, realPosition[0], realPosition[1], position[0], position[1] );
			break;

		case SCREEN_EVENT_MTOUCH_RELEASE:
			square_fade[touchID] = 0.0;
			//setStale(false);

			fprintf(stderr, "touch release: %d %d,%d  %d,%d \n", touchID, realPosition[0], realPosition[1], position[0], position[1] );
			break;

		case SCREEN_EVENT_POINTER:
	        screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_BUTTONS,
				&buttons);

	    	switch (buttons) {
	    		case SCREEN_LEFT_MOUSE_BUTTON:
	    		case SCREEN_RIGHT_MOUSE_BUTTON:
	                //handleClick(position[0], position[1]);
	    			break;
	    	}

			break;
	}
}
*/
