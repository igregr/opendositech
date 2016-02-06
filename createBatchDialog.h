#ifndef CREATEBATCHDIALOG_H
#define CREATEBATCHDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDateEdit>
#include <QDate>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>
#include <QTextStream>
#include <QProgressBar>
#include <QCheckBox>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

#include "LineEdit.h"
#include "database.h"



class CreateBatchDialog : public QDialog
{
	Q_OBJECT

	QComboBox* m_batch_char;
	QComboBox* m_insurance_type;
	QDateEdit* m_de_month;
	QDateEdit* m_de_year;
	QProgressBar* m_creation_progressBar;
	QCheckBox* m_backup_checkbox;

	CrateBatch m_batch_info;
	
	QPushButton* m_createButton;
	QVBoxLayout* m_main_layout;

	QString m_batch_number;
	QString m_insurance_company;

	QFile m_batch_file;
	QString m_file_name;
	QString m_disc_name;

	void setupUI();
	QString prependSpace(QString str, int length);
	QString appendSpace(QString str, int length);
	QString createVetaZahlavi(QString cislo_dokladu, QString icp, QString odb,
		                                     QString var_symbol, QString rc, QString zdiag, QString pbodu,
											 QString pvykonu, int pcislo);
	QString createVetaVykony(QString datum, QString kod, QString pocet, QString odb, QString diag, QString body);
	QString getInsuranceType();
	QString createPruvodniListDavky();
private slots:
	void createBatch();
	void saveBatch();
	void saveDiscName(QString disc_name);
	void saveDialog();
	void accept();

public:
	CreateBatchDialog(CrateBatch batch_info);
};


class LoadBatch : public QObject
{
	Q_OBJECT
	
	QFile m_batch_file;
	QString m_batch_id;
	QString m_poj;
	QString m_record_id;
	QProgressBar* m_progress;
	
	bool checkFileName(QString path);
	bool processLine(QString line);
	bool processPruvodniList(QString line);
	bool processDoklady(QString line);
	bool processVykony(QString line);
public:
	LoadBatch();
	bool loadBatch(QString filename);
};



#endif
