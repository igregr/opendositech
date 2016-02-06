#ifndef XMLHANDLER_H
#define XMLHANDLER_H
#include <QXmlDefaultHandler>


class XmlHandler : public QXmlDefaultHandler
{
public:
//	bool startElement(const QString &namespaceURI,
//			  const QString &localName,
//		     	  const QString &qName,
//		     	  const QXmlAttributes &attributes);

	bool endElement(const QString &namespaceURI,
		   	const QString &localName,
		   	const QString &qName);

	bool characters(const QString &str);

	QString icz()  const { return m_icz;  };
	QString dcid() const { return m_dcid; };
	QString hcid() const { return m_hcid; };
	QString hodb() const { return m_hodb; };
	QString pobocka() const { return m_pobocka; };


private:
	QString currentText;
	QString m_icz; 	//identifikacni cislo smluvniho zarizeni
	QString m_dcid;	//cislo davky
	QString m_hcid;	//cislo dokladu
	QString m_hodb;	//odbornost
	QString m_pobocka;
};







#endif
