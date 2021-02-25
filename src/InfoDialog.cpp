#include "InfoDialog.h"

InfoDialog::InfoDialog(QObject *parent)
	: QDialog()
{
	ui.setupUi(this);
	this->show();
}

InfoDialog::~InfoDialog()
{
}
