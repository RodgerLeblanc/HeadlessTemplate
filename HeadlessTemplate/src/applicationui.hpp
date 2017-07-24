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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include "common.hpp"
#include <src/HeadlessCommunication/HeadlessCommunication.h>
#include <src/Logger/Logger.h>
#include <src/Settings/Settings.h>

#include <QObject>

namespace bb {
    namespace cascades {
        class LocaleHandler;
    }
    namespace system {
        class InvokeManager;
    }
}

class QTranslator;

/*!
 * @brief Application UI object
 *
 * Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI: public QObject
{
    Q_OBJECT
public:
    ApplicationUI();
    virtual ~ApplicationUI() { }

    Q_INVOKABLE void invokeHL(QString action);
    Q_INVOKABLE void requestBugReport();
    Q_INVOKABLE void sendToHL(QString action);
    Q_INVOKABLE void shutdown();

private slots:
    void checkForChangelog();
    void onReceivedData(QString reason, QVariant data);
    void onSystemLanguageChanged();

private:
    void init();
    void sendBugReport();

    HeadlessCommunication* headlessCommunication;
    bb::system::InvokeManager* invokeManager;
    bb::cascades::LocaleHandler* localeHandler;
    Settings* settings;
    QTranslator* translator;

    Environment::Type environment;
    QString supportEmail;

signals:
    void newChangelog(QString);
};

#endif /* ApplicationUI_HPP_ */
