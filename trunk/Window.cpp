/*
	Name        : Window.cpp
	Author      : Charles N. Burns (charlesnburns|gmail|com / burnchar|isu|edu)
	Date        : August 2009
	License     : GPL3. See license.txt or www.gnu.org/licenses/gpl-3.0.txt
	Requirements: Qt 4, data to parse, any spreadsheet program.
	Notes       : Tested only with Atmel hardware. Best viewed with tab width 4.
	Description : This is the main class of the program. It dynamically creates
				  the GUI, responds to user input, and does all necessary
				  calculations, parsing, and file I/O.
*/

#include "Window.h"

//! Constructor for Window class. @see Window
Window::Window()
{
	this->setWindowTitle("CNB Data Parser 1.0");
	this->configFileURI = "config.xml";
	this->maxComboItems = 10;
	this->config = new Config();
	this->createStatusBar();
	this->createDataLayout();
	this->createAdvFeaturesLayout();
	this->createMainLayout();
	this->updateColumnList();
	this->mainLayoutCreateConnections();
	this->importSettings();
	this->setMaximumWidth(300);
	this->setAcceptDrops(true);
}


//! Sets up and creates the status bar (the information panel at the bottom)
//! @see window()
void Window::createStatusBar()
{
	statusBar = new QStatusBar(this);
	statusBarMessage = new QLabel();
	statusBarMessage->setText(defaultStatusMessage);
	statusBar->addWidget(statusBarMessage, 100);
}


//! Finds the integer by which the row count must divide to stay within limits.
//! For example, if there are 200 rows, and the row limit is 50, returns 4
//! @returns That integer
//! @see infileNumberRows()
//! @see updateInfileRowsDisplay()
quint64 Window::infileRowLimitDivisor()
{
	quint64 rowLimit = comboRowLimit->currentText().toULongLong();
	quint64 rows = infileNumberRows();
	quint64 divider = 1;
	quint64 start;
	if((rowLimit < rows) && (rows > 0) && (rowLimit > 0)) {
		if(checkBoxWriteColNames->isChecked()) rows += 1;
		start = rows / (rowLimit + 1);
		for(divider = start; ((rows / divider) > rowLimit); divider += 1);
	}
	return divider;
}

//! Creates and configures main layout
//! @see mainLayoutAllocateWidgets()
//! @see mainLayoutConfigureWidgets()
//! @see mainLayoutAddWidgets()
void Window::createMainLayout()
{
	mainLayout = new QGridLayout();
	mainLayout->setColumnStretch(1, 2);
	mainLayoutAllocateWidgets();
	mainLayoutConfigureWidgets();
	mainLayoutAddWidgets();
	this->setLayout(mainLayout);
}


//! Allocates widgets for the main layout.
//! @see createMainLayout()
void Window::mainLayoutAllocateWidgets()
{
	comboInfile = new QComboBox();
	comboOutfile = new QComboBox();
	comboRowLimit = new QComboBox();
	spinColumns = new QSpinBox();
	infileRowsDisplay = new QLabel();
	buttonBrowseInput = new QPushButton(tr("Browse"));
	buttonBrowseOutput = new QPushButton(tr("Browse"));
	buttonProcessData = new QPushButton(tr("Process data"));
	checkBoxOpenWhenDone = new QCheckBox(tr("Open output file when finished"));
}


//! Configures widgets for the main layout. Widgets must already be allocated.
//! @see createMainLayout()
void Window::mainLayoutConfigureWidgets()
{
	QIcon iconExcel(":/embedded/icon_excel.png");
	QIcon iconOocalc(":/embedded/icon_oocalc.png");
	QIcon iconQuattro(":/embedded/icon_quattropro.png");
	QIcon iconUnlimited(":/embedded/icon_unlimited.png");

	spinColumns->setRange(1, 255);
	comboOutfile->setEditable(true);
	comboOutfile->setMaxCount(maxComboItems);
	comboOutfile->setInsertPolicy(QComboBox::InsertAtTop);
	comboInfile->setEditable(true);
	comboInfile->setMaxCount(maxComboItems);
	comboInfile->setInsertPolicy(QComboBox::InsertAtTop);
	comboInfile->setMaximumWidth(300);
	comboOutfile->setMaximumWidth(300);
	comboRowLimit->setEditable(true);
	comboRowLimit->setMaxCount(maxComboItems);
	comboRowLimit->addItem(iconUnlimited, tr("No limit"));
	comboRowLimit->addItem(iconOocalc, tr("65536 (OpenOffice, Excel)"));
	comboRowLimit->addItem(iconExcel, tr("1048576 (Excel 2007+)"));
	comboRowLimit->addItem(iconQuattro, tr("1000000 (Quattro Pro)"));
	comboRowLimit->setCompleter(0);
	comboRowLimit->insertSeparator(0);
	comboRowLimitDefaultItemCount = 4;
}


//! Adds widgets to the appropriate grid coordinates of the main layout.
//! @see createMainLayout()
void Window::mainLayoutAddWidgets()
{
	mainLayout->addWidget(new QLabel(tr("Data file:")), 0, 0);
	mainLayout->addWidget(comboInfile, 0, 1, 1, 2);
	mainLayout->addWidget(buttonBrowseInput, 0, 3);
	mainLayout->addWidget(new QLabel(tr("Output file:")), 1, 0);
	mainLayout->addWidget(comboOutfile, 1, 1, 1, 2);
	mainLayout->addWidget(buttonBrowseOutput, 1, 3);
	mainLayout->addWidget(checkBoxOpenWhenDone, 2, 1, 1, 2);
	mainLayout->addWidget(new QLabel(tr("Limit rows:")), 3, 0);
	mainLayout->addWidget(comboRowLimit, 3, 1, 1, 1);
	mainLayout->addWidget(infileRowsDisplay, 3, 2, 1, 2);
	mainLayout->addWidget(new QLabel(tr("Columns:")), 4, 0);
	mainLayout->addWidget(spinColumns, 4, 1, 1, 1);
	mainLayout->addWidget(scrollArea, 5, 0, 1, 4);
	mainLayout->addLayout(advFeaturesLayout, 6, 0, 1, 4);
	mainLayout->addWidget(buttonProcessData, 7, 0, 1, 4);
	mainLayout->addWidget(statusBar, 8, 0, 1, 4);
}

//! Connects the signals of widgets in the main layout to the appropriate slots.
//! @see createMainLayout()
void Window::mainLayoutCreateConnections() const
{
	connect(comboInfile, SIGNAL(editTextChanged(QString)), this,
			SLOT(updateDisplay()));
	connect(comboOutfile, SIGNAL(editTextChanged(QString)), this,
			SLOT(updateDisplay()));
	connect(spinColumns, SIGNAL(valueChanged(int)), this,
			SLOT(updateColumnList()));
	connect(spinColumns, SIGNAL(valueChanged(int)), this,
			SLOT(updateDisplay()));
	connect(buttonBrowseInput, SIGNAL(clicked()), this,
			SLOT(openFileDialog()));
	connect(buttonBrowseOutput, SIGNAL(clicked()), this,
			SLOT(saveFileDialog()));
	connect(buttonProcessData, SIGNAL(clicked()), this,
			SLOT(dataToCsv()));
	connect(comboRowLimit, SIGNAL(editTextChanged(const QString&)), this,
			SLOT(filterLimitRowsName(const QString&)));
	connect(comboRowLimit, SIGNAL(editTextChanged(const QString&)), this,
			SLOT(updateDisplay()));
	connect(statusBarMessage, SIGNAL(linkActivated(QString)), this,
			SLOT(openSystemWebBrowser(QString)));
}


//! Allocates, configures, and adds widgets to the layout which shows columns.
//! @see Window()
void Window::createDataLayout()
{
	dataLayout = new QGridLayout;
	dataLayout->addWidget(new QLabel(tr("#")), 0, 0);
	checkBoxWriteColNames = new QCheckBox("Column Name");
	checkBoxWriteColNames->setChecked(true);
	dataLayout->addWidget(checkBoxWriteColNames, 0, 1, 1, 1, Qt::AlignCenter);
	dataLayout->addWidget(new QLabel(tr("# bytes")), 0, 2);
	dataLayout->addWidget(new QLabel(tr("Count")), 0, 3);
	dataLayout->setColumnStretch(1, 2);
	dataLayout->setAlignment(Qt::AlignTop);
	dataGroupBox = new QGroupBox();
	dataGroupBox->setFocusPolicy(Qt::WheelFocus);
	dataGroupBox->setLayout(dataLayout);
	scrollArea = new QScrollArea;
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(dataGroupBox);
	connect(checkBoxWriteColNames, SIGNAL(toggled(bool)), this,
			SLOT(updateDisplay()));
}


//! Allocates, configures, and adds widgets to the advanced features layout.
//! @see Window()
void Window::createAdvFeaturesLayout()
{
	minVoltage = new QDoubleSpinBox();
	maxVoltage = new QDoubleSpinBox();
	checkBoxEndian = new QCheckBox("Swap byte order");
	checkBoxEndian->setChecked(true);
	advFeaturesLayout = new QHBoxLayout();
	advFeaturesLayout->addWidget(
			new QLabel(tr("Low voltage:")), 0,  Qt::AlignRight);
	advFeaturesLayout->addWidget(minVoltage, 0);
	advFeaturesLayout->addWidget(
			new QLabel(tr("High voltage:")), 0, Qt::AlignRight);
	advFeaturesLayout->addWidget(maxVoltage);
	advFeaturesLayout->addWidget(checkBoxEndian);
	minVoltage->setValue(0.0);
	maxVoltage->setValue(5.0);
	minVoltage->setRange(-1000.0, 1000.0);
	maxVoltage->setRange(-1000.0, 1000.0);
	minVoltage->setSuffix("v");
	maxVoltage->setSuffix("v");
	minVoltage->setDecimals(3);
	maxVoltage->setDecimals(3);
	connect(minVoltage, SIGNAL(valueChanged(double)), this,
			SLOT(minVoltageChanged(double)));
	connect(maxVoltage, SIGNAL(valueChanged(double)), this,
			SLOT(maxVoltageChanged(double)));
}


//! This slot changes the number of items displayed in the data layout.
//! @see mainLayoutCreateConnections()
void Window::updateColumnList()
{
	static int oldCount; //! Remembers previous number of columns
	int newCount = spinColumns->value();
	for(; oldCount < newCount; ++oldCount) {
		if(dataLayout->rowCount() -1 > oldCount)
			dataRowSetVisible(oldCount, true);
		else
			dataRowCreate(oldCount);
	}
	while(oldCount > newCount) {
		--oldCount;
		dataRowSetVisible(newCount, false);
	}
}


//! Changes the visibility of a row in the data layout.
//! @param index Index of the row of controls (0-based) to make show or hide.
//! @param visible Should this row be visible? True or false?
//! @see updateColumnList()
void Window::dataRowSetVisible(const int index, const bool visible)
{
	dataLabel.at(index)->setVisible(visible);
	dataComboName.at(index)->setVisible(visible);
	dataSpinNumBytes.at(index)->setVisible(visible);
	dataCheckBox.at(index)->setVisible(visible);
}


//! Creates and configures an entire row of widgets in the data layout.
//! @param index Index of the row (0-based) to create controls in
//! @see updateColumnList()
void Window::dataRowCreate(const int index)
{
	dataLabel.append(new QLabel(QString::number(index + 1)));
	dataComboName.append(new QComboBox());
	dataComboName.at(index)->setEditable(true);
	dataSpinNumBytes.append(new QSpinBox());
	dataSpinNumBytes.at(index)->setRange(1, maxColumnBytes);
	dataCheckBox.append(new QCheckBox());
	dataLayout->addWidget(dataLabel.at(index));
	dataLayout->addWidget(dataComboName.at(index));
	dataLayout->addWidget(dataSpinNumBytes.at(index));
	connect(dataSpinNumBytes.at(index), SIGNAL(valueChanged(int)),
			this, SLOT(updateDisplay()));
	dataLayout->addWidget(dataCheckBox.at(index));
	connect(dataComboName.at(index),
			SIGNAL(editTextChanged(const QString&)), this,
			SLOT(filterColumnName(const QString&)));
}


//! Sets up the default file dialog box.
//! @see Window()
void Window::openFileDialog()
{
	QString fileURI = QFileDialog::getOpenFileName(
			this, tr("Open data file"), comboInfile->currentText(),
			tr("All Files (*.*);;Data files (*.dat *.bin)"));
	QFileInfo fInfo(fileURI);
	if(fInfo.exists()) {
		int dupeIndex = comboInfile->findText(
				fileURI, Qt::MatchFixedString | Qt::MatchCaseSensitive);
		if(dupeIndex >= 0) comboInfile->removeItem(dupeIndex);
		comboInfile->insertItem(0, QDir::convertSeparators(fileURI));
		comboInfile->setCurrentIndex(0);
	}
}


//! Computes the sum of bytes in the columns of any one row of input data.
//! @returns the number of bytes
//! @see infileNumberRows()
//! @see dataToCsv()
int Window::rowDataSize()
{
	int accumulator = 0, columns = spinColumns->value();
	for(int index = 0; index < columns; ++index)
		accumulator += dataSpinNumBytes.at(index)->value();
	return accumulator;
}


//! Computes number of rows in input file based on its size and bytes per row.
//! @returns The number of rows
//! @see rowDataSize()
//! @see dataToCsv()
//! @see updateInfileRowsDisplay()
//! @see updateStatusBarFileStats()
//! @see infileRowLimitDivisor()
quint64 Window::infileNumberRows()
{
	quint64 rows;
	QFile infile(comboInfile->currentText().trimmed());
	if(!infile.open(QIODevice::ReadOnly))
		rows = 0;
	else {
		int rowSize = this->rowDataSize();
		quint64 fileSize = infile.size();
		rows = fileSize / rowSize;
		if(fileSize % rowSize != 0) rows += 1; // Don't ignore partial rows
		infile.close();
	}
	return rows;
}


//! Opens file dialog box to determine the file in which to save processed data
//! @see mainLayoutCreateConnections()
void Window::saveFileDialog()
{
	QString fileURI = QFileDialog::getSaveFileName(
			this, tr("Save as..."), comboOutfile->currentText(),
			tr("Comma-separated values file (*.csv *.txt);; All files (*.* )"));
	// If file is already in list, delete it and re-insert at the top.
	int dupeIndex = comboOutfile->findText(
			fileURI, Qt::MatchFixedString | Qt::MatchCaseSensitive);
	if(dupeIndex >= 0) comboOutfile->removeItem(dupeIndex);
	comboOutfile->insertItem(0, QDir::convertSeparators(fileURI));
	comboOutfile->setCurrentIndex(0);
}

/*
000000000000AA80  Correct value

00000000000080AA  Raw data before qbswap
AA80000000000000  After qbswap
000000000000AA80  Bit shifted 8 - bytecount * 8 bits.
*/

//! Opens both input and output files for processing.
//! @param infile Unassociated QFile object representing input file.
//! @param outfile Unassociated QFile object representing output file.
//! @returns True if there was an error opening either one, false otherwise.
//! @see dataToCsv()
bool Window::csvOpenFiles(QFile &infile, QFile &outfile)
{
	bool errorState = false;
	infile.setFileName(comboInfile->currentText());
	if(!infile.open(QIODevice::ReadOnly)) {
		statusBarMessage->setText(tr("Cannot open data file for reading."));
		errorState = true;
	}
	else {
		outfile.setFileName(comboOutfile->currentText());
		if(!outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			statusBarMessage->setText(tr(
					"Cannot open output file for writing."));
			errorState = true;
		}
		else {
			QFileInfo ifinfo(infile);
			QFileInfo ofinfo(outfile);
			if(ifinfo.canonicalFilePath() == ofinfo.canonicalFilePath()) {
				statusBarMessage->setText(tr(
						"Input and output files must not be the same file!"));
				errorState = true;
			}
		}
	}
	return errorState;
}


//! Writes the first row of the output file -- the names of each column.
//! @param ts QTextStream object already associated with an output file
//! @returns True if column names were written, false otherwise
//! @see dataToCsv()
bool Window::csvWriteColumnNames(QTextStream &ts)
{
	bool retval = false;
	for(int index = 0; index < spinColumns->value(); ++index) {
		if(dataComboName.at(index)->isHidden()) break;
		else {
			ts << dataComboName.at(index)->currentText().trimmed() << ',';
			retval = true;
		}
	}
	ts << endl;
	return retval;
}

//! Processes all data in one row of input & writes appropriate data to output
//! @param ts QTextStream already associated with an output file
//! @param infile QFile object representing input (raw data) file
//! @param colCount Interpret data in infile as having this many "columns"
//! @param colSize Array of column sizes with colSize[n] = bytes in column n
//! @param byteSwap Swap endianness of source data. True or false.
//! @param vMin If device outputs between 1.1v and 5.5v, this is the 1.1v
//! @param vMax If device outputs between 1.1v and 5.5v, this is the 5.1v
//! @see dataToCsv()
void inline Window::csvProcessRow(QTextStream &ts, QFile &infile, int colCount,
								  QByteArray &colSize, bool byteSwap,
								  double vMin, double vMax)
{
	static raw2quint64 raw;
	raw.ui64 = 0;
	for(int col = 0; col < colCount; ++col) { // For each column in this row
		int readError = infile.read(raw.bytes, colSize[col]);
		if(readError == -1) {
			statusBarMessage->setText(tr("Error reading data file."));
			return; // TODO: Get rid of multiple returns
		}
		if(byteSwap) raw.ui64 = qbswap(raw.ui64) >> ((8 - colSize[col]) << 3);
		// TODO: Get rid of this check against a GUI checkbox
		if(dataCheckBox.at(col)->isChecked()) ts << raw.ui64 << ',';
		else {
			double colVal = rawIntToVoltage(raw.ui64, colSize[col], vMin, vMax);
			ts << qSetRealNumberPrecision(15) << colVal << ',';
		}
	}
	ts << endl;
}


//! Controller function to convert input file data to an output .CSV file
//! @see mainLayoutCreateConnections()
void Window::dataToCsv()
{
	QFile infile, outfile;
	bool errorOpeningEitherFile = csvOpenFiles(infile,  outfile);

	if(!errorOpeningEitherFile) {
		int colCount = spinColumns->value();
		int rowSize = rowDataSize();
		quint64 rowsOutput = 0;
		quint64 rowLimit = comboRowLimit->currentText().toULongLong();
		quint64 divCount = infileRowLimitDivisor();
		bool byteSwap = checkBoxEndian->isChecked();
		bool cancelled = false;
		double vMin = minVoltage->value();
		double vMax = maxVoltage->value();
		QTextStream ts(&outfile);
		if(checkBoxWriteColNames->isChecked())
			if(csvWriteColumnNames(ts)) rowsOutput += 1;

		QByteArray colSize(colCount, '\n');
		for(int index = 0; index < colCount; ++index)
			colSize[index] = dataSpinNumBytes.at(index)->value();

		quint64 pMax = infileNumberRows();
		if(rowLimit > 0 && rowLimit < pMax) pMax = rowLimit;
		QProgressDialog progress("Saving CSV file...", "Cancel", 0, pMax, this);
		progress.setModal(true);

		statusBarMessage->setText(tr("Processing data file..."));
		while(!infile.atEnd()) {
//		while(rowsOutput <= pMax) {
			if(rowLimit > 0 && rowsOutput >= rowLimit) break;

			// Updating the status bar for every line processed is too slow!
			if(rowsOutput % 1024 == 0) {
				progress.setValue(rowsOutput);
				if(progress.wasCanceled()) {
					cancelled = true;
					statusBarMessage->setText(tr("Processing cancelled."));
					outfile.remove();
					break;
				}
			}
			csvProcessRow(ts, infile, colCount, colSize, byteSwap, vMin, vMax);
			++rowsOutput;
			if(divCount > 1)
				infile.seek(infile.pos() + (rowSize * (divCount - 1)));
		}
		infile.close();
		outfile.close();
		if(!cancelled) statusBarMessage->setText(tr("Processing complete."));
		if(checkBoxOpenWhenDone->isChecked()) openFileWithAssociatedProgram();
	}
}


//! Converts a raw unsigned integer value range [0, 2^numBits] to a voltage.
//! Assumes that vMax > vMin
//! @param value One piece of the raw uninterpreted data from the source file
//! @param numBytes The size of value in number of bytes
//! @param vMin If device outputs between 1.1v and 5.5v, this is the 1.1v
//! @param vMax If device outputs between 1.1v and 5.5v, this is the 5.1v
//! @returns The computed voltage in the range [vMin, vMax]
//! @see dataToCsv()
inline double Window::rawIntToVoltage(const quint64 value, const int numBytes,
									 const double vMin, const double vMax) const
{
	double range = vMax - vMin;
	// Corrected bug reported by Riley Pack, 26 Jan 2010
	//	quint64 maxVal = 1 << (numBytes << 3); // pow(2,(bits in numBytes))
	quint64 maxVal = (1 << (numBytes << 3)) -1; // pow(2,(bits in numBytes))

	return (value / (maxVal / range)) + vMin;
}


//! Opens a file or URI with an applications the host operating system suggests.
//! @see dataToCsv()
void Window::openFileWithAssociatedProgram() const
{
	QString filePath("file:///");
	filePath += comboOutfile->currentText();
	QUrl fileURI(filePath, QUrl::TolerantMode);
	QDesktopServices::openUrl(fileURI);
}


//! Reads program settings from the data structures in the Config class
//! @returns false on any error, true otherwise
//! @see Window()
bool Window::importSettings()
{
	bool retval = config->xmlRead(configFileURI, "charles_n_burns-data_parser");
	if(!retval) statusBarMessage->setText("Unable to read config file.");
	retval = config->xmlParse();
	if(!retval) statusBarMessage->setText("Unable to parse config file.");

	if(! config->pathlistInfile.isEmpty())
		comboInfile->addItems(config->pathlistInfile);
	if(! config->pathlistOutfile.isEmpty())
		comboOutfile->addItems(config->pathlistOutfile);

	checkBoxOpenWhenDone->setChecked(config->boxOpen);
	comboRowLimit->insertItem(0, QString::number(
			getUint64(config->limitRows.trimmed())));
	comboRowLimit->setCurrentIndex(0);

	int colCount = config->colCount;

	if(this->spinColumns->value() < colCount) spinColumns->setValue(colCount);
	for(int index = 0; index < colCount; ++index) {
		if(config->colNames.size() > index)
			dataComboName.at(index)->addItems(config->colNames.at(index));
		if(config->colBoxChecked.size() > index)
			dataCheckBox.at(index)->setChecked(config->colBoxChecked.at(index));
		if(config->colBytes.size() > index)
			dataSpinNumBytes.at(index)->setValue(config->colBytes.at(index));
	}
	spinColumns->setValue(config->colCount);
	return retval;
}


//! Writes current program state to data structures in Config class
//! @see closeEvent()
void Window::exportSettings() const
{
	config->clear();
	int index;

	config->pathlistInfile.append(comboInfile->currentText());
	for(index = 0; index < comboInfile->count(); ++index) {
		config->pathlistInfile.append(comboInfile->itemText(index));
	}
	config->pathlistInfile.removeDuplicates();

	config->pathlistOutfile.append(comboOutfile->currentText());
	for(index = 0; index < comboOutfile->count(); ++index) {
		config->pathlistOutfile.append(comboOutfile->itemText(index));
	}
	config->pathlistOutfile.removeDuplicates();

	config->boxOpen = checkBoxOpenWhenDone->isChecked();

	if(comboRowLimit->count() > comboRowLimitDefaultItemCount) {
		config->limitRows = comboRowLimit->currentText();
	}
	config->colCount = spinColumns->value();

	QStringList sl;
	for(index = 0; index < spinColumns->value(); ++index) { // For each column
		QComboBox *cBox = dataComboName.at(index);
		sl.append(cBox->currentText()); // Add the currently displayed text
		// For each name in this combobox
		for(int name = 0; name < cBox->count(); ++name) {
			sl.append(cBox->itemText(name));
		}
		sl.removeDuplicates();
		config->colNames.append(sl);
		sl.clear();
		config->colBoxChecked.append(dataCheckBox.at(index)->isChecked());
		config->colBytes.append(dataSpinNumBytes.at(index)->value());
	}
}


//! Grabs digits from a string. Stops when non-digit is reached or at maxDigits
//! @param text A QString of characters which theoretically contain a number
//! @param maxDigits The max number of individual numeric characters to extract
//! @returns the integer whose digits were collected from the string.
//! @see importSettings()
quint64 Window::getUint64(const QString &text, const quint8 maxDigits) const
{
	int limit = (text.size() > maxDigits) ? maxDigits : text.size();
	int index;
	for(index = 0; index < limit; ++index) if(!text.at(index).isDigit()) break;
	return text.left(index).toULongLong();
}


//!	Blocks disruptive characters from being entered in a column name field.
//! This function directly changes the text in the GUI combobox controls.
//! @param text The string to insert into the output CSV file after filtering
//! @see dataRowCreate()
void Window::filterColumnName(const QString &text)
{
	QString filtered = text;
	// The following characters cause problems with CSV and so are banned: \",
	filtered.remove(QRegExp("[\\\\\\\",]"));
	if(text.length() != filtered.length()) {
		// To find out which combobox is being typed in, we have to request
		// the sender. It arrives as a QObject, which we cast to a QComboBox
		QComboBox * cBox = reinterpret_cast<QComboBox*>(QObject::sender());
		cBox->setEditText(filtered);
	}
}


//! This slot updates to on-screen stats (number of rows in current file, etc.)
//! @see mainLayoutCreateConnections()
//! @see createDataLayout()
//! @see dataRowCreate()
void Window::updateDisplay()
{
	this->updateInfileRowsDisplay();
	this->updateStatusBarFileStats();
}


//! Updates the display of the number of data rows in the current input file.
//! @see updateDisplay()
void Window::updateInfileRowsDisplay()
{
	QString display;
	int addRows = 0;
	quint64 numRows = infileNumberRows();
	if(numRows < 1) display = "Unknown # rows";
	else {
		if(checkBoxWriteColNames->isChecked()) addRows = 1;
		display = QString("/ %1 rows").arg(numRows + addRows);
	}
	infileRowsDisplay->setText(display);
}


//! Updates the status bar with statistics regarding current file vs. row limit.
//! @see updateDisplay()
void Window::updateStatusBarFileStats()
{
	quint64 rowLimit = comboRowLimit->currentText().toULongLong();
	quint64 rowDivide = infileRowLimitDivisor();
	QString display = defaultStatusMessage;
	if(rowLimit < infileNumberRows() && rowLimit != 0)
		display = QString("Row limit %1: Keeping 1 in %2 rows."
						  ).arg(rowLimit).arg(rowDivide);
	statusBarMessage->setText(display);
}


//! Ensures only digits are accepted into comboRowLimit, for integer validity
//! comboRowLimit displays individual row limits for several office suites.
//! This function removes text such as (Excel 2007+) and grabs just the number.
//! @see mainLayoutCreateConnections()
void Window::filterLimitRowsName(const QString &text)
{
	int indexNonInt = text.indexOf(QRegExp("[^0-9]"));
	QString filtered = text.left(indexNonInt);
	if(text.length() != filtered.length()) comboRowLimit->setEditText(filtered);
}


//! Slot called when minVoltage is changed. Ensures maxVoltage > minVoltage
//! newValue The new minimum voltage value from the min voltage spinbox
//! @see createAdvFeaturesLayout()
void Window::minVoltageChanged(double newValue)
{
	if(newValue > (maxVoltage->value() - minVoltageDifference)) {
		maxVoltage->setValue(newValue + minVoltageDifference);
	}
}


//! Slot called when maxVoltage is changed. Ensures minVoltage < maxVoltage
//! newValue The new maximum voltage value from the max voltage spinbox
//! @see createAdvFeaturesLayout()
void Window::maxVoltageChanged(double newValue)
{
	if(newValue < minVoltage->value() + minVoltageDifference) {
		minVoltage->setValue(newValue - minVoltageDifference);
	}
}


//! Slot which open's a url on system's default web browser
//! @param url The URI to open in the web browser, such as www.formortals.com
//! @see window()
//! @see createStatusBar()
void Window::openSystemWebBrowser(QString uri)
{
	QDesktopServices::openUrl(uri);
}


//! Accepts a drag over the main window if and only if it's a list of files.
void Window::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasUrls()) event->acceptProposedAction();
}


//! Sets comboInfile text to the first in a file list dropped on the main window
void Window::dropEvent(QDropEvent *event)
{
	QFileInfo fInfo;
	QList<QUrl> urls = event->mimeData()->urls();
	if(! urls.isEmpty()) {
		QString fileURI = urls.first().toLocalFile();
		if(! fileURI.isEmpty()) {
			fInfo.setFile(fileURI);
			if(fInfo.isFile()) {
				comboInfile->insertItem(0, QDir::toNativeSeparators(fileURI));
				comboInfile->setCurrentIndex(0);
			}
		}
	}
}


//! Called when program is closed. Saves settings and writes them to disk.
void Window::closeEvent(QCloseEvent *event)
{
	exportSettings();
	config->xmlWrite(this->configFileURI);
	event->accept();
}
