#include "message.h"


void Message::setupUI(QDialog* parent, Type t)
{
	QVBoxLayout* vboxLayout = new QVBoxLayout(parent);
	QLabel* msg = new QLabel(parent);
	QLabel* verify = new QLabel(parent);
	QShortcut* shortcut = new QShortcut(parent);
    QHBoxLayout* h = new QHBoxLayout();
	
	QRect r = QApplication::desktop()->availableGeometry(parent);
	move(r.center() - rect().center());

	switch(t){
		case wrongPersNo:
					parent->setWindowTitle(tr("Varování"));
					
					msg->setText(tr("Nedělitelné rodné číslo\nPotvrzení může způsobit problémy"));
					vboxLayout->addWidget(msg);
					vboxLayout->addStretch();
					verify->setText(tr("Potvrdíte po stisknutí CTRL+A"));
					vboxLayout->addWidget(verify);
					parent->setLayout(vboxLayout);
					shortcut->setKey(QKeySequence(tr("Ctrl+A", "Verify")));
					connect(shortcut, SIGNAL(activated()), parent , SLOT(accept()));
					type = wrongPersNo;
					break;
		case wrongDate:
					parent->setWindowTitle(tr("Chyba!"));
					msg->setText(tr("Špatné datum"));
					vboxLayout->addWidget(msg);
//					vboxLayout->addStretch();
					verify->setText(tr("Stiskni libovolnou klávesu"));
					vboxLayout->addWidget(verify);
					parent->setLayout(vboxLayout);
					type = wrongDate;
					break;
		case wrongCode:
					parent->setWindowTitle(tr("Chyba!"));
					msg->setText(tr("Špatný kód"));
					vboxLayout->addWidget(msg);
//					vboxLayout->addStretch();
					verify->setText(tr("Stiskni libovolnou klávesu"));
					vboxLayout->addWidget(verify);
					parent->setLayout(vboxLayout);
					type = wrongCode;
					break;
		case wrongDiag:
					parent->setWindowTitle(tr("Chyba!"));
					msg->setText(tr("Špatná diagnóza"));
					vboxLayout->addWidget(msg);
//					vboxLayout->addStretch();
					verify->setText(tr("Stiskni libovolnou klávesu"));
					vboxLayout->addWidget(verify);
					parent->setLayout(vboxLayout);
					type = wrongDiag;
					break;
		case starDiag:
					parent->setWindowTitle(tr("Chyba!"));
					msg->setText(tr("CHybná diagnóza\nTato diagnóza nemůže být použita jako základní"));
					vboxLayout->addWidget(msg);
//					vboxLayout->addStretch();
					verify->setText(tr("Stiskni libovolnou klávesu"));
					vboxLayout->addWidget(verify);
					parent->setLayout(vboxLayout);
					type = starDiag;
					break;
		case wrongP:
					parent->setWindowTitle(tr("Chyba!"));
					msg->setText(tr("Chybný počet"));
					vboxLayout->addWidget(msg);
//					vboxLayout->addStretch();
					verify->setText(tr("Stiskni libovolnou klávesu"));
					vboxLayout->addWidget(verify);
					parent->setLayout(vboxLayout);
					type = wrongP;
					break;
		case laterDate:
					parent->setWindowTitle(tr("Chyba!"));
					msg->setText(tr("Nelze zvolit budoucí datum"));
					vboxLayout->addWidget(msg);
				//	vboxLayout->addStretch();
					verify->setText(tr("Stiskni libovolnou klávesu"));
					vboxLayout->addWidget(verify);
					parent->setLayout(vboxLayout);
					type = laterDate;
					break;
		case wrongInCoNo:
            parent->setWindowTitle(tr("Chyba!"));
            msg->setText(tr("Špatné číslo pojišťovny"));
            vboxLayout->addWidget(msg);
//					vboxLayout->addStretch();
            verify->setText(tr("Stiskni libovolnou klávesu"));
            vboxLayout->addWidget(verify);
            parent->setLayout(vboxLayout);
            type = wrongInCoNo;
            break;
//TODO spravit layout..at je to a/n uprostred
        case changeDiag:
            parent->setWindowTitle(tr("Dotaz!"));
            msg->setText(tr("Změna hlavní diagnózy. Změnit i ostatní ?"));
            vboxLayout->addWidget(msg);
            verify->setText(tr("A / N"));
            vboxLayout->addWidget(verify);
            parent->setLayout(vboxLayout);
            type = changeDiag;
            break;
        case diffInsComp:
            parent->setWindowTitle(tr("Dotaz!"));
            msg->setText(tr("Pacient je v kartotéce veden u jiné pojišťovny.\nPřepsat pojišťovnu v kartotéce?"));
            vboxLayout->addWidget(msg);
            verify->setText(tr("A / N"));
            h->addStretch();
            h->addWidget(verify);
            h->addStretch();
            vboxLayout->addLayout(h);
            parent->setLayout(vboxLayout);
            type = diffInsComp;
            break;
		case noBatchNo:
            parent->setWindowTitle(tr("Chyba!"));
            msg->setText(tr("Dávka nenalezena"));
            vboxLayout->addWidget(msg);
//					vboxLayout->addStretch();
            verify->setText(tr("Stiskni libovolnou klávesu"));
            vboxLayout->addWidget(verify);
            parent->setLayout(vboxLayout);
            type = noBatchNo;
            break;
		default:
				break;

	}


}



void Message::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
	  case Qt::Key_Control:
		  if(type != wrongPersNo){
			  break;
			}
			else {
			  QDialog::keyPressEvent(event);
				break;
			}
    case Qt::Key_A:
      if (type == changeDiag || type == diffInsComp) {
        accept();
      }
      break;
    case Qt::Key_N:
      if (type == changeDiag || type == diffInsComp) {
        reject();
      }
      break;
    default:
      reject();
	}
}
