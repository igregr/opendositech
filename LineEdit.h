#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QApplication>
#include <QLineEdit>
#include <QString>
#include <QDate>
#include <QShortcut>
#include <QFontMetrics>
#include <QDebug>
#include <QRegExpValidator>

#include "message.h"

class LineEdit : public QLineEdit
{
	Q_OBJECT

	LineEdit* m_previous;
	LineEdit* m_next;

        QShortcut* key_up_shortcut;
        QShortcut* key_dow_shortcut;


	int m_length;

signals:
	void sendData(QString data);
        void plusShortcut();
protected:
        void keyPressEvent(QKeyEvent *event);

public slots:
	void focusNext();
	void focusPrevious();
	virtual void send();
	virtual void itemFilled(int , int newp);


public:
	LineEdit();
        //TODO kdyztak predelat, at to neni public..je to kvuli tomu, at to v jinych lineeditech muzu odpojit
        //a pripojit k jinemu slotu
        QShortcut* enter_shortcut;
	void setNeighbours(LineEdit* previous, LineEdit* next);
	void setLength(int l) { m_length = l; }
	virtual bool checkLength();
	void setValue(QString value);
};


class DateLineEdit : public LineEdit
{
	Q_OBJECT

	QDate m_date;

private slots:
	 void dateReturnPressed();

public:
	DateLineEdit(QDate nd);
};


class BatchNumberLineEdit : public LineEdit
{
	Q_OBJECT

	bool checkLength();

private slots:
	void send();

public:
	BatchNumberLineEdit();
};


class InsuranceCompanyLineEdit : public LineEdit
{
public:
	InsuranceCompanyLineEdit();

};

class RecordNumberLineEdit : public LineEdit
{
	Q_OBJECT
	
	bool checkLength();

private slots:
	void send();

public:
	RecordNumberLineEdit();
};


class IcpLineEdit : public LineEdit
{
public:
	IcpLineEdit();
};

class DiagnosisLineEdit : public LineEdit
{
	Q_OBJECT
	
	QString m_main_diagnosis;
	bool checkLength();

private slots:
	void diagReturnPressed();
	void send();
public:
	DiagnosisLineEdit(QString mainDiagnosis);
	void setMainDiagnose(const QString diag);
};


class OdbLineEdit : public LineEdit
{
	Q_OBJECT

	bool checkLength();

private slots:
	void send();
public:
	OdbLineEdit();
};

class VarSymbolLineEdit : public LineEdit
{
	Q_OBJECT
	
	bool checkLength();

private slots:
	void send();

public:
	VarSymbolLineEdit();
};


class PersonalNumberLineEdit : public LineEdit
{
	Q_OBJECT
	
	bool checkLength();

private slots:
	void send();


public:
	PersonalNumberLineEdit();
};

class CodeLineEdit : public LineEdit
{
	Q_OBJECT
	
	QString m_last_code;

private slots:
	void codeReturnPressed();
	void send();

public:
	CodeLineEdit(QString s);
};


class PLineEdit : public LineEdit
{
	Q_OBJECT

private slots:
	void send();
        void pReturnPressed();
public:
	PLineEdit();
};

class DiskNameLineEdit : public LineEdit
{
	QString m_disc_name;
public:
	DiskNameLineEdit();
	QString discName() { return m_disc_name; }
};




#endif
