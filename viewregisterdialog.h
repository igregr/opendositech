#ifndef VIEWREGISTERDIALOG_H
#define VIEWREGISTERDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QTableView>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QAction>
#include <QRegExp>
#include <QSortFilterProxyModel>
#include <QComboBox>

#include "database.h"

class ViewRegisterDialog : public QDialog
{
  Q_OBJECT

  QSqlTableModel* m_model;
  QSortFilterProxyModel* m_proxy;

  QTableView* m_view;
  QTableView* m_proxy_view;
  QVBoxLayout* m_main_layout;
  QLineEdit* m_search;
  QComboBox* m_search_column;

  void setupUI();

private slots:
    void deletePacient();
    void search();
    void searchColumnChanged();

public:
  ViewRegisterDialog();
};

class AddPacientDialog : public QDialog
{
    Q_OBJECT
    QString m_rc;
    QVBoxLayout* m_main_layout;

    QLabel* m_name_label;
    QLabel* m_surname_label;
    
    QLineEdit* m_name_edit;
    QLineEdit* m_surname_edit;

    QPushButton* m_ok_button;
    QPushButton* m_cancel_button;
    void setupUI();

signals:
	void sendNameSurname(QString name, QString surname);

private slots:
    void ok();

public:
    AddPacientDialog(QString rc);

};

#endif // VIEWREGISTERDIALOG_H
