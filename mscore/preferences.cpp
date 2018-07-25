//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//  $Id: preferences.cpp 5660 2012-05-22 14:17:39Z wschweer $
//
//  Copyright (C) 2002-2016 Werner Schweer and others
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

#include "libmscore/style.h"
#include "libmscore/mscore.h"
#include "preferences.h"

namespace Ms {

Preferences preferences;


void Preferences::init(bool storeInMemoryOnly)
      {
      if (!storeInMemoryOnly) {
            if (_settings)
                  delete _settings;
            _settings = new QSettings();
            }

      _storeInMemoryOnly = storeInMemoryOnly;

#if defined(Q_OS_MAC) || (defined(Q_OS_WIN) && !defined(FOR_WINSTORE))
      bool checkUpdateStartup = true;
      bool checkExtensionsUpdateStartup = true;
#else
      bool checkUpdateStartup = false;
      bool checkExtensionsUpdateStartup = false;
#endif

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
      // use system native file dialogs
      // Qt file dialog is very slow on Windows and Mac
      bool nativeDialogs           = true;
#else
      bool nativeDialogs           = false;    // don't use system native file dialogs
#endif
      bool defaultUsePortAudio = false;
      bool defaultUsePulseAudio = false;
      bool defaultUseJackAudio = false;
      bool defaultUseAlsaAudio = false;

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
      defaultUsePortAudio  = true;
      // Linux
#elif defined(USE_PULSEAUDIO)
      defaultUsePulseAudio  = true;
#elif defined(USE_ALSA)
      defaultUseAlsaAudio = true;
#elif defined(USE_PORTAUDIO)
      defaultUsePortAudio = true;
#endif

      QString wd = QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).arg(QCoreApplication::applicationName());

      // The following define is needed in order to translate the advanced preferences.
      // These translations are accessible via qApp->translate("MusEScore", #WantedDefine)
#define TR_PREF(s) QString(QT_TRANSLATE_NOOP3("MusEScore", s, "Do not change where the slashes ('/') are. Translate what's in between.")).toLatin1()
      // Some of these translations won't be used for now, since
      //  1) enums are still not supported in the advanced tab, and
      //  2) there is still no option to show all preferences in the advanced tab.
      // And the advancedTab is (for now) the only place these translations are used.

      _allPreferences = prefs_map_t(
      {
            {TR_PREF(PREF_APP_AUTOSAVE_AUTOSAVETIME),                       new IntPreference(2 /* minutes */, false)},
            {TR_PREF(PREF_APP_AUTOSAVE_USEAUTOSAVE),                        new BoolPreference(true, false)},
            {TR_PREF(PREF_APP_KEYBOARDLAYOUT),                              new StringPreference("US - International")},
            {TR_PREF(PREF_APP_PATHS_INSTRUMENTLIST1),                       new FilePreference(":/data/instruments.xml", QCoreApplication::translate("instrument_list", "Instrument List") + " (*.xml)", false)},
            {TR_PREF(PREF_APP_PATHS_INSTRUMENTLIST2),                       new FilePreference("", QCoreApplication::translate("instrument_list", "Instrument List") + " (*.xml)", false)},
            {TR_PREF(PREF_APP_PATHS_MYIMAGES),                              new DirPreference(QFileInfo(QString("%1/%2").arg(wd).arg(QCoreApplication::translate("images_directory", "Images"))).absoluteFilePath(), false)},
            {TR_PREF(PREF_APP_PATHS_MYPLUGINS),                             new DirPreference(QFileInfo(QString("%1/%2").arg(wd).arg(QCoreApplication::translate("plugins_directory", "Plugins"))).absoluteFilePath(), false)},
            {TR_PREF(PREF_APP_PATHS_MYSCORES),                              new DirPreference(QFileInfo(QString("%1/%2").arg(wd).arg(QCoreApplication::translate("scores_directory", "Scores"))).absoluteFilePath(), false)},
            {TR_PREF(PREF_APP_PATHS_MYSOUNDFONTS),                          new DirPreference(QFileInfo(QString("%1/%2").arg(wd).arg(QCoreApplication::translate("soundfonts_directory", "SoundFonts"))).absoluteFilePath(), false)},
            {TR_PREF(PREF_APP_PATHS_MYSHORTCUTS),                           new DirPreference(QFileInfo(QString("%1/%2").arg(wd).arg(QCoreApplication::translate("shortcuts_directory", "Shortcuts"))).absoluteFilePath(), false)},
            {TR_PREF(PREF_APP_PATHS_MYSTYLES),                              new DirPreference(QFileInfo(QString("%1/%2").arg(wd).arg(QCoreApplication::translate("styles_directory", "Styles"))).absoluteFilePath(), false)},
            {TR_PREF(PREF_APP_PATHS_MYTEMPLATES),                           new DirPreference(QFileInfo(QString("%1/%2").arg(wd).arg(QCoreApplication::translate("templates_directory", "Templates"))).absoluteFilePath(), false)},
            {TR_PREF(PREF_APP_PATHS_MYEXTENSIONS),                          new DirPreference(QFileInfo(QString("%1/%2").arg(wd).arg(QCoreApplication::translate("extensions_directory", "Extensions"))).absoluteFilePath(), false)},
            {TR_PREF(PREF_APP_PLAYBACK_FOLLOWSONG),                         new BoolPreference(true)},
            {TR_PREF(PREF_APP_PLAYBACK_PANPLAYBACK),                        new BoolPreference(true)},
            {TR_PREF(PREF_APP_PLAYBACK_PLAYREPEATS),                        new BoolPreference(true)},
            {TR_PREF(PREF_APP_USESINGLEPALETTE),                            new BoolPreference(false)},
            {TR_PREF(PREF_APP_STARTUP_FIRSTSTART),                          new BoolPreference(true)},
            {TR_PREF(PREF_APP_STARTUP_SESSIONSTART),                        new EnumPreference(QVariant::fromValue(SessionStart::SCORE), false)},
            {TR_PREF(PREF_APP_STARTUP_STARTSCORE),                          new FilePreference(":/data/My_First_Score.mscz", QCoreApplication::translate("MuseScore_files", "MuseScore Files") + " (*.mscz *.mscx);;" + QCoreApplication::translate("MuseScore_files", "All") + " (*)", false)},
            {TR_PREF(PREF_APP_WORKSPACE),                                   new StringPreference("Basic", false)},
            {TR_PREF(PREF_EXPORT_AUDIO_SAMPLERATE),                         new IntPreference(44100, false)},
            {TR_PREF(PREF_EXPORT_MP3_BITRATE),                              new IntPreference(128, false)},
            {TR_PREF(PREF_EXPORT_MUSICXML_EXPORTBREAKS),                    new EnumPreference(QVariant::fromValue(MusicxmlExportBreaks::ALL), false)},
            {TR_PREF(PREF_EXPORT_MUSICXML_EXPORTLAYOUT),                    new BoolPreference(true, false)},
            {TR_PREF(PREF_EXPORT_PDF_DPI),                                  new IntPreference(300, false)},
            {TR_PREF(PREF_EXPORT_PNG_RESOLUTION),                           new DoublePreference(300.0, false)},
            {TR_PREF(PREF_EXPORT_PNG_USETRANSPARENCY),                      new BoolPreference(true, false)},
            {TR_PREF(PREF_IMPORT_GUITARPRO_CHARSET),                        new StringPreference("UTF-8", false)},
            {TR_PREF(PREF_IMPORT_MUSICXML_IMPORTBREAKS),                    new BoolPreference(true, false)},
            {TR_PREF(PREF_IMPORT_MUSICXML_IMPORTLAYOUT),                    new BoolPreference(true, false)},
            {TR_PREF(PREF_IMPORT_OVERTURE_CHARSET),                         new StringPreference("GBK", false)},
            {TR_PREF(PREF_IMPORT_STYLE_STYLEFILE),                          new FilePreference("", QCoreApplication::translate("MuseScore_styles", "MuseScore Styles") + " (*.mss)", false)},
            {TR_PREF(PREF_IO_ALSA_DEVICE),                                  new StringPreference("default", false)},
            {TR_PREF(PREF_IO_ALSA_FRAGMENTS),                               new IntPreference(3, false)},
            {TR_PREF(PREF_IO_ALSA_PERIODSIZE),                              new IntPreference(1024, false)},
            {TR_PREF(PREF_IO_ALSA_SAMPLERATE),                              new IntPreference(48000, false)},
            {TR_PREF(PREF_IO_ALSA_USEALSAAUDIO),                            new BoolPreference(defaultUseAlsaAudio, false)},
            {TR_PREF(PREF_IO_JACK_REMEMBERLASTCONNECTIONS),                 new BoolPreference(true, false)},
            {TR_PREF(PREF_IO_JACK_TIMEBASEMASTER),                          new BoolPreference(false, false)},
            {TR_PREF(PREF_IO_JACK_USEJACKAUDIO),                            new BoolPreference(defaultUseJackAudio, false)},
            {TR_PREF(PREF_IO_JACK_USEJACKMIDI),                             new BoolPreference(false, false)},
            {TR_PREF(PREF_IO_JACK_USEJACKTRANSPORT),                        new BoolPreference(false, false)},
            {TR_PREF(PREF_IO_MIDI_ADVANCEONRELEASE),                        new BoolPreference(true, false)},
            {TR_PREF(PREF_IO_MIDI_ENABLEINPUT),                             new BoolPreference(true, false)},
            {TR_PREF(PREF_IO_MIDI_EXPANDREPEATS),                           new BoolPreference(true, false)},
            {TR_PREF(PREF_IO_MIDI_EXPORTRPNS),                              new BoolPreference(false, false)},
            {TR_PREF(PREF_IO_MIDI_REALTIMEDELAY),                           new IntPreference(750 /* ms */, false)},
            {TR_PREF(PREF_IO_MIDI_SHORTESTNOTE),                            new IntPreference(MScore::division/4, false)},
            {TR_PREF(PREF_IO_MIDI_SHOWCONTROLSINMIXER),                     new BoolPreference(false, false)},
            {TR_PREF(PREF_IO_MIDI_USEREMOTECONTROL),                        new BoolPreference(false, false)},
            {TR_PREF(PREF_IO_OSC_PORTNUMBER),                               new IntPreference(5282, false)},
            {TR_PREF(PREF_IO_OSC_USEREMOTECONTROL),                         new BoolPreference(false, false)},
            {TR_PREF(PREF_IO_PORTAUDIO_DEVICE),                             new IntPreference(-1, false)},
            {TR_PREF(PREF_IO_PORTAUDIO_USEPORTAUDIO),                       new BoolPreference(defaultUsePortAudio, false)},
            {TR_PREF(PREF_IO_PORTMIDI_INPUTBUFFERCOUNT),                    new IntPreference(100)},
            {TR_PREF(PREF_IO_PORTMIDI_INPUTDEVICE),                         new StringPreference("")},
            {TR_PREF(PREF_IO_PORTMIDI_OUTPUTBUFFERCOUNT),                   new IntPreference(65536)},
            {TR_PREF(PREF_IO_PORTMIDI_OUTPUTDEVICE),                        new StringPreference("")},
            {TR_PREF(PREF_IO_PORTMIDI_OUTPUTLATENCYMILLISECONDS),           new IntPreference(0)},
            {TR_PREF(PREF_IO_PULSEAUDIO_USEPULSEAUDIO),                     new BoolPreference(defaultUsePulseAudio, false)},
            {TR_PREF(PREF_SCORE_CHORD_PLAYONADDNOTE),                       new BoolPreference(true, false)},
            {TR_PREF(PREF_SCORE_MAGNIFICATION),                             new DoublePreference(1.0, false)},
            {TR_PREF(PREF_SCORE_NOTE_PLAYONCLICK),                          new BoolPreference(true, false)},
            {TR_PREF(PREF_SCORE_NOTE_DEFAULTPLAYDURATION),                  new IntPreference(300 /* ms */, false)},
            {TR_PREF(PREF_SCORE_NOTE_WARNPITCHRANGE),                       new BoolPreference(true, false)},
            {TR_PREF(PREF_SCORE_STYLE_DEFAULTSTYLEFILE),                    new FilePreference("", QCoreApplication::translate("MuseScore_styles", "MuseScore Styles") + " (*.mss)", false)},
            {TR_PREF(PREF_SCORE_STYLE_PARTSTYLEFILE),                       new FilePreference("", QCoreApplication::translate("MuseScore_styles", "MuseScore Styles") + " (*.mss)", false)},
            {TR_PREF(PREF_UI_CANVAS_BG_USECOLOR),                           new BoolPreference(true, false)},
            {TR_PREF(PREF_UI_CANVAS_FG_USECOLOR),                           new BoolPreference(true, false)},
            {TR_PREF(PREF_UI_CANVAS_BG_COLOR),                              new ColorPreference(QColor("#dddddd"), false)},
            {TR_PREF(PREF_UI_CANVAS_FG_COLOR),                              new ColorPreference(QColor("#f9f9f9"), false)},
            {TR_PREF(PREF_UI_CANVAS_BG_WALLPAPER),                          new FilePreference(QFileInfo(QString("%1%2").arg(mscoreGlobalShare).arg("wallpaper/background1.png")).absoluteFilePath(), QCoreApplication::translate("images_files", "Images") + " (*.jpg *.jpeg *.png);;" + QCoreApplication::translate("images_files", "All") + " (*)", false)},
            {TR_PREF(PREF_UI_CANVAS_FG_WALLPAPER),                          new FilePreference(QFileInfo(QString("%1%2").arg(mscoreGlobalShare).arg("wallpaper/paper5.png")).absoluteFilePath(), QCoreApplication::translate("images_files", "Images") + " (*.jpg *.jpeg *.png);;" + QCoreApplication::translate("images_files", "All") + " (*)", false)},
            {TR_PREF(PREF_UI_CANVAS_MISC_ANTIALIASEDDRAWING),               new BoolPreference(true, false)},
            {TR_PREF(PREF_UI_CANVAS_MISC_SELECTIONPROXIMITY),               new IntPreference(6, false)},
            {TR_PREF(PREF_UI_CANVAS_SCROLL_LIMITSCROLLAREA),                new BoolPreference(false, false)},
            {TR_PREF(PREF_UI_CANVAS_SCROLL_VERTICALORIENTATION),            new BoolPreference(false, false)},
            {TR_PREF(PREF_UI_APP_STARTUP_CHECKUPDATE),                      new BoolPreference(checkUpdateStartup, false)},
            {TR_PREF(PREF_UI_APP_STARTUP_CHECK_EXTENSIONS_UPDATE),          new BoolPreference(checkExtensionsUpdateStartup, false)},
            {TR_PREF(PREF_UI_APP_STARTUP_SHOWNAVIGATOR),                    new BoolPreference(false, false)},
            {TR_PREF(PREF_UI_APP_STARTUP_SHOWPLAYPANEL),                    new BoolPreference(false, false)},
            {TR_PREF(PREF_UI_APP_STARTUP_SHOWSPLASHSCREEN),                 new BoolPreference(true, false)},
            {TR_PREF(PREF_UI_APP_STARTUP_SHOWSTARTCENTER),                  new BoolPreference(true, false)},
            {TR_PREF(PREF_UI_APP_GLOBALSTYLE),                              new EnumPreference(QVariant::fromValue(MuseScoreStyleType::LIGHT_FUSION), false)},
            {TR_PREF(PREF_UI_APP_LANGUAGE),                                 new StringPreference("system", false)},
            {TR_PREF(PREF_UI_APP_RASTER_HORIZONTAL),                        new IntPreference(2)},
            {TR_PREF(PREF_UI_APP_RASTER_VERTICAL),                          new IntPreference(2)},
            {TR_PREF(PREF_UI_APP_SHOWSTATUSBAR),                            new BoolPreference(true)},
            {TR_PREF(PREF_UI_APP_USENATIVEDIALOGS),                         new BoolPreference(nativeDialogs)},
            {TR_PREF(PREF_UI_PIANOHIGHLIGHTCOLOR),                          new ColorPreference(QColor("#1259d0"))},
            {TR_PREF(PREF_UI_SCORE_NOTEDROPCOLOR),                          new ColorPreference(QColor("#1778db"))},
            {TR_PREF(PREF_UI_SCORE_DEFAULTCOLOR),                           new ColorPreference(QColor("#000000"))},
            {TR_PREF(PREF_UI_SCORE_FRAMEMARGINCOLOR),                       new ColorPreference(QColor("#5999db"))},
            {TR_PREF(PREF_UI_SCORE_LAYOUTBREAKCOLOR),                       new ColorPreference(QColor("#5999db"))},
            {TR_PREF(PREF_UI_SCORE_VOICES_VOICE1COLOR),                     new ColorPreference(QColor("#1259d0"))},    // blue
            {TR_PREF(PREF_UI_SCORE_VOICES_VOICE2COLOR),                     new ColorPreference(QColor("#009234"))},    // green
            {TR_PREF(PREF_UI_SCORE_VOICES_VOICE3COLOR),                     new ColorPreference(QColor("#c04400"))},    // orange
            {TR_PREF(PREF_UI_SCORE_VOICES_VOICE4COLOR),                     new ColorPreference(QColor("#70167a"))},    // purple
            {TR_PREF(PREF_UI_THEME_ICONWIDTH),                              new IntPreference(28, false)},
            {TR_PREF(PREF_UI_THEME_ICONHEIGHT),                             new IntPreference(24, false)}
      });
#undef TR_PREF

      _initialized = true;
      }

void Preferences::save()
      {
      settings()->sync();
      }

QVariant Preferences::defaultValue(const QString key) const
      {
      checkIfKeyExists(key);
      Preference* pref = _allPreferences.value(key);
      return pref->defaultValue();
      }

QSettings* Preferences::settings() const
      {
      if (!_initialized) {
            qWarning("Preferences is not initialized. Call init() to initialize.");
            Q_ASSERT(_initialized);
            }

      return _settings;
      }

QVariant Preferences::get(const QString key) const
      {
      QVariant pref = _inMemorySettings.value(key);

      if (_storeInMemoryOnly)
            return (_inMemorySettings.contains(key)) ? pref : QVariant(); // invalid QVariant returned when not found
      else if (_inMemorySettings.contains(key)) // if there exists a temporary value stored "in memory" return this value
            return pref;
      else
            return settings()->value(key);
      }

void Preferences::set(const QString key, QVariant value, bool temporary)
      {
      if (_storeInMemoryOnly || temporary)
            _inMemorySettings[key] = value;
      else
            settings()->setValue(key, value);
      }

void Preferences::remove(const QString key)
      {
      // remove both preference stored "in memory" and in QSettings
      _inMemorySettings.remove(key);
      settings()->remove(key);
      }

bool Preferences::has(const QString key) const
      {
      return _inMemorySettings.contains(key) > 0 || settings()->contains(key);
      }

QVariant Preferences::preference(const QString key) const
      {
      checkIfKeyExists(key);
      QVariant pref = get(key);

      // pref is invalid both if setting is not found or pref is an invalid QVariant object
      if (!pref.isValid() || pref.isNull() || _returnDefaultValues)
            return defaultValue(key);
      else
            return pref;
      }

bool Preferences::checkIfKeyExists(const QString key) const
      {
      bool exists = _allPreferences.contains(key);
      if (!exists) {
            qWarning("Preference not found: %s", key.toStdString().c_str());
            Q_ASSERT(exists);
            }
      return exists;
      }

QMetaType::Type Preferences::type(const QString key) const
      {
      if (_allPreferences.contains(key))
            return _allPreferences.value(key)->type();
      else {
            return QMetaType::UnknownType;
            }
      }

bool Preferences::checkType(const QString key, QMetaType::Type t) const
      {
      if (type(key) != t) {
            qWarning("Preference is not of correct type: %s", key.toStdString().c_str());
            Q_ASSERT(type(key) == QMetaType::Bool);
            }
      return type(key) == t;
      }

Preferences::Preferences()
      : _settings(0)
      {}

Preferences::~Preferences()
      {
      // clean up _allPreferences
      for (Preference* pref : _allPreferences.values())
            delete pref;

      if (_settings) {
            delete _settings;
            }
      }

bool Preferences::getBool(const QString key) const
      {
      checkType(key, QMetaType::Bool);
      return preference(key).toBool();
      }

QColor Preferences::getColor(const QString key) const
      {
      checkType(key, QMetaType::QColor);
      QVariant v = preference(key);
      if (v.type() == QVariant::Color)
            return v.value<QColor>();
      else {
            // in case the color is expressed in settings file as a textual color representation
            QColor c(v.toString());
            return c.isValid() ? c : defaultValue(key).value<QColor>();
            }
      }

QString Preferences::getString(const QString key) const
      {
      checkType(key, QMetaType::QString);
      return preference(key).toString();
      }

int Preferences::getInt(const QString key) const
      {
      checkType(key, QMetaType::Int);
      QVariant v = preference(key);
      bool ok;
      int pref = v.toInt(&ok);
      if (!ok) {
            qWarning("Can not convert preference %s to int. Returning default value.", key.toStdString().c_str());
            return defaultValue(key).toInt();
            }
      return pref;
}

double Preferences::getDouble(const QString key) const
      {
      checkType(key, QMetaType::Double);
      QVariant v = preference(key);
      bool ok;
      double pref = v.toDouble(&ok);
      if (!ok) {
            qWarning("Can not convert preference %s to double. Returning default value.", key.toStdString().c_str());
            return defaultValue(key).toDouble();
            }
      return pref;
      }

SessionStart Preferences::sessionStart() const
      {
      return preference(PREF_APP_STARTUP_SESSIONSTART).value<SessionStart>();
      }

MusicxmlExportBreaks Preferences::musicxmlExportBreaks() const
      {
      return preference(PREF_EXPORT_MUSICXML_EXPORTBREAKS).value<MusicxmlExportBreaks>();
      }

MuseScoreStyleType Preferences::globalStyle() const
      {
      return preference(PREF_UI_APP_GLOBALSTYLE).value<MuseScoreStyleType>();
      }

bool Preferences::isThemeDark() const
      {
      return globalStyle() == MuseScoreStyleType::DARK_FUSION;
      }

void Preferences::revertToDefaultValue(const QString key)
      {
      set(key, defaultValue(key));
      }


void Preferences::setPreference(const QString key, QVariant value)
      {
      checkIfKeyExists(key);
      set(key, value);
      }

void Preferences::setTemporaryPreference(const QString key, QVariant value)
      {
      // note: this function should not call checkIfKeyExists() because it may be
      // called before init() which is ok since the preference is only stored "in memory"
      set(key, value, true);
      }

MidiRemote Preferences::midiRemote(int recordId) const
      {
      MidiRemote remote;
      QString baseKey = QString(PREF_IO_MIDI_REMOTE) + QString("%1%2%3").arg("/").arg(recordId).arg("/");

      if (has(baseKey + "type")) {
            remote.type = MidiRemoteType(get(baseKey + "type").toInt());
            remote.data = get(baseKey + "data").toInt();
            }
      else {
            remote.type = MIDI_REMOTE_TYPE_INACTIVE;
            }

      return remote;
      }

void Preferences::updateMidiRemote(int recordId, MidiRemoteType type, int data)
      {
      QString baseKey = QString(PREF_IO_MIDI_REMOTE) + QString("%1%2%3").arg("/").arg(recordId).arg("/");
      set(baseKey + "type", static_cast<int>(type));
      set(baseKey + "data", data);
      }

void Preferences::clearMidiRemote(int recordId)
      {
      QString baseKey = QString(PREF_IO_MIDI_REMOTE) + QString("%1%2").arg("/").arg(recordId);
      remove(baseKey);
      }

Preference::Preference(QVariant defaultValue, QMetaType::Type type, bool showInAdvancedList)
      : _defaultValue(defaultValue),
        _showInAdvancedList(showInAdvancedList),
        _type(type)
      {}

IntPreference::IntPreference(int defaultValue, bool showInAdvancedList)
      : Preference(defaultValue, QMetaType::Int, showInAdvancedList)
      {}

void IntPreference::accept(QString key, QTreeWidgetItem* parent, PreferenceVisitor& v)
      {
      v.visit(key, parent, this);
      }

DoublePreference::DoublePreference(double defaultValue, bool showInAdvancedList)
      : Preference(defaultValue, QMetaType::Double, showInAdvancedList)
      {}

void DoublePreference::accept(QString key, QTreeWidgetItem* parent, PreferenceVisitor& v)
      {
      v.visit(key, parent, this);
      }

BoolPreference::BoolPreference(bool defaultValue, bool showInAdvancedList)
      : Preference(defaultValue, QMetaType::Bool, showInAdvancedList)
      {}

void BoolPreference::accept(QString key, QTreeWidgetItem* parent, PreferenceVisitor& v)
      {
      v.visit(key, parent, this);
      }

StringPreference::StringPreference(QString defaultValue, bool showInAdvancedList)
      : Preference(defaultValue, QMetaType::QString, showInAdvancedList)
      {}

void StringPreference::accept(QString key, QTreeWidgetItem* parent, PreferenceVisitor& v)
      {
      v.visit(key, parent, this);
      }

QString FilePreference::filter() const
      {
      return _filter;
      }

FilePreference::FilePreference(QString defaultValue, QString filter, bool showInAdvancedList)
      : Preference(defaultValue, QMetaType::QString, showInAdvancedList),
        _filter(filter)
      {}

void FilePreference::accept(QString key, QTreeWidgetItem* parent, PreferenceVisitor& v)
      {
      v.visit(key, parent, this);
      }

DirPreference::DirPreference(QString defaultValue, bool showInAdvancedList)
      : Preference(defaultValue, QMetaType::QString, showInAdvancedList)
      {}

void DirPreference::accept(QString key, QTreeWidgetItem* parent, PreferenceVisitor& v)
      {
      v.visit(key, parent, this);
      }

ColorPreference::ColorPreference(QColor defaultValue, bool showInAdvancedList)
      : Preference(defaultValue, QMetaType::QColor, showInAdvancedList)
      {}

void ColorPreference::accept(QString key, QTreeWidgetItem* parent, PreferenceVisitor& v)
      {
      v.visit(key, parent, this);
      }

EnumPreference::EnumPreference(QVariant defaultValue, bool showInAdvancedList)
      : Preference(defaultValue, QMetaType::User, showInAdvancedList)
      {}

void EnumPreference::accept(QString, QTreeWidgetItem*, PreferenceVisitor&)
      {
      }

} // namespace Ms
