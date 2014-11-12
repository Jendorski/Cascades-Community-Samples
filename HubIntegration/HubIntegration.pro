APP_NAME = HubIntegration

CONFIG += qt warn_on cascades10

QMAKE_CFLAGS *= -g
QMAKE_CXXFLAGS *= -g

LIBS += -lbb -lbbsystem -lbbdata

include(config.pri)
