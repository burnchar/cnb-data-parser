# This file stores configuration settings for the project

SOURCES += main.cpp \
	Window.cpp \
	Config.cpp
HEADERS += Window.h \
	Config.h
QT += xml
RESOURCES = embedded.qrc	# Icons for various supproted spreadsheet programs

#CONFIG += static

static {
	DEFINES += STATIC
	QTPLUGIN += qjpeg qgif  # Statically link image format support
	message("Static build.")
}
