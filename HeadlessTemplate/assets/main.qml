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

import bb.cascades 1.4
import "Changelog"

Page {
    onCreationCompleted: {
        app.newChangelog.connect(onNewChangelog)
    }
    
    function onNewChangelog(newChangelogForThisUser) {
        console.log("onNewChangelog: " + newChangelogForThisUser)
        var createdControl = changelogSheet.createObject()
        createdControl.changelogText = newChangelogForThisUser

        //If you don't specify countdownSeconds, it will default to 10 
        createdControl.countdownSeconds = 5

        //If you don't specify colors, it will default to white background with black border and text
        createdControl.changelogOuterBackground = Color.Magenta
        createdControl.changelogBorderColor = Color.Cyan
        createdControl.changelogInnerBackground = Color.LightGray
        createdControl.changelogTextColor = Color.DarkBlue
                
        createdControl.open()
    }
    
    attachedObjects: [
        ComponentDefinition { id: changelogSheet; ChangelogSheet {} }
    ]

    Container {
        layout: DockLayout {}
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        Container {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            Container {
                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                horizontalAlignment: HorizontalAlignment.Center
                ToggleButton {
                    checked: settings.value("checked", false)
                    verticalAlignment: VerticalAlignment.Center
                    onCheckedChanged: settings.setValue("checked", checked)
                }
                Label {
                    //Supports Q_PROPERTY notifications using the 'allSettings' property
                    text: "Settings value for 'checked': " + settings.allSettings["checked"]
                    verticalAlignment: VerticalAlignment.Center
                }
            }
            Button {
                text: qsTr("HeadlessCommunication test") + Retranslate.onLocaleOrLanguageChanged
                horizontalAlignment: HorizontalAlignment.Center
                onClicked: {
                    app.sendToHL("Test")
                }
            }
            Button {
                text: qsTr("Send bug report") + Retranslate.onLocaleOrLanguageChanged
                horizontalAlignment: HorizontalAlignment.Center
                onClicked: {
                    app.requestBugReport();
                }
            }
            Button {
                text: qsTr("Shutdown") + Retranslate.onLocaleOrLanguageChanged
                horizontalAlignment: HorizontalAlignment.Center
                onClicked: {
                    app.shutdown();
                }
            }
        }
    }
}
