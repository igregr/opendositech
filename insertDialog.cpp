#include "insertDialog.h"

//TODO upravit vzhled
//TODO spravit vzhled pri zvetsovani okna

InsertDialog::InsertDialog(DialogType t, QWidget* parent ) : QDialog(parent), m_type(t)
{
	m_batch_number = "";
	m_in_company_number = "";
	setupUI(this);
}


void InsertDialog::setupUI(QDialog* dialog)
{
	setWindowTitle(tr("Zadejte informace o dávce"));
	resize(225,150);
	QLabel* L1 = new QLabel(tr("Zadejte číslo dávky"));
	L1->setAlignment(Qt::AlignCenter);

	LEbatchNo = new BatchNumberLineEdit();

	QLabel* L2 = new QLabel(tr("Pojišťovna:"));
	L2->setAlignment(Qt::AlignCenter);

	LEinCo = new InsuranceCompanyLineEdit();

	QHBoxLayout *mid = new QHBoxLayout;
	mid->addStretch();
	mid->addWidget(LEbatchNo);
	mid->addStretch();

	QHBoxLayout *mid2 = new QHBoxLayout;
	mid2->addStretch();
	mid2->addWidget(LEinCo);
	mid2->addStretch();

	QVBoxLayout *main = new QVBoxLayout(dialog);
	main->addWidget(L1);
	main->addLayout(mid);
	main->addWidget(L2);
	main->addLayout(mid2);
	main->addStretch();
	
	LEbatchNo->setNeighbours(0, LEinCo);
	LEinCo->setNeighbours(LEbatchNo, 0);

	QShortcut* shortcut = new QShortcut(QKeySequence(tr("Esc", "Close")), this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(reject()));
	
	connect(LEbatchNo, SIGNAL(sendData(QString)), LEbatchNo, SLOT(focusNext())); 
	connect(LEinCo, SIGNAL(sendData(QString)), this, SLOT(checkInsuranceCompany(QString)));
	setAttribute ( Qt::WA_DeleteOnClose,true );
}


void InsertDialog::findBatch()
{
	m_batch_number = LEbatchNo->text();
	
	m_batch_info = DB::getInstance()->getBatchInfo(m_batch_number, m_in_company_number);

	if (!m_batch_info.isEmpty()) {
		if (m_batch_info.size() > 1) { //vice davek
			qDebug() << "DEBUG dodelat dialog na vyber davky";
		}
		emit sendData(m_batch_info.first());
        qDebug() << m_batch_info.first().id_davky << m_batch_info.first().cislo_davky << m_batch_info.first().pojistovna;        
		done(QDialog::Accepted);
	} else { 
		if (m_type == insert) {// davka nenalezena..neexistuje,tudiz vytvarim novou, davam na vyber typ davky
			m_pick_type = new PickBatchType();
			connect(m_pick_type, SIGNAL(sendData(int)), this, SLOT(newBatchInfo(int)));
			m_pick_type->move(x() + 100, y() +100);
			if (!m_pick_type->exec()) {
				qDebug() << "Zrusen dialog->vyber typ davky";
				done(QDialog::Rejected);
			}
		} else if (m_type == select) {//davka nenalezena, zprava,ze nebyla nalezena
			Message* msg = new Message();
			msg->setupUI(msg, Message::noBatchNo);
			msg->exec();
			LEinCo->clear();
			LEbatchNo->clear();
			LEbatchNo->setFocus();
		}
	}
}

void InsertDialog::newBatchInfo(int t)// vyplnim hodnoty pro novou davku
{
	if (t == ambulantni) {
		m_type = 98;
	} else if (t == hospitalizacni) {
		m_type = 99;
	}
    
    //vyplnim pouze hodnoty, ktere znam 
    CrateBatch b;
//          b.id_davky         = query.value(BT_ID_DAVKY).toString();
//          b.charakter        = query.value(BT_CHARAKTER).toString();
            b.typ              = QString("%1").arg(m_type);
//          b.icz              = query.value(BT_ICZ).toString();
//          b.pob              = query.value(BT_POB).toString();
//          b.rok              = query.value(BT_ROK).toString();
//          b.mesic            = query.value(BT_MESIC).toString();
            b.cislo_davky      = m_batch_number;
            b.pojistovna       = m_in_company_number;
            b.pocet_dokladu    = "0";
            b.pocet_bodu       = "0";
//          b.vytvoreni_davky  = query.value(BT_VYTVORENI_DAVKY).toString();

//	m_batch_info.clear();
//    m_batch_info.append(b);
	emit sendData(b);
	done(QDialog::Accepted);
}


void InsertDialog::checkInsuranceCompany(QString company)
{
	if (DB::getInstance()->isInsuranceCompany(company)) {// pojistovna je v poradku, muzeme vyhledat presnou davku
		m_in_company_number = company;
		findBatch();
	} else {
		Message* msg = new Message();
		msg->setupUI(msg, Message::wrongInCoNo);
		msg->exec();
		LEinCo->clear();
		LEinCo->home(false);
	}
}


PickBatchType::PickBatchType()
{
	w = new QListWidget(this);
	
	new QListWidgetItem(tr("98 - Ambulantní dávka smíšená"), w);
	new QListWidgetItem(tr("99 - Hospitalizační dávka smíšená"), w);
	w->setCurrentRow(0);
	setWindowTitle(tr("Vyberte typ dávky"));
	setAttribute ( Qt::WA_DeleteOnClose,true );

	QVBoxLayout* main_layout = new QVBoxLayout(this);
	main_layout->addWidget(w);

}


void PickBatchType::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Return:
			emit sendData(w->currentRow());
			done(QDialog::Accepted);
			break;
                case Qt::Key_Enter:
                        emit sendData(w->currentRow());
                        done(QDialog::Accepted);
                        break;
		default:

                        QDialog::keyPressEvent(event);
		}
		
}
