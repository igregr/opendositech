#include "LineEdit.h"



LineEdit::LineEdit() : m_previous(0), m_next(0), m_length(0)
{
	setFocusPolicy(Qt::NoFocus);
	setAttribute(Qt::WA_DeleteOnClose);

        key_up_shortcut = new QShortcut(QKeySequence(Qt::Key_Up), this);
        key_dow_shortcut = new QShortcut(QKeySequence(Qt::Key_Down), this);
        enter_shortcut = new QShortcut(QKeySequence(Qt::Key_Enter), this);

	connect(key_up_shortcut, SIGNAL(activated()), this, SLOT(focusPrevious()));
	connect(key_dow_shortcut, SIGNAL(activated()), this, SLOT(focusNext()));
        connect(enter_shortcut, SIGNAL(activated()), this, SLOT(send()));

	key_up_shortcut->setContext(Qt::WidgetShortcut);
	key_dow_shortcut->setContext(Qt::WidgetShortcut);
        enter_shortcut->setContext(Qt::WidgetShortcut);

	connect(this, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(itemFilled(int,int)));
	connect(this, SIGNAL(returnPressed()), this, SLOT(send()));


}

void LineEdit::itemFilled(int , int newp)
{
	if (newp == m_length) {
		send();
	}
}

void LineEdit::setValue(QString value)
{
	disconnect(SIGNAL(cursorPositionChanged(int,int)));
	setText(value);
	connect(this, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(itemFilled(int, int)));

}

bool LineEdit::checkLength()
{
	if (text().length() == m_length)	{
		return true;
	}
	return false;
}

void LineEdit::send()
{
	QString value = text();
	if (value.length() == m_length) {
		emit sendData(value);
	}
}


void LineEdit::setNeighbours(LineEdit* p, LineEdit* n)
{
	m_previous = p;
	m_next = n;
}

void LineEdit::focusNext()
{
  if (m_next != 0) {
      if(checkLength()){
         m_next->setFocus();
         m_next->home(false);
      }
   }
}

void LineEdit::focusPrevious()
{
	if (m_previous != 0) {
		 m_previous->setFocus();
	}
}

void LineEdit::keyPressEvent(QKeyEvent *event)
{
        switch (event->key()) {
                case Qt::Key_Plus:
                        emit plusShortcut();
                        break;
                default:
                    QLineEdit::keyPressEvent(event);
                }
}




DateLineEdit::DateLineEdit(QDate nd) : m_date(nd) 
{
	QFontMetrics metrics(QApplication::font());
	setFixedSize(metrics.width("99.99.99") + 10, metrics.height() + 4);
	setInputMask("00.00.00;_");
	setMaxLength(6);
	setLength(8);
	disconnect(SIGNAL(returnPressed()), this, SLOT(send()));
	connect(this, SIGNAL(returnPressed()), this, SLOT(dateReturnPressed()));
	disconnect(enter_shortcut, SIGNAL(activated()), this, SLOT(send()));
	connect(enter_shortcut, SIGNAL(activated()), this, SLOT(dateReturnPressed()));
}

void DateLineEdit::dateReturnPressed()
{
	QString value = text();
    int size = value.size();
    QString date = m_date.toString("dd.MM.yy");
    QStringList list = value.split(".");
    QString tmp;
   
   	switch(size){
        case 2:		//prazdne datum, ukoncim switch a nastavim datum
            break;
        case 8:		//vse vyplneno, odeslu na kontrolu
            send();
            break;
        default:
            if (list.at(0).size() == 2) { //vyplneny den v mesici
                date.replace(0, 2, list.at(0));
            } else if (list.at(0).size() == 1) { 
				tmp = list.at(0);
                tmp.prepend("0");
                date.replace(0, 2, tmp); 
			}
			if (list.at(1).size() == 2) {	//vyplneny mesic
				date.replace(3, 2, list.at(1));
			} else if (list.at(1).size() == 1) {
				tmp = list.at(1);
				tmp.prepend("0");
				date.replace(3, 2, tmp);
			} 
         break;
 	}
	setText(date);
}

BatchNumberLineEdit::BatchNumberLineEdit()
{
	QFontMetrics metrics(QApplication::font());
    setFixedSize(metrics.width("888888") + 10, metrics.height()+4);
	setInputMask("000009");
	setMaxLength(6);
	setLength(6);
}

bool BatchNumberLineEdit::checkLength()
{
	if (text().length() >= 1) {
		return true;
	}
	return false;

}

void BatchNumberLineEdit::send()
{
	if (text().length() >= 1) {
		emit sendData(text());
	}
}

InsuranceCompanyLineEdit::InsuranceCompanyLineEdit()
{
	QFontMetrics metrics(QApplication::font());
	setFixedSize(metrics.width("888") + 10, metrics.height() + 4);
	setInputMask("999");
	setMaxLength(3);
	setLength(3);
}

RecordNumberLineEdit::RecordNumberLineEdit()
{
	QFontMetrics metrics(QApplication::font());
	setFixedSize(metrics.width("8888888") + 10, metrics.height()+4);
//	QRegExp rx("^\\d{0,6}\\d$");
	QValidator *validator = new QIntValidator(this);
	setValidator(validator);
//	setInputMask("0000009");
//	setAlignment(Qt::AlignRight);
	setMaxLength(7);
	setLength(7);
}

bool RecordNumberLineEdit::checkLength()
{
	if (text().length() >= 1) {
		return true;
	}
	return false;
}

void RecordNumberLineEdit::send()
{
	if (text().length() >= 1) {
		qDebug() << "datadatadata";
		emit sendData(text());
	}
}


IcpLineEdit::IcpLineEdit()
{
	QFontMetrics metrics(QApplication::font());
	setFixedSize(metrics.width("88888888")+10, metrics.height() + 4);
	setInputMask("99999999");
	setMaxLength(8);
	setLength(8);
}

DiagnosisLineEdit::DiagnosisLineEdit(QString s) : m_main_diagnosis(s)
{
	QFontMetrics metrics(QApplication::font());
	setFixedSize(metrics.width("Z888") + 10, metrics.height() + 4);
	setMaxLength(4);
	setLength(4);
	setInputMask(">a000");
	disconnect(SIGNAL(returnPressed()), this, SLOT(send()));
	connect(this, SIGNAL(returnPressed()), this, SLOT(diagReturnPressed()));
        disconnect(enter_shortcut, SIGNAL(activated()), this, SLOT(send()));
        connect(enter_shortcut, SIGNAL(activated()), this, SLOT(diagReturnPressed()));

	
}

void DiagnosisLineEdit::diagReturnPressed()
{
	if (text().isEmpty() && !m_main_diagnosis.isEmpty()) {
		setText(m_main_diagnosis);
        send();
	} else {//diagnoza uz je vyplnena, dalsi kontroly by nemusely byt,protoze
			 //returnPress se neemitne,pokud validator neda ok
			send();
	}
}

bool DiagnosisLineEdit::checkLength()
{
	int len = text().length();
	if (len == 4 || len == 3 || len == 0) {
		return true;
	}
	return false;
}

void DiagnosisLineEdit::send()
{
	if (text().length() == 4 || text().length() == 3) {
		emit sendData(text());
	}
}

void DiagnosisLineEdit::setMainDiagnose(const QString diag)
{
	m_main_diagnosis = diag;
}

OdbLineEdit::OdbLineEdit()
{
	QFontMetrics metrics(QApplication::font());
	setFixedSize(metrics.width("888") + 10, metrics.height() + 4);
	setInputMask("000");
	setMaxLength(3);
	setLength(3);
}

void OdbLineEdit::send()
{
	if (text().length() == 3 || text().length() == 0) {
		emit sendData(text());
	}
}

bool OdbLineEdit::checkLength()
{	
	int len = text().length();
	if (len == 3 || len == 0) {
		return true;	
	}
	return false;
}


VarSymbolLineEdit::VarSymbolLineEdit()
{
	QFontMetrics metrics(QApplication::font());
	setFixedSize(metrics.width("888888") + 10, metrics.height() + 4);
	setInputMask("000000");
	setMaxLength(6);
	setLength(6);

}

void VarSymbolLineEdit::send()
{
	if (text().length() >=  0) {
		emit sendData(text());
	}
}

bool VarSymbolLineEdit::checkLength()
{	
	int len = text().length();
	if (len >= 0) {
		return true;	
	}
	return false;
}



PersonalNumberLineEdit::PersonalNumberLineEdit()
{
	QFontMetrics metrics(QApplication::font());
	setFixedSize(metrics.width("8888888888") + 10, metrics.height() + 4);
	setInputMask("9999999990");
	setMaxLength(10);
	setLength(10);

}

bool PersonalNumberLineEdit::checkLength()
{	
	int len = text().length();
	if (len == 10 || len == 9) {
		return true;	
	}
	return false;
}

void PersonalNumberLineEdit::send()
{
	if (text().length() == 10 || text().length() == 9) {
		emit sendData(text());
	}
}






CodeLineEdit::CodeLineEdit(QString s) : m_last_code(s)
{
	QFontMetrics metrics(QApplication::font());
	setFixedSize(metrics.width("88888") + 10, metrics.height() + 4);
	setInputMask("00000");
	setMaxLength(5);
	setLength(5);
	disconnect(SIGNAL(returnPressed()), this, SLOT(send()));
	connect(this, SIGNAL(returnPressed()), this, SLOT(codeReturnPressed()));
        disconnect(enter_shortcut, SIGNAL(activated()), this, SLOT(send()));
        connect(enter_shortcut, SIGNAL(activated()), this, SLOT(codeReturnPressed()));


}

void CodeLineEdit::codeReturnPressed()
{
	 //QString tx = text();
	if (text().isEmpty() && !m_last_code.isEmpty()) {
		setText(m_last_code);
	} else {
		if (checkLength()) {
			send();
		}
	}
}

void CodeLineEdit::send()
{
	if (text().length() == 5 || text().length() == 0) {
		emit sendData(text());
	}
}

PLineEdit::PLineEdit()
{
	QFontMetrics metrics(QApplication::font());
	setFixedSize(metrics.width("8") + 10, metrics.height() + 4);
	setInputMask("0");
	setMaxLength(1);
	setLength(1);
	disconnect(SIGNAL(returnPressed()), this, SLOT(send()));
	connect(this, SIGNAL(returnPressed()), this, SLOT(pReturnPressed()));
        disconnect(enter_shortcut, SIGNAL(activated()), this, SLOT(send()));
        connect(enter_shortcut, SIGNAL(activated()), this, SLOT(pReturnPressed()));


}

void PLineEdit::pReturnPressed()
{
	if (text().isEmpty()) {
		setText("1");
	} else {
		send();
	}
}

void PLineEdit::send()
{
	if (text().length() == 1) {
		emit sendData(text());
	}
}

DiskNameLineEdit::DiskNameLineEdit()
{
	QFontMetrics metrics(QApplication::font());
	m_disc_name = "A";
	setFixedSize(metrics.width("8") + 10, metrics.height() + 4);
	setInputMask("A");
	setMaxLength(1);
	setLength(1);
	setText(m_disc_name);
	setFocusPolicy(Qt::StrongFocus);
}


