#include "viewbatchdialog.h"


ViewBatchDialog::ViewBatchDialog()
{
    m_model = new QSqlTableModel(this);
    m_model->setTable("seznam_davek");
    m_model->setSort(6, Qt::DescendingOrder);
    m_model->setHeaderData(BT_CISLO_DAVKY, Qt::Horizontal, tr("Číslo dávky"));
    m_model->setHeaderData(BT_ICZ, Qt::Horizontal, tr("IČZ"));
    m_model->setHeaderData(BT_POCET_DOKLADU, Qt::Horizontal, tr("Počet dokladů"));
    m_model->setHeaderData(BT_POCET_BODU, Qt::Horizontal, tr("Počet bodů"));
    m_model->setHeaderData(BT_POJISTOVNA, Qt::Horizontal, tr("Pojišťovna"));
    m_model->select();

    setupUI();
}
    
void ViewBatchDialog::setupUI()
{
    setWindowTitle(tr("Seznam dávek"));
    resize(550,300);
    m_view = new QTableView(this);
    m_view->setModel(m_model);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setColumnHidden(BT_ID_DAVKY, true);
    m_view->setColumnHidden(BT_CHARAKTER, true);
    m_view->setColumnHidden(BT_TYP, true);
    m_view->setColumnHidden(BT_POB, true);
    m_view->setColumnHidden(BT_ROK, true);
    m_view->setColumnHidden(BT_MESIC, true);
    m_view->setColumnHidden(BT_VYTVORENI_DAVKY, true);
    m_view->resizeColumnsToContents();
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
        
		
    m_changeBatchNumberAction = new QAction(tr("Upravit"), this);
    m_changeBatchNumberAction->setIcon(QIcon("icons/Edit-16x16.png"));
    connect(m_changeBatchNumberAction, SIGNAL(triggered()), this, SLOT(changeBatchNumber()));

    m_renumberAction = new QAction(tr("Posun dokladů"), this);
    m_renumberAction->setIcon(QIcon("icons/Play-icon-16.png"));
    connect(m_renumberAction, SIGNAL(triggered()), this, SLOT(renumberRecords()));

    m_deleteBatchAction = new QAction(tr("Smazat"), this);
    m_deleteBatchAction->setIcon(QIcon("icons/Delete-16x16.png"));
    connect(m_deleteBatchAction, SIGNAL(triggered()), this, SLOT(deleteBatch()));

    m_view->addAction(m_changeBatchNumberAction);
    m_view->addAction(m_renumberAction);
    m_view->addAction(m_deleteBatchAction);
    m_view->setContextMenuPolicy(Qt::ActionsContextMenu);

    QHeaderView *header = m_view->horizontalHeader();
    header->setStretchLastSection(true);
    m_view->selectRow(0);

    m_printPruvodniList = new QPushButton(tr("Průvodní list"), this);
    connect(m_printPruvodniList, SIGNAL(clicked()), this, SLOT(printPruvodniList()));
    m_obsahDavky = new QPushButton(tr("Obsah dávky"));
    connect(m_obsahDavky, SIGNAL(clicked()), this, SLOT(showBatchDetail()));

    QSpacerItem* horizontalSpacer = new QSpacerItem(290, 22, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_main_layout = new QGridLayout(this);
    m_main_layout->addWidget(m_view,0,0,1,3);
    m_main_layout->addWidget(m_printPruvodniList, 1,0,1,1);
    m_main_layout->addWidget(m_obsahDavky,1,1,1,1);
    m_main_layout->addItem(horizontalSpacer, 1, 2, 1, 1);

	setAttribute ( Qt::WA_DeleteOnClose,true );
}

void ViewBatchDialog::renumberRecords()
{
	QSqlRecord data = m_model->record(m_view->currentIndex().row());
    RenumberDialog* renumber_dialog = new RenumberDialog(data.value(BT_ID_DAVKY).toString());
    if (renumber_dialog->exec()) {
	    QMessageBox::information(0, tr("Přečíslování dokladů"), tr("Doklady v dávce byly přečíslovány"), QMessageBox::Ok);        
    }

     
}

void ViewBatchDialog::deleteBatch()
{
	QSqlRecord data = m_model->record(m_view->currentIndex().row());
	int ret = QMessageBox::information(0,   tr("Smazat dávku"), 
                                            tr("Opravdu smazat dávku %1 u pojišťovny %2 ?").arg(data.value(BT_CISLO_DAVKY).toString(),
																								data.value(BT_POJISTOVNA).toString()),
		QMessageBox::Ok | QMessageBox::Cancel);
	switch (ret) {
		case QMessageBox::Ok:
			if (DB::getInstance()->deleteBatch(data.value(BT_ID_DAVKY).toString())) {
				QMessageBox::information(0, tr("Smazat dávku"), tr("Dávka byla vymazána"), QMessageBox::Ok);
				updateView();
				break;
			} else {
				QMessageBox::critical(0, tr("Smazat dávku"), tr("Nepodařilo se smazat dávku z databáze !"), QMessageBox::Ok);
				break;
			}
		default:
			break;
	}
}

void ViewBatchDialog::changeBatchNumber()
{
    QSqlRecord data = m_model->record(m_view->currentIndex().row());
	EditBatchNumber* edit = new EditBatchNumber(data.value(BT_CISLO_DAVKY).toString(), data.value(BT_POJISTOVNA).toString());
	connect(edit, SIGNAL(updateViewBatchDialog()), this, SLOT(updateView()));
	if (edit->exec()) {
		qDebug() << "prijato";
	} else {
		qDebug() << "odmitnuto";
	}
}

void ViewBatchDialog::showBatchDetail()
{
    QSqlRecord data = m_model->record(m_view->currentIndex().row());
    ViewBatchDetail* detail_widget = new ViewBatchDetail(data.value(BT_CISLO_DAVKY).toString(), data.value(BT_POJISTOVNA).toString(), 
                                                        data.value(BT_ID_DAVKY).toString());
	connect(detail_widget, SIGNAL(updateViewBatchDialog()), this, SLOT(updateView()));
    detail_widget->exec();
}

void ViewBatchDialog::updateView()
{
    m_model->select();	
    m_view->horizontalHeader()->setVisible(m_model->rowCount() > 0);
}

void ViewBatchDialog::printPruvodniList()
{
    QString html;
    QSqlRecord data = m_model->record(m_view->currentIndex().row());
    QDate d = QDate::fromString(data.value(BT_VYTVORENI_DAVKY).toString(), "ddMMyyyy");
    html =  "<html>\n"
            "<head>\n"
            "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
            "<style type=\"text/css\">\n"
            "table {width: 100%; font-family: Courier New; font-size:12pt;}\n"
            "td {width: 60%;}\n"
            "</style>\n"
            "</head><body>\n"
            "<table width=\"100%\" border=0 cellspacing=0\">\n" +
            tr("<tr><td width=\"40%\"><b>IČZ organizace:</b></td>\n") +
            "<td>" +data.value(BT_ICZ).toString() +"</td></tr>\n" +
            tr("<tr><td><b>Číslo dávky:</b></td>\n") +
            "<td>"+data.value(BT_CISLO_DAVKY).toString()+"</td></tr>\n" +
            tr("<tr><td><b>Počet dokladů:</b></td>\n") +
            "<td>"+data.value(BT_POCET_DOKLADU).toString()+"</td></tr>\n" +
            tr("<tr><td><b>Počet bodů v dávce:</b></td>\n") +
            "<td>"+data.value(BT_POCET_BODU).toString()+"</td></tr>\n" +
            tr("<tr><td><b>Období:</b></td>\n") +
            "<td>"+data.value(BT_MESIC).toString()+ " " + data.value(BT_ROK).toString()+"</td></tr>\n" +
            tr("<tr><td><b>Datum vytvoření:</b></td>\n") +
            "<td>"+d.toString("dd.MM.yyyy")+"</td></tr>\n" +
            tr("<tr><td><b>Pojišťovna:</b></td>\n") +
            "<td>"+data.value(BT_POJISTOVNA).toString()+"</td></tr></table></body></html>";
    qDebug() << html;
    printHtml(html);
}

void ViewBatchDialog::printHtml(QString html)
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec()) {
        QTextDocument textDocument;
        textDocument.setHtml(html);
        textDocument.print(&printer);
    }
}



ViewBatchDetail::ViewBatchDetail(const QString bn, const QString incomp, const QString bi) : m_batch_number(bn), m_batch_id(bi), m_in_comp_number(incomp)
{
    createRecordPanel();
    createDetailsPanel();
    setupUI();
}


void ViewBatchDetail::createRecordPanel()
{
    m_record_groupBox = new QGroupBox(tr("Doklady"), this);
    QVBoxLayout* layout = new QVBoxLayout(m_record_groupBox);
    m_record_model = new QSqlTableModel(this);
    m_record_model->setTable("doklady");
    m_record_model->setFilter(QString("id_davky = %1").arg(m_batch_id));
    m_record_model->setHeaderData(RT_CISLO_DOKLADU, Qt::Horizontal, tr("Číslo dokladu"));
    m_record_model->setHeaderData(RT_ICP, Qt::Horizontal, tr("IČP"));
    m_record_model->setHeaderData(RT_ODBORNOST, Qt::Horizontal, tr("Odbornost"));
    m_record_model->setHeaderData(RT_VAR_SYMBOL, Qt::Horizontal, tr("Variabilní symbol"));
    m_record_model->setHeaderData(RT_RODNE_CISLO, Qt::Horizontal, tr("Rodné číslo"));
    m_record_model->setHeaderData(RT_ZAKL_DIAGNOZA, Qt::Horizontal, tr("Základní diagnóza"));
    m_record_model->setHeaderData(RT_BODY_CELKEM, Qt::Horizontal, tr("Body"));
    m_record_model->setHeaderData(RT_VYKONY_CELKEM, Qt::Horizontal, tr("Počet výkonů"));
    m_record_model->select();

    m_record_view = new QTableView(m_record_groupBox);
    m_record_view->setModel(m_record_model);
    m_record_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_record_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_record_view->setColumnHidden(RT_ID_DOKLADU, true);
    m_record_view->setColumnHidden(RT_ID_DAVKY, true);
    m_record_view->sortByColumn(RT_CISLO_DOKLADU, Qt::AscendingOrder);
    m_record_view->setEditTriggers(QAbstractItemView::NoEditTriggers);


    m_record_view->resizeColumnsToContents();
    m_record_view->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_record_view);

    connect(m_record_view->selectionModel(),
            SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(updateDetailsView()));

}

void ViewBatchDetail::createDetailsPanel()
{
    m_detail_groupBox = new QGroupBox(tr("Výkony"), this);
    QVBoxLayout* layout = new QVBoxLayout(m_detail_groupBox);
    m_detail_model = new QSqlTableModel(this);
    m_detail_model->setTable("vykony");
    //m_detail_model->removeColumn(0);
    m_detail_model->setHeaderData(ST_DATUM, Qt::Horizontal, tr("Datum"));
    m_detail_model->setHeaderData(ST_KOD, Qt::Horizontal, tr("Kód"));
    m_detail_model->setHeaderData(ST_POCET, Qt::Horizontal, tr("Počet"));
    m_detail_model->setHeaderData(ST_ODBORNOST, Qt::Horizontal, tr("Odbornost"));
    m_detail_model->setHeaderData(ST_DIAGNOZA, Qt::Horizontal, tr("Diagnóza"));
    m_detail_model->setHeaderData(ST_BODY, Qt::Horizontal, tr("Body"));

    m_detail_view = new QTableView(m_detail_groupBox);
    m_detail_view->setModel(m_detail_model);
    m_detail_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_detail_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_detail_view->setColumnHidden(ST_ID_VYKONU, true);
    m_detail_view->setColumnHidden(ST_ID_DOKLADU, true);
    m_detail_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_detail_view);

}

void ViewBatchDetail::updateDetailsView()
{
//TODO zmenit nazvy promennych..at neni id_vykonu
    QModelIndex index = m_record_view->currentIndex();
    if (index.isValid()) {
        QSqlRecord record = m_record_model->record(index.row());
        qint64 id_vykonu = record.value("id_dokladu").toULongLong();
        m_detail_model->setFilter(QString("id_dokladu = %1").arg(id_vykonu));
    } else {
        m_detail_model->setFilter("id_dokladu = -1");
    }
    m_detail_model->select();
    m_detail_view->horizontalHeader()->setVisible(m_detail_model->rowCount() > 0);
}


void ViewBatchDetail::updateRecordView()
{
    m_record_model->select();	
    m_record_view->horizontalHeader()->setVisible(m_record_model->rowCount() > 0);
}

void ViewBatchDetail::setupUI()
{
    setWindowTitle(tr("Obsah dávky"));
    resize(900,650);
    QLabel* l1 = new QLabel(tr("Číslo dávky: %1").arg(m_batch_number), this);
    QLabel* l2 = new QLabel(tr("Pojišťovna: %1").arg(m_in_comp_number), this);

    m_top_layout = new QHBoxLayout();
    m_top_layout->addWidget(l1);
    m_top_layout->addStretch();
    m_top_layout->addWidget(l2);
    
    m_delete_record_action = new QAction(tr("Smazat"), this);
	m_delete_record_action->setIcon(QIcon("icons/Delete-16x16.png"));
	connect(m_delete_record_action, SIGNAL(triggered()), this, SLOT(deleteRow()));

	m_record_view->addAction(m_delete_record_action);
	m_record_view->setContextMenuPolicy(Qt::ActionsContextMenu);

	QShortcut* delete_row_shortcut = new QShortcut(QKeySequence(tr("Del", "Delete")), this);
	connect(delete_row_shortcut, SIGNAL(activated()), this, SLOT(deleteRow()));

    m_bottom_layout = new QHBoxLayout();
	
	m_replace_button = new QPushButton(tr("Nahradit posledním"), this);
	connect(m_replace_button, SIGNAL(clicked()), this, SLOT(replaceByLast()));
	m_bottom_layout->addWidget(m_replace_button);

    QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_bottom_layout->addItem(horizontalSpacer);

    m_quit_button = new QPushButton(tr("Zavřít"), this);
    connect(m_quit_button, SIGNAL(clicked()), this, SLOT(close()));
    m_bottom_layout->addWidget(m_quit_button);

    m_main_layout = new QVBoxLayout(this);
    m_main_layout->addLayout(m_top_layout);
    m_main_layout->addWidget(m_record_groupBox);
    m_main_layout->addWidget(m_detail_groupBox);
    m_main_layout->addLayout(m_bottom_layout);

    m_record_view->selectRow(0);

	setAttribute ( Qt::WA_DeleteOnClose,true );
}

void ViewBatchDetail::deleteRow()
{
	
	if (m_record_view->hasFocus()) { //chci mazat doklad
    	QSqlRecord data = m_record_model->record(m_record_view->currentIndex().row());	
		int ret = QMessageBox::information(0,   tr("Smazat doklad"), 
                                                tr("Opravdu smazat doklad č. %1 ?").arg(data.value(RT_CISLO_DOKLADU).toString()), 
                                        		QMessageBox::Ok | QMessageBox::Cancel);
		switch (ret) {
			case QMessageBox::Ok:
            // zahajim transakci
                if (!DB::getInstance()->startTransaction()) {
                    QMessageBox::critical(0,tr("Smazat doklad"), 
                                            tr("Nepodařilo se zahajit transakci. Doklad nebyl vymazán."),
                                            QMessageBox::Ok);
                    return;
                }
            // pokusim se smazat doklad
			    if (!DB::getInstance()->deleteRecord(data.value(RT_ID_DOKLADU).toULongLong())) {
                    DB::getInstance()->rollbackTransaction();
                    return;
                }
            // posunu cisla dokladu
                if (!DB::getInstance()->shiftRecordNumber(data.value(RT_CISLO_DOKLADU).toInt(),       //od
                                                         0,                                     // vsechny vetsi nez od
                                                         -1,                                     // o 1
                                                         data.value(RT_ID_DAVKY).toString())) {             // v davce id_davky 
                    DB::getInstance()->rollbackTransaction();
                    return;
                }
            //commit
				if (!DB::getInstance()->commitTransaction()) {
                    QMessageBox::critical(0,tr("Smazat doklad"), 
                                            tr("Nepodařilo se ukoncit transakci. Doklad nebyl vymazán."),
                                            QMessageBox::Ok);
                    return;
                } else {                
                    QMessageBox::information(0, tr("Smazat doklad"), 
                                                tr("Doklad č. %1 byl vymazán").arg(data.value(RT_CISLO_DOKLADU).toString()), 
                                                QMessageBox::Ok);
                    updateRecordView();
                }
				break;
			default:
				break;
		}
	
	}
	if (m_detail_view->hasFocus()) { //chci mazat vykon
		qDebug() << "spodek";
	}
}

void ViewBatchDetail::replaceByLast()
{
//TODO zmenit nazvy promennych
	QModelIndex index = m_record_view->currentIndex();
	
	if (index.isValid()) {
	    int last_row_index = m_record_model->rowCount() - 1; //pocet radku (tedy pocet dokladu) - 1 (cisluje se od 0)
		if (index.row() == last_row_index) {
			QMessageBox::critical(0, tr("Chyba"), tr("Je zbytečné nahrazovat poslední doklad posledním dokladem."), QMessageBox::Ok);
			return;
		}
		QSqlRecord record = m_record_model->record(index.row());
		QString record_number = record.value("cislo_dokladu").toString();
		QString id_dokladu = record.value("id_dokladu").toString();
		QSqlRecord last_record = m_record_model->record(last_row_index); 
		QString last_record_number = last_record.value("cislo_dokladu").toString();
		QString last_id_dokladu = last_record.value("id_dokladu").toString();

		int ret = QMessageBox::information(0, tr("Nahrazení dokladu"), tr("Doklad %1 bude vymazán a nahrazen dokladem %2").arg(record_number, last_record_number),
		QMessageBox::Ok | QMessageBox::Cancel);
		switch (ret) {
			case QMessageBox::Ok:{
			if (!m_record_model->removeRow(index.row())) {
					QMessageBox::critical(0, tr("Chyba"), tr("Nepodařilo se vymazat doklad !"), QMessageBox::Ok);
					return;
				}
				m_record_model->submitAll();
                
				QModelIndex i_cd = m_record_model->index(m_record_model->rowCount() - 1, RT_CISLO_DOKLADU); // index cisla dokladu posledniho zaznamu
				m_record_model->setData(i_cd, record_number);	//u posledniho zaznamu zmenim cislo dokladu na to, ktere jsem smazal
				m_record_model->submitAll();
				}
				break;
			case QMessageBox::Cancel:
				qDebug() << "cancel";
				break;
			default:
				break;

		}	
	}
}

void ViewBatchDetail::closeEvent(QCloseEvent *event)
{
	emit updateViewBatchDialog();
	event->accept();
}



EditBatchNumber::EditBatchNumber(QString old_number, QString in_comp) : m_old_batch(old_number), m_in_company(in_comp)
{
	setupUI();
}

void EditBatchNumber::setupUI()
{
	setWindowTitle(tr("Upravit záznam"));
	QVBoxLayout* main_layout = new QVBoxLayout(this);
	QHBoxLayout* top_layout = new QHBoxLayout();
	QLabel* l1 = new QLabel(tr("Číslo dávky:"), this);
	top_layout->addWidget(l1);
	
	QFontMetrics metrics(QApplication::font());

	m_batch_number = new QLineEdit(this);
	m_batch_number->setInputMask("000009");
	m_batch_number->setMaxLength(6);
    m_batch_number->setFixedSize(metrics.width("888888") + 11, metrics.height()+4);
	top_layout->addWidget(m_batch_number);

	QLabel* l2 = new QLabel(tr("Číslo pojišťovny:"), this);
	top_layout->addWidget(l2);

	m_new_in_comp = new QLineEdit(this);
	m_new_in_comp->setInputMask("999");
	m_new_in_comp->setMaxLength(3);
    m_new_in_comp->setFixedSize(metrics.width("888") + 11, metrics.height()+4);
	m_new_in_comp->setText(m_in_company);
	top_layout->addWidget(m_new_in_comp);
	top_layout->addStretch();

	main_layout->addLayout(top_layout);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
	main_layout->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(okClicked()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	
}

void EditBatchNumber::okClicked()
{
	const QString new_batch_number = m_batch_number->text();
	const QString new_in_comp = m_new_in_comp->text();
	
	if (new_batch_number.isEmpty() || new_in_comp.isEmpty()) {
		QMessageBox::information(0, tr("Upravit záznam"), tr("Číslo dávky a pojišťovny musí být vyplněno"), QMessageBox::Ok);
		return;
	}
	if (!DB::getInstance()->isInsuranceCompany(new_in_comp)) {
		QMessageBox::critical(0, tr("Upravit záznam"), tr("Chybné číslo pojišťovny (%1)").arg(new_in_comp)); 
		m_new_in_comp->clear();
		m_new_in_comp->home(false);
		return;
	}
	
	QList<CrateBatch> batch_info = DB::getInstance()->getBatchInfo(new_batch_number, new_in_comp);
    if (batch_info.size() > 1) {
        qDebug() << "ERROR: EditBatchNumber::okClicked - nedokazu zpracovat vice davek";
    }
	if (batch_info.isEmpty()) {
		if (!DB::getInstance()->changeBatchNumber(m_old_batch, m_in_company, new_batch_number, new_in_comp)) {
			QMessageBox::critical(0, tr("Upravit záznam"), tr("Nepodařilo se upravit záznam v databázi"), QMessageBox::Ok);
			return;
		} else {
			QMessageBox::information(0, tr("Upravit záznam"), tr("Číslo dávky bylo změněno z %1 na %2.\n"
			                                                     "Číslo pojišťovny bylo změněno z %3 na %4").arg(m_old_batch, 
																                                                 new_batch_number,
																												 m_in_company,
																												 new_in_comp), 
			QMessageBox::Ok);
		}

    	emit updateViewBatchDialog();	

	} else {
		QMessageBox::information(0, tr("Upravit záznam"), tr("Dávka %1 u pojišťovny %2 již existuje").arg(new_batch_number, 
		new_in_comp), QMessageBox::Ok);
		m_batch_number->clear();
		m_batch_number->home(false);
		m_new_in_comp->setText(m_in_company);
		m_batch_number->setFocus();
		return;	
	}
		
	accept();
}


RenumberDialog::RenumberDialog(QString bid) : m_batch_id(bid)
{
    setupUI();
}

void RenumberDialog::setupUI()
{
    setWindowTitle(tr("Přečíslování dokladů"));

    QHBoxLayout* top_layout = new QHBoxLayout();
    QLabel* l1 = new QLabel(tr("Posunout doklady o: "), this);
    QValidator *validator = new QIntValidator(this);
    m_how_much = new QLineEdit(this);
    m_how_much->setMaxLength(7);
    m_how_much->setValidator(validator);

    top_layout->addWidget(l1);
    top_layout->addWidget(m_how_much);
    
    QVBoxLayout* main_layout = new QVBoxLayout(this);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(okClicked()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    main_layout->addLayout(top_layout);
    main_layout->addWidget(buttonBox);
}

void RenumberDialog::okClicked()
{
    int number = m_how_much->text().toInt();
    if (!DB::getInstance()->startTransaction()) {
        QMessageBox::critical(0,tr("Přečíslování dokladů"), 
                               tr("Nepodařilo se zahajit transakci. Přečíslování dokladů neuspělo."),
                               QMessageBox::Ok);
        reject();
    }
    if (!DB::getInstance()->shiftRecordNumber(number, m_batch_id)) {
        DB::getInstance()->rollbackTransaction();
        reject();
    }        

    if (!DB::getInstance()->commitTransaction()) {
        QMessageBox::critical(0,tr("Přečíslování dokladů"), 
                                tr("Nepodařilo se ukončit transakci. Přečíslování dokladů neuspělo."),
                                QMessageBox::Ok);
        reject();
    } else {                
        accept();
    }
}
