#pragma once

#include <QDialog>
#include "ui_InfoDialog.h"

class InfoDialog : public QDialog
{
	Q_OBJECT

public:
	InfoDialog(QObject *parent = NULL);
	~InfoDialog();

private:
	Ui::InfoDialog ui;
};
