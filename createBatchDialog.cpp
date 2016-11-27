#include "createBatchDialog.h"

CreateBatchDialog::CreateBatchDialog(CrateBatch bi) : m_batch_info(bi)
{
	setupUI();
}

void CreateBatchDialog::setupUI()
{
	setWindowTitle(tr("Vytvoření dávky"));
	resize(350,200);
	
	QLabel* L1 = new QLabel(tr("Charakter dávky"), this);
	QLabel* L2 =new QLabel(tr("Měsíc vytvoření dávky"), this);
	QLabel* L3 =new QLabel(tr("Rok vytvoření dávky"), this);
	QLabel* L4 = new QLabel(tr("Typ pojištění"), this);
	
	m_insurance_type = new QComboBox(this);
	m_insurance_type->addItem(tr("Veřejné zdravotní pojištění"));
	m_insurance_type->addItem(tr("Smluvní pojištění"));
	m_insurance_type->addItem(tr("Cestovní zdravotní připojištění"));
	m_insurance_type->addItem(tr("Pojištění EU a mezinárodní smlouvy"));


	m_batch_char = new QComboBox(this);
	m_batch_char->addItem(tr("Původní dávka (P)")); // doklady poprve predkladane
	m_batch_char->addItem(tr("Opravná dávka (O)")); // obsahuje opravene doklady

	m_de_month = new QDateEdit(QDate::currentDate());
	m_de_year = new QDateEdit(QDate::currentDate());

	m_de_month->setDisplayFormat("MM");
	m_de_year->setDisplayFormat("yyyy");
	
	m_createButton = new QPushButton(tr("Vytvoř dávku"),this);
	connect(m_createButton, SIGNAL(clicked()), this, SLOT(createBatch()));

	m_creation_progressBar = new QProgressBar(this);
	m_creation_progressBar->setValue(0);

	m_backup_checkbox = new QCheckBox(tr("Zkopírovat dávku do archívu"), this);
	m_backup_checkbox->setChecked(true);
	QFormLayout* formLayout = new QFormLayout();
	formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

	formLayout->setWidget(0, QFormLayout::LabelRole, L1);
	formLayout->setWidget(0, QFormLayout::FieldRole, m_batch_char);
	formLayout->setWidget(1, QFormLayout::LabelRole, L2);
	formLayout->setWidget(1, QFormLayout::FieldRole, m_de_month);
	formLayout->setWidget(2, QFormLayout::LabelRole, L3);
	formLayout->setWidget(2, QFormLayout::FieldRole, m_de_year);
	formLayout->setWidget(3, QFormLayout::LabelRole, L4);
	formLayout->setWidget(3, QFormLayout::FieldRole, m_insurance_type);

	QHBoxLayout* horizontalLayout = new QHBoxLayout();
	horizontalLayout->addWidget(m_creation_progressBar);
	horizontalLayout->addStretch();
	horizontalLayout->addWidget(m_createButton);

	m_main_layout = new QVBoxLayout(this);
	m_main_layout->addLayout(formLayout);
	m_main_layout->addWidget(m_backup_checkbox);
	m_main_layout->addLayout(horizontalLayout);

}

void CreateBatchDialog::createBatch()
{
	//batchinfo 
	//0 - create/select
	//1 - cislodavky
	//2 - pojistovna
	//3 - pocet dokladu
	//4 - pocet bodu
	//5 - typ
	//6 - id davky
	//7 - icz
	//8 - pobocka
	
	m_creation_progressBar->setMaximum(m_batch_info.pocet_dokladu.toInt());
	
	m_file_name = QString("KDAVKA.%1").arg(m_batch_info.pojistovna);
//	QFile file(filename);
	m_batch_file.setFileName(m_file_name);
	if (!m_batch_file.open(QIODevice::WriteOnly)){
		qDebug() << "Nepodarilo se vytvorit soubor kdavka !";
		return;
	}
	QTextStream output(&m_batch_file);
	output.setCodec("cp852");

	QString pruvodni_list_davky = createPruvodniListDavky();
	output << pruvodni_list_davky << "\r\n";

	QString command;	
	QSqlQuery query;

	command = QString("SELECT D.cislo_dokladu, D.icp, D.odbornost, D.var_symbol, D.rodne_cislo, D.zakl_diagnoza, D.body_celkem, "
			          "D.vykony_celkem, V.datum, V.kod, V.pocet, V.odbornost, V.diagnoza, V.body FROM doklady D, vykony V "
                                          "WHERE D.id_davky=%1 AND D.id_dokladu = V.id_dokladu ORDER BY D.cislo_dokladu ASC").arg(m_batch_info.id_davky);
	query.exec(command);
	if (query.lastError().isValid()) {
		      qDebug() << query.lastError();
	}

	QString cislo_dokladu;

	int poradove_cislo = 1;
	while (query.next()) {
		if (query.value(0).toString() != cislo_dokladu) {
			cislo_dokladu = query.value(0).toString();
			output << createVetaZahlavi(query.value(0).toString(), 
								 query.value(1).toString(),
								 query.value(2).toString(),
								 query.value(3).toString(),
								 query.value(4).toString(),
								 query.value(5).toString(),
								 query.value(6).toString(),
								 query.value(7).toString(),
								 poradove_cislo) << "\r\n";
			output << createVetaVykony(query.value(8).toString(),
					     query.value(9).toString(),
						 query.value(10).toString(),
						 query.value(11).toString(),
						 query.value(12).toString(),
						 query.value(13).toString()) << "\r\n";
			poradove_cislo++;
			m_creation_progressBar->setValue(poradove_cislo);
		} else {
			output << createVetaVykony(query.value(8).toString(),
					     query.value(9).toString(),
						 query.value(10).toString(),
						 query.value(11).toString(),
						 query.value(12).toString(),
						 query.value(13).toString()) << "\r\n";
		}
	}

	command = QString("UPDATE seznam_davek SET vytvoreni_davky='%1' WHERE id_davky='%2'").arg(QDate::currentDate().toString("ddMMyyyy"),m_batch_info.id_davky);
	qDebug() << "SQLCOMMAND " << command;
	query.exec(command);
	if (query.lastError().isValid()) {
	      qDebug() << query.lastError();
	}


	QLabel* LFinnished = new QLabel(tr("Dávka vytvořena"), this);
	m_main_layout->addWidget(LFinnished);

	m_batch_file.close();
	if (m_backup_checkbox->isChecked()) {
		QString new_filename = "archiv/" + m_batch_info.cislo_davky + "_" + m_batch_info.pojistovna + "_" + QDate::currentDate().toString("ddMMyyyy");
		if (QFile::exists(new_filename)) {
			qDebug() << "Dávka uz v archívu existuje";
		} else if (!m_batch_file.copy(new_filename)) {
	 		qDebug() << "Nepodarilo se zkopirovat davku do archivu";
	 	}
	}

	QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

	m_main_layout->addWidget(line);

	QHBoxLayout* bottomLayout = new QHBoxLayout();

	QLabel* Lsave = new QLabel(tr("Uložit na disketu/disk"));
	DiskNameLineEdit* disc_name = new DiskNameLineEdit();
	m_disc_name = disc_name->discName();
	connect(disc_name, SIGNAL(sendData(QString)), this, SLOT(saveDiscName(QString)));
	
	QPushButton* saveButton = new QPushButton(tr("Ulož"), this);
	connect(saveButton, SIGNAL(clicked()), this, SLOT(saveBatch()));
	QPushButton* saveAsButton = new QPushButton(tr("Ulož jinam"), this);
	connect(saveAsButton, SIGNAL(clicked()), this, SLOT(saveDialog()));

	bottomLayout->addWidget(Lsave);
	bottomLayout->addWidget(disc_name);
	bottomLayout->addWidget(saveButton);
	bottomLayout->addStretch();
	bottomLayout->addWidget(saveAsButton);

	m_main_layout->addLayout(bottomLayout);
	m_createButton->setEnabled(false);
	disc_name->setFocus();

}

void CreateBatchDialog::saveBatch()
{
	QString path = m_disc_name + ":/" + m_file_name;
	
	if (QFile::exists(path)) {
		int ret = QMessageBox::critical(this, tr("Kopírování dávky"),
	                                 tr("Dávka již existuje. Přepsat ?"),
	                                 QMessageBox::Ok | QMessageBox::Cancel);
		switch (ret) {
			case QMessageBox::Ok:
				if (QFile::remove(path)) {//uspesne vymazan
					if (!m_batch_file.copy(path)) {
						qDebug() << "nepodarilo se zkopirovat davku";
						QMessageBox::critical(this, tr("Kopírování dávky"),
	                                 tr("Dávku nelze zkopírovat do %1").arg(path),
	                                 QMessageBox::Ok);
					} else {
						QMessageBox msgBox;
						msgBox.setWindowTitle(tr("Uložení dávky"));
  						msgBox.setText(tr("Dávka uložena"));
   						if (msgBox.exec()) {
							accept();	
						}	

					}	
				} else {
					QMessageBox::critical(this, tr("Kopírování dávky"),
	                                 tr("Dávku se nepodařilo vymazat"),
	                                 QMessageBox::Ok);
	
				}
				break;
			case QMessageBox::Cancel:
				break;
			default:
				break;
		}
	
	} else {
		if (!m_batch_file.copy(path)) {
			qDebug() << "nepodarilo se zkopirovat davku";
			QMessageBox::critical(this, tr("Kopírování dávky"),
	                                 tr("Dávku nelze zkopírovat do %1").arg(path),
	                                 QMessageBox::Ok);
		} else {
			QMessageBox msgBox;
			msgBox.setWindowTitle(tr("Uložení dávky"));
  			msgBox.setText(tr("Dávka uložena"));
   			if (msgBox.exec()) {
				accept();	
			}	
		}
	}
}

void CreateBatchDialog::saveDialog()
{
	QString current_path = QDir::currentPath();
	current_path.append(tr("/%1").arg(m_file_name));
	QString fileName = QFileDialog::getSaveFileName(this, tr("Ulož dávku"),
			                            current_path);
	qDebug() << fileName;
	
	if (QFile::exists(fileName)) {
		if (QFile::remove(fileName)) {//uspesne vymazan
			qDebug() << "davka uspesne vymazana";
			if (!m_batch_file.copy(fileName)) {
				qDebug() << "nepodarilo se zkopirovat davku";
				QMessageBox::critical(this, tr("Kopírování dávky"),
	    	        tr("Dávku nelze zkopírovat do %1").arg(fileName),
	        	    QMessageBox::Ok);
			} else {
				QMessageBox msgBox;
				msgBox.setWindowTitle(tr("Uložení dávky"));
  				msgBox.setText(tr("Dávka uložena"));
   				if (msgBox.exec()) {
					accept();	
				}	
			}	
		} else {
			QMessageBox::critical(this, tr("Kopírování dávky"),
	                               tr("Dávku se nepodařilo vymazat"),
	                               QMessageBox::Ok);
		}
	} else {
		if (!m_batch_file.copy(fileName)) {
			qDebug() << "nepodarilo se zkopirovat davku";
			QMessageBox::critical(this, tr("Kopírování dávky"),
	                                 tr("Dávku nelze zkopírovat do %1").arg(fileName),
	                                 QMessageBox::Ok);
		} else {
 			QMessageBox msgBox;
			msgBox.setWindowTitle(tr("Uložení dávky"));
  			msgBox.setText(tr("Dávka uložena"));
   			if (msgBox.exec()) {
				accept();	
			}
		}
	}	
}

void CreateBatchDialog::saveDiscName(QString disc_name)
{
	m_disc_name = disc_name;
}

void CreateBatchDialog::accept()
{
//davka je zkopirovana, muzu smazat tu ve svem adresari
	if (!m_batch_file.remove()) {
		qDebug() << "davku v domovskem adresari se nepodarilo vymazat";
	}
	done(QDialog::Accepted);

}


QString CreateBatchDialog::createVetaZahlavi(QString cislo_dokladu, QString icp, QString odb,
		                                     QString var_symbol, QString rc, QString zdiag, QString pbodu,
											 QString , int pcislo)
{
//	Název |Typ|Délka|Začátek|Popis
//	------|---|-----|-------|-------------------------------------	
//	TYP   | C | 1   |   0   |Typ věty „A“ – záhlaví
//	------|---|-----|-------|-------------------------------------	
//	HCID  | N | 7   |   1   |Číslo dokladu
//	------|---|-----|-------|-------------------------------------	
//	HSTR  | N | 1   |   8   |Pořadové číslo listu dokladu - nevyplnuje se
//	------|---|-----|-------|-------------------------------------	
//	HPOC  | N | 1   |   9   |Celkový počet listů dokladu - nevyplnuje se
//	------|---|-----|-------|-------------------------------------	
//	HPOR  | N | 3   |   10  |Pořadové číslo listu v dávce. Povolený rozsah od 1 do 999
//	------|---|-----|-------|-------------------------------------	
//	HCPO  | C | 3   |   13  |Číslo pojišťovny
//	------|---|-----|-------|-------------------------------------	
//	HTPP  | C | 1   |   16  |Typ připojištění - Pro zákonné pojištění se vyplňuje kód 1
//	------|---|-----|-------|-------------------------------------	
//	HICO  | C | 8   |   17  |Identifikační číslo pracoviště (IČP)
//	------|---|-----|-------|-------------------------------------	
//	HVAR  | C | 6   |   25  |Variabilní symbol
//	------|---|-----|-------|-------------------------------------	
//	HODB  | C | 3   |   31  |Smluvní odbornost pracoviště
//	------|---|-----|-------|-------------------------------------	
//	HROD  | C | 10  |   34  |Číslo pojištěnce
//	------|---|-----|-------|-------------------------------------	
//	HZDG  | C | 5   |   44  |Číslo základní diagnózy, pro kterou byl pacient v tomto období léčen
//	------|---|-----|-------|-------------------------------------	
//	HKON  | C | 1   |   49  |Údaj se nevyplňuje
//	------|---|-----|-------|-------------------------------------	
//	HICZ  | C | 8   |   50  |IČP žadatele – nevyplňuje se
//	------|---|-----|-------|-------------------------------------	
//	HCDZ  | N | 7   |   58  |Číslo dokladu žadatele – nevyplňuje se
//	------|---|-----|-------|-------------------------------------	
//	HREZ  | C | 10  |   65  |Rezerva – nevyplňuje se
//	------|---|-----|-------|-------------------------------------	
//	HCCEL | $ | 10.2|   75  |Cena celkem – nepovinný údaj
//	------|---|-----|-------|-------------------------------------	
//	HCBOD | N | 7   |   85  |Body celkem – nepovinný údaj
//	------|---|-----|-------|-------------------------------------	
//	DTYP  | C | 1   |   92  |Doplněk typu věty záhlaví. Rezerva, nevyplňuje se.
//	--------------------------------------------------------------	
//  Celkem: délka věty 93
		
	QString TYP = "A";
	QString HCID = prependSpace(cislo_dokladu,7);
	QString HSTR = "0";
	QString HPOC = "0";
	QString HPOR = prependSpace(QString("%1").arg(pcislo), 3);
	QString HCPO = m_batch_info.pojistovna;
	QString HTPP = "1";
	QString HICO = icp;
	QString HVAR = prependSpace(var_symbol, 6);
	QString HODB = odb;
	QString HROD = appendSpace(rc, 10);
	QString HZDG = appendSpace(zdiag, 5);
	QString HKON = " ";
	QString HICZ = "        ";
	QString HCDZ = "      0";
	QString HREZ = "          ";
	QString HCCEL = "      0.00";
	QString HCBOD = prependSpace(pbodu,7);
	QString DTYP = " ";

	QString veta = TYP + HCID + HSTR + HPOC + HPOR + HCPO + HTPP + HICO + HVAR + HODB + HROD 
		           + HZDG + HKON + HICZ + HCDZ + HREZ + HCCEL + HCBOD + DTYP;

//	qDebug() << "veta zahlavi = " << veta;
//	qDebug() << "delka vety = " << veta.size();

	return veta;

}


QString CreateBatchDialog::createVetaVykony(QString datum, QString kod, QString pocet, QString odb, QString diag, QString body)
{
//
//	Název |Typ|Délka|Začátek|Popis
//	------|---|-----|-------|-------------------------------------	
//   TYP  | C | 1   |  0    |Typ věty „V“ – výkony 
//	------|---|-----|-------|-------------------------------------	
//	 VDAT | D | 8   |  1    |Datum provedení výkonu. Jeho uvedení je povinné u prvního výkonu v daném dnu.
//	------|---|-----|-------|-------------------------------------	
//	 VKOD | C | 5   |  9    |číslo výkonu
//	------|---|-----|-------|-------------------------------------	
//   VPOC | N | 1   |  14   |Počet provedení výkonu
//	------|---|-----|-------|-------------------------------------	
//	 VODB | C | 3   |  15   |Odbornost - uvádí se povinně v případě, kdy vykazovaný výkon byl proveden na pracovišti jiné
//	      |   |     |       |smluvní odbornosti (stejného SZZ), než je uvedena v záhlaví dokladu. Jinak může zůstat údaj nevypl-
//	      |   |     |       |něn. Platí, že na pracovišti, které požadovaný výkon provedlo, se v tomto případě poskytnuté služby neúčtují.
//	------|---|-----|-------|-------------------------------------	
//	 VDIA | C | 5   | 18    |Diagnóza – je-li uvedena základní diagnóza, uvádí se jen u výkonů, které se k základní diagnóze ne-
//	      |   |     |       |vztahují. Nevyplněná řádková diagnóza se považuje za diagnózu základní.
//	------|---|-----|-------|-------------------------------------	
//	 VBOD | N | 5   | 23    |Body za výkon – nepovinný údaj
//	------|---|-----|-------|-------------------------------------	
//	 VTYP | C | 1   | 28    |Doplněk typu věty výkony. Rezerva, nevyplňuje se.
//	------|---|-----|-------|-------------------------------------	
//	delka vety 29

	QString TYP = "V";
	QString VDAT = datum.replace(".", "");
	QString VKOD = kod;
	QString VPOC = pocet;
	QString VODB = odb;
	QString VDIA = appendSpace(diag, 5);
	QString VBOD = prependSpace(body, 5);
	QString VTYP = " ";

	QString veta = TYP + VDAT + VKOD + VPOC + VODB + VDIA + VBOD + VTYP;
//	qDebug() << "veta vykony = " << veta;
//	qDebug() << "delka vety = " << veta.size();

	return veta;
}


QString CreateBatchDialog::createPruvodniListDavky()
{
//Název  |Typ|Délka|Začátek|Popis
//-------|---|-----|-------|--------------------------------------------
//TYP    | C |  1  |   0   |Typ věty „D“ – úvodní věta dávky
//----------------------------------------------------------------------
//CHAR   | C |  1  |   1   |Charakter dávky – určuje stav předkládaných 
//       |   |     |       |dokladů v dávce
//----------------------------------------------------------------------
//DTYP   | C |  2  |   2   |Typ dávky (viz dále) – je určen druhem 
//       |   |     |       |dokladu, který dávka obsahuje, resp. přesně 
//       |   |     |       |definovanou posloupností druhů dokladů
//----------------------------------------------------------------------
//DICO   | C |  8  |   4   |Identifikační číslo smluvního zařízení (IČZ), které
//       |   |     |       |dávku předkládá. Nepovinné pro ZZP.
//----------------------------------------------------------------------
//DPOB   | C |  4  |   12   |Územní pracoviště VZP – kód územního praco-
//       |   |     |       |viště podle číselníku „Územní pracoviště VZP“,
//       |   |     |       |kterému je dávka předkládána
//----------------------------------------------------------------------
//DROK   | N |  4  |   16   |Rok, ve kterém byly předkládané doklady uzavřeny
//------------------------------------------------------------------------
//DMES   | N |  2  |   20  |Měsíc, ve kterém byly předkládané doklady uza-
//       |   |     |       |vřeny. Jsou povoleny pouze číslice „0“ až „9“, na
//       |   |     |       |první pozici „0“, nebo „1“.
//---------------------------------------------------------------------------
//DCID   | N |  6  |   22  |Číslo dávky – jednoznačné číslo dávky v rámci smluvního zařízení a roku
//----------------------------------------------------------------------------
//DPOC   | N |  3  |   28  |Počet dokladů v dávce – slouží ke kontrole kompletnosti dávky
//------------------------------------------------------------------------------
//DBODY  | N |  11 |   31  |Počet bodů za doklady v dávce (počet bodů za výkony, režii obsaženou ve výkonu, výkony agre-
//       |   |     |       |gované do OD, režii podle kategorie zařízení ústavní péče a kategorii pacienta). Nepovinné vyplnění.
//------------------------------------------------------------------------------
//DFIN   | $ | 18.2|   42  |Celkem Kč za doklady v dávce (Pmat, cena zvlášť účtovaných léčivých přípravků a ZP, ambulantní a
//       |   |     |       |hospitalizační paušál). Nepovinné vyplnění.
//--------------------------------------------------------------------------------
//DDPP   | C |  1  |   60  |Druh pojistného vztahu: 1 = veřejné zdravotní pojištění,
//       |   |     |       |                        2 = smluvní připojištění,
//       |   |     |       |                        3 = cestovní zdravotní připojištění,
//       |   |     |       |                        4 = pojištění EU a mezinárodní smlouvy.
//---------------------------------------------------------------------------------------       
//DVDR1  | C |  13 |   61  |Verze datového rozhraní 1. V případě smíšené dávky se vyplní verze
//       |   |     |       |DR výkonového dokladu (01, 01s, 02, 02s nebo 06).
//------------------------------------------------------------------------------------------
//DVDR2  | C |  13 |   74  |Verze datového rozhraní 2. V případě jednoduché dávky se nevyplňuje.
//       |   |     |       |V případě smíšené dávky se vyplní verze DR pro doklad 03 nebo 03s.
//------------------------------------------------------------------------------------------
//DDTYP  | C |  1  |   87  |Doplněk typu věty záhlaví. Pro dávku 10 obsahující e_recepty se povinně
//       |   |     |       |vyplní hodnota "E". V ostatních případech se nevyplňuje.
////------------------------------------------------------------------------------------------
//celkova delka vety 88

	QString TYP = "D";
	QString CHAR;
	if (m_batch_char->currentIndex() == 0) {//vybrana puvodni davka (P)
		CHAR = "P";
	} else if (m_batch_char->currentIndex() == 1) {// vybrana davka opravna
		CHAR = "O";
	}
	QString DTYP = m_batch_info.typ;
	QString DICO = m_batch_info.icz;
	QString DPOB = m_batch_info.pob;
	QString DROK = m_de_year->date().toString("yyyy");
	QString DMES = m_de_month->date().toString("MM");
	QString DCID = prependSpace(m_batch_info.cislo_davky, 6);
	QString DPOC = prependSpace(m_batch_info.pocet_dokladu, 3);
	QString DBODY = prependSpace(m_batch_info.pocet_bodu, 11);
	QString	DFIN = "              0.00";
	QString DDPP = getInsuranceType();
    QString DVDR1 = "01:6.2.31    ";
    QString DVDR2 = "03:6.2.31    ";
	QString DDTYP = " ";

	QString command = QString("UPDATE seznam_davek SET rok = '%1', mesic = '%2' WHERE id_davky='%3'").arg(DROK,DMES,m_batch_info.id_davky);
	qDebug() << "SQLCOMMAND " << command;
	QSqlQuery query;
	query.exec(command);	
	if (query.lastError().isValid()) {
  		qDebug() << query.lastError();
	}



	QString pruvodni_list = TYP + CHAR + DTYP + DICO + DPOB + DROK + DMES + DCID + DPOC + DBODY + DFIN + DDPP + DVDR1 + DVDR2 + DDTYP;
//	qDebug() << "uvodni veta davky = " << pruvodni_list;
//	qDebug() << "delka vety = " << pruvodni_list.size();
	return pruvodni_list;


}

QString CreateBatchDialog::prependSpace(QString str, int length)
{
	while (str.size() != length) {
		str.prepend(" ");
	}
	return str;
}

QString CreateBatchDialog::appendSpace(QString str, int length)
{
	while (str.size() != length) {
		str.append(" ");
	}
	return str;
}

QString CreateBatchDialog::getInsuranceType()
{
	QString type = QString("%1").arg(m_insurance_type->currentIndex() + 1);
	return type;
}



LoadBatch::LoadBatch()
{
}

bool LoadBatch::checkFileName(QString path)
{
 	QFileInfo fi(path);
 	QString file_name = fi.fileName();
	
	if(!file_name.startsWith("KDAVKA.")){
		return false;
	}
	m_poj = file_name.right(3);

    if (!DB::getInstance()->isInsuranceCompany(m_poj)) {
        return false;
    } else {
        return true;
    }
}

bool LoadBatch::loadBatch(QString filename)
{
	if (!checkFileName(filename)) {
		QMessageBox::critical(0, tr("Načítání dávky"),
					             tr("Soubor nemá předepsaný název (KDAVKA.xxx)"),
								 QMessageBox::Ok);

		return false;
	}
	
	m_batch_file.setFileName(filename);
	if (!m_batch_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return false;	
	}

//TODO nacist id_davky

	m_progress = new QProgressBar();
	m_progress->setValue(0);
	m_progress->show();

    DB::getInstance()->startTransaction();

    QTextStream f(&m_batch_file);
	while (!f.atEnd()) {
		QString line = f.readLine();
		if (!processLine(line)) {
            DB::getInstance()->rollbackTransaction();        
            return false;
        }

	}
	m_progress->close();
    DB::getInstance()->commitTransaction();
	return true;
}

bool LoadBatch::processLine(QString line)
{

	if (line.startsWith("D")) {
		if (!processPruvodniList(line)) { return false; }
	} else if (line.startsWith("A")) {
		if (!processDoklady(line)) { return false; }
	} else if (line.startsWith("V")) {
		if (!processVykony(line)) { return false; }
	} else {
		QMessageBox::critical(0, tr("Načítání dávky"),
					             tr("Nedokážu načíst dávku !"),
								 QMessageBox::Ok);
        
        return false;
    }

    return true;
}

bool LoadBatch::processPruvodniList(QString line)
{
//Název  |Typ|Délka|Začátek|Popis
//-------|---|-----|-------|--------------------------------------------
//TYP    | C |  1  |   0   |Typ věty „D“ – úvodní věta dávky
//----------------------------------------------------------------------
//CHAR   | C |  1  |   1   |Charakter dávky – určuje stav předkládaných 
//       |   |     |       |dokladů v dávce
//----------------------------------------------------------------------
//DTYP   | C |  2  |   2   |Typ dávky (viz dále) – je určen druhem 
//       |   |     |       |dokladu, který dávka obsahuje, resp. přesně 
//       |   |     |       |definovanou posloupností druhů dokladů
//----------------------------------------------------------------------
//DICO   | C |  8  |   4   |Identifikační číslo smluvního zařízení (IČZ), které
//       |   |     |       |dávku předkládá. Nepovinné pro ZZP.
//----------------------------------------------------------------------
//DPOB   | C |  4  |   12  |Územní pracoviště VZP – kód územního praco-
//       |   |     |       |viště podle číselníku „Územní pracoviště VZP“,
//       |   |     |       |kterému je dávka předkládána
//----------------------------------------------------------------------
//DROK   | N |  4  |   16   |Rok, ve kterém byly předkládané doklady uzavřeny
//------------------------------------------------------------------------
//DMES   | N |  2  |   20  |Měsíc, ve kterém byly předkládané doklady uza-
//       |   |     |       |vřeny. Jsou povoleny pouze číslice „0“ až „9“, na
//       |   |     |       |první pozici „0“, nebo „1“.
//---------------------------------------------------------------------------
//DCID   | N |  6  |   22  |Číslo dávky – jednoznačné číslo dávky v rámci smluvního zařízení a roku
//----------------------------------------------------------------------------
//DPOC   | N |  3  |   28  |Počet dokladů v dávce – slouží ke kontrole kompletnosti dávky
//------------------------------------------------------------------------------
//DBODY  | N |  11 |   31  |Počet bodů za doklady v dávce (počet bodů za výkony, režii obsaženou ve výkonu, výkony agre-
//       |   |     |       |gované do OD, režii podle kategorie zařízení ústavní péče a kategorii pacienta). Nepovinné vyplnění.
//------------------------------------------------------------------------------
//DFIN   | $ | 18.2|   42  |Celkem Kč za doklady v dávce (Pmat, cena zvlášť účtovaných léčivých přípravků a ZP, ambulantní a
//       |   |     |       |hospitalizační paušál). Nepovinné vyplnění.
//--------------------------------------------------------------------------------
//DDPP   | C |  1  |   60  |Druh pojistného vztahu: 1 = veřejné zdravotní pojištění,
//       |   |     |       |                        2 = smluvní připojištění,
//       |   |     |       |                        3 = cestovní zdravotní připojištění,
//       |   |     |       |                        4 = pojištění EU a mezinárodní smlouvy.
//---------------------------------------------------------------------------------------       
//DDTYP  | C |  1  |   61  |Doplněk typu věty záhlaví. Rezerva, nevyplňuje se.
//------------------------------------------------------------------------------------------
//celkova delka vety 62

	if (line.size() != 62) {
		return false;
	}

//TODO kontrola kazde hodnoty !!!!!!!!

	QString TYP =   line.left(1);   line.remove(0,1);
	QString CHAR =  line.left(1);   line.remove(0,1);
	QString DTYP =  line.left(2);	line.remove(0,2);
	QString DICO =  line.left(8);	line.remove(0,8);
	QString DPOB =  line.left(4);	line.remove(0,4);
	QString DROK =  line.left(4);	line.remove(0,4);
	QString DMES =  line.left(2);	line.remove(0,2);
	QString DCID =  line.left(6);	line.remove(0,6);   DCID = DCID.trimmed();
	QString DPOC =  line.left(3);	line.remove(0,3);	DPOC = DPOC.trimmed();
	QString DBODY = line.left(11);	line.remove(0,1);	DBODY.trimmed();
	QString	DFIN =  line.left(18);	line.remove(0,18);
	QString DDPP =  line.left(1);	line.remove(0,1);
	QString DDTYP = line.left(1);	line.remove(0,1);

    QList<CrateBatch> batch = DB::getInstance()->getBatchInfo(DCID, m_poj);

    if (!batch.isEmpty()) {
        //TODO dodelat pro vice davek
        if (batch.first().rok == DROK) {
			QMessageBox::critical(0, tr("Načítání dávky"),
					          tr("Dávka již v databázi existuje"),QMessageBox::Ok);
			return false;
        }
    }
	
    m_progress->setMaximum(DPOC.toInt());

    CrateBatch b;
    b.charakter = CHAR;
    b.typ = DTYP;
    b.icz = DICO;
    b.pob = DPOB;
    b.rok = DROK;
    b.mesic = DMES;
    b.cislo_davky = DCID;
    b.pojistovna = m_poj;
    b.pocet_dokladu = "0";
    b.pocet_bodu = "0";

    qint64 x;
    if ((x = DB::getInstance()->createNewBatchSignature(b)) == -1) {
        return false;
    } else {
        m_batch_id = QString("%1").arg(x);
        return true;
    }
}

bool LoadBatch::processDoklady(QString line)
{
//	Název |Typ|Délka|Začátek|Popis
//	------|---|-----|-------|-------------------------------------	
//	TYP   | C | 1   |   0   |Typ věty „A“ – záhlaví
//	------|---|-----|-------|-------------------------------------	
//	HCID  | N | 7   |   1   |Číslo dokladu
//	------|---|-----|-------|-------------------------------------	
//	HSTR  | N | 1   |   8   |Pořadové číslo listu dokladu - nevyplnuje se
//	------|---|-----|-------|-------------------------------------	
//	HPOC  | N | 1   |   9   |Celkový počet listů dokladu - nevyplnuje se
//	------|---|-----|-------|-------------------------------------	
//	HPOR  | N | 3   |   10  |Pořadové číslo listu v dávce. Povolený rozsah od 1 do 999
//	------|---|-----|-------|-------------------------------------	
//	HCPO  | C | 3   |   13  |Číslo pojišťovny
//	------|---|-----|-------|-------------------------------------	
//	HTPP  | C | 1   |   16  |Typ připojištění - Pro zákonné pojištění se vyplňuje kód 1
//	------|---|-----|-------|-------------------------------------	
//	HICO  | C | 8   |   17  |Identifikační číslo pracoviště (IČP)
//	------|---|-----|-------|-------------------------------------	
//	HVAR  | C | 6   |   25  |Variabilní symbol
//	------|---|-----|-------|-------------------------------------	
//	HODB  | C | 3   |   31  |Smluvní odbornost pracoviště
//	------|---|-----|-------|-------------------------------------	
//	HROD  | C | 10  |   34  |Číslo pojištěnce
//	------|---|-----|-------|-------------------------------------	
//	HZDG  | C | 5   |   44  |Číslo základní diagnózy, pro kterou byl pacient v tomto období léčen
//	------|---|-----|-------|-------------------------------------	
//	HKON  | C | 1   |   49  |Údaj se nevyplňuje
//	------|---|-----|-------|-------------------------------------	
//	HICZ  | C | 8   |   50  |IČP žadatele – nevyplňuje se
//	------|---|-----|-------|-------------------------------------	
//	HCDZ  | N | 7   |   58  |Číslo dokladu žadatele – nevyplňuje se
//	------|---|-----|-------|-------------------------------------	
//	HREZ  | C | 10  |   65  |Rezerva – nevyplňuje se
//	------|---|-----|-------|-------------------------------------	
//	HCCEL | $ | 10.2|   75  |Cena celkem – nepovinný údaj
//	------|---|-----|-------|-------------------------------------	
//	HCBOD | N | 7   |   85  |Body celkem – nepovinný údaj
//	------|---|-----|-------|-------------------------------------	
//	DTYP  | C | 1   |   92  |Doplněk typu věty záhlaví. Rezerva, nevyplňuje se.
//	--------------------------------------------------------------	
//  Celkem: délka věty 93

	if (line.size() != 93) {
		return false;
	}

	QString TYP =   line.left(1);   line.remove(0,1);
	QString HCID =  line.left(7);   line.remove(0,7);   HCID = HCID.trimmed();
	QString HSTR =  line.left(1);   line.remove(0,1);
	QString HPOC =  line.left(1);   line.remove(0,1);
	QString HPOR =  line.left(3);   line.remove(0,3); 
	QString HCPO =  line.left(3);   line.remove(0,3); 
	QString HTPP =  line.left(1);   line.remove(0,1);
	QString HICO =  line.left(8);   line.remove(0,8);   HICO = HICO.trimmed();
	QString HVAR =  line.left(6);   line.remove(0,6);
	QString HODB =  line.left(3);   line.remove(0,3);
	QString HROD =  line.left(10);  line.remove(0,10);
	QString HZDG =  line.left(5);   line.remove(0,5);   HZDG = HZDG.trimmed();
	QString HKON =  line.left(1);   line.remove(0,1);
	QString HICZ =  line.left(8);   line.remove(0,8);
	QString HCDZ =  line.left(7);   line.remove(0,7);
	QString HREZ =  line.left(10);  line.remove(0,10);
	QString HCCEL = line.left(10);  line.remove(0,10);
	QString HCBOD = line.left(7);   line.remove(0,7);
	QString DTYP =  line.left(1);   line.remove(0,1);
	
	if (HCPO != m_poj) {
		return false;
	}
	
	m_record_id = QString("%1").arg(HCID);
	m_record_id.prepend(m_batch_id);

    CrateRecordInfo d;
    d.id_davky = m_batch_id;
    d.cislo_dokladu = HCID;
    d.icp = HICO;
    d.odb = HODB;
    d.var_symbol = HVAR;
    d.rodne_cislo = HROD;
    d.zakl_diag = HZDG;
    d.body_celkem = "0";
    d.vykony_celkem = "0";

    qint64 x;
    if ((x = DB::getInstance()->saveRecordInfo(d)) == -1) {
        return false;
    } else {
	    m_record_id = QString("%1").arg(x);
    }

	int i = m_progress->value();
	i++;
	m_progress->setValue(i);

	return true;
}

bool LoadBatch::processVykony(QString line)
{
//	Název |Typ|Délka|Začátek|Popis
//	------|---|-----|-------|-------------------------------------	
//   TYP  | C | 1   |  0    |Typ věty „V“ – výkony 
//	------|---|-----|-------|-------------------------------------	
//	 VDAT | D | 8   |  1    |Datum provedení výkonu. Jeho uvedení je povinné u prvního výkonu v daném dnu.
//	------|---|-----|-------|-------------------------------------	
//	 VKOD | C | 5   |  9    |číslo výkonu
//	------|---|-----|-------|-------------------------------------	
//   VPOC | N | 1   |  14   |Počet provedení výkonu
//	------|---|-----|-------|-------------------------------------	
//	 VODB | C | 3   |  15   |Odbornost - uvádí se povinně v případě, kdy vykazovaný výkon byl proveden na pracovišti jiné
//	      |   |     |       |smluvní odbornosti (stejného SZZ), než je uvedena v záhlaví dokladu. Jinak může zůstat údaj nevypl-
//	      |   |     |       |něn. Platí, že na pracovišti, které požadovaný výkon provedlo, se v tomto případě poskytnuté služby neúčtují.
//	------|---|-----|-------|-------------------------------------	
//	 VDIA | C | 5   | 18    |Diagnóza – je-li uvedena základní diagnóza, uvádí se jen u výkonů, které se k základní diagnóze ne-
//	      |   |     |       |vztahují. Nevyplněná řádková diagnóza se považuje za diagnózu základní.
//	------|---|-----|-------|-------------------------------------	
//	 VBOD | N | 5   | 23    |Body za výkon – nepovinný údaj
//	------|---|-----|-------|-------------------------------------	
//	 VTYP | C | 1   | 28    |Doplněk typu věty výkony. Rezerva, nevyplňuje se.
//	------|---|-----|-------|-------------------------------------	
//	delka vety 29

	if (line.size() != 29) {
		return false;
	}

//TODO kontrola kazdeho udaje !!!!!!

    QString TYP = line.left(1); line.remove(0,1);
	QString VDAT = line.left(8); line.remove(0,8); 
	QString VKOD = line.left(5); line.remove(0,5);
	QString VPOC = line.left(1); line.remove(0,1);
	QString VODB = line.left(3); line.remove(0,3);
	QString VDIA = line.left(5); line.remove(0,5); VDIA = VDIA.trimmed();
	QString VBOD = line.left(5); line.remove(0,5); VBOD = VBOD.trimmed();
	QString VTYP = line.left(1); line.remove(0,1);
	
    CrateRecordInfoData d;
    d.datum = QDate::fromString(VDAT,"ddMMyyyy");
    d.kod = VKOD;
    d.pocet = VPOC;
    d.odb = VODB;
    d.diag = VDIA;
    d.body = VBOD.toInt();

    if (DB::getInstance()->saveRecord(m_record_id, d) == -1) {
        return false;
    }
    return true;
}
