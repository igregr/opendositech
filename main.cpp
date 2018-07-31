#include <QApplication>
#include <QTextCodec>
#include <QFile>
#include <QObject>
#include <QtDebug>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include "mainwindow.h"
#include "ICZdialog.h"
#include "database.h"

QTextStream logfile;

void logMessageOutput(QtMsgType type, const char* msg)
{
    
    switch (type) {
        case QtDebugMsg:
            logfile << QTime::currentTime().toString().toAscii().data() <<  " " << msg << "\n";
            break;
        case QtCriticalMsg:
            logfile << QTime::currentTime().toString().toAscii().data() << " CRITICAL: " << msg << "\n";
            break;
        case QtWarningMsg:
            logfile << QTime::currentTime().toString().toAscii().data() << " WARNING: " << msg << "\n";
            break;
        case QtFatalMsg:
            logfile << QTime::currentTime().toString().toAscii().data() <<  " FATAL: " << msg << "\n";
            abort();
    }
    logfile.flush();
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName("OpenDositech");
    QFont font = QApplication::font();
    font.setPointSize(16);
    app.setFont(font);

	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

	QDir dir("archiv");
	if (!dir.exists()) {
		QDir dir2 = QDir::current();
		if(dir2.mkdir("archiv")){
			qDebug() << "Vytvoren adresar archiv";
		}
	}
    if (!DB::getInstance()->init()) {
        return 1;
    }
/*
#ifndef QT_NO_DEBUG_OUTPUT
    QFile file("log.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        logfile.setDevice(&file);
        qInstallMsgHandler(logMessageOutput);
        qDebug() << "Starting logging";
    } else {
        return 1;
    }
#endif
*/

//TODO co kdyz uz databaze s vecma existuhe a config byl akorat smazany a zada jine ICZ?
	bool config_exists = QFile::exists("config.xml");
		if (config_exists){
			MainWindow* window = new MainWindow();
			window->show();
		}
		else{
		//	SettingsDialog* sd = new SettingsDialog();
		//	sd->setupUI(sd);
		//	if(sd->exec()){
		//		MainWindow* window = new MainWindow();
		//		window->show();	
		//	}
		//	else return 1;
			ICZdialog* id = new ICZdialog();
			if(id->exec()){
				MainWindow* window = new MainWindow();
				window->show();
			}
			else {
				return 1;
			}
			
		}	
	return app.exec();
}
