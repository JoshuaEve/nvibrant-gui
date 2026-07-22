#include "AboutDialog.h"
#include <QApplication>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QVBoxLayout>
AboutDialog::AboutDialog(const QString &v,QWidget*p):QDialog(p){setWindowTitle(tr("About nVibrant GUI"));auto*l=new QVBoxLayout(this);auto*icon=new QLabel(this);icon->setPixmap(QIcon(":/icons/nvibrant-gui.svg").pixmap(72,72));icon->setAlignment(Qt::AlignCenter);l->addWidget(icon);auto*text=new QLabel(tr("<h2>nVibrant GUI %1</h2><p>Native Qt frontend for nvibrant.</p><p>Qt %2<br>nvibrant %3</p><p>GPL-3.0-or-later</p>").arg(NVIBRANT_GUI_VERSION,qVersion(),v),this);text->setAlignment(Qt::AlignCenter);text->setWordWrap(true);l->addWidget(text);auto*github=new QPushButton(tr("Open GitHub"),this);connect(github,&QPushButton::clicked,this,[]{QDesktopServices::openUrl(QUrl(NVIBRANT_GUI_GITHUB));});l->addWidget(github);auto*b=new QDialogButtonBox(QDialogButtonBox::Close,this);connect(b,&QDialogButtonBox::rejected,this,&QDialog::reject);l->addWidget(b);}
