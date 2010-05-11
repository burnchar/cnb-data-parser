/*
	Name        : Config.cpp
	Author      : Charles N. Burns (charlesnburns|gmail|com / burnchar|isu|edu)
	Date        : August 2009
	License     : GPL3. See license.txt or www.gnu.org/licenses/gpl-3.0.txt
	Requirements: Qt 4, data to parse, any spreadsheet program.
	Notes       : Best viewed with tab width 4.
	Description : The Config class reads, rebuilds, and verifies the program's
				  XML configuration file. If no such file exists, it generates
				  a default file. This file stores all program settings and
				  stores the most recently used settings for all fields.
*/

#include "Config.h"

//! Constructor for Config class
Config::Config()
{
	this->limitRows = DEFAULT_LIMIT_ROWS;
	this->colCount = DEFAULT_COLUMN_COUNT;
	this->boxOpen = DEFAULT_BOX_OPEN;
}


//! Attempts to read the configuration file
bool Config::xmlRead(const QString &fileURI, const QString &rootTagName)
{
	bool retval = true;
	QDomDocument doc;
	QDomElement root;
	QFile file(fileURI);
	if(! file.open(QFile::ReadOnly | QFile::Text)) {
		errorMessage = "Settings file doesn't seem to exist.";
		retval = xmlWrite(fileURI);
		if(retval == false) {
			errorMessage = "Unable to read or create settings file: " + fileURI;
		}
	}
	else {
		int errorLine, errorColumn;
		QString errorStr;
		if(!doc.setContent(&file, &errorStr, &errorLine, &errorColumn)) {
			errorMessage = QString(
					"Cannot get settings from %1 because of a problem with line %2, column %3. The error message was: %4")
					.arg(fileURI)
					.arg(errorLine)
					.arg(errorColumn)
					.arg(errorStr);
			retval = false;
		}
		else file.close();
	}

	if(! rootTagName.isEmpty()) {
		this->rootElement = doc.documentElement();
		if(rootElement.tagName() != rootTagName) {
			retval = false;
		}
	}

	return retval;
}


//! Looks at main XML tags, then calls the appropriate parsing function.
//! @returns true
bool Config::xmlParse()
{
	bool retval = true;
	QDomNode child = this->rootElement.firstChild();
	QString tagName;

	while(!child.isNull()) {
		tagName = child.toElement().tagName();
		if(tagName == "pathlist")
			parsePathlistElement(child.toElement());
		else if(tagName == "options")
			parseOptionsElement(child.toElement());
		else if(tagName == "columns")
			parseColumnElement(child.toElement());
		child = child.nextSibling();
	}
	return retval;
}


//! Parses the child nodes of <pathlist>, and extracts the file paths.
//! @see xmlParse()
void Config::parsePathlistElement(const QDomElement &element)
{
	QString attrib;
	QString tagName;
	QDomNode child = element.firstChild();

	attrib = element.attribute("type", "none");
	if(attrib == "infile") {
		while(!child.isNull()) {
			tagName = child.toElement().tagName();
			this->pathlistInfile.append(child.toElement().text().trimmed());
			child = child.nextSibling();
		}
	}
	else if(attrib == "outfile") {
		while(!child.isNull()) {
			tagName = child.toElement().tagName();
			this->pathlistOutfile.append(child.toElement().text().trimmed());
			child = child.nextSibling();
		}
	}
}


//! Parses the child nodes of <options>, gets program settings
//! @see xmlParse()
void Config::parseOptionsElement(const QDomElement &element)
{
	QDomNode child = element.firstChild();
	QString tagName, text;
	while(!child.isNull()) {
		tagName = child.toElement().tagName();
		text = child.toElement().text().trimmed();
		if(tagName == "openbox" && text == "checked") this->boxOpen = true;
		else if(tagName == "columncount") {
			int temp = text.toInt();
			if(temp > 0 && temp <= 255) this->colCount = temp;
		}
		else if(tagName == "limitrows") this->limitRows = text;
		child = child.nextSibling();
	}
}


//! Parses the child nodes of <columns>, gets the name, etc. for all columns.
//! @see xmlParse()
void Config::parseColumnElement(const QDomElement &element)
{
	QDomNode child = element.firstChild();
	QString index, name, bytecount, counterbox;
	while(!child.isNull()) {

		index = child.toElement().attribute("index", "0");
		name = child.toElement().attribute("name", "0");
		bytecount = child.toElement().attribute("bytes", "1");
		counterbox = child.toElement().attribute("counterbox", "0");

		QDomNode colChild = child.firstChild();
		QString innerTagName, innerText;
		QStringList sl;

		while(! colChild.isNull()) {
			innerTagName = colChild.toElement().tagName();
			if(innerTagName == "name")
				sl.append(colChild.toElement().text().trimmed());
			colChild = colChild.nextSibling();
		}
		colBytes.append(quint8(bytecount.toInt()));
		if(counterbox == "checked") colBoxChecked.append(true);
		else colBoxChecked.append(false);
		colNames.append(sl);
		child = child.nextSibling();
	}

}


//! Writes the contents of Config data structures to a formatted XML file
//! @returns false if output fill cannot be opened, true otherwise
//! @see xmlRead()
bool Config::xmlWrite(const QString &fileURI)
{
	bool retval = true;

	QFile file(fileURI);
	if(! file.open(QFile::WriteOnly | QFile::Text)) {
		errorMessage = QString("Cannot write to file: %1").arg(fileURI);
		retval = false;
	}
	else {
		QXmlStreamWriter xml(&file);
		xml.setAutoFormatting(true);
		xml.setAutoFormattingIndent(-1); // -1: Use 1 tab for formatting
		xml.writeStartDocument();
		xml.writeStartElement(DEFAULT_START_ELEMENT);

		xmlWriteInfilePaths(xml);
		xmlWriteOutfilePaths(xml);
		xmlWriteOptions(xml);
		xmlWriteColumns(xml);

		xml.writeEndDocument();
		file.close();
	}
	return retval;
}


//! Writes the list of input file paths to the XML stream
//! @see xmlWrite()
void Config::xmlWriteInfilePaths(QXmlStreamWriter &xml)
{
	xml.writeStartElement("pathlist");
	xml.writeAttribute("type", "infile");
	for(int counter = 0; counter < pathlistInfile.size(); ++counter) {
		xml.writeTextElement("path", pathlistInfile.at(counter));
	}
	xml.writeEndElement();
}


//! Writes the list of output file paths to the XML stream
//! @see xmlWrite()
void Config::xmlWriteOutfilePaths(QXmlStreamWriter &xml)
{
	xml.writeStartElement("pathlist");
	xml.writeAttribute("type", "outfile");
	for(int counter = 0; counter < pathlistOutfile.size(); ++counter) {
		xml.writeTextElement("path", pathlistOutfile.at(counter));
	}
	xml.writeEndElement();
}


//! Writes the list of program options to the XML stream
//! @see xmlWrite()
void Config::xmlWriteOptions(QXmlStreamWriter &xml)
{
	xml.writeStartElement("options");
	xml.writeTextElement("openbox", boxOpen ? "checked" : "unchecked");
	xml.writeTextElement("limitrows", limitRows);
	xml.writeTextElement("columncount", QString::number(colCount));
	xml.writeEndElement();
}


//! Writes the column data (name, # bytes, etc.) to XML stream.
//! @see xmlWrite()
void Config::xmlWriteColumns(QXmlStreamWriter &xml)
{
	xml.writeStartElement("columns");
	for(int counter = 0; counter < colNames.size(); ++counter) {
		xml.writeStartElement("column");
		xml.writeAttribute("index", QString::number(counter));
		xml.writeAttribute("bytes", QString::number(colBytes.at(counter)));
		xml.writeAttribute("counterbox",
						   colBoxChecked.at(counter) ? "checked" : "unchecked");
		for(int names = 0; names < colNames.at(counter).size(); ++names) {
			QString tmp = colNames.at(counter).at(names);
			xml.writeTextElement("name", colNames.at(counter).at(names));
		}
		xml.writeEndElement();
	}
	xml.writeEndElement();
}


//! Clear out all data structures which store configuration information.
void Config::clear()
{
	pathlistInfile.clear();
	pathlistOutfile.clear();
	limitRows.clear();
	colNames.clear();
	colBoxChecked.clear();
	colBytes.clear();
}
