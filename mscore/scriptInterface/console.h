#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>
#include <QWidget>
#include <QPlainTextEdit>
#include <QFont>
#include <QMenu>
#include <QContextMenuEvent>
#include <QAction>

namespace Ms {

class Console : public QPlainTextEdit
      {
      Q_OBJECT

   public:
      Console(QWidget* parent = nullptr);
      ~Console();

      int fontSize() const {return fontInfo().pointSize();}
      void setFontSize(int);

   public slots:
      void grayOutContents();
      void decreaseFontSize();
      void increaseFontSize();
      void resetFontSize() { setFontSize(m_originalFontSize); }

   protected:
#ifndef QT_NO_CONTEXTMENU
      virtual void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

   private:
      const unsigned char FontSizes[20] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14,
                                           16, 18, 20, 22, 24, 28, 36, 48, 64};

      QMenu *contextMenu;

      int m_originalFontSize;

   private slots:
      void copyAllToClipboard();

   signals:

      }; //class Console

} //namespace Ms

#endif // CONSOLE_H
