#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QShortcut>
#include <QKeyEvent>
#include <QApplication>
#include <QRect>
#include <QDesktopWidget>


class Message : public QDialog
{
		
public:
	enum Type{
		wrongPersNo,
		wrongDate,
		wrongP,
		laterDate,
		wrongInCoNo,
		wrongCode,
		wrongDiag,
		starDiag,
        noBatchNo,
        changeDiag,
        diffInsComp
	};
	void setupUI(QDialog* parent, Type t);

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	Type type;


};





#endif
