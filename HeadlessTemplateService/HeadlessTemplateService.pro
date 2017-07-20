APP_NAME = HeadlessTemplateService

CONFIG += qt warn_on

include(config.pri)
INCLUDEPATH += ../../$${APP_NAME}/src

LIBS += -lbb -lbbdevice -lbbdata -lbbplatform -lbbsystem
QT += network
