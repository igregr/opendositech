#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QLabel>
#include <QDialog>
#include <QDate>
#include <QDateEdit>
#include <QSqlResult>
#include <QMessageBox>
#include <QThread>
#include <QSound>
//#include "/usr/include/phonon/mediaobject.h"
//#include "/usr/include/phonon/audiooutput.h"
#include "insertDialog.h"
#include "createBatchDialog.h"
#include "viewbatchdialog.h"
#include "viewregisterdialog.h"
#include "registerBatchDialog.h"
#include "database.h"

class BatchInfo;
class RecordInfo;
class RegisterBatchDialog;

class MainWindow : public QMainWindow
{
	Q_OBJECT

//	QDate d; //aktualni datum, zmeni se pokud uzivatel nastavi pomoci dialogu jine datum

//	Phonon::MediaObject *m_error;
//	Phonon::MediaObject *m_ok;

  QSound *m_error;
  QSound *m_ok;


	static QDate d;
	BatchInfo* m_batch_info_widget;
	RecordInfo* m_record_info;
	QWidget* centralWidget; 
	QGridLayout* centralWLayout;
	InsertDialog* inDialog;
	CreateBatchDialog* m_createBatchDialog;
    ViewBatchDialog* m_viewBatchDialog;
    ViewRegisterDialog* m_viewRegisterDialog;
    RegisterBatchDialog* m_registerBatchDialog;

	QString m_icz; 	//identifikacni cislo smluvniho zarizeni
	QString m_pobocka; 	//cislo pobocky
	QString m_dcid;	//cislo davky
	QString m_hcid;	//cislo dokladu
	QString m_hodb;	//odbornost
	QString m_hcpo; //cislo pojistovny
	int m_num_records;	//pocet dokladu v davce, hodnota se priradi z databaze
	int m_num_points;	//pocet bodu v davce, hodnota se priradi z databaze
    qint64 m_batch_id; //id_davky
    
    CrateBatch m_batch_info;

	QLabel* ICZLabel;
	QMenu* accountMenu;
	QMenu* settingsMenu;
	QMenu* m_batch_menu;
	QMenu* m_register_menu;

	
	QAction* insertDataAction;
	QAction* fullscreenAction;
	QAction* changeDateAction;
	QAction* settingsAction;

	QAction* m_action_create_batch;
	QAction* m_action_load_batch;
    QAction* m_action_view_batch_list;
    QAction* m_action_create_register_batch;

  QAction* m_action_view_register;

	bool parseFile(const QString &filename);
	void createActions();
	void createMenu();
	void createStatusBar();

private slots:
	void toggleFullscreen();
	void insertDataForm();
	void createNewBatch(CrateBatch batch_info);
	void createBatch(CrateBatch batch_info);
	void changeDateDialog();
	void settingsDialog();
	void createBatchDialog();
	void loadBatchDialog();
    void viewBatchDialog();
    void viewRegisterDialog();
    void viewRegisterBatchDialog();
    void deleteInfoWidgets();
    void setBatchInfo(CrateBatch list);

public slots:
	void playError();
	void playOk();

protected:
	void closeEvent(QCloseEvent *event);

public:
	MainWindow();
	~MainWindow();
	
  static QDate date() { return d; }
  QString icz() const { return m_icz; }
  QString pobocka() const { return m_pobocka; }
  QString dcid() const { return m_dcid; }
  QString hcid() const { return m_hcid; }
  QString hodb() const { return m_hodb; }
  QString hcpo() const { return m_hcpo; }
  QString batchId() const { return QString("%1").arg(m_batch_id); }
  int numRecords() const { return m_num_records; }
  int numPoints() const { return m_num_points; }
};



class DateDialog : public QDialog
{
	Q_OBJECT

	QDate d; //datum
	QDateEdit* dateEdit;

private slots:
	void updateDate(QDate new_date);
	void okClicked();

public:
	DateDialog();
	void setupUI(QDialog* dialog);
	QDate date() { return d; };


};


//class Sound 
//{
//public:
	//potrebuju inicializovat v mainu, proto je to public, i kdyz u singltonu 
	//to samozrejme public byt nesmi..bude to inicializovano pouze v main.cpp
//	Sound();
//	~Sound();
//	static Sound* getInstance() { return m_instance; }
//	static void playError();
//	static void playOk();

//private:
//	Sound(const Sound&);
//	Sound& operator=(const Sound&);
//	static Sound* m_instance;	

//	static Phonon::MediaObject *m_error;
//	static Phonon::MediaObject *m_ok;

//};

/*
class SettingsDialog : public QDialog
{
	Q_OBJECT

	QString icz;
	QString dcid;
	QString hcid;
	QString odb;
	
	QLineEdit* le_icz;
	QLineEdit* le_dcid;
	QLineEdit* le_hcid;
	QLineEdit* le_odb;

private slots:
	void okClicked();
	


public:
	void setupUI(QDialog* dialog);
	SettingsDialog();
};

*/
#endif
