#include "backend/ConfigManager.h"
#include "backend/NvibrantController.h"
#include "backend/PresetManager.h"
#include "ui/MainWindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QSystemTrayIcon>
int main(int argc,char**argv){QApplication app(argc,argv);QCoreApplication::setOrganizationName("nvibrant-gui");QCoreApplication::setApplicationName("nvibrant-gui");QCoreApplication::setApplicationVersion(NVIBRANT_GUI_VERSION);app.setQuitOnLastWindowClosed(false);QCommandLineParser parser;parser.setApplicationDescription("Native Qt frontend for nvibrant");parser.addHelpOption();parser.addVersionOption();parser.addOption({"minimized","Start hidden in the system tray"});parser.process(app);ConfigManager config;QString error;if(!config.load(&error))qWarning().noquote()<<"Could not load configuration:" << error;PresetManager presets;if(!presets.load(&error))qWarning().noquote()<<"Could not load presets:" << error;NvibrantController controller;MainWindow window(&controller,&config,&presets);const bool minimized=parser.isSet("minimized")||config.value("startMinimized",false).toBool();if(!minimized||!QSystemTrayIcon::isSystemTrayAvailable())window.show();return app.exec();}
