/*
 * Logger.cpp
 *
 *  Created on: 2014-09-06
 *      Author: Roger
 */

/*
 *
 * To add this object to your project, follow these steps :
 * 1- Copy Logger folder to your src folder
 * 2- In your applicationui.hpp or service.hpp, add those lines :
 *      #include <src/Logger/Logger.h>
 *
 * 3- Use it anywhere in your cpp file using the macro, it accepts
 * any number of arguments of QVariant type :
 *      LOG("Hello World!", 123, true, QDateTime::currentDateTime());
 *
 * There's also two other macros available :
 *      LOG_VAR(x)  ===> will log {variable name}: {variable value}
 *      STRING(x)   ===> will convert any QVariant to QString
 *
 */

#include <src/Logger/Logger.h>
#include <QDebug>
#include <QFile>

#include <bb/device/HardwareInfo>
#include <bb/platform/Notification>
#include <bb/platform/PlatformInfo>

using namespace bb::data;
using namespace bb::device;
using namespace bb::platform;

Logger::Logger(QObject* parent) :
    QObject(parent),
    lastSaveDateTime(QDateTime::currentDateTime())
{
}

Logger::~Logger()
{
    log("~Logger()");
    save();
}

QString Logger::convertToString(QVariant variant) {
    if (variant.canConvert(QVariant::String)) {
        return variant.toString();
    }

    QString toReturn;
    JsonDataAccess jda;
    jda.saveToBuffer(variant, &toReturn);

    return toReturn;
}

QVariantMap Logger::getLog() {
    HardwareInfo hardwareInfo;
    PlatformInfo platformInfo;

    save();
    QVariantMap log = Logger::instance()->logMap;
    log.insert("App version", QCoreApplication::applicationVersion());
    log.insert("Device", QString(hardwareInfo.modelName() + " " + platformInfo.osVersion()));
    return log;
}

Logger* Logger::init(HeadlessCommunication* headlessCommunication, QObject* parent) {
    Q_ASSERT(headlessCommunication->getEnvironment() != Environment::Unknown);

    Logger* instance = Logger::instance(parent);

    instance->loggerCommunication = headlessCommunication;
    instance->setEnvironment(headlessCommunication->getEnvironment());

    if (!instance->isInUiThread) {
        connect(instance->loggerCommunication, SIGNAL(receivedData(QString, QVariant)), instance, SLOT(onReceivedData(QString, QVariant)));
        instance->sync();
    }

    return instance;
}

void Logger::logFinal(QString message) {
    Logger* logger = Logger::instance();
    if (logger->isInUiThread) {
        logger->loggerCommunication->sendMessage(LOGGER_LOG_THIS, message);
        return;
    }

    QStringList log = logger->logMap["log"].toStringList();
    log.prepend(QTime::currentTime().toString(TIME_TO_STRING) + " " + QString::number(HeapUsage::measureMem()) + " " + message);

    while (log.size() > MAXIMUM_LOG_SIZE) {
        log.removeLast();
    }

    logger->logMap.insert("log", log);

    QDateTime now = QDateTime::currentDateTime();
    if (logger->lastSaveDateTime.secsTo(now) > 60) {
        save();
    }
}

void Logger::onReceivedData(QString reason, QVariant data) {
    if (reason == LOGGER_LOG_THIS) {
        QString message = data.toString();
        log(message);
    }
    else if (reason == HEADLESS_INVOCATION_SEND_BUG_REPORT_ACTION) {
        save();
        loggerCommunication->sendMessage(LOG_READY_FOR_BUG_REPORT);
    }
    else if (reason == HEADLESS_INVOCATION_SEND_LOG_TO_HUB_ACTION) {
        QString title = APP_NAME;
        QString body = STRING(Logger::getLog());
        Notification notification;
        notification.setTitle(title);
        notification.setBody(body);
        notification.notify();
    }
}

void Logger::save()
{
    Helpers::safeWriteJsonFile(Logger::instance()->logMap, LOG_FILE);
}

void Logger::setEnvironment(Environment::Type environmentType) {
    environment = environmentType;
    isInUiThread = environment == Environment::UI;
    qDebug() << "Logger::setEnvironment()" << environment << isInUiThread;
}

void Logger::sync() {
    logMap = Helpers::safeReadJsonFile(LOG_FILE).toMap();
}
