/*
	Name        : Config.h
	Author      : Charles N. Burns (charlesnburns|gmail|com / burnchar|isu|edu)
	Date        : August 2009
	License     : GPL3. See license.txt or www.gnu.org/licenses/gpl-3.0.txt
	Requirements: Qt 4, data to parse, any spreadsheet program.
	Notes       : Best viewed with tab width 4.
	Description : Header file to define Config class.
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <QtXml/QDomDocument>
#include <QtXml/QXmlStreamWriter>
#include <QFile>
#include <QStringList>
#include <QMultiMap>
#include <QLabel>

// Globals
const bool DEFAULT_BOX_OPEN = false;
const quint8 DEFAULT_COLUMN_COUNT = 0;
const char DEFAULT_LIMIT_ROWS[] = "0";
const char DEFAULT_START_ELEMENT[] = "charles_n_burns-data_parser";

class Config
{
	void parsePathlistElement(const QDomElement &element);
	void parseOptionsElement(const QDomElement &element);
	void parseColumnElement(const QDomElement &element);

	void xmlWriteInfilePaths(QXmlStreamWriter &xml);
	void xmlWriteOutfilePaths(QXmlStreamWriter &xml);
	void xmlWriteOptions(QXmlStreamWriter &xml);
	void xmlWriteColumns(QXmlStreamWriter &xml);

	QDomElement rootElement;

public:
	QString errorMessage;
	Config();
	bool xmlRead(const QString &fileURI, const QString &rootTagName);
	bool xmlParse();
	bool xmlWrite(const QString &fileURI);
	void clear();

	QString limitRows;
	QStringList pathlistInfile;
	QStringList pathlistOutfile;
	QList<QStringList> colNames;
	QList<quint8> colBytes;
	QList<bool> colBoxChecked;
	bool boxOpen;
	quint8 colCount;
};


#endif // CONFIG_H
