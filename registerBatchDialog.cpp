#include "registerBatchDialog.h"


RegisterBatchDialog::RegisterBatchDialog(MainWindow* parent) 
{
    m_main_window = parent;
    setupUI(); 
}

void RegisterBatchDialog::setupUI()
{
	setWindowTitle(tr("Vytvoření registračních dávek"));
	resize(350,200);

    QLabel* l = new QLabel(QString::fromUtf8("Počet nezaregistrovaných pacientů v databázi."));
   	m_month = new QComboBox();
	m_month->addItem(tr("leden")); 
	m_month->addItem(tr("únor")); 
	m_month->addItem(tr("březen")); 
	m_month->addItem(tr("duben")); 
	m_month->addItem(tr("květen")); 
	m_month->addItem(tr("červen")); 
	m_month->addItem(tr("červenec")); 
	m_month->addItem(tr("srpen")); 
	m_month->addItem(tr("září")); 
	m_month->addItem(tr("říjen")); 
	m_month->addItem(tr("listopad")); 
	m_month->addItem(tr("prosinec"));

    m_month->setCurrentIndex(QDate::currentDate().month()-1);
 
    m_year = new QComboBox();
    m_year->addItem(QString("%1").arg(QDate::currentDate().year()));
    m_year->addItem(QString("%1").arg(QDate::currentDate().year()-1));

    QLabel* l1 = new QLabel(tr("Pojišťovna"));
    QLabel* l2 = new QLabel(tr("Registrovaní pacienti"));

	QHBoxLayout* horizontalLayout2 = new QHBoxLayout();
    horizontalLayout2->addWidget(l1);
    horizontalLayout2->addStretch();
    horizontalLayout2->addWidget(l2);


	QHBoxLayout* horizontalLayout3 = new QHBoxLayout();
    QLabel* l3 = new QLabel(tr("Vytvořit registrační dávky za období"));
    m_create_button = new QPushButton(tr("Vytvoř dávky"));
    connect(m_create_button, SIGNAL(clicked()), this, SLOT(createRegisterBatch()));
    horizontalLayout3->addWidget(l3);
	horizontalLayout3->addWidget(m_month);
	horizontalLayout3->addWidget(m_year);
    horizontalLayout3->addStretch();
    horizontalLayout3->addWidget(m_create_button);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->addWidget(l);
    m_mainLayout->addLayout(horizontalLayout2);
    m_mainLayout->addStretch();
    m_mainLayout->addLayout(horizontalLayout3);

    listInsPacients();
}

void RegisterBatchDialog::listInsPacients()
{
    m_map = DB::getInstance()->numberUnregPacients();
    QMap<int, int>::const_iterator i = m_map.constBegin();
    while (i != m_map.constEnd()) {
        QLabel* l = new QLabel(QString("%1").arg(i.key()));
        QLabel* l1 = new QLabel(QString("%1").arg(i.value()));
        QHBoxLayout* h = new QHBoxLayout();
        h->addWidget(l);
        h->addStretch();
        h->addWidget(l1);
        m_mainLayout->insertLayout(2, h);
        ++i;
    }
}

void RegisterBatchDialog::createRegisterBatch()
{

    QString dir = QFileDialog::getExistingDirectory(this, tr("Vyberte adresář pro uložení registračních dávek"));
    QMap<int, int>::const_iterator i = m_map.constBegin();
    while (i != m_map.constEnd()) {
        QString filename = QString(dir + QString("/KDAVKA.%1").arg(i.key()));	
	    if (QFile::exists(filename)) {
            int ret = QMessageBox::critical(this, tr("Ukládání dávky"), tr("Dávka již existuje. Přepsat ?"),
	                                        QMessageBox::Ok | QMessageBox::Cancel);
            
            if (ret == QMessageBox::Ok) {
                QFile::remove(filename);
            } else {
                continue;
            }
        } 
        QFile file;
        file.setFileName(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, tr("Ukládání dávky"), tr("Nepodařilo se vytvořit soubor"), QMessageBox::Ok);
            break;
        } else {
	        QTextStream output(&file);
            QTextCodec* codec = new QIBM852Codec();
	        output.setCodec(codec);
            QList<CratePacient> list = DB::getInstance()->getUnregisteredPacients(QString("%1").arg(i.key()));
            QString cislo_davky = DB::getInstance()->getBatchID(); 
            QString pruvodni_list = createPruvodniList(cislo_davky);  
            output << pruvodni_list << "\r\n";
        
            QDate d = QDate(m_year->currentText().toInt(), m_month->currentIndex()+1, 1);
            QString zahlavi = createZahlavi(QString("%1").arg(i.key()), m_main_window->icz(), DB::getInstance()->getRecordID(), 
                                                d.toString("yyyy"), d.toString("MM"), "603");
            output << zahlavi << "\r\n";

            for (int i = 0; i < list.size(); ++i) {
                QString date = list.at(i).vlozeno;
                date.remove(QChar('.')); 
                output << createRegistrace(QString("%1").arg(i+1), list.at(i).prijmeni, list.at(i).jmeno, list.at(i).rc, date) << "\r\n";                    
            }         
            DB::getInstance()->saveUnregisterPacient(list);
        }
        ++i;
    } 
    accept();
}


QString RegisterBatchDialog::createPruvodniList(QString cislo_davky)
{
//Název  |Typ|Délka|Začátek|Popis
//-------|---|-----|-------|--------------------------------------------
//TYP    | C |  1  |   0   |Typ věty „D“ – úvodní věta dávky
//----------------------------------------------------------------------
//CHAR   | C |  1  |   1   |Charakter dávky - P 
//----------------------------------------------------------------------
//DTYP   | C |  2  |   2   |Typ dávky - 80
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
//DVDR1  | C |  13 |   61  |Verze datového rozhraní 1. V případě smíšené dávky se vyplní verze
//       |   |     |       |DR výkonového dokladu (01, 01s, 02, 02s nebo 06).
//------------------------------------------------------------------------------------------
//DVDR2  | C |  13 |   74  |Verze datového rozhraní 2. V případě jednoduché dávky se nevyplňuje.
//       |   |     |       |V případě smíšené dávky se vyplní verze DR pro doklad 03 nebo 03s.
//------------------------------------------------------------------------------------------
//DDTYP  | C |  1  |   87  |Doplněk typu věty záhlaví. Pro dávku 10 obsahující e_recepty se povinně
//       |   |     |       |vyplní hodnota "E". V ostatních případech se nevyplňuje.
////------------------------------------------------------------------------------------------
//Celkova delka vety 88

    QDate d = QDate(m_year->currentText().toInt(), m_month->currentIndex()+1, 1);

	QString TYP  = "D";
	QString CHAR = "P";
	QString DTYP = "80";
	QString DICO = m_main_window->icz();
	QString DPOB = m_main_window->pobocka();
	QString DROK = d.toString("yyyy");
	QString DMES = d.toString("MM");

	QString DCID = cislo_davky.rightJustified(6,' ');
	QString DPOC = "  1";
	QString DBODY = "          0";
	QString	DFIN = "              0.00";
	QString DDPP = "1";
    QString DVDR1 = "80:6.2.31    ";
    QString DVDR2 = "             ";
	QString DDTYP = " ";

//	QString command = QString("UPDATE seznam_davek SET rok = '%1', mesic = '%2' WHERE id_davky='%3'").arg(DROK,DMES,m_batch_info.id_davky);
//	qDebug() << "SQLCOMMAND " << command;
//	QSqlQuery query;
//	query.exec(command);	
//	if (query.lastError().isValid()) {
//  		qDebug() << query.lastError();
//	}


	QString pruvodni_list = TYP + CHAR + DTYP + DICO + DPOB + DROK + DMES + DCID + DPOC + DBODY + DFIN + DDPP + DVDR1 + DVDR2 + DDTYP;
	qDebug() << "uvodni veta davky = " << pruvodni_list;
	qDebug() << "delka vety = " << pruvodni_list.size();
	return pruvodni_list;

}

QString RegisterBatchDialog::createZahlavi(QString pojistovna, QString icp, QString cislo_dokladu, QString rok, QString mesic, QString odb)
{
// Záhlaví
//Název  |Typ|Délka|Začátek|Popis
//-------|---|-----|-------|--------------------------------------------
//TYP    | C |  1  |   0   | Typ věty „H“ – záhlaví
//----------------------------------------------------------------------
//HCPO   | C |  3  |   1   | Kód pojišťovny
//----------------------------------------------------------------------
//HICP   | C |  8  |   4   | Přidělené identifikační číslo pracoviště (IČP) lékaře, který provedl registraci
//----------------------------------------------------------------------
//HCID   | N |  7  |  12   | Jednoznačné číslo dokladu v rámci PZS a roku
//----------------------------------------------------------------------
//HROK   | N |  4  |  19   | Rok
//----------------------------------------------------------------------
//HMES   | N |  2  |  23   | Měsíc, ke kterému je registrace podávána
//----------------------------------------------------------------------
//HODB   | C |  3  |  25   | Smluvní odbornost pracoviště registrujícího lékaře
//----------------------------------------------------------------------
//DTYP   | C |  1  |  28   | Doplněk typu věty záhlaví. Rezerva, nevyplňuje se.
//----------------------------------------------------------------------
//Délka věty: 29
    
    QString TYP   = "H";
    QString HCPO  = pojistovna;
    QString HICP  = icp;
    QString HCID  = cislo_dokladu.rightJustified(7,' '); 
    QString HROK  = rok;
    QString HMES  = mesic;
    QString HODB  = odb;
    QString HDTYP = " ";

	QString zahlavi = TYP + HCPO + HICP + HCID + HROK + HMES + HODB + HDTYP;
	qDebug() << "veta zahlavi = " << zahlavi;
	qDebug() << "delka vety = " << zahlavi.size();
	return zahlavi;

}

QString RegisterBatchDialog::createRegistrace(QString c_radku, QString prijmeni, QString jmeno, QString rc, QString datum)
{
// Registrovaní pojištěnci
//Název  |Typ|Délka|Začátek|Popis
//-------|---|-----|-------|--------------------------------------------
//TYP    | C |  1  |   0   | Typ věty „I“ – registrace
//----------------------------------------------------------------------
//IPOR   | N |  4  |   1   | Pořadové číslo řádku
//----------------------------------------------------------------------
//IPRI   | C |  30 |   5   | Příjmení
//----------------------------------------------------------------------
//IJME   | C |  24 |   35  | Jméno
//----------------------------------------------------------------------
//ICIP   | C |  10 |   59  | Číslo pojištěnce
//----------------------------------------------------------------------
//IDAT   | D |  8  |   69  | Datum registrace u lékaře ve tvaru DDMMRRRR
//----------------------------------------------------------------------
//ITYP   | C |  1  |   77  | Doplněk typu věty registrace. Rezerva, nevyplňuje se.
//----------------------------------------------------------------------
//Délka věty: 78

    QString TYP  = "I";
    QString IPOR = c_radku.rightJustified(4,' ');
    QString IPRI = prijmeni.leftJustified(30,' ');
    QString IJME = jmeno.leftJustified(24,' ');
    QString ICIP = rc.leftJustified(10,' ');
    QString IDAT = datum;
    QString ITYP = " ";

	QString registrace = TYP + IPOR + IPRI + IJME + ICIP + IDAT + ITYP;
	qDebug() << "veta registrace = " << registrace;
	qDebug() << "delka vety = " << registrace.size();
	return registrace;
}

