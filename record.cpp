#include "record.h"
#include <iostream>



//TODO nedostatu se k diagnoze,kdyz to upravuju !!!


RecordInfo::RecordInfo(MainWindow* p)
{
	m_main_window = p;
	m_hcid = m_main_window->hcid().toInt();
	//m_date = m_main_window->date();
	m_mainDiag = "";
	m_main_window->menuBar()->hide();
//	m_sequence_number = 1;
	m_record_id = "";
	m_isChanged = false;
	setAttribute ( Qt::WA_DeleteOnClose,true );
}

void RecordInfo::setupUI(QWidget* parent)
{
	GBAmbRecord = new QGroupBox(tr("Ambulantní účet"), parent);
	GBAmbRecord->setObjectName("RecordInfo");
	
    QSpacerItem* sp1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QHBoxLayout* l1 = new QHBoxLayout();
	l1->addItem(sp1);
	
	QLabel*	L4= new QLabel(tr("Číslo dokladu:"), GBAmbRecord);
	LERecordNo = new RecordNumberLineEdit();
	LERecordNo->setText(QString("%1").arg(m_hcid));
	l1->addWidget(L4);
	l1->addWidget(LERecordNo);


    QSpacerItem* sp2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QHBoxLayout* l2 = new QHBoxLayout();
	l2->addItem(sp2);

	QLabel*	L5= new QLabel(tr("IČP:"), GBAmbRecord);
	LEIcp = new IcpLineEdit();
		LEIcp->setText(m_main_window->icz());

	l2->addWidget(L5);
	l2->addWidget(LEIcp);

	QSpacerItem* sp3 = new QSpacerItem(40,20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QHBoxLayout* l3 = new QHBoxLayout();
	l3->addItem(sp3);

	QLabel*	L6= new QLabel(tr("Odbornost:"), GBAmbRecord);
	LEExpertise = new OdbLineEdit();
		LEExpertise->setText(m_main_window->hodb());
	l3->addWidget(L6);
	l3->addWidget(LEExpertise);
		
	QSpacerItem* sp4 = new QSpacerItem(40,20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QHBoxLayout* l4 = new QHBoxLayout();
	l4->addItem(sp4);

	QLabel*	L7= new QLabel(tr("Variabilní symbol:"), GBAmbRecord);
	LEVariableS = new VarSymbolLineEdit();
	l4->addWidget(L7);
	l4->addWidget(LEVariableS);

	QSpacerItem* sp5 = new QSpacerItem(40,20,QSizePolicy::Expanding, QSizePolicy::Minimum);
	QHBoxLayout* l5 = new QHBoxLayout();
	l5->addItem(sp5);
	QLabel*	L8= new QLabel(tr("Rodné číslo:"), GBAmbRecord);
	LEPersonalNo = new PersonalNumberLineEdit();

	l5->addWidget(L8);
	l5->addWidget(LEPersonalNo);


	QSpacerItem* sp6 = new QSpacerItem(40,20,QSizePolicy::Expanding, QSizePolicy::Minimum);
	QHBoxLayout* l6 = new QHBoxLayout();
	l6->addItem(sp6);

	QLabel*	L9= new QLabel(tr("Základní diagnóza:"), GBAmbRecord);
	LEDiagnosis = new DiagnosisLineEdit(m_mainDiag);
	l6->addWidget(L9);
	l6->addWidget(LEDiagnosis);

	LERecordNo->setNeighbours(0, LEIcp);
	LEIcp->setNeighbours(LERecordNo, LEExpertise);
	LEExpertise->setNeighbours(LEIcp, LEVariableS);
	LEVariableS->setNeighbours(LEExpertise, LEPersonalNo);
	LEPersonalNo->setNeighbours(LEVariableS, LEDiagnosis);
	LEDiagnosis->setNeighbours(LEPersonalNo, 0);

	gridLayout = new QGridLayout(GBAmbRecord);
		gridLayout->addLayout(l1, 0, 0, 1, 1);
		gridLayout->addLayout(l2, 0, 1, 1, 1);
		gridLayout->addLayout(l3, 0, 2, 1, 1);
		gridLayout->addLayout(l4, 0, 3, 1, 1);
		gridLayout->addLayout(l5, 1, 0, 1, 1);
		gridLayout->addLayout(l6, 1, 1, 1, 1);


QString style = "QLabel{background-color: blue; color: white;}"
				"QFrame{background-color: blue;}";



	mainLayout= new QVBoxLayout(parent);
	mainLayout->addWidget(GBAmbRecord);
	mainLayout->addStretch();
	helpFrame = new QFrame(this);
	helpFrame->setObjectName("HelpFrame");
	helpFrame->setStyleSheet(style);
		helpFrame->setFrameShape(QFrame::StyledPanel);
		QLabel* escl = new QLabel(tr("Esc = konec"), helpFrame);
		QLabel* pgl = new QLabel(tr("PageUp/PageDown = nahoru/dolů"), helpFrame);
		QLabel* del = new QLabel(tr("Alt+D = vymazání řádku"), helpFrame);
		QLabel* f4 = new QLabel(tr("F4 = záhlaví dokladu"), helpFrame);
		QHBoxLayout* helpFrameLayout = new QHBoxLayout(helpFrame);
		helpFrameLayout->addWidget(escl);
		helpFrameLayout->addWidget(pgl);
		helpFrameLayout->addWidget(del);
		helpFrameLayout->addWidget(f4);
		helpFrame->setFrameShadow(QFrame::Raised);
		mainLayout->addWidget(helpFrame);
		helpFrame->hide();

	connect(LEPersonalNo, SIGNAL(sendData(QString)), this, SLOT(checkRc(QString)));
	connect(LEDiagnosis, SIGNAL(sendData(QString)), this, SLOT(checkDiag(QString)));
	connect(LERecordNo, SIGNAL(sendData( QString)), this, SLOT(checkRecordNo(QString)));
	connect(LEIcp, SIGNAL(sendData(QString)), this, SLOT(checkIcp(QString)));
	connect(LEExpertise, SIGNAL(sendData(QString)), this, SLOT(checkOdb(QString)));
	connect(LEVariableS, SIGNAL(sendData(QString)), this, SLOT(checkVarS(QString)));
	shortcut = new QShortcut(QKeySequence(tr("Esc", "Close")), this);

	connect(shortcut, SIGNAL(activated()), this, SIGNAL(exitBatchWriting()));
}

void RecordInfo::checkRecordNo(QString record_number)
{
	
	CrateRecordInfo ri = DB::getInstance()->getRecordInfo(m_main_window->batchId(),record_number);

	if (!ri.isEmpty()) { //id_vykonu -> nalezeno
		//otestuj, zda-li neni rc v kartotece a kdyztak dopln jmeno a prijmeni
        LEIcp->setValue(ri.icp);
		LEExpertise->setValue(ri.odb);
		LEVariableS->setValue(ri.var_symbol);
		LEPersonalNo->setValue(ri.rodne_cislo);
		LEDiagnosis->setValue(ri.zakl_diag);

        findRcInRegister(ri.rodne_cislo);

		m_record_id = ri.id_vykonu;
		m_record_row_id = ri.rowid;
		m_mainDiag = ri.zakl_diag;

		QList<CrateRecordInfoData> data = DB::getInstance()->getRecordData(m_record_id);

		m_record = new Record(this);
		m_record->setupUI(m_record);
		mainLayout->insertWidget(1,m_record);
		shortcut->setEnabled(false);
		
		connect(m_record, SIGNAL(done()), this, SLOT(newRecord()));
		connect(m_record, SIGNAL(updateBatchInfo()), this, SIGNAL(updateBatchInfo()));
		connect(m_record, SIGNAL(recordDeleted()), this, SLOT(recordDeleted()));

		helpFrame->show();
		CrateRecordInfoData d;
	    while (!data.isEmpty()) {
			d = data.takeFirst();
			m_record->createNewLine(d.datum, d.kod, d.pocet, d.odb, d.diag, d.rowid);
		}
		m_record->createNewLine(QDate(), "");
		LineEdit* first_item= m_record->firstItem();
		first_item->setFocus();
		emit updateBatchInfo();
	} else { //takove cislo dokladu se jeste nepsalo
		m_hcid = LERecordNo->text().toInt();
		LERecordNo->focusNext();
	}
}

void RecordInfo::checkVarS(QString value)
{
	LEVariableS->focusNext();
}

void RecordInfo::checkIcp(QString value)
{
	LEIcp->focusNext();
}

//TODO pridat kontrolu odbornosti podle metodiky vyhodnocovani dokladu
void RecordInfo::checkOdb(QString value)
{
	LEExpertise->focusNext();
}

//TODO pokud se nemeni diagnoza,tak by nemusel byt select do databaze,uz je zkontrolovana
//TODO pridat kontrolu diagnozy podle metodiky!!!
//TODO pokud uz tam jednou je,tak nevkladat znova !!!!
void RecordInfo::checkDiag(QString diag)
{
	DiagnoseStatus status = DB::getInstance()->isValidDiagnose(diag);
	
	if (status == DIAGNOSE_OK) {
		if (m_isChanged) { //upravuju diagnozu pomoci f4
			if (m_mainDiag == diag)	{// diagnoza se nezmenila, muzu updatnout + zmenit focus
				updateRecordInfo(); //update (kvuli zmene nektere hodnoty)
				LineEdit* last_item= m_record->lastItem();
				last_item->setFocus();
                        } else { //diagnoza se zmenila a je ok -> zeptat se, zda udelat update
				                Message* msg = new Message();
                                msg->setupUI(msg, Message::changeDiag);
                                emit playError();
                                if (msg->exec()){ //ano..chci precislovat
                                    qDebug() << "ano chci precislovat";
                                    updateRecordInfo(); //update (kvuli zmene nektere hodnoty)
                                    DB::getInstance()->changeDiagnose(m_record_id, m_mainDiag, diag ); //zmenim diagnozu
                                    m_mainDiag = diag;
                                    m_record->refreshDiagnoses();
                                    LineEdit* last_item= m_record->lastItem();
                                    last_item->setFocus();
                                } else { //nechci..nechavam to tak
                                    qDebug() << "necham tak jak je";
                                    updateRecordInfo(); //update (kvuli zmene nektere hodnoty)
                                    m_mainDiag = diag;
                                    LineEdit* last_item= m_record->lastItem();
                                    last_item->setFocus();

                                }
			}
                        m_isChanged = false;				//zrusim upravovani diagnozy
                        LEIcp->setNeighbours(LERecordNo, LEExpertise); //povolim pohyb na cislo dokladu
                } else {
                    m_mainDiag = diag;

                    m_record = new Record(this);
                    mainLayout->insertWidget(1,m_record);
                    m_record->setupUI(m_record);
                    m_record->createNewLine(QDate(), "");

                    LineEdit* first_item= m_record->firstItem();
                    first_item->setFocus();

                    helpFrame->show();

                    shortcut->setEnabled(false);
		
                    connect(m_record, SIGNAL(done()), this, SLOT(newRecord()));
                    connect(m_record, SIGNAL(updateBatchInfo()), this, SIGNAL(updateBatchInfo()));
                    connect(m_record, SIGNAL(recordDeleted()), this, SLOT(recordDeleted()));
		
                    saveRecordInfo();
                }
	} else if (status == DIAGNOSE_BAD) {
		Message* msg = new Message();
		msg->setupUI(msg, Message::wrongDiag);
		emit playError();
		msg->exec();
		LEDiagnosis->clear();
		LEDiagnosis->home(false);
	} else if (status == DIAGNOSE_STAR) {
		Message* msg = new Message();
		msg->setupUI(msg, Message::starDiag);
		emit playError();
		msg->exec();
		LEDiagnosis->clear();
		LEDiagnosis->home(false);

	}

}

void RecordInfo::updateRecordInfo()
{
	CrateRecordInfo d;
    d.cislo_dokladu = LERecordNo->text();
	d.icp = LEIcp->text();
	d.odb = LEExpertise->text();
	QString var_symbol = LEVariableS->text();
	if (var_symbol.isEmpty()){
		var_symbol = "      ";
	}
	d.var_symbol = var_symbol;
	d.rodne_cislo = LEPersonalNo->text();
	d.zakl_diag = LEDiagnosis->text();
	d.rowid = m_record_row_id;
	
	if (!DB::getInstance()->updateRecordInfo(d) ) {
		qDebug() << "ERROR: Nepodarily se ulozit data o dokladu";
	}
}

void RecordInfo::recordDeleted()
{
	//cislo dokladu,zustane stejne
	//poradove cislo dokladu,zustane stejne

	LEPersonalNo->clear();
	LEDiagnosis->clear();
	LEPersonalNo->setFocus();
	shortcut->setEnabled(true);
	helpFrame->hide();
	emit updateBatchInfo();
}

void RecordInfo::saveRecordInfo()
{
		CrateRecordInfo d;
		d.id_davky = m_main_window->batchId();
		d.cislo_dokladu = QString("%1").arg(m_hcid);
		d.icp = LEIcp->text();
		d.odb = LEExpertise->text();
		QString var_symbol = LEVariableS->text();
		if (var_symbol.isEmpty()){
			var_symbol = "      ";
		}
		d.var_symbol = var_symbol;
		d.rodne_cislo = LEPersonalNo->text();
		d.zakl_diag = m_mainDiag;
		d.body_celkem = "0";
		d.vykony_celkem	= "0";
//		m_record_id = QString("%1").arg(m_hcid);
//		m_record_id.prepend(d.id_davky);
//		d.id_vykonu = m_record_id;
		
		if ((m_record_row_id = DB::getInstance()->saveRecordInfo(d)) != -1) {
            m_record_id = QString("%1").arg(m_record_row_id);
			emit updateBatchInfo();
		} else {
			qDebug() << "ERROR: nepodarily se ulozit data o dokladu";
		}
}

void RecordInfo::newRecord()
{

  //novy doklady - smazeme jmeno a prijmeni pacienta z nazvu groupboxu
  GBAmbRecord->setTitle(tr("Ambulantní účet")); 

	m_hcid = DB::getInstance()->getMaxRecordNumber(m_main_window->batchId()).toInt() + 1;
	CrateRecordInfo ri = DB::getInstance()->getRecordInfo(m_main_window->batchId(),QString("%1").arg(m_hcid));

	if (!ri.isEmpty()) { //id_vykonu -> nalezeno, pokud ve vykonech dam esc, a dalsi doklad uz je v databazi, tak ho rovnou zobrazim
		LERecordNo->setText(ri.cislo_dokladu);
		LEIcp->setValue(ri.icp);
		LEExpertise->setValue(ri.odb);
		LEVariableS->setValue(ri.var_symbol);
		LEPersonalNo->setValue(ri.rodne_cislo);
		LEDiagnosis->setValue(ri.zakl_diag);
		m_record_id = ri.id_vykonu;
		m_record_row_id = ri.rowid;
		m_mainDiag = ri.zakl_diag;

		QList<CrateRecordInfoData> data = DB::getInstance()->getRecordData(m_record_id);

		m_record = new Record(this);
		mainLayout->insertWidget(1,m_record);
		m_record->setupUI(m_record);
		shortcut->setEnabled(false);
		
		connect(m_record, SIGNAL(done()), this, SLOT(newRecord()));
		connect(m_record, SIGNAL(updateBatchInfo()), this, SIGNAL(updateBatchInfo()));
		connect(m_record, SIGNAL(recordDeleted()), this, SLOT(recordDeleted()));

		helpFrame->show();
		CrateRecordInfoData d;
	    while (!data.isEmpty()) {
			d = data.takeFirst();
			m_record->createNewLine(d.datum, d.kod, d.pocet, d.odb, d.diag, d.rowid);
		}
		m_record->createNewLine(QDate(), "");
		LineEdit* first_item= m_record->firstItem();
		first_item->setFocus();
		emit updateBatchInfo();
	} else {
//	m_sequence_number++;
		LERecordNo->setText(QString("%1").arg(m_hcid));
		LEPersonalNo->clear();
		LEDiagnosis->clear();
		LEPersonalNo->setFocus();
		shortcut->setEnabled(true);
		helpFrame->hide();
		emit clearBatchInfo();
	}
}

void RecordInfo::findRcInRegister(QString rc)
{
    if (DB::getInstance()->findPersonalNumber(rc)) {
        GBAmbRecord->setTitle(QString::fromUtf8("Ambulantní účet - %1 %2").arg(DB::getInstance()->getPacientName(rc),DB::getInstance()->getPacientSurname(rc)));
        if (DB::getInstance()->getPacientInsComp(rc) != m_main_window->hcpo()) {
            Message* msg = new Message();
            msg->setupUI(msg, Message::diffInsComp);
            emit playError();
            if (msg->exec()){ //ano..chci prepsat
                DB::getInstance()->changeInsComp(rc, m_main_window->hcpo());    
            }
        } 
    } else {
        AddPacientDialog* add_pacient = new AddPacientDialog(rc);  
        connect(add_pacient, SIGNAL(sendNameSurname(QString, QString)), this, SLOT(savePacient(QString, QString)));
        add_pacient->exec();
    } 
}

void RecordInfo::savePacient(QString name, QString surname)
{
    CratePacient p;
    p.rc = LEPersonalNo->text();
    qDebug() << "rc - " << p.rc;
    p.jmeno = name;
    p.prijmeni = surname;
    p.pojistovna = m_main_window->hcpo();
    p.vlozeno = m_main_window->date().toString("dd.MM.yyyy");
    DB::getInstance()->savePacient(p);
    GBAmbRecord->setTitle(QString::fromUtf8("Ambulantní účet - %1 %2").arg(DB::getInstance()->getPacientName(p.rc),DB::getInstance()->getPacientSurname(p.rc))); 
}




void RecordInfo::checkRc(QString value)
{
	if(checkPersonalNo(value)){
    qDebug() << "INFO: Rodne cislo OK";
    findRcInRegister(value);
		LEPersonalNo->focusNext();		
	}
	else{
		qDebug() << "WARNING: Spatne rodne cislo";
		Message* msg = new Message();
		msg->setupUI(msg, Message::wrongPersNo);
		emit playError();

		if (msg->exec()){
			qDebug() << "WARNING: potvrzeno, i kdyz je spatne";
			LEPersonalNo->focusNext();
		}
		else{
			LEPersonalNo->home(false);
		}
	}
}

bool RecordInfo::checkPersonalNo(QString Rc)
{
  int year, month, day, ext, c = 0;
  bool conv_ok;
	QRegExp rx;
	rx.setPattern("^(\\d\\d)(\\d\\d)(\\d\\d)(\\d\\d\\d)(\\d?)$");
	if (rx.indexIn(Rc) != -1) {
		year = rx.cap(1).toInt();
		month = rx.cap(2).toInt();
		day = rx.cap(3).toInt();
		ext = rx.cap(4).toInt();
		c = rx.cap(5).toInt(&conv_ok);
	} else { 
    return false; 
  }
 	
  if ((c == 0) && (conv_ok == false) ) { // rok < 1954, nelze kontrolovat delitelnost 11, pouze validni datum
	  year += 1900;
    if (QDate::isValid(year, month > 50 ? month - 50 : month, day)) {
          qDebug() << "INFO: rok: " << year << "mesic: " << month << "den: " << day << "koncovka: " << ext << "kontrolni cislo: " << c;
					return true;
	  } else {
      return false;
    }
	} else { // rok >= 1954
		Rc.chop(1);
		int mod = Rc.toInt() % 11;  //zbytek po deleni 11
		if (mod == 10) mod = 0;     //pokud je zbytek 10, kontrolni cislice je 0
		if (mod != c) return false;

		year = year < 54 ? year + 2000 : year + 1900;
	  
    // 53/2004 Sb. http://cs.wikipedia.org/wiki/Rodn%C3%A9_%C4%8D%C3%ADslo
    // chybi detekce pro muze
		if (month > 70 && year >= 2004) month -= 70; 
		else if (month > 50) month -= 50;
    
    // cizinky mohou mit den + 50
    day = day > 50 ? day - 50 : day;

		if (QDate::isValid(year, month, day)) {
			qDebug() << "INFO: rok: " << year << "mesic: " << month << "den: " << day << "koncovka: " << ext << "kontrolni cislo: " << c;
      return true;
		} else {
			return false; //cislo je delitelne, ale rok,mesic nebo den jsou spatne
		}
	}
 return false;
}

void RecordInfo::f4()
{
	LEDiagnosis->setFocus(); //focus na hlavni diagnozu
	m_isChanged = true;				//nastavim, ze se bude upravovat informace o dokladu
	LEIcp->setNeighbours(0, LEExpertise); //zakazu pohyb na cislo dokladu
}

BatchInfo::BatchInfo(MainWindow *parent, RecordInfo* record_info)
{
	m_main_window = parent;
	m_record_info = record_info;
	setAttribute ( Qt::WA_DeleteOnClose,true );
}

void BatchInfo::setupUI(QWidget* parent)
{
	GBInsComp = new QGroupBox(tr("Pojišťovna: %1").arg(m_main_window->hcpo()), parent);
	GBInsComp->setObjectName("BatchInfo");
	QLabel* L1 = new QLabel(tr("Číslo dávky: "), GBInsComp);
	LBatchNo = new QLabel(tr("%1").arg(m_main_window->dcid()), GBInsComp);
	LBatchNo->setStyleSheet("color: yellow");
	QLabel* L2 = new QLabel(tr("Počet dokladů:"), GBInsComp);
	LRecordsNo = new QLabel(tr("%1").arg(m_main_window->numRecords()), (GBInsComp));
	LRecordsNo->setStyleSheet("color: yellow");
	QLabel* L3 = new QLabel(tr("Body: "), GBInsComp);
	LPoints = new QLabel(tr("%1").arg(m_main_window->numPoints()), GBInsComp);
	LPoints->setStyleSheet("color: yellow");

	QLabel* L4 = new QLabel(tr("Počet výkonů:"),GBInsComp);
	LServiceNo = new QLabel("0");
	LServiceNo->setStyleSheet("color: red");
	QLabel* L5 = new QLabel(tr("Body za doklad:"),GBInsComp);
	LRecordPoints = new QLabel("0");
	LRecordPoints->setStyleSheet("color: red");

	QSpacerItem* sp1 = new QSpacerItem(45, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QSpacerItem* sp2 = new QSpacerItem(45, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QSpacerItem* sp3 = new QSpacerItem(45, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QSpacerItem* sp4 = new QSpacerItem(45, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	gridLayout = new QGridLayout(GBInsComp);
	gridLayout->addWidget(L1, 0, 0);
	gridLayout->addWidget(LBatchNo, 0, 1);
    gridLayout->addItem(sp1, 0, 3);
	gridLayout->addWidget(L2, 0, 4);
	gridLayout->addWidget(LRecordsNo, 0, 5);
    gridLayout->addItem(sp2, 0, 6);
	gridLayout->addWidget(L3, 0, 7);
	gridLayout->addWidget(LPoints, 0, 8);
	gridLayout->addWidget(L4, 1, 0);
	gridLayout->addWidget(LServiceNo, 1, 1);
    gridLayout->addItem(sp3, 1, 2, 1, 2);
	gridLayout->addWidget(L5, 1, 4);
	gridLayout->addWidget(LRecordPoints, 1, 5);
    gridLayout->addItem(sp4, 1, 6, 1, 2);

	mainLayout= new QVBoxLayout(parent);
	mainLayout->addWidget(GBInsComp);
}

void BatchInfo::clearRecordInfo()
{
	LServiceNo->setText("0");
	LRecordPoints->setText("0");
}

void BatchInfo::updateInfo()
{
	int pocet_dokladu = -1;	
	//0 - pocet dokladu
	//1 - pocet bodu
	QStringList d = DB::getInstance()->getNumberOfRecordsPoints(m_main_window->batchId());
	if (!d.isEmpty()) {
		LRecordsNo->setText(d.at(0));
		pocet_dokladu = d.at(0).toInt();
		LPoints->setText(d.at(1));
	} else {
		qDebug() << "ERROR: updateBatchInfo(), davka nenalezena! ";
	}
	if (pocet_dokladu == 0) {
		LRecordPoints->setText("0");
		LServiceNo->setText("0");
	} else {
		//0 - body celkem
		//1 - vykony celkem
		QStringList x = DB::getInstance()->getNumberOfPointsServices(m_record_info->recordRowID());
		if (!x.isEmpty()) {
			LRecordPoints->setText(x.at(0));
			LServiceNo->setText(x.at(1));
		} else {
			qDebug() << "INFO: BatchInfo::updateInfo() doklad nenalezen -> nejspise smazan,protoze byl prazdny";
		}
	}
}

Record::Record(RecordInfo* parent)
{
	m_record_info = parent;
	m_line_number = 0;
	m_work_line_number = 0;
	m_last_code = "";
	m_points = "";
	m_main_diag = m_record_info->mainDiagnose();
//	m_last_date = m_record_info->date();

	setAttribute(Qt::WA_DeleteOnClose);
	m_last_date = MainWindow::date();
//	qDebug() << "datum ziskane z mainwindow je:" << m_last_date;
}

//TODO spravit layout..pri zmensovani se to zobrazuje spatne
void Record::setupUI(QWidget* parent)
{
	GBRecord = new QGroupBox(tr("Výkony"),parent);
	GBRecord->setObjectName("Vykony");
	QLabel* date_label = new QLabel(tr("datum"), GBRecord);
	date_label->setStyleSheet("color: yellow");
	QLabel* code_label = new QLabel(tr("kód"), GBRecord);
	code_label->setStyleSheet("color: yellow");
	QLabel* p_label = new QLabel(tr("p"), GBRecord);
	p_label->setStyleSheet("color: yellow");
	QLabel* odb_label = new QLabel(tr("odb"), GBRecord);
	odb_label->setStyleSheet("color: yellow");
	QLabel* diag_label = new QLabel(tr("diag"), GBRecord);
	diag_label->setStyleSheet("color: yellow");
	mainLayout= new QGridLayout(GBRecord);
	mainLayout->addWidget(date_label, m_line_number, 1);
	mainLayout->addWidget(code_label, m_line_number, 2);
	mainLayout->addWidget(p_label, m_line_number, 3);
	mainLayout->addWidget(odb_label, m_line_number, 4);
	mainLayout->addWidget(diag_label, m_line_number, 5);
	mainLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), m_line_number, 6);

	QVBoxLayout* widget_layout = new QVBoxLayout(parent);
	widget_layout->addWidget(GBRecord);
	widget_layout->setContentsMargins(0, 0, 0, 0);
	

	QShortcut* shortcut = new QShortcut(QKeySequence(tr("Esc", "Close")), parent);
	connect(shortcut, SIGNAL(activated()), parent, SLOT(closeRecord()));
	
	QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::Key_PageUp), this);
	connect(shortcut2, SIGNAL(activated()), this, SLOT(pgUp()));

	QShortcut* shortcut3 = new QShortcut(QKeySequence(Qt::Key_PageDown), this);
	connect(shortcut3, SIGNAL(activated()), this, SLOT(pgDown()));
	
	QShortcut* shortcut4 = new QShortcut(QKeySequence(tr("Alt+D")), this);
	connect(shortcut4, SIGNAL(activated()),this , SLOT(altD()));

	QShortcut* shortcut5 = new QShortcut(QKeySequence(Qt::Key_F4), parent);
	connect(shortcut5, SIGNAL(activated()), m_record_info, SLOT(f4()));

}

LineEdit* Record::firstItem()
{
	return m_list.last()->date();
}

LineEdit* Record::lastItem()
{
	return m_list.first()->date();
}

void Record::closeRecord()
{
	int vykony_celkem = DB::getInstance()->getNumberOfServices(m_record_info->recordRowID());

	if (vykony_celkem == -1){
		qDebug() << "ERROR: Nelze nalezt vykony_celkem v dokladech, nemuzu smazat doklad,bez vykonu";
	} else if (vykony_celkem == 0) {
		qDebug() << "INFO: V dokladu neni ulozen zadny vykon -> smazu";
		if (!DB::getInstance()->deleteRecord(m_record_info->recordRowID())) {
			qDebug() << "ERROR: doklad se nepodarilo smazat";
		}
		emit recordDeleted();
		close(); //bez emit ?
	} else {
		emit done();
		close();
	}
}

void Record::createNewLine(QDate last_date, QString last_code)
{
	if (last_date.isValid()) {
		m_last_date = last_date;
		m_last_code = last_code;
	}

	m_line_number++;
	m_work_line_number = m_line_number;	
	
	RecordLine* line = new RecordLine(this);
	line->setupUI();

	connect(line, SIGNAL(newLine(QDate, QString)), this, SLOT(createNewLine(QDate, QString)));
	connect(line, SIGNAL(focusDown()), this, SLOT(pgDown()));
	connect(line, SIGNAL(lineUpdated(QDate, QString)), this, SLOT(lineUpdated(QDate, QString)));
	connect(line, SIGNAL(lineDeleted()), this, SIGNAL(updateBatchInfo()));

	connect(line, SIGNAL(playError()), m_record_info->getMainWindow(), SLOT(playError()));
	connect(line, SIGNAL(playOk()), m_record_info->getMainWindow(), SLOT(playOk()));

	m_list << line;
	line->date()->setFocus();
	emit updateBatchInfo();
}

void Record::createNewLine(QDate date, QString code, QString p, QString odb, QString diag, qint64 line_id)
{

	m_last_date = date;
	m_last_code = code;
	m_line_number++;
	m_work_line_number = m_line_number;	

	RecordLine* line = new RecordLine(this,date,code,p,odb,diag,line_id);
	line->setupUI();
	line->setValues();
	
	connect(line, SIGNAL(newLine(QDate, QString)), this, SLOT(createNewLine(QDate, QString)));
	connect(line, SIGNAL(lineUpdated(QDate, QString)), this, SLOT(lineUpdated(QDate, QString)));
	connect(line, SIGNAL(lineDeleted()), this, SIGNAL(updateBatchInfo()));
	connect(line, SIGNAL(focusDown()), this, SLOT(pgDown()));

	connect(line, SIGNAL(playError()), m_record_info->getMainWindow(), SLOT(playError()));
	connect(line, SIGNAL(playOk()), m_record_info->getMainWindow(), SLOT(playOk()));
	
	m_list << line;


}

void Record::lineUpdated(QDate last_date, QString last_code)
{
	m_last_date = last_date;
	m_last_code = last_code;
	emit updateBatchInfo();
}

void Record::pgUp()
{
	if (m_work_line_number > 1) {
		m_work_line_number--;
		m_list.at(m_work_line_number - 1)->date()->setFocus();
	}
	else {
		m_list.at(m_work_line_number - 1)->date()->setFocus();
	}
}

void Record::pgDown()
{
	if (m_work_line_number < m_line_number) {
		m_work_line_number++;
		m_list.at(m_work_line_number - 1)->date()->setFocus();
	}
	else {
		m_list.at(m_work_line_number - 1)->date()->setFocus();
	}
}

void Record::altD()
{

	if (m_line_number == m_work_line_number) {//jsem na poslednim radku, akorat ho vymazu
		m_list.at(m_work_line_number - 1)->clearLine();
		m_list.at(m_work_line_number - 1)->date()->setFocus();
	} else { // jsem vys,musim to smazat a prepocitat
		for (int i = 0; i < m_list.count(); i++) {//odstranim radky z layoutu
			m_list.at(i)->removeFromLayout();
		}
		m_list.at(m_work_line_number - 1)->deleteFromDatabase(); 
		delete m_list.at(m_work_line_number - 1); 
		m_list.removeAt(m_work_line_number - 1);

		m_line_number--;
		if (m_work_line_number != 1) {
			m_work_line_number--;
		}
		
		m_list.at(m_work_line_number- 1)->date()->setFocus();
		m_list.at(m_work_line_number- 1)->date()->home(false);

		for (int i = 0; i < m_list.count(); i++){//precisluju cisla radku a vlozim zpet do layoutu
			m_list.at(i)->setLineNumberLabel(i + 1);
			m_list.at(i)->addToLayout(i + 1);
		}
	}
}

void Record::refreshDiagnoses()
{
	m_main_diag = m_record_info->mainDiagnose(); 

    for (int i = 0; i < m_list.count(); i++) {
        m_list.at(i)->refreshDiagnose();
	}

}


RecordLine::RecordLine(Record* parent)
{
	m_record = parent;
	update = false;
	m_date = QDate();
	m_code = "";
	m_p = "";
	m_odb = "";
	m_diag = "";
}

RecordLine::RecordLine(Record* parent, QDate date, QString code, QString p, QString odb, QString diag, qint64 line_id)
{
	m_record = parent;
	update = false;
	m_date = date;
	m_code = code;
	m_p = p;
	m_odb = odb;
	m_diag = diag;
	m_line_id = line_id;
}

RecordLine::~RecordLine()
{
	
	m_line_number->close();
	m_le_date->close();
	m_le_code->close();
	m_le_p->close();
	m_le_odb->close();
	m_le_diag->close();
	delete m_sp;

}

void RecordLine::setupUI()
{
	m_line_number = new QLabel(QString("%1").arg(m_record->lineNumber()));
	m_line_number->setAttribute(Qt::WA_DeleteOnClose);

	m_le_date = new DateLineEdit(m_record->lastDate());
	connect(m_le_date, SIGNAL(sendData(QString)), this, SLOT(checkDate(QString))); 
	
	m_le_code = new CodeLineEdit(m_record->lastCode());
	connect(m_le_code, SIGNAL(sendData(QString)), this, SLOT(checkCode(QString))); 
	
	m_le_p = new PLineEdit();
	connect(m_le_p, SIGNAL(sendData(QString)), this, SLOT(checkP(QString))); 
  connect(m_le_p, SIGNAL(plusShortcut()), this, SLOT(plusShortcut()));

	m_le_odb = new OdbLineEdit();
	connect(m_le_odb, SIGNAL(sendData(QString)), this, SLOT(checkOdb(QString))); 
  connect(m_le_odb, SIGNAL(plusShortcut()), this, SLOT(plusShortcut()));

	m_le_diag = new DiagnosisLineEdit(m_record->mainDiag());
  connect(m_le_diag, SIGNAL(sendData(QString)), this, SLOT(checkDiag(QString)));
  connect(m_le_diag, SIGNAL(plusShortcut()), this, SLOT(plusShortcut()));

	m_sp = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	addToLayout(m_record->lineNumber());
	
	m_le_date->setNeighbours(0, m_le_code);
	m_le_code->setNeighbours(m_le_date, m_le_p);
	m_le_p->setNeighbours(m_le_code, m_le_odb);
	m_le_odb->setNeighbours(m_le_p, m_le_diag);
	m_le_diag->setNeighbours(m_le_odb, 0);
}

void RecordLine::plusShortcut()
{
    if (m_le_p->hasFocus()) { //focus ma pocet vykonu
        m_diag = m_record->mainDiag();
        m_p = "1";
        saveLine(m_date, m_code,m_p, m_odb, m_diag, m_points);
        m_record->closeRecord();

    } else if (m_le_odb->hasFocus()) {
        m_diag = m_record->mainDiag();
        saveLine(m_date, m_code,m_p, m_odb, m_diag, m_points);
        m_record->closeRecord();
    } else if (m_le_diag->hasFocus()) {
        if (m_le_diag->text().isEmpty()) {
            m_diag = m_record->mainDiag();
            saveLine(m_date, m_code,m_p, m_odb, m_diag, m_points);
            m_record->closeRecord();
        } else {
            if (checkDiag(m_le_diag->text())) {
                m_record->closeRecord();
            }
        }

    }
}

void RecordLine::setValues()
{
	m_le_date->setValue(m_date.toString("dd.MM.yy"));
	m_le_code->setValue(m_code);
	m_le_p->setValue(m_p);
	m_le_odb->setValue(m_odb);
	m_le_diag->setValue(m_diag);
}

void RecordLine::removeFromLayout()
{
	m_record->mainLayout->removeWidget(m_line_number);
	m_record->mainLayout->removeWidget(m_le_date);
	m_record->mainLayout->removeWidget(m_le_code);
	m_record->mainLayout->removeWidget(m_le_p);
	m_record->mainLayout->removeWidget(m_le_odb);
	m_record->mainLayout->removeWidget(m_le_diag);
	m_record->mainLayout->removeItem(m_sp);
}

void RecordLine::addToLayout(int line_number)
{
	m_record->mainLayout->addWidget(m_line_number, line_number, 0);
	m_record->mainLayout->addWidget(m_le_date, line_number, 1);
	m_record->mainLayout->addWidget(m_le_code, line_number, 2);
	m_record->mainLayout->addWidget(m_le_p, line_number, 3);
	m_record->mainLayout->addWidget(m_le_odb, line_number, 4);
	m_record->mainLayout->addWidget(m_le_diag, line_number, 5);
	m_record->mainLayout->addItem(m_sp, line_number, 6);	
}

void RecordLine::checkDate(QString value)
{

	QDate check_date = QDate::fromString(value, "dd.MM.yy");
	if (!check_date.isValid()) { //kontrola,zda se podarilo datum zparsovat
		Message* msg = new Message();
		msg->setupUI(msg, Message::wrongDate);
		emit playError();
		msg->exec();
		m_le_date->clear();
		m_le_date->home(false);
		return;
	}

	QDate main_date = MainWindow::date();
	int main_day,check_day,main_month,check_month,main_year,check_year;

	main_day = main_date.day();
	main_month = main_date.month();
	main_year = main_date.year();

	check_day = check_date.day();
	check_month = check_date.month();
	check_year = check_date.year();

	if(main_year >= 2000){ //kontrola abych neporovnaval rok 2000 s rokem 1900
		check_year += 100;
	}
	if(check_date.setDate(check_year, check_month, check_day)){
		if(check_date > main_date){//datum nemuze byt budouci
			Message* msg = new Message();
			msg->setupUI(msg, Message::laterDate);
			emit playError();
			msg->exec();
			m_le_date->clear();
			m_le_date->home(false);
		}
		else{
			qDebug() << "INFO: Datum OK";
			if (m_date.isNull()) { // datum se vklada prvne,neni upravovano
				m_date = check_date; // ulozim si datum,abych ho pak mohl porovnavat pokud se dodatecne zmeni
			} else {
				if (check_date != m_date){ //datum bylo upraveno
					update = true; //radek se bude upravovat v databazi
					m_date = check_date; //ulozim si datum
				} 
			}
			m_le_date->focusNext();
		}
	}
	else{
		Message* msg = new Message();
		msg->setupUI(msg, Message::wrongDate);
		emit playError();
		msg->exec();
		m_le_date->clear();
		m_le_date->home(false);
	}
}

void RecordLine::refreshDiagnose()
{
    m_le_diag->setText(DB::getInstance()->getDiagnose(m_line_id));
	  m_le_diag->setMainDiagnose(m_record->mainDiag());
}


void RecordLine::checkCode(QString code)
{
	int points = DB::getInstance()->isValidCode(code);
	
	if (points != -1){
		if (m_code.isEmpty()) {
			m_code = code;
		} else {
			if (code != m_code) {
				update = true;
				m_code = code;
			}
		}
		m_points = points;
		m_le_code->focusNext();
    }
	else{
		Message* msg = new Message();
		msg->setupUI(msg, Message::wrongCode);
		emit playError();
		msg->exec();
		m_le_code->clear();
		m_le_code->home(false);

	}
}

void RecordLine::checkP(QString value)
{
	bool ok;
	int p = value.toInt(&ok, 10);
	if(ok && p > 0){
		if (m_p.isEmpty()) {
			m_p = value;
		} else {
			if (value != m_p) {
				update = true;
				m_p = value;
			}
		}
		m_le_p->focusNext();
	}
	else{
		Message* msg = new Message();
		msg->setupUI(msg, Message::wrongP);
		emit playError();
		msg->exec();
		m_le_p->clear();
		m_le_p->home(false);
	}

}

//TODO kontrola podle metodiky
void RecordLine::checkOdb(QString value)
{
	m_le_odb->focusNext();
}

//TODO pokud se nemeni diagnoza,tak by nemusel byt select do databaze,uz je zkontrolovana
//TODO pridat kontrolu diagnozy podle metodiky!!!
bool RecordLine::checkDiag(QString value)
{
	if (value == m_record->mainDiag()) { // diagnoza,je stejnak,jako hlavni diagnoza,nemusim tedy kontrolovat z ciselnku
		if (m_diag.isEmpty()) { //prvni zadani
			m_diag = value;
			saveLine(m_date, m_code,m_p, m_odb, m_diag, m_points);
		} else { //diagnoza uz byla zadana..tedy radek byl upraven,nebo jenom budu posunovat
			if (update || value != m_diag) { //nejake hodnoty se zmenily,posilam update radku
				m_diag = value;
				updateLine(m_date, m_code,m_p, m_odb, m_diag, m_points);
			} else { //nic se nezmenilo,jenom se chci posunout na dalsi radek
				emit focusDown();
			}
		}
	}
	else {
		DiagnoseStatus status = DB::getInstance()->isValidDiagnose(value);
		if (status == DIAGNOSE_OK) {
			if (m_diag.isEmpty()) { //prvni zadani
				m_diag = value;
				saveLine(m_date, m_code,m_p, m_odb, m_diag, m_points);
			} else { //diagnoza uz byla zadana..tedy radek byl upraven,nebo jenom budu posunovat
				if (update || value != m_diag) { //nejake hodnoty se zmenily,posilam update radku
					m_diag = value;
					updateLine(m_date, m_code,m_p, m_odb, m_diag, m_points);
				} else { //nic se nezmenilo,jenom se chci posunout na dalsi radek
					emit focusDown();
				}
			}		
		} else if (status == DIAGNOSE_BAD) {
			qDebug() << "diagnoza nenalezena";
			Message* msg = new Message();
			msg->setupUI(msg, Message::wrongDiag);
			emit playError();
			msg->exec();
			m_le_diag->clear();
			m_le_diag->home(false);
			return false;
		} else if (status == DIAGNOSE_STAR) {
			Message* msg = new Message();
			msg->setupUI(msg, Message::starDiag);
			emit playError();
			msg->exec();
			m_le_diag->clear();
			m_le_diag->home(false);
			return false;
		}
	}
        return true;
}

void RecordLine::saveLine(QDate date, QString code, QString p, QString odb, QString diag, int points)
{
	if (odb.isEmpty()) {
		odb = "   ";
	}
	points *= p.toInt();
	CrateRecordInfoData d;
	d.datum= date;
	d.kod = code;
	d.pocet = p;
	d.odb = odb;
	d.diag = diag;
	d.body = points;

	qint64 x = DB::getInstance()->saveRecord(m_record->recordId(), d);
	if (x == -1) {
		qDebug() << "ERROR: nepodarilo se ulozit doklad";
	} else {
		m_line_id = x;
		emit newLine(date, code);
		emit playOk();
		
	}
}

void RecordLine::updateLine(QDate date, QString code, QString p, QString odb, QString diag, int points)
{
	if (odb.isEmpty()) {
		odb = "   ";
	}

	points *= p.toInt();
	CrateRecordInfoData d;
	d.datum= date;
	d.kod = code;
	d.pocet = p;
	d.odb = odb;
	d.diag = diag;
	d.body = points;
	
	if (!DB::getInstance()->updateRecord(m_record->recordId(), m_line_id, d)) {
		qDebug() << "ERROR: nepodarilo se upravit doklad";
	}
	
	emit lineUpdated(date, code);
	emit focusDown();
	emit playOk();
}

void RecordLine::deleteFromDatabase()
{
	if (!DB::getInstance()->deleteService(m_line_id)) {
		qDebug() << "ERROR: nepodarilo se smazat vykon";
	} else {
		emit lineDeleted();
	}
}

void RecordLine::clearLine()
{
	m_le_date->clear();
	m_le_code->clear();
	m_le_p->clear();
	m_le_odb->clear();
	m_le_diag->clear();
}
