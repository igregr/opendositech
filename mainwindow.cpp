#include <QtGui>
#include "mainwindow.h"
#include "xmlhandler.h"
#include "insertDialog.h"
#include "record.h"
#include "stylesheet.h"
#include <iostream>

extern QString stylesheet;


QDate MainWindow::d = QDate::currentDate();



MainWindow::MainWindow()
{
//TODO doresit podminku..jestli ukoncit nebo ne..Vymyslet jak to osefovat	
	if(!parseFile("config.xml")){
		QMessageBox::critical(this, tr("Chyba!"), tr("Nepodařilo se zpracovat soubor config.xml"), QMessageBox::Close );
	}
	
	resize(800, 600);
	setMinimumSize (700, 450);
	centralWidget = new QWidget();
	setCentralWidget(centralWidget);
	centralWLayout = new QGridLayout(centralWidget);
	createActions();
	createMenu();
	createStatusBar();
	m_num_records = 0;	
	m_num_points = 0;	

    m_error = new QSound("sounds/err.wav");
    m_ok = new QSound("sounds/done.wav");
	
        //m_ok = Phonon::createPlayer(Phonon::NotificationCategory, Phonon::MediaSource("sounds/done.wav"));
	m_viewBatchDialog = 0;
	d = QDate::currentDate();
}

MainWindow::~MainWindow()
{
	qDebug() << "~mainwindow";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	DB::getInstance()->closeDatabase();
	event->accept();
}

void MainWindow::playError()
{
        m_error->play();
}

void MainWindow::playOk()
{
        m_ok->play();
}

void MainWindow::createActions()
{
    insertDataAction = new QAction(tr("&Vkládání dat"), this);
	connect(insertDataAction, SIGNAL(triggered()), this, SLOT(insertDataForm()));

	changeDateAction = new QAction(tr("&Změna data"), this);
	connect(changeDateAction, SIGNAL(triggered()), this, SLOT(changeDateDialog()));
	
	settingsAction = new QAction(tr("&Nastavení"), this);
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(settingsDialog()));
	
	m_action_create_batch = new QAction(tr("Vytvoření dávky"), this);	
	connect(m_action_create_batch, SIGNAL(triggered()), this, SLOT(createBatchDialog()));

	m_action_load_batch = new QAction(tr("Načtení dávky"), this);
	connect(m_action_load_batch, SIGNAL(triggered()), this, SLOT(loadBatchDialog()));

    m_action_view_batch_list = new QAction(tr("Přehled dávek"), this);
    connect(m_action_view_batch_list, SIGNAL(triggered()), this, SLOT(viewBatchDialog()));

    m_action_view_register = new QAction(tr("Seznam pacientů"), this);
    connect(m_action_view_register, SIGNAL(triggered()), this, SLOT(viewRegisterDialog()));

    m_action_create_register_batch = new QAction(tr("Vytvoření registrační dávky"), this);
    connect(m_action_create_register_batch, SIGNAL(triggered()), this, SLOT(viewRegisterBatchDialog()));

	fullscreenAction = new QAction(tr("&Celá obrazovka"), this);
	fullscreenAction->setShortcut(tr("Alt+Return"));
	connect(fullscreenAction, SIGNAL(triggered()), this, SLOT(toggleFullscreen()));
}

void MainWindow::viewBatchDialog()
{
    m_viewBatchDialog = new ViewBatchDialog();
    m_viewBatchDialog->exec();
}

void MainWindow::viewRegisterBatchDialog()
{
    m_registerBatchDialog = new RegisterBatchDialog(this);
    m_registerBatchDialog ->exec();
}

void MainWindow::viewRegisterDialog()
{
    m_viewRegisterDialog = new ViewRegisterDialog();
    m_viewRegisterDialog->exec();
}

void MainWindow::createBatchDialog()
{
	inDialog = new InsertDialog(InsertDialog::select);
	inDialog->move(x() + 100, y() +100);
	connect(inDialog, SIGNAL(sendData(CrateBatch)), this, SLOT(createBatch(CrateBatch)));

	inDialog->LEbatchNo->setFocus();
	if (!inDialog->exec()) {
		qDebug() << "Zrusen dialog";
	}
	
}

void MainWindow::setBatchInfo(CrateBatch bi)
{
    m_batch_info = bi;
    m_batch_info.icz = m_icz;
    m_batch_info.pob = m_pobocka;
}

void MainWindow::createBatch(CrateBatch batch_info)
{
	inDialog->hide();
	m_createBatchDialog = new CreateBatchDialog(batch_info);
	if (!m_createBatchDialog->exec()) {
		qDebug() << "Zrusen dialog";
	}	
}

void MainWindow::loadBatchDialog()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Zvolte soubor s dávkou"));
	if (!fileName.isEmpty()) {
		LoadBatch* new_batch = new LoadBatch();
		if (new_batch->loadBatch(fileName)) {
			QMessageBox msgBox;
			msgBox.setWindowTitle(tr("Nahrání dávky"));
			msgBox.setText(tr("Dávka úspěšně uložena do databáze."));
			msgBox.exec();
		} else {
		    QMessageBox::critical(0, tr("Načítání dávky"),
					             tr("Načtení dávky do databáze se nezdařilo."),
								 QMessageBox::Ok);
            
        }
	}
}

void MainWindow::settingsDialog()
{

/*
	SettingsDialog* sd = new SettingsDialog();
	sd->setupUI(sd);
	if(sd->exec()){
		if(!parseFile("config.xml")){
			QMessageBox::critical(this, tr("Chyba!"), tr("Nepodařilo se zpracovat soubor config.xml"), QMessageBox::Close );
		}
		ICZLabel->setText(tr(" IČZ: %1 ").arg(icz()));
	}
*/
}

void MainWindow::changeDateDialog()
{
	DateDialog* dd = new DateDialog();
	dd->setupUI(dd);
	if (dd->exec()) {
		d = dd->date();
	}
}

void MainWindow::insertDataForm()
{

	inDialog = new InsertDialog(InsertDialog::insert);
	inDialog->move(x() + 100, y() +100);
	connect(inDialog, SIGNAL(sendData(CrateBatch)), this, SLOT(setBatchInfo(CrateBatch)));

	inDialog->LEbatchNo->setFocus();
	if (inDialog->exec()) {
        createNewBatch(m_batch_info);
	}

}

void MainWindow::createMenu()
{
	accountMenu = menuBar()->addMenu(tr("Úč&ty"));
	accountMenu->addAction(insertDataAction);

	m_batch_menu = menuBar()->addMenu(tr("&Dávky"));
    m_batch_menu->addAction(m_action_create_batch);
    m_batch_menu->addAction(m_action_create_register_batch);
    m_batch_menu->addAction(m_action_view_batch_list);
	m_batch_menu->addAction(m_action_load_batch);


    settingsMenu = menuBar()->addMenu(tr("&Nastavení"));
    settingsMenu->addAction(changeDateAction);
  //settingsMenu->addAction(settingsAction);
//	settingsMenu->addAction(fullscreenAction);
  m_register_menu = menuBar()->addMenu(tr("&Kartotéka")); 
  m_register_menu->addAction(m_action_view_register);

}

void MainWindow::toggleFullscreen()
{
//	setWindowState(windowState() ^ Qt::WindowMaximized);		
}

void MainWindow::createStatusBar()
{
	ICZLabel = new QLabel(tr(" IČZ: %1 ").arg(icz()));
	ICZLabel->setAlignment(Qt::AlignHCenter);
	ICZLabel->setMinimumSize(ICZLabel->sizeHint());
	statusBar()->addWidget(ICZLabel);
}

bool MainWindow::parseFile(const QString &filename)
{
	QFile file(filename);
	QXmlInputSource inputSource(&file);
	QXmlSimpleReader reader;
	XmlHandler handler;
	reader.setContentHandler(&handler);
	if(reader.parse(inputSource)){
		m_icz = handler.icz();
		m_pobocka = handler.pobocka();
//		m_dcid = handler.getDCID();
//		m_hcid = handler.getHCID();
//		m_hodb = handler.getHODB();
		return true;
	}
	else 
		return false;

}

void MainWindow::createNewBatch(CrateBatch batch_info)
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
    qDebug() << batch_info.id_davky;
	int month = date().month();
	if (batch_info.id_davky.isEmpty()) {
		if ((m_batch_id = DB::getInstance()->createNewBatchSignature(batch_info)) == -1) {
			qDebug() << "ERROR nepodarilo se vytvorit popis davky";
            QMessageBox::critical(this, tr("CHYBA !"),
	                                 tr("Nepodařilo se vytvořit průvodní list dávky v databázi."),
	                                 QMessageBox::Ok);

            return;
		} else {
            batch_info.id_davky = QString("%1").arg(m_batch_id);
        }
	} else { //davka existuje, musim zjistit jak se cislujou doklady
		if ((month = DB::getInstance()->getBatchMonth(batch_info.id_davky)) == -1) {
			qDebug() << "ERROR nepodaril se ziskat mesic z cisla dokladu";
		}
        m_batch_id = batch_info.id_davky.toULongLong();
	}

	m_dcid = batch_info.cislo_davky;
	m_hcpo = batch_info.pojistovna;
	m_num_records = batch_info.pocet_dokladu.toInt();
	m_num_points = batch_info.pocet_bodu.toInt();

    int record_number = 1;
        if (month != -1) {
            record_number = (month * 10000) + m_num_records + 1; // +1 protoze je to doklad,ktery teprve budu psat
        } else {
            record_number = DB::getInstance()->getMaxRecordNumber(batch_info.id_davky).toInt() + 1;
        }
	//TODO pridat klikatko na zadani odbornosti
	m_hcid = QString("%1").arg(record_number);
	m_hodb = "603";


  m_record_info = new RecordInfo(this);
	m_record_info->setupUI(m_record_info);
	

	m_batch_info_widget = new BatchInfo(this, m_record_info);
	m_batch_info_widget->setupUI(m_batch_info_widget);
	
	centralWLayout->addWidget(m_batch_info_widget,0,0);
	centralWLayout->addWidget(m_record_info,1,0);
	connect(m_record_info, SIGNAL(exitBatchWriting()), this, SLOT(deleteInfoWidgets()));
	connect(m_record_info, SIGNAL(updateBatchInfo()), m_batch_info_widget, SLOT(updateInfo()));
	connect(m_record_info, SIGNAL(clearBatchInfo()), m_batch_info_widget, SLOT(clearRecordInfo()));

	connect(m_record_info, SIGNAL(playError()), this, SLOT(playError()));
	connect(m_record_info, SIGNAL(playOk()), this, SLOT(playOk()));
	m_record_info->LEPersonalNo->setFocus();
	setStyleSheet(stylesheet);

}

void MainWindow::deleteInfoWidgets()
{
    QList<CrateBatch> data = DB::getInstance()->getBatchInfo(m_dcid, m_hcpo);
    if (data.size() > 1) {
        qDebug() << "ERROR: MainWindow::deleteInfoWidget - nedokazu zpracovat vice davek";
    }
    if (data.first().pocet_dokladu.toInt() == 0) { //v davce neni zadny doklad
        qDebug() << "Davka bez dokladu -> mazu";
        DB::getInstance()->deleteBatch(data.first().id_davky);
    }

	m_record_info->close();
	m_batch_info_widget->close();
	menuBar()->show();
	m_dcid = "";	//cislo davky
	m_hcid = "";	//cislo dokladu
	m_hodb = "";	//odbornost
	m_hcpo = "";    //cislo pojistovny
	m_num_records = 0;	//pocet dokladu v davce, hodnota se priradi z databaze
	m_num_points = 0;	//pocet bodu v davce, hodnota se priradi z databaze
	setStyleSheet("");
}




DateDialog::DateDialog()
{
	d = QDate::currentDate();
}

void DateDialog::setupUI(QDialog* dialog)
{

	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->resize(475, 120);
	dialog->setWindowTitle(tr("Změna data"));

	QVBoxLayout *vboxLayout = new QVBoxLayout(dialog);
	QLabel *l1 = new QLabel(tr("Standardně je dosazováno systémové datum.\n"
	"Toto datum zde můžete změnit, například z důvodu, že píšete účty za minulý měsíc."), dialog);

	vboxLayout->addWidget(l1);

	QHBoxLayout* hboxLayout = new QHBoxLayout();
	hboxLayout->addStretch();
	
    dateEdit = new QDateEdit(d, dialog);
	dateEdit->setDisplayFormat("dd/MM/yy");
	hboxLayout->addWidget(dateEdit);

	hboxLayout->addStretch();
	
	vboxLayout->addLayout(hboxLayout);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(dialog);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

	vboxLayout->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(okClicked()));	
	connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));	
	connect(dateEdit, SIGNAL(dateChanged(QDate)), dialog, SLOT(updateDate(QDate)));
	
	QRect r = QApplication::desktop()->availableGeometry();
	move(r.center() - rect().center());
	
}

void DateDialog::updateDate(QDate new_date)
{
	if(new_date > d){
		Message* msg = new Message();
		msg->setupUI(msg, Message::laterDate);
		msg->exec();
		dateEdit->setDate(d);
	}
}

void DateDialog::okClicked()
{
	d = dateEdit->date();
	emit accept();
}
