//=============================================================================
//  MusE Score
//  Linux Music Score Editor
//  $Id: preferences.h 5660 2012-05-22 14:17:39Z wschweer $
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

#ifndef __PREFERENCES_H__
#define __PREFERENCES_H__

/*
 * HOW TO ADD A NEW PREFERENCE
 * - Add a new define to the list of defines below
 * - Add the preference to the _allPreferences map in the init() function in preferences.cpp
 *   and specify the default value for this preference
 * - That's it. The preference is stored and retrieved automatically and can be read
 *   using getString(), getInt(), etc., and changed using setPreference()
 */

#include "globals.h"

namespace Ms {

extern QString mscoreGlobalShare;

enum class SessionStart : char {
      EMPTY, LAST, NEW, SCORE
      };

// midi remote control values:
enum {
      RMIDI_REWIND,
      RMIDI_TOGGLE_PLAY,
      RMIDI_PLAY,
      RMIDI_STOP,
      RMIDI_NOTE1,
      RMIDI_NOTE2,
      RMIDI_NOTE4,
      RMIDI_NOTE8,
      RMIDI_NOTE16,
      RMIDI_NOTE32,
      RMIDI_NOTE64,
      RMIDI_REST,
      RMIDI_DOT,
      RMIDI_DOTDOT,
      RMIDI_TIE,
      RMIDI_UNDO,
      RMIDI_NOTE_EDIT_MODE,
      RMIDI_REALTIME_ADVANCE,
      MIDI_REMOTES
      };

enum class MuseScoreStyleType : char {
      DARK_FUSION = 0,
      LIGHT_FUSION
      };

// MusicXML export break values
enum class MusicxmlExportBreaks : char {
      ALL, MANUAL, NO
      };

//
// Defines for all preferences
// Every preference should have a define to ease the usage of the preference
// Make sure the string key has a sensible grouping - use / for grouping
//
#define PREF_APP_AUTOSAVE_AUTOSAVETIME                      "application/autosave/autosave time"
#define PREF_APP_AUTOSAVE_USEAUTOSAVE                       "application/autosave/use autosave"
#define PREF_APP_KEYBOARDLAYOUT                             "application/keyboard layout"
// file path of instrument templates
#define PREF_APP_PATHS_INSTRUMENTLIST1                      "application/paths/instrument list 1"
#define PREF_APP_PATHS_INSTRUMENTLIST2                      "application/paths/instrument list 2"
#define PREF_APP_PATHS_MYIMAGES                             "application/paths/myImages"
#define PREF_APP_PATHS_MYPLUGINS                            "application/paths/myPlugins"
#define PREF_APP_PATHS_MYSCORES                             "application/paths/myScores"
#define PREF_APP_PATHS_MYSHORTCUTS                          "application/paths/myShortcuts"
#define PREF_APP_PATHS_MYSOUNDFONTS                         "application/paths/mySoundfonts"
#define PREF_APP_PATHS_MYSTYLES                             "application/paths/myStyles"
#define PREF_APP_PATHS_MYTEMPLATES                          "application/paths/myTemplates"
#define PREF_APP_PATHS_MYEXTENSIONS                         "application/paths/myExtensions"
#define PREF_APP_PLAYBACK_FOLLOWSONG                        "application/playback/follow song"
#define PREF_APP_PLAYBACK_PANPLAYBACK                       "application/playback/pan playback"
#define PREF_APP_PLAYBACK_PLAYREPEATS                       "application/playback/play repeats"
#define PREF_APP_USESINGLEPALETTE                           "application/use single palette"
#define PREF_APP_STARTUP_FIRSTSTART                         "application/startup/first start"
#define PREF_APP_STARTUP_SESSIONSTART                       "application/startup/session start"
#define PREF_APP_STARTUP_STARTSCORE                         "application/startup/start score"
#define PREF_APP_WORKSPACE                                  "application/workspace"
#define PREF_EXPORT_AUDIO_SAMPLERATE                        "export/audio/sample rate"
#define PREF_EXPORT_MP3_BITRATE                             "export/mp3/bit rate"
#define PREF_EXPORT_MUSICXML_EXPORTLAYOUT                   "export/musicXML/export layout"
#define PREF_EXPORT_MUSICXML_EXPORTBREAKS                   "export/musicXML/export breaks"
#define PREF_EXPORT_PDF_DPI                                 "export/pdf/dpi"
#define PREF_EXPORT_PNG_RESOLUTION                          "export/png/resolution"
#define PREF_EXPORT_PNG_USETRANSPARENCY                     "export/png/use transparency"
#define PREF_IMPORT_GUITARPRO_CHARSET                       "import/guitarpro/charset"
#define PREF_IMPORT_MUSICXML_IMPORTBREAKS                   "import/musicXML/import breaks"
#define PREF_IMPORT_MUSICXML_IMPORTLAYOUT                   "import/musicXML/import layout"
#define PREF_IMPORT_OVERTURE_CHARSET                        "import/overture/charset"
#define PREF_IMPORT_STYLE_STYLEFILE                         "import/style/style file"
#define PREF_IO_ALSA_DEVICE                                 "io/alsa/device"
#define PREF_IO_ALSA_FRAGMENTS                              "io/alsa/fragments"
#define PREF_IO_ALSA_PERIODSIZE                             "io/alsa/period size"
#define PREF_IO_ALSA_SAMPLERATE                             "io/alsa/sample rate"
#define PREF_IO_ALSA_USEALSAAUDIO                           "io/alsa/use AlsaAudio"
#define PREF_IO_JACK_REMEMBERLASTCONNECTIONS                "io/jack/remember last connections"
#define PREF_IO_JACK_TIMEBASEMASTER                         "io/jack/timebase master"
#define PREF_IO_JACK_USEJACKAUDIO                           "io/jack/use JackAudio"
#define PREF_IO_JACK_USEJACKMIDI                            "io/jack/use JackMIDI"
#define PREF_IO_JACK_USEJACKTRANSPORT                       "io/jack/use JackTransport"
#define PREF_IO_MIDI_ADVANCEONRELEASE                       "io/midi/advance on release"
#define PREF_IO_MIDI_ENABLEINPUT                            "io/midi/enable input"
#define PREF_IO_MIDI_EXPANDREPEATS                          "io/midi/expand repeats"
#define PREF_IO_MIDI_EXPORTRPNS                             "io/midi/export RPN's"
#define PREF_IO_MIDI_REALTIMEDELAY                          "io/midi/realtime delay"
#define PREF_IO_MIDI_REMOTE                                 "io/midi/remote"
#define PREF_IO_MIDI_SHORTESTNOTE                           "io/midi/shortest note"
#define PREF_IO_MIDI_SHOWCONTROLSINMIXER                    "io/midi/show controls in mixer"
#define PREF_IO_MIDI_USEREMOTECONTROL                       "io/midi/use remote control"
#define PREF_IO_OSC_PORTNUMBER                              "io/osc/port number"
#define PREF_IO_OSC_USEREMOTECONTROL                        "io/osc/use remote control"
#define PREF_IO_PORTAUDIO_DEVICE                            "io/portAudio/device"
#define PREF_IO_PORTAUDIO_USEPORTAUDIO                      "io/portAudio/use PortAudio"
#define PREF_IO_PORTMIDI_INPUTBUFFERCOUNT                   "io/portMidi/input buffer count"
#define PREF_IO_PORTMIDI_INPUTDEVICE                        "io/portMidi/input device"
#define PREF_IO_PORTMIDI_OUTPUTBUFFERCOUNT                  "io/portMidi/output buffer count"
#define PREF_IO_PORTMIDI_OUTPUTDEVICE                       "io/portMidi/output device"
#define PREF_IO_PORTMIDI_OUTPUTLATENCYMILLISECONDS          "io/portMidi/output latency milliseconds"
#define PREF_IO_PULSEAUDIO_USEPULSEAUDIO                    "io/pulseAudio/use PulseAudio"
#define PREF_SCORE_CHORD_PLAYONADDNOTE                      "score/chord/play on add note"
#define PREF_SCORE_MAGNIFICATION                            "score/magnification"
#define PREF_SCORE_NOTE_PLAYONCLICK                         "score/note/play on click"
#define PREF_SCORE_NOTE_DEFAULTPLAYDURATION                 "score/note/default play duration"
#define PREF_SCORE_NOTE_WARNPITCHRANGE                      "score/note/warn pitch range"
#define PREF_SCORE_STYLE_DEFAULTSTYLEFILE                   "score/style/default style file"
#define PREF_SCORE_STYLE_PARTSTYLEFILE                      "score/style/part style file"
#define PREF_UI_CANVAS_BG_USECOLOR                          "ui/canvas/background/use color"
#define PREF_UI_CANVAS_FG_USECOLOR                          "ui/canvas/foreground/use color"
#define PREF_UI_CANVAS_BG_COLOR                             "ui/canvas/background/color"
#define PREF_UI_CANVAS_FG_COLOR                             "ui/canvas/foreground/color"
#define PREF_UI_CANVAS_BG_WALLPAPER                         "ui/canvas/background/wallpaper"
#define PREF_UI_CANVAS_FG_WALLPAPER                         "ui/canvas/foreground/wallpaper"
#define PREF_UI_CANVAS_MISC_ANTIALIASEDDRAWING              "ui/canvas/misc/antialiased drawing"
#define PREF_UI_CANVAS_MISC_SELECTIONPROXIMITY              "ui/canvas/misc/selection proximity"
#define PREF_UI_CANVAS_SCROLL_VERTICALORIENTATION           "ui/canvas/scroll/vertical orientation"
#define PREF_UI_CANVAS_SCROLL_LIMITSCROLLAREA               "ui/canvas/scroll/limit scroll area"
#define PREF_UI_APP_STARTUP_CHECKUPDATE                     "ui/application/startup/check for update"
#define PREF_UI_APP_STARTUP_CHECK_EXTENSIONS_UPDATE         "ui/application/startup/check for extensions update"
#define PREF_UI_APP_STARTUP_SHOWNAVIGATOR                   "ui/application/startup/show navigator"
#define PREF_UI_APP_STARTUP_SHOWPLAYPANEL                   "ui/application/startup/show play panel"
#define PREF_UI_APP_STARTUP_SHOWSPLASHSCREEN                "ui/application/startup/show splash screen"
#define PREF_UI_APP_STARTUP_SHOWSTARTCENTER                 "ui/application/startup/show start center"
#define PREF_UI_APP_GLOBALSTYLE                             "ui/application/global style"
#define PREF_UI_APP_LANGUAGE                                "ui/application/language"
#define PREF_UI_APP_RASTER_HORIZONTAL                       "ui/application/raster/horizontal"
#define PREF_UI_APP_RASTER_VERTICAL                         "ui/application/raster/vertical"
#define PREF_UI_APP_SHOWSTATUSBAR                           "ui/application/show status bar"
#define PREF_UI_APP_USENATIVEDIALOGS                        "ui/application/use native dialogs"
#define PREF_UI_PIANOHIGHLIGHTCOLOR                         "ui/piano highlight color"
#define PREF_UI_SCORE_NOTEDROPCOLOR                         "ui/score/note drop color"
#define PREF_UI_SCORE_DEFAULTCOLOR                          "ui/score/default color"
#define PREF_UI_SCORE_FRAMEMARGINCOLOR                      "ui/score/frame margin color"
#define PREF_UI_SCORE_LAYOUTBREAKCOLOR                      "ui/score/layout break color"
#define PREF_UI_SCORE_VOICES_VOICE1COLOR                    "ui/score/voices/voice 1 color"
#define PREF_UI_SCORE_VOICES_VOICE2COLOR                    "ui/score/voices/voice 2 color"
#define PREF_UI_SCORE_VOICES_VOICE3COLOR                    "ui/score/voices/voice 3 color"
#define PREF_UI_SCORE_VOICES_VOICE4COLOR                    "ui/score/voices/voice 4 color"
#define PREF_UI_THEME_ICONHEIGHT                            "ui/theme/icon height"
#define PREF_UI_THEME_ICONWIDTH                             "ui/theme/iconWidth"


class PreferenceVisitor;

//---------------------------------------------------------
//   Preference
//---------------------------------------------------------
class Preference {
   private:
      QVariant _defaultValue = 0;
      bool _showInAdvancedList = true;

   protected:
      QMetaType::Type _type = QMetaType::UnknownType;
      Preference(QVariant defaultValue) : _defaultValue(defaultValue) {}

   public:
      Preference(QVariant defaultValue, QMetaType::Type type, bool showInAdvancedList = true);
      virtual ~Preference() {}

      QVariant defaultValue() const {return _defaultValue;}
      bool showInAdvancedList() const {return _showInAdvancedList;}
      QMetaType::Type type() {return _type;}
      virtual void accept(QString, QTreeWidgetItem*, PreferenceVisitor&) = 0;
      };

class IntPreference : public Preference {
   public:
      IntPreference(int defaultValue, bool showInAdvancedList = true);
      virtual void accept(QString, QTreeWidgetItem*, PreferenceVisitor&) override;
      };

class DoublePreference : public Preference {
   public:
      DoublePreference(double defaultValue, bool showInAdvancedList = true);
      virtual void accept(QString, QTreeWidgetItem*, PreferenceVisitor&) override;
      };

class BoolPreference : public Preference {
   public:
      BoolPreference(bool defaultValue, bool showInAdvancedList = true);
      virtual void accept(QString, QTreeWidgetItem*, PreferenceVisitor&) override;
      };

class StringPreference: public Preference {
   public:
      StringPreference(QString defaultValue, bool showInAdvancedList = true);
      virtual void accept(QString, QTreeWidgetItem*, PreferenceVisitor&) override;
      };

class ColorPreference: public Preference {
   public:
      ColorPreference(QColor defaultValue, bool showInAdvancedList = true);
      virtual void accept(QString, QTreeWidgetItem*, PreferenceVisitor&) override;
      };

// Support for EnumPreference is currently not fully implemented
class EnumPreference: public Preference {
   public:
      EnumPreference(QVariant defaultValue, bool showInAdvancedList = true);
      virtual void accept(QString, QTreeWidgetItem*, PreferenceVisitor&) override;
      };

//---------------------------------------------------------
//   Preferences
//---------------------------------------------------------

class Preferences {
   public:
      typedef QHash<QString, Preference*> prefs_map_t;

   private:

      // Map of all preferences and their default values
      // A preference can not be read or set if it is not present in this map
      // This map is not used for storing a preference it is only for default values
      prefs_map_t _allPreferences;
      // used for storing preferences in memory when _storeInMemoryOnly is true
      // and for storing temporary preferences
      QHash<QString, QVariant> _inMemorySettings;
      bool _storeInMemoryOnly = false;
      bool _returnDefaultValues = false;
      bool _initialized = false;
      QSettings* _settings; // should not be used directly but through settings() accessor

      QSettings* settings() const;
      // the following functions must be used to access and change a preference
      // instead of using QSettings directly
      QVariant get(const QString key) const;
      bool has(const QString key) const;
      void set(const QString key, QVariant value, bool temporary = false);
      void remove(const QString key);

      QVariant preference(const QString key) const;
      QMetaType::Type type(const QString key) const;
      bool checkIfKeyExists(const QString key) const;
      bool checkType(const QString key, QMetaType::Type t) const;

   public:
      Preferences();
      ~Preferences();
      void init(bool storeInMemoryOnly = false);
      void save();
      // set to true to let getters return default values instead of values from QSettings
      void setReturnDefaultValues(bool returnDefaultValues) {_returnDefaultValues = returnDefaultValues;}

      const prefs_map_t& allPreferences() const {return _allPreferences;}

      // general getters
      QVariant defaultValue(const QString key) const;
      bool getBool(const QString key) const;
      QColor getColor(const QString key) const;
      QString getString(const QString key) const;
      int getInt(const QString key) const;
      double getDouble(const QString key) const;

      // general setters
      void revertToDefaultValue(const QString key);
      void setPreference(const QString key, QVariant value);

      // A temporary preference is stored "in memory" only and not written to file.
      // If there is both a "normal" preference and a temporary preference with the same
      // key the temporary preference is used
      void setTemporaryPreference(const QString key, QVariant value);

      /*
       * Some preferences like enums and structs/classes are not easily read using the general set/get methods
       * and therefore require specific getters and/or setters
       */
      SessionStart sessionStart() const;
      MusicxmlExportBreaks musicxmlExportBreaks() const;
      MuseScoreStyleType globalStyle() const;
      bool isThemeDark() const;

      template<typename T>
      void setCustomPreference(const QString key, T t)
            {
            set(key, QVariant::fromValue<T>(t));
            }

      // The midiRemote preference requires special handling due to its complexity
      MidiRemote midiRemote(int recordId) const;
      void updateMidiRemote(int recordId, MidiRemoteType type, int data);
      void clearMidiRemote(int recordId);
      };

// singleton
extern Preferences preferences;

// Stream operators for enum classes
// enum classes don't play well with QSettings without custom serialization
template<typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
inline QDataStream &operator<<(QDataStream &out, const T &val)
{
    return out << static_cast<int>(val);
}

template<typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
inline QDataStream &operator>>(QDataStream &in, T &val)
{
    int tmp;
    in >> tmp;
    val = static_cast<T>(tmp);
    return in;
}

class PreferenceVisitor {
   public:
      virtual void visit(QString key, QTreeWidgetItem*, IntPreference*) = 0;
      virtual void visit(QString key, QTreeWidgetItem*, DoublePreference*) = 0;
      virtual void visit(QString key, QTreeWidgetItem*, BoolPreference*) = 0;
      virtual void visit(QString key, QTreeWidgetItem*, StringPreference*) = 0;
      virtual void visit(QString key, QTreeWidgetItem*, ColorPreference*) = 0;
      };


} // namespace Ms

Q_DECLARE_METATYPE(Ms::SessionStart);
Q_DECLARE_METATYPE(Ms::MusicxmlExportBreaks);
Q_DECLARE_METATYPE(Ms::MuseScoreStyleType);

#endif
