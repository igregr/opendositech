#include "ICZdialog.h"
#include "mainwindow.h"


ICZdialog::ICZdialog() 
{
		
	setWindowModality(Qt::ApplicationModal);
	label = new QLabel(tr("Zadejte Vaše IČZ"));
	label2 = new QLabel(tr("IČZ"));
	inputLine = new QLineEdit;
	okButton = new QPushButton(tr("OK"));
	cancelButton = new QPushButton(tr("Cancel"));

	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));

	QHBoxLayout *mid = new QHBoxLayout;
	mid->addWidget(label2);
	mid->addWidget(inputLine);
	
	QHBoxLayout *bottom = new QHBoxLayout;
	bottom->addStretch();
	bottom->addWidget(okButton);
	bottom->addWidget(cancelButton);

	QVBoxLayout *main = new QVBoxLayout;
	main->addWidget(label);
	main->addLayout(mid);
	main->addLayout(bottom);
	setLayout(main);
	setWindowTitle(tr("Nastaveni IČZ"));
	inputLine->setInputMask ("99999999" );
}


void ICZdialog::okClicked()
{

	QString icz = inputLine->text();
	QString pobocka = icz.left(2);
	pobocka.append("00");

	//ICZ musi mit dle rozhrani 8 cislic 
	if (icz.length() == 8){ 
	//TODO zatim predpokladam,ze soubor neexistuje a vytvarim ho novy
	//takze pripadne vylepsit a pridat vic kontrol napriklad pokud tam bude ulozene
	//i neco jineho nez ICZ,tak to asi prvni nacist a pak zapsat
		QFile configFile("config.xml");
		if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text)){
			QMessageBox::critical(this, tr("Nastavení IČZ"), tr("Nepodařilo se otevřít soubor config.xml"), QMessageBox::Close );
			return;
		}
		if (!DB::getInstance()->isInsuranceOffice(pobocka)) {
   			QMessageBox::critical(this, tr("Nastavení IČZ"), tr("Pobočka %1 nenalezena v číselníku").arg(pobocka), QMessageBox::Close );
			inputLine->clear();
			inputLine->home(false);
			return;
		}
		QTextStream out(&configFile);
		out << 
			"<?xml version=\"1.0\"?>\n" << 
			"<config>\n" <<
			"	<icz>" << icz << "</icz>\n" <<
			"	<pobocka>" << pobocka << "</pobocka>\n" <<
			"</config>\n";
	}
	else{
		QMessageBox::critical(this, tr("Nastavení IČZ"), tr("IČZ nemá správnou délku"), QMessageBox::Ok );
		inputLine->clear();
		inputLine->home(false);
		inputLine->setFocus();
		return;
	}
		
	accept();
}
