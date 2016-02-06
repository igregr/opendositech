#ifndef VIEWBATCHDIALOG_H
#define VIEWBATCHDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QTableView>
#include <QHeaderView>
#include <QGridLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QDebug>
#include <QSqlRecord>
#include <QDate>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintDialog>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QCloseEvent>
#include <QAction>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QIcon>
#include <QApplication>
#include <QShortcut>

#include "database.h"

class RenumberDialog : public QDialog
{
    Q_OBJECT

    QString m_batch_id;
    QLineEdit* m_how_much;
    void setupUI();

private slots:
    void okClicked();

public:
    RenumberDialog(QString batch_id);
};


class ViewBatchDialog : public QDialog
{
    Q_OBJECT

    QSqlTableModel* m_model;
    QTableView* m_view;
    QGridLayout* m_main_layout;
    QPushButton* m_printPruvodniList;
    QPushButton* m_obsahDavky;
	QAction* m_changeBatchNumberAction;
	QAction* m_deleteBatchAction;
    QAction* m_renumberAction;
    void printHtml(QString html);
    void setupUI();

private slots:
    void printPruvodniList();
    void showBatchDetail();
	void updateView();
	void changeBatchNumber();
	void deleteBatch();
    void renumberRecords();

public:
    ViewBatchDialog();
};

class EditBatchNumber : public QDialog
{
	Q_OBJECT

	QString m_old_batch;
	QString m_in_company;

	QLineEdit* m_batch_number;
	QLineEdit* m_new_in_comp; 
	void setupUI();

private slots:
	void okClicked();

signals:
	void updateViewBatchDialog();

public:
	EditBatchNumber(QString m_old_batch, QString m_in_company);
};


class ViewBatchDetail : public QDialog
{
    Q_OBJECT

    QString m_batch_number;
    QString m_batch_id;
    QString m_in_comp_number;

    QGroupBox* m_record_groupBox;
    QGroupBox* m_detail_groupBox;
    QPushButton* m_quit_button;
    QPushButton* m_replace_button;

    QSqlTableModel* m_record_model;
    QSqlTableModel* m_detail_model;

    QTableView* m_record_view;
    QTableView* m_detail_view;

    QAction* m_delete_record_action;

    QHBoxLayout* m_top_layout;
    QHBoxLayout* m_bottom_layout;
    QVBoxLayout* m_main_layout;

    void createRecordPanel();
    void createDetailsPanel();
    void setupUI();

signals:
	void updateViewBatchDialog();

protected:
	void closeEvent(QCloseEvent *event);	

private slots:
    void updateDetailsView();
    void updateRecordView();
	void replaceByLast();
	void deleteRow();


public:
    ViewBatchDetail(const QString batch_number, const QString insurence_comp_number, const QString batch_id);
};



#endif // VIEWBATCHDIALOG_H
