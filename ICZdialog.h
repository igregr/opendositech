#ifndef ICZDIALOG_H
#define ICZDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <QTextStream>
#include <QMessageBox>

#include "database.h"

class ICZdialog : public QDialog
{

	Q_OBJECT

	QLabel* label;
	QLabel* label2;
	QLineEdit* inputLine;
	QPushButton* okButton;
	QPushButton* cancelButton;

private slots:
	void okClicked();

public:
	ICZdialog();
};

#endif
