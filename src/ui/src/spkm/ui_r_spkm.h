// -------------------------------------------------------------------------
//                            ui_class.h
// Definicje klasy Ui
//
// Ostatnia modyfikacja: 2010
// -------------------------------------------------------------------------

#ifndef __UI_R_SPKM_H
#define __UI_R_SPKM_H

#include "ui/ui.h"
#include "ui/ui_robot.h"

//
//
// KLASA UiRobotSpkm
//
//


class Ui;
class ui_tfg_and_conv_robot;

class UiRobotSpkm: public UiRobot {
private:

public:

	ui_tfg_and_conv_robot *ui_ecp_robot;

	UiRobotSpkm(Ui& _ui);
	int reload_configuration();
	int manage_interface();
};

#endif

