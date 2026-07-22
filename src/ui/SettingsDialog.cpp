#include "SettingsDialog.h"
#include "backend/ConfigManager.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QVBoxLayout>
SettingsDialog::SettingsDialog(ConfigManager *c,QWidget *p):QDialog(p),config_(c){setWindowTitle(tr("Settings"));
 auto *l=new QVBoxLayout(this); auto add=[&](QCheckBox *&b,const QString&t,const char*k,bool d){b=new QCheckBox(t,this);b->setChecked(config_->value(k,d).toBool());l->addWidget(b);};
 add(startMinimized_,tr("Start minimized"),"startMinimized",false);add(startLogin_,tr("Start on login"),"startOnLogin",false);add(closeTray_,tr("Close to tray"),"closeToTray",true);add(rememberMonitor_,tr("Remember last monitor"),"rememberMonitor",true);add(rememberGpu_,tr("Remember last GPU"),"rememberGpu",true);add(restoreExit_,tr("Restore previous value on exit"),"restoreOnExit",false);add(debugLog_,tr("Enable debug logging"),"debugLogging",false);auto*profileLabel=new QLabel(tr("Process profiles (one process=preset per line):"),this);profileLabel->setWordWrap(true);l->addWidget(profileLabel);profiles_=new QPlainTextEdit(this);QStringList lines;for(const auto&v:config_->value("processProfiles",QJsonArray{}).toArray()){auto o=v.toObject();lines<<o.value("process").toString()+"="+o.value("preset").toString();}profiles_->setPlainText(lines.join('\n'));l->addWidget(profiles_);
 auto *buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,this);connect(buttons,&QDialogButtonBox::accepted,this,&SettingsDialog::accept);connect(buttons,&QDialogButtonBox::rejected,this,&QDialog::reject);l->addWidget(buttons);}
void SettingsDialog::accept(){config_->setValue("startMinimized",startMinimized_->isChecked());config_->setValue("startOnLogin",startLogin_->isChecked());config_->setValue("closeToTray",closeTray_->isChecked());config_->setValue("rememberMonitor",rememberMonitor_->isChecked());config_->setValue("rememberGpu",rememberGpu_->isChecked());config_->setValue("restoreOnExit",restoreExit_->isChecked());config_->setValue("debugLogging",debugLog_->isChecked());QJsonArray rules;for(const auto&line:profiles_->toPlainText().split('\n')){int i=line.indexOf('=');if(i>0&&!line.mid(i+1).trimmed().isEmpty())rules.append(QJsonObject{{"process",line.left(i).trimmed()},{"preset",line.mid(i+1).trimmed()}});}config_->setValue("processProfiles",rules);QString e;if(!config_->setAutostart(startLogin_->isChecked(),&e)||!config_->save(&e)){QMessageBox::critical(this,tr("Settings error"),e);return;}QDialog::accept();}
