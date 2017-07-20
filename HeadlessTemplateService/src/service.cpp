/*
 * Copyright (c) 2013-2015 BlackBerry Limited.
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

#include "service.hpp"

#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/system/InvokeManager>

#include <QTimer>

using namespace bb::platform;
using namespace bb::system;

Service::Service() :
        QObject(),
        headlessCommunication(NULL),
        invokeManager(NULL),
        notification(NULL),
        settings(NULL),
        environment(Environment::Headless)
{
    headlessCommunication = new HeadlessCommunication(environment, this);

    Logger::init(headlessCommunication, this);

    // Add this one line of code to all your projects, it will save you a ton of problems
    // when dealing with foreign languages. No more ??? characters.
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    settings = Settings::instance(headlessCommunication, this);

    init();
}

void Service::init() {
    invokeManager = new InvokeManager(this);
    notification = new Notification(this);

    connect(headlessCommunication, SIGNAL(receivedData(QString, QVariant)), this, SLOT(onReceivedData(QString, QVariant)));
    invokeManager->connect(invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoked(const bb::system::InvokeRequest&)));

    NotificationDefaultApplicationSettings notificationDefaultApplicationSettings;
    notificationDefaultApplicationSettings.setPreview(NotificationPriorityPolicy::Allow);
    notificationDefaultApplicationSettings.apply();

    if (SHOULD_ALWAYS_KEEP_HEADLESS_ALIVE) {
        // Next code will reopen this app in case it gets terminated
        QString registerTimerName = bb::Application::instance()->applicationName().remove(" ");
        InvokeReply* reply = invokeManager->deregisterTimer(registerTimerName);
        reply->deleteLater();

        InvokeRecurrenceRule recurrenceRule(bb::system::InvokeRecurrenceRuleFrequency::Minutely);
        recurrenceRule.setInterval(6); // Minimum valid interval for Minutely frequency
        if (recurrenceRule.isValid()) {
            InvokeTimerRequest invokeTimerRequest("registerTimerName", recurrenceRule, HEADLESS_INVOCATION_TARGET);
            InvokeReply* reply = invokeManager->registerTimer(invokeTimerRequest);
            connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
        }
    }

    if (!settings->contains(SETTINGS_INSTALLED_DATE)) {
        settings->setValue(SETTINGS_INSTALLED_DATE, QDateTime::currentDateTime());
    }

    LOG("******* Headless Started *******");
}

void Service::onInvoked(const bb::system::InvokeRequest & request)
{
    QString action = request.action().split(".").last();

    if (action == "TIMER_FIRED") {
        return;
    }

    LOG("onInvoked()", request.action());

    if (request.action() == HEADLESS_INVOCATION_SHUTDOWN_ACTION) {
        bb::Application::instance()->quit();
    }
    else if (request.action() == HEADLESS_INVOCATION_SEND_BUG_REPORT_ACTION) {
        Logger::save();
        headlessCommunication->sendMessage(LOG_READY_FOR_BUG_REPORT);
    }
    else if (request.action() == HEADLESS_INVOCATION_SEND_LOG_TO_HUB_ACTION) {
        QString title = APP_NAME;
        QString body = STRING(Logger::getLog());
        notification->setTitle(title);
        notification->setBody(body);
        notification->notify();
    }
}

void Service::onReceivedData(QString reason, QVariant data) {
    //Headless use invocation framework to receive data whenever possible.
    //This allows the headless app to be restarted if it was not running
    //when the data was sent from UI.
    Q_UNUSED(reason);
    Q_UNUSED(data);

    qDebug() << "Service::onReceivedData()" << reason << data;
}
