#include "InfoDialog.h"

InfoDialog::InfoDialog(QObject *parent)
	: QDialog(0, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
	ui.setupUi(this);
	this->show();
}

InfoDialog::~InfoDialog()
{
}
