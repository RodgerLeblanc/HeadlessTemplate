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

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/system/InvokeManager>

#include <bb/PpsObject>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>
#include <bb/data/JsonDataAccess>

#include <bb/PackageInfo>
#include <bb/device/HardwareInfo>
#include <bb/platform/PlatformInfo>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::device;
using namespace bb::platform;
using namespace bb::system;

ApplicationUI::ApplicationUI() :
        QObject(),
        headlessCommunication(NULL),
        invokeManager(NULL),
        localeHandler(NULL),
        settings(NULL),
        translator(NULL),
        environment(Environment::UI)
{
    this->invokeHL(HEADLESS_INVOCATION_START_ACTION);

    headlessCommunication = new HeadlessCommunication(environment, this);

    Logger::init(headlessCommunication, this);

    // Add this one line of code to all your projects, it will save you a ton of problems
    // when dealing with foreign languages. No more ??? characters.
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    settings = Settings::instance(headlessCommunication, this);

    init();
}

void ApplicationUI::checkForChangelog() {
    if (settings->contains(SETTINGS_LAST_VERSION_LOADED)) {
        QVariantMap changelogMap = Helpers::safeReadJsonFile(CHANGELOG_FILE).toMap();
        QStringList lastVersionLoadedInSettings = settings->value(SETTINGS_LAST_VERSION_LOADED).toString().split(".");

        QString newChangelogForThisUser;

        QStringList changelogVersions = changelogMap.keys();
        foreach(QString changelog, changelogVersions) {
            QStringList changelogVersion = changelog.split(".");

            bool showThisChangelog = false;
            int maxSize = qMax(changelogVersion.size(), lastVersionLoadedInSettings.size());

            for (int i = 0; i < maxSize; i++) {
                if (changelogVersion[i].toInt() > lastVersionLoadedInSettings[i].toInt()) {
                    showThisChangelog = true;
                    break;
                }
                else {
                    if (changelogVersion[i].toInt() < lastVersionLoadedInSettings[i].toInt()) {
                        break;
                    }
                }
            }

            if (showThisChangelog) {
                newChangelogForThisUser.prepend(changelog + "\n" + changelogMap[changelog].toString() + "\n\n");
            }
        }
        if (!newChangelogForThisUser.isEmpty())
            emit newChangelog(newChangelogForThisUser);
    }

    settings->setValue(SETTINGS_LAST_VERSION_LOADED, APP_VERSION);
}

void ApplicationUI::init() {
    invokeManager = new InvokeManager(this);
    localeHandler = new LocaleHandler(this);
    translator = new QTranslator(this);

    connect(headlessCommunication, SIGNAL(receivedData(QString, QVariant)), this, SLOT(onReceivedData(QString, QVariant)));
    connect(localeHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));

    onSystemLanguageChanged();

    // Registering QTimer for easy QML access. We add it to bb.cascades to avoid having to import
    // a separate library for such a tiny addition. (Don't worry, it won't slow down your bb.cascades
    // library loading)
    qmlRegisterType<QTimer>("bb.cascades", 1, 0, "QTimer");

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("app", this);
    qml->setContextProperty("settings", settings);
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    Application::instance()->setScene(root);

    LOG("******* UI Started *******");

    QMetaObject::invokeMethod(this, "checkForChangelog", Qt::QueuedConnection);
}

void ApplicationUI::invokeHL(QString action) {
    InvokeRequest request;
    request.setTarget(HEADLESS_INVOCATION_TARGET);
    request.setAction(action);
    invokeManager->invoke(request);
}

void ApplicationUI::onReceivedData(QString reason, QVariant data) {
    Q_UNUSED(data);

    if (reason == LOG_READY_FOR_BUG_REPORT) {
        this->sendBugReport();
    }
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(translator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("%1_%2").arg(MOMENTICS_PROJECT_NAME, locale_string);
    if (translator->load(file_name, "app/native/qm")) {
    QCoreApplication::instance()->installTranslator(translator);
    }
}

void ApplicationUI::requestBugReport() {
    this->sendToHL(HEADLESS_INVOCATION_SEND_BUG_REPORT_ACTION);
}

void ApplicationUI::sendBugReport() {
    HardwareInfo hardwareInfo;
    bb::PackageInfo packageInfo;
    PlatformInfo platformInfo;

    InvokeRequest request;
    request.setTarget("sys.pim.uib.email.hybridcomposer");
    request.setAction("bb.action.COMPOSE");
    request.setMimeType("message/rfc822");
    QVariantMap data;
    data["to"] = BUG_REPORT_EMAIL_TO;
    data["subject"] = tr("Bug Report for ") + APP_NAME + " " + APP_VERSION + " (" + hardwareInfo.modelName() + " " + platformInfo.osVersion() + ")";
    data["body"] = tr("Hi ") + packageInfo.author() + tr(", I've found this bug in the app:\n\n\nHere's the steps to reproduce:\n\n");

    QStringList log = Helpers::safeReadJsonFile(LOG_FILE).toMap()["log"].toStringList();
    QVariantMap userSettings = Helpers::safeReadJsonFile(SETTINGS_FILE).toMap();

    QVariantMap logMap;
    logMap.insert("Time", QDateTime::currentDateTime().toString(DATETIME_TO_STRING));
    logMap.insert("applicationVersion", APP_VERSION);
    logMap.insert("installedDate", settings->value(SETTINGS_INSTALLED_DATE).toDateTime().toString(DATETIME_TO_STRING));
    logMap.insert("log", log);
    logMap.insert("settings", userSettings);

    Helpers::safeWriteJsonFile(logMap, LOG_SHAREWITH_PATH);

    data["attachment"] = QDir::currentPath() + "/" + LOG_SHAREWITH_PATH;
    QVariantMap moreData;
    moreData.insert("data", data);

    bool ok;
    request.setData(bb::PpsObject::encode(moreData, &ok));

    if (!ok) {
        LOG("PpsObject wasn't able to encode data:", moreData);
    }

    invokeManager->invoke(request);
}

void ApplicationUI::sendToHL(QString action) {
    headlessCommunication->sendMessage(action);
}

void ApplicationUI::shutdown() {
    this->invokeHL(HEADLESS_INVOCATION_SHUTDOWN_ACTION);

    bb::Application::instance()->quit();
}
