#ifndef WGT_TEACHING_H
#define WGT_TEACHING_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QDockWidget>
#include "ui_wgt_teaching.h"
#include "../wgt_base.h"

namespace mrrocpp {
namespace ui {
namespace common {
class Interface;
}
}
}

class wgt_teaching : public wgt_base
{
Q_OBJECT

public:
	wgt_teaching(mrrocpp::ui::common::Interface& _interface, QWidget *parent = 0);
	~wgt_teaching();

	Ui::wgt_teachingClass * get_ui();

	void hideEvent(QHideEvent * event);

private:
	Ui::wgt_teachingClass* ui;

private slots:

	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();

};

#endif // WGT_TEACHING_H
