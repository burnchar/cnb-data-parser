/*
	Name        : Window.h
	Author      : Charles N. Burns (charlesnburns|gmail|com / burnchar|isu|edu)
	Date        : August 2009
	License     : GPL3. See license.txt or www.gnu.org/licenses/gpl-3.0.txt
	Requirements: Qt 4, data to parse, any spreadsheet program.
	Notes       : Best viewed with tab width 4.
	Description : Header file to define Window class.
*/


#ifndef WINDOW_H
#define WINDOW_H

#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QDragEnterEvent>
#include <QUrl>
#include <QApplication>
#include <QFileDialog>
#include <QDesktopServices>
#include <QtEndian>
#include <QtDebug>
#include <QtGui/QStatusBar>
#include <QProgressDialog>
#include "Config.h"

//const QString defaultStatusMessage("© 2009 Charles N. Burns, RockOn! 2009 - for <a href=\"http://spacegrant.colorado.edu/rockon/\">RockOn! Workshop</a>");
const QString defaultStatusMessage("© 2009 Charles N. Burns");

const quint8 maxColumnBytes = 8;
const float minVoltageDifference = 1.0f;


//! Used in the processing out output data
union raw2quint64 {
	char bytes[8];
	quint64 ui64;
};


class Window : public QWidget
{
	Q_OBJECT

	// Main UI
	QGridLayout *mainLayout;
	QPushButton *buttonBrowseInput, *buttonBrowseOutput, *buttonProcessData;
	QComboBox *comboInfile, *comboOutfile, *comboRowLimit;
	QCheckBox *checkBoxOpenWhenDone, *checkBoxWriteColNames, *checkBoxEndian;

	QDoubleSpinBox *minVoltage, *maxVoltage;

	QSpinBox *spinColumns;
	QScrollArea *scrollArea;
	QStatusBar *statusBar;
	QLabel *statusBarMessage, *infileRowsDisplay;


	// Data columns information (column name, # bytes, counter or non-counter)
	QGroupBox *dataGroupBox;
	QHBoxLayout *advFeaturesLayout;
	QGridLayout *dataLayout;
	QList<QLabel*> dataLabel;
	QList<QSpinBox*> dataSpinNumBytes;
	QList<QComboBox*> dataComboName;
	QList<QCheckBox*> dataCheckBox;

	// Function prototypes
	void createMainLayout();
	void mainLayoutAllocateWidgets();
	void mainLayoutConfigureWidgets();
	void mainLayoutAddWidgets();

	void createStatusBar();
	void createDataLayout();
	void createAdvFeaturesLayout();
	bool importSettings();
	void exportSettings() const;
	void dataRowSetVisible(const int index, const bool visible);
	void dataRowCreate(const int index);
	void mainLayoutCreateConnections() const;
	double rawIntToVoltage(const quint64 value, const int numBytes,
						   const double vMin = 0.0,
						   const double vMax = 5.0) const;
	int rowDataSize();
	quint64 infileRowLimitDivisor();
	quint64 infileNumberRows();
	void closeEvent(QCloseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void openFileWithAssociatedProgram() const;
	quint64 getUint64(const QString &text = "",
					  const quint8 maxDigits = 19) const;

	bool csvOpenFiles(QFile &infile, QFile &outfile);
	bool csvWriteColumnNames(QTextStream &ts);
	void csvProcessRow(QTextStream &ts, QFile &infile, int columnCount,
					   QByteArray &columnSize, bool swapEndian,
					   double vMin, double vMax);

	// Private member variables
	quint8 comboRowLimitDefaultItemCount;
	quint8 maxComboItems;
//	static const quint8 maxColumnBytes = 8;
//	static const float minVoltageDifference = 1.0f;
		QString configFileURI;
	Config *config;

private slots:
	void openFileDialog();
	void saveFileDialog();
	void updateColumnList();
	void updateInfileRowsDisplay();
	void updateDisplay();
	void updateStatusBarFileStats();
	void dataToCsv();
	void filterColumnName(const QString &text);
	void filterLimitRowsName(const QString &text);
	void maxVoltageChanged(double newValue);
	void minVoltageChanged(double newValue);
	void openSystemWebBrowser(QString uri);

public:
	Window();
	~Window() {}
};

#endif // WINDOW_H
