//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2019-2020 MuseScore BVBA and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

import "../palettes"

FocusScope {
    id: root

    property var model

    Keys.onEscapePressed: {
        root.closeRequested()
    }

    Rectangle {
        anchors.fill: parent

        color: globalStyle.window
    }

    ColumnLayout {
        id: contentWrapper

        anchors.fill: parent
        anchors.margins: 20

        spacing: 20

        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop

            spacing: 20

            Text {
                Layout.fillWidth: true

                font.family: globalStyle.font.family
                font.bold: true
                font.pixelSize: 26
                color: globalStyle.buttonText
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignHCenter
                Accessible.role: Accessible.StaticText
                Accessible.name: text

                text: qsTr("Try our improved style settings")
            }

            Image {
                Layout.alignment: Qt.AlignHCenter
                source: "placeholder.png"
            }
        }

        ColumnLayout {
            Layout.fillWidth: true

            spacing: 8

            Text {
                Layout.fillWidth: true

                font.family: globalStyle.font.family
                font.bold: true
                font.pixelSize: 14
                color: globalStyle.buttonText
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignLeft
                Accessible.role: Accessible.StaticText
                Accessible.name: text

                text: qsTr("Here are some big improvements you can apply to your score:")
            }

            CheckBoxControl {
                checked: root.model ? root.model.areStylingImprovementsAllowed : false
                text: qsTr("Our professional style, spacing and font")

                onToggled: {
                    root.model.areStylingImprovementsAllowed = checked
                }
            }

            CheckBoxControl {
                checked: root.model ? root.model.areInstrumentsImprovementsAllowed : false
                text: qsTr("Instrument ordering & bracketing (configurable in the \'instruments\' dialog)")

                onToggled: {
                    root.model.areInstrumentsImprovementsAllowed = checked
                }
            }

            Text {
                Layout.fillWidth: true
                Layout.topMargin: 12

                font.family: globalStyle.font.family
                font.bold: true
                font.pixelSize: 14
                color: globalStyle.buttonText
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignLeft
                Accessible.role: Accessible.StaticText
                Accessible.name: text

                text: qsTr("<a href=\"https://musescore.com\">Learn more</a>") // TODO link to official announcement

                onLinkActivated: {
                    if (!root.model) {
                        return
                    }

                    root.model.showMoreDetails()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.margins: -12

            height: 2

            color: globalStyle.button
        }

        RowLayout {
            Layout.fillWidth: true

            CheckBoxControl {
                Layout.alignment: Qt.AlignLeft
                Layout.fillWidth: true

                checked: root.model ? root.model.shouldNeverAskForThisScoreAgain : false
                text: qsTr("Don't ask again for this score")

                onToggled: {
                    root.model.shouldNeverAskForThisScoreAgain = checked
                }
            }

            CheckBoxControl {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true

                checked: root.model ? root.model.shouldNeverAskAgain : false
                text: qsTr("Remember my choice and don't ask again")

                onToggled: {
                    root.model.shouldNeverAskAgain = checked
                }
            }
        }

        RowLayout {
            Layout.maximumWidth: parent.width / 2
            Layout.alignment: Qt.AlignRight
            Layout.leftMargin: 12

            spacing: 4

            StyledButton {
                id: ignoreButton
                Layout.fillWidth: true
                text: qsTr("Ignore")

                focus: true

                onClicked: {
                    if (!root.model) {
                        return
                    }

                    root.model.ignore()
                }
            }

            StyledButton {
                Layout.fillWidth: true
                text: qsTr("Apply")

                enabled: root.model ? root.model.isApplyingAvailable : false

                onPressed: {
                    if (!root.model) {
                        return
                    }

                    root.model.apply()
                }
            }
        }
    }
}