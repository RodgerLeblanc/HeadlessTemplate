# HeadlessTemplate
BlackBerry 10 headless app that can be taken as a starting point.

Includes Settings, inter-process communication and logging tool with bug report sent to a predefined email address.

- Create an headless app using Momentics headless template.
- Copy HeadlessCommunication folder, Logger folder, Settings folder, applicationui.cpp and applicationui.hpp into your UI src folder.
- Copy HeadlessCommunication folder, Logger folder, Settings folder, common.hpp, helpers.hpp, service.cpp and service.hpp into your Service src folder.
- Open your UI .pro file, keep only the first line and add this after :
```
HEADLESS_SERVICE = $${APP_NAME}Service

CONFIG += qt warn_on cascades10

include(config.pri)
INCLUDEPATH += ../../$${HEADLESS_SERVICE}/src

LIBS += -lbb -lbbdata -lbbdevice -lbbplatform -lbbsystem
```
- Open your Service .pro file, keep only the first line and add this after :
```
CONFIG += qt warn_on

include(config.pri)
INCLUDEPATH += ../../$${APP_NAME}/src

LIBS += -lbb -lbbdevice -lbbdata -lbbplatform -lbbsystem
QT += network
```
- Open Service/src/common.hpp and edit the first few lines to match your desired default values.
```
/*** ▼ ▼ ▼ CHANGE VALUES BETWEEN COMMENTS FOR EVERY NEW APP ▼ ▼ ▼ ***/
#define PACKAGE_NAME                                QString("com.example.HeadlessTemplate")
#define MOMENTICS_PROJECT_NAME                      QString("HeadlessTemplate")
#define BUG_REPORT_EMAIL_TO                         QString("support@yourCompany.com")
#define SHOULD_ALWAYS_KEEP_HEADLESS_ALIVE           false
#define HEADLESS_COMMUNICATION_PORT_UI              14788
#define HEADLESS_COMMUNICATION_PORT_HL              HEADLESS_COMMUNICATION_PORT_UI + 1
/*** ▲ ▲ ▲ CHANGE VALUES BETWEEN COMMENTS FOR EVERY NEW APP ▲ ▲ ▲ ***/
```
