#include "viewregisterdialog.h"

ViewRegisterDialog::ViewRegisterDialog()
{
    m_model = new QSqlTableModel(this);
    m_model->setTable("kartoteka");

    m_model->setHeaderData(R_RODNE_CISLO, Qt::Horizontal, tr("Rodné číslo"));
    m_model->setHeaderData(R_JMENO, Qt::Horizontal, tr("Jméno"));
    m_model->setHeaderData(R_PRIJMENI, Qt::Horizontal, tr("Příjmení"));
    m_model->setHeaderData(R_POJISTOVNA, Qt::Horizontal, tr("Pojišťovna"));
    m_model->setHeaderData(R_DATUM_VLOZENI, Qt::Horizontal, tr("Datum vložení"));
    m_model->setHeaderData(R_DATUM_REGISTRACE, Qt::Horizontal, tr("Datum registrace"));

    m_model->select();

    setupUI();
}

void ViewRegisterDialog::setupUI()
{
    setWindowTitle(tr("Seznam pacientů"));
    resize(700,400);
 //   m_view = new QTableView(this);
  //  m_view->setModel(m_model);


    m_proxy_view = new QTableView(this);
    m_proxy = new QSortFilterProxyModel();
    m_proxy->setDynamicSortFilter(true);
    m_proxy_view->setModel(m_proxy);
    m_proxy->setSourceModel(m_model);


//    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
      m_proxy_view->setSelectionBehavior(QAbstractItemView::SelectRows);
      m_proxy_view->setColumnHidden(R_ID_PACIENT, true);
      m_proxy_view->resizeColumnsToContents();
//    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
//        
//	  m_view->setContextMenuPolicy(Qt::ActionsContextMenu);
//
    m_search = new QLineEdit(); 
    connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(search()));

    m_main_layout = new QVBoxLayout(this);
    //m_main_layout->addWidget(m_view);
    m_main_layout->addWidget(m_proxy_view);

    QHBoxLayout* search_layout = new QHBoxLayout();
    QLabel* l = new QLabel(tr("Vyhledat:"));
    m_search_column = new QComboBox();
    m_search_column->addItem(tr("Rodné číslo"), R_RODNE_CISLO);
    m_search_column->addItem(tr("Jméno"), R_JMENO);
    m_search_column->addItem(tr("Příjmení"), R_PRIJMENI);
    m_search_column->addItem(tr("Pojišťovna"), R_POJISTOVNA);

    connect(m_search_column, SIGNAL(currentIndexChanged(int)), this, SLOT(searchColumnChanged()));

    search_layout->addWidget(l);
    search_layout->addWidget(m_search_column);
    search_layout->addWidget(m_search);

    m_main_layout->addLayout(search_layout);

    m_proxy->setFilterKeyColumn(m_search_column->itemData(m_search_column->currentIndex()).toInt());

    QAction* deletePacientAction = new QAction(tr("Smazat"), this);
    deletePacientAction->setIcon(QIcon("icons/Delete-16x16.png"));
    connect(deletePacientAction, SIGNAL(triggered()), this, SLOT(deletePacient()));

    m_proxy_view->addAction(deletePacientAction);
    m_proxy_view->setContextMenuPolicy(Qt::ActionsContextMenu);

    setAttribute ( Qt::WA_DeleteOnClose, true );
}

void ViewRegisterDialog::searchColumnChanged()
{
    m_proxy->setFilterKeyColumn(m_search_column->itemData(m_search_column->currentIndex()).toInt());
}

void ViewRegisterDialog::deletePacient()
{
    QModelIndex index = m_proxy->mapToSource(m_proxy_view->currentIndex());
    QSqlRecord data = m_model->record(index.row());
	int ret = QMessageBox::information(0,   tr("Smazat pacienta"), 
                                            tr("Opravdu smazat pacienta?"), QMessageBox::Ok | QMessageBox::Cancel);
	switch (ret) {
		case QMessageBox::Ok:
			if (DB::getInstance()->deletePacient(data.value(R_ID_PACIENT).toString())) {
				QMessageBox::information(0, tr("Smazat pacienta"), tr("Pacient byl vymazán"), QMessageBox::Ok);
				m_model->select();	
//                m_view->horizontalHeader()->setVisible(m_model->rowCount() > 0);
				break;
			} else {
				QMessageBox::critical(0, tr("Smazat pacienta"), tr("Nepodařilo se smazat pacienta z databáze !"), QMessageBox::Ok);
				break;
			}
		default:
			break;
	}
}

void ViewRegisterDialog::search()
{
    QString s = "^" + m_search->text();
    qDebug() << s;
    QRegExp regExp(s, Qt::CaseInsensitive);
    m_proxy->setFilterRegExp(regExp);
}

AddPacientDialog::AddPacientDialog(QString rc)
{
    m_rc = rc;
    setupUI();
}

void AddPacientDialog::setupUI()
{
    setWindowTitle(tr("Přidat pacienta"));
    m_main_layout = new QVBoxLayout(this);

    QLabel* text = new QLabel(QString::fromUtf8("Pacient s rodným číslem %1 nebyl nalezen v kartotéce.").arg(m_rc));

    m_name_label = new QLabel(tr("Jméno:"));
    m_surname_label = new QLabel(tr("Příjmení:"));

    m_name_edit = new QLineEdit;
    m_surname_edit = new QLineEdit;

    m_ok_button = new QPushButton(tr("Přidat"));
    m_ok_button->setAutoDefault(false);
    m_cancel_button = new QPushButton(tr("Zrušit"));
    m_cancel_button->setAutoDefault(false);


    connect(m_ok_button, SIGNAL(clicked()), this, SLOT(ok()));
    connect(m_cancel_button, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout *name_layout = new QHBoxLayout();
    name_layout->addWidget(m_name_label);
    name_layout->addWidget(m_name_edit);

    QHBoxLayout *surname_layout = new QHBoxLayout();
    surname_layout->addWidget(m_surname_label);
    surname_layout->addWidget(m_surname_edit);

    QHBoxLayout *button_layout = new QHBoxLayout();
    button_layout->addWidget(m_ok_button);
    button_layout->addWidget(m_cancel_button);
    button_layout->addStretch();

    m_main_layout->addWidget(text);
    m_main_layout->addLayout(name_layout);
    m_main_layout->addLayout(surname_layout);
    m_main_layout->addLayout(button_layout);

}

void AddPacientDialog::ok()
{
    emit sendNameSurname(m_name_edit->text(), m_surname_edit->text());
    accept();
}
