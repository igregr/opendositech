#ifndef REGISTERBATCHDIALOG_H
#define REGISTERBATCHDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDate>
#include <QComboBox>
#include <QPushButton>
#include <QMap>
#include <QFileDialog>
#include <QFile>
#include <QChar>

#include "database.h"
#include "mainwindow.h"
#include "852codec.h"

class MainWindow;
class RegisterBatchDialog : public QDialog
{
	Q_OBJECT

	MainWindow* m_main_window;
    
    QMap<int, int> m_map;

    QComboBox* m_month;
    QComboBox* m_year;
    
    QVBoxLayout* m_mainLayout;
    QPushButton* m_create_button;

    void setupUI();
    void listInsPacients();
    QString createPruvodniList(QString cislo_davky);
    QString createZahlavi(QString pojistovna, QString icp, QString cislo_dokladu, QString rok, QString mesic, QString odb);
    QString createRegistrace(QString c_radku, QString prijmeni, QString jmeno, QString rc, QString datum);

private slots:
    void createRegisterBatch();

public:
    RegisterBatchDialog(MainWindow* parent);

};
#endif


