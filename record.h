#ifndef RECORD_H 
#define RECORD_H

#include <QWidget>
#include <QApplication>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QShortcut>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QKeyEvent>
#include <QDialog>
//#include "/usr/include/phonon/mediaobject.h"

#include <QDate>
#include <QTextStream>
#include <QList>
#include <QMenuBar>

//#include "lineEdit.h"
#include "LineEdit.h"
#include "message.h"
#include "mainwindow.h"
#include "database.h"

class Record;
//groupbox,ktery se stara o cislo dokladu, icp,var. symbol, rodne cislo a zakladni diagnozu
class RecordInfo : public QWidget
{
	Q_OBJECT
	
	MainWindow* m_main_window;
	Record* m_record;

	bool m_isChanged;

	int m_hcid; //cislo dokladu
	int m_sequence_number; //poradove cislo dokladu v davce
	QString m_mainDiag;
	QString m_record_id; // sloupec id_vykonu, pouzivany jako foreign key do tabulky vykony

	qint64 m_record_row_id; // id cisla dokladu,ktery prave vytvarim,pripadne upravuju 

	QGroupBox* GBAmbRecord;
	QGridLayout* gridLayout;
	
	RecordNumberLineEdit* LERecordNo;
	IcpLineEdit* LEIcp;
	OdbLineEdit* LEExpertise;
	VarSymbolLineEdit* LEVariableS;
	DiagnosisLineEdit* LEDiagnosis;

	QShortcut* shortcut;
	QFrame* helpFrame;
	QVBoxLayout* mainLayout;
	bool checkPersonalNo(QString Rc);
	void saveRecordInfo();
	void updateRecordInfo(); 
  void findRcInRegister(QString rc);


signals:
	void exitBatchWriting();
	void updateBatchInfo();
	void clearBatchInfo();
	void playError();
	void playOk();
	void focusLast();

private slots:
//	void checkValue(int t, QString value);
	void newRecord();
	void recordDeleted();
	void checkRc(QString value);
	void checkDiag(QString value);
	void checkOdb(QString value);
	void checkRecordNo(QString value);
	void checkIcp(QString value);
	void checkVarS(QString value);

public slots:
	void f4();
  void savePacient(QString name, QString surname);

public:
	RecordInfo(MainWindow* parent);
	void setupUI(QWidget* parent);
  QString mainDiagnose() const { return m_mainDiag; }
  QString recordId() const { return m_record_id; }
  int hcid() const { return m_hcid; }
	//LineEdit* LEPersonalNo;
	PersonalNumberLineEdit* LEPersonalNo;
  qint64 recordRowID() { return m_record_row_id; }
	MainWindow* getMainWindow() { return m_main_window; };


};

//groubox,zobrazuje cislo davky,pocet dokladu a pocet bodu
class BatchInfo : public QWidget
{
	Q_OBJECT

	MainWindow* m_main_window;
	RecordInfo* m_record_info;

	QGroupBox* GBInsComp;
	QVBoxLayout* mainLayout;
	QGridLayout* gridLayout;
	QLabel* LBatchNo;		//cislo davky
	QLabel* LRecordsNo;		//pocet dokladu
	QLabel* LPoints;		//pocet bodu za davku
	QLabel* LServiceNo;		//pocet vykonu
	QLabel* LRecordPoints;	//pocet bodu za vykony

private slots:
	void clearRecordInfo();
	void updateInfo();

public:
	BatchInfo(MainWindow* parent, RecordInfo* record_info);
	void setupUI(QWidget* parent);

};

class Record;
class RecordLine : public QObject
{
	Q_OBJECT

	QDate m_date; //datum vykonu
	QString m_code; //kod ve vykonu
	QString m_p; //pocet kodu ve vykonu
	QString m_odb; //odbornost vykonu
	QString m_diag; //diagnoza vykonu
	
	int m_points; //pocet bodu za vykon
	qint64 m_line_id; // id vykonu..stejne jako row.id v databazi,pouziva se proto,abych odlisil 2 stejne vykony v jednom dokladu
	
	bool update; // default false, nastavi se na true,pokud vykon upravuju

	Record* m_record;

	QLabel* m_line_number;
//	LineEdit* m_le_date;
//	LineEdit* m_le_code;
//	LineEdit* m_le_p;
//	LineEdit* m_le_odb;
//	LineEdit* m_le_diag;

	DateLineEdit* m_le_date;
	CodeLineEdit* m_le_code;
	PLineEdit* m_le_p;
	OdbLineEdit* m_le_odb;
	DiagnosisLineEdit* m_le_diag;


	QSpacerItem* m_sp;
  void saveLine(QDate date, QString code, QString p, QString odb, QString diag, int points);
	void updateLine(QDate date, QString code, QString p, QString odb, QString diag, int points);
signals:
	void focusDown();
	void newLine(QDate last_date, QString last_code);
	void lineUpdated(QDate last_date, QString last_code);
	void lineDeleted();
	void playError();
	void playOk();

private slots:
//	void checkValue(int t, QString value);
	void checkDate(QString value);
	void checkCode(QString value);
	void checkP(QString value);
	void checkOdb(QString value);
  bool checkDiag(QString value);
  void plusShortcut();

public:
	RecordLine(Record* parent);
	RecordLine(Record* parent, QDate date, QString code, QString p, QString odb, QString diag, qint64 line_id);

	~RecordLine();
  void setLineId(qint64 id) { m_line_id = id; }
	void setupUI();
	void setValues();
	//	LineEdit* date() const { return m_le_date; }
  DateLineEdit* date() const { return m_le_date; }
	void clearLine();
	void removeFromLayout();
	void addToLayout(int line_number);
  void setLineNumberLabel(int number) { m_line_number->setText(QString("%1").arg(number)); }
	void deleteFromDatabase();
  void refreshDiagnose();
};


class Record : public QWidget
{
	Q_OBJECT

	int m_hcid;
	int m_line_number;
	int m_work_line_number;
	QString m_points;
	QString m_last_code;
	QDate m_last_date; 
	QString m_main_diag;
	
	RecordInfo* m_record_info;

	QList<RecordLine *> m_list;

	QGroupBox* GBRecord;

private slots:

	void pgUp();
	void pgDown();
	void altD();
	void lineUpdated(QDate last_date, QString last_code);

public slots:
	void createNewLine(QDate last_date, QString last_code);
  void closeRecord();

signals:
	void done();
	void recordDeleted();
	void updateBatchInfo();

public:
	Record(RecordInfo* parent);
	void setupUI(QWidget* parent);
	LineEdit* firstItem();	
	LineEdit* lastItem();
	QGridLayout* mainLayout;
	int lineNumber() const { return m_line_number; }
	QString lastCode() const { return m_last_code; }
	QDate lastDate() const { return m_last_date; }
	QString mainDiag() const { return m_main_diag; }
	QString recordId() const { return m_record_info->recordId(); }
	void createNewLine(QDate date, QString code, QString p, QString odb, QString diag, qint64 line_id);
	void refreshDiagnoses();
};




#endif
