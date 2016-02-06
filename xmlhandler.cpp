#include "xmlhandler.h"
#include "mainwindow.h"
#include <iostream>

bool XmlHandler::endElement(const QString &,
		   	const QString &,
		   	const QString &qName)
{
	if (qName == "icz") {
		m_icz = currentText;
	}
	if (qName == "dcid") {
		m_dcid = currentText;
	}
	if (qName == "hcid") {
		m_hcid = currentText;
	}
	if (qName == "hodb") {
		m_hodb = currentText;
	}
	if (qName == "pobocka") {
		m_pobocka = currentText;	
	}
	return true;
}


bool XmlHandler::characters(const QString &str)
{
	currentText = str;
	return true;
}




