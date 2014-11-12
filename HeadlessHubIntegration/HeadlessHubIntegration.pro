APP_NAME = HeadlessHubIntegration

CONFIG += qt warn_on cascades10

QMAKE_CFLAGS *= -g
QMAKE_CXXFLAGS *= -g

LIBS += -lbb -lbbsystem -lbbdata -lbbnetwork -lunifieddatasourcec -lbbpim

include(config.pri)
