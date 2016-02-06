#ifndef INSERTDIALOG_H
#define INSERTDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QListWidget>
#include <QListWidgetItem>
#include <QKeyEvent>
#include <QStringList>
#include <QShortcut>

#include "LineEdit.h"
#include "message.h"
#include "database.h"

enum Batch_type { ambulantni, hospitalizacni };
enum Dialog_type { insert_dialog, select_dialog }; 
									

//class PickBatchType : public QListWidget
class PickBatchType : public QDialog
{
	Q_OBJECT

	QListWidget* w;
signals:
	void sendData(int batchType);

protected:
	void keyPressEvent(QKeyEvent *event);

public:
	PickBatchType();
};


class InsertDialog : public QDialog
{
	Q_OBJECT

public:
	enum DialogType { insert, select };// zda se jedna o dialog pri pocatecnim vkladani dokladu (pokud davka neexistuje,je vytvorena
													//nebo zda jenom vybiram davku ze seznamu, pokud neexistuje -> akorat hlaska
	InsertDialog(DialogType type, QWidget* parent = 0);
	BatchNumberLineEdit* LEbatchNo;
	InsuranceCompanyLineEdit* LEinCo;

signals:
	void sendData(CrateBatch batch_info);

private slots:
	void newBatchInfo(int type);	
	void checkInsuranceCompany(QString value);

private:
	int m_type;
	
	QString m_batch_number; 
	QString m_in_company_number;
	QList<CrateBatch> m_batch_info;

	QGridLayout* m_gridLayout;
	PickBatchType* m_pick_type;	

	void setupUI(QDialog* dialog);
	void findBatch();
};

 





#endif

