#include <QCoreApplication>
#include <QtWidgets/qtwidgetsglobal.h>
#include <QtWidgets/qabstractscrollarea.h>
#include <QtGui/qtextdocument.h>
#include <QtGui/qtextoption.h>
#include <QtGui/qtextcursor.h>
#include <QtGui/qtextformat.h>
#include <QTextEdit>
#include <QGuiApplication>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
        QTextEdit window;
        window.show();
    return app.exec();
}
