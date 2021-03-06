/* --------------------------------------------------------------------- */
/*                          SERVO_GROUP Process                          */
// ostatnia modyfikacja - styczen 2005
/* --------------------------------------------------------------------- */

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "base/lib/typedefs.h"
#include "base/lib/impconst.h"
#include "base/lib/com_buf.h"

// Klasa edp_sarkofag_effector.
//#include "base/edp/sarkofag/edp_sarkofag_effector.h"
#include "base/edp/reader.h"
#include "base/edp/servo_gr.h"
// Klasa hardware_interface.
//#include "base/edp/sarkofag/hi_sarkofag.h"
// Klasa servo_buffer.
#include "robot/sarkofag/regulator_sarkofag.h"

#include "robot/sarkofag/edp_e_sarkofag.h"

#include "robot/sarkofag/kinematic_model_sarkofag.h"

namespace mrrocpp {
namespace edp {
namespace sarkofag {

/*-----------------------------------------------------------------------*/
NL_regulator_8_sarkofag::NL_regulator_8_sarkofag(uint8_t _axis_number, uint8_t reg_no, uint8_t reg_par_no, double aa, double bb0, double bb1, double k_ff, common::motor_driven_effector &_master, common::REG_OUTPUT _reg_output) :
		NL_regulator(_axis_number, reg_no, reg_par_no, aa, bb0, bb1, k_ff, _master, _reg_output)
{
// pzredefiniwoane na potrzeby eksperymentþów
	strict_current_mode = true;
	desired_velocity_limit = 0.5;
	reg_state = next_reg_state = prev_reg_state = lib::GRIPPER_START_STATE;
	sum_of_currents = current_index = 0;
	display = 0;
	deviation = 0;
	deviation_integral = 0;
	m = master.config.value <float>("weight", "[edp_sarkofag]");
	dl = master.config.value <float>("weight_pos", "[edp_sarkofag]");
	an = 0.105- 0.1*0.105;
	q = 9.81;
	pozycja_joint = 0;


	// Konstruktor regulatora konkretnego
	// Przy inicjacji nalezy dopilnowac, zeby numery algorytmu regulacji oraz zestawu jego parametrow byly
	// zgodne z faktycznie przekazywanym zestawem parametrow inicjujacych.
}
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
uint8_t NL_regulator_8_sarkofag::compute_set_value(void)
{


	//m - masa,
	//p - przekladania silnika,
	//an - parametr silnika,
	//dl - odleglosc od punktu obrotu do punktu srodka ciezkosci,
	//q - przyspieszenie ziemskie.
	//zero - pozycja pi/2 w motors,

	//	static long iteracja = 0;

	// algorytm regulacji dla serwomechanizmu
	// position_increment_old - przedostatnio odczytany przyrost polozenie
	//                         (delta y[k-2] -- mierzone w impulsach)
	// position_increment_new - ostatnio odczytany przyrost polozenie
	//                         (delta y[k-1] -- mierzone w impulsach)
	// step_old_pulse               - poprzednia wartosc zadana dla jednego kroku
	//                         regulacji (przyrost wartosci zadanej polozenia --
	//                         delta r[k-2] -- mierzone w impulsach)
	// step_new               - nastepna wartosc zadana dla jednego kroku
	//                         regulacji (przyrost wartosci zadanej polozenia --
	//                         delta r[k-1] -- mierzone w radianach)
	// set_value_new          - wielkosc kroku do realizacji przez HIP
	//                         (wypelnienie PWM -- u[k]): czas trwania jedynki
	// set_value_old          - wielkosc kroku do realizacji przez HIP
	//                         (wypelnienie PWM -- u[k-1]): czas trwania jedynki
	// set_value_very_old     - wielkosc kroku do realizacji przez HIP
	//                         (wypelnienie PWM -- u[k-2]): czas trwania jedynki

	//double step_new_pulse; // nastepna wartosc zadana dla jednego kroku regulacji
	// (przyrost wartosci zadanej polozenia --
	// delta r[k-1] -- mierzone w impulsach)
	uint8_t alg_par_status; // okresla prawidlowosc numeru algorytmu regulacji
	// i zestawu jego parametrow

	alg_par_status = common::ALGORITHM_AND_PARAMETERS_OK;

	// double root_position_increment_new=position_increment_new;

	// przeliczenie radianow na impulsy
	// step_new_pulse = step_new*IRP6_POSTUMENT_INC_PER_REVOLUTION/(2*M_PI); // ORIGINAL
	step_new_pulse = step_new * INC_PER_REVOLUTION / (2 * M_PI);
	//position_increment_new=position_increment_new/AXE_0_TO_5_POSTUMENT_TO_TRACK_RATIO;

	//	printf("(%d) step_new_pulse: %f, position_increment_new: %f\n", ++iteracja, step_new_pulse,
	//			position_increment_new);

	/*
	 if (!aaa)
	 if ( (fabs(step_new_pulse) < 0.0001) && (fabs(position_increment_new ) > 1) ) {
	 aaa++;
	 }
	 */
	// if (aaa > 0 && aaa < 30 ) {
	//  cprintf("O1: svn=%4.0lf svo=%4.0lf svvo=%4.0lf de=%4.0lf deo=%4.0lf snp=%4.0lf pin=%4.0lf pio=%4.0lf\n",set_value_new,set_value_old,
	//  set_value_very_old, delta_eint, delta_eint_old, step_new_pulse,position_increment_new,position_increment_old);
	//  cprintf("O1: snp=%4.0lf pin=%4.0lf pio=%4.0lf L=%4x U=%4x\n", step_new_pulse,position_increment_new,position_increment_old,md.robot_status[0].adr_224,md.robot_status[0].adr_226);
	//  aaa++;
	//  if (aaa == 9) aaa=0;
	// }
	/* // by Y - bez sensu
	 // Jesli rzeczywisty przyrost jest wiekszy od dopuszczalnego
	 if (fabs(position_increment_new) > common::MAX_INC)
	 position_increment_new = position_increment_old;
	 */

	// kumulacja przyrostu polozenia w tym makrokroku // ORIGINAL
	// pos_increment_new_sum += position_increment_new*POSTUMENT_TO_TRACK_RATIO;
	// servo_pos_increment_new_sum += position_increment_new*POSTUMENT_TO_TRACK_RATIO; // by Y
	// kumulacja przyrostu polozenia w tym makrokroku
	// pos_increment_new_sum += root_position_increment_new;
	// servo_pos_increment_new_sum += root_position_increment_new;// by Y
	// Przyrost calki uchybu
	delta_eint = delta_eint_old + 1.008 * (step_new_pulse - position_increment_new)
			- 0.992 * (step_old_pulse - position_increment_old);

	// if (fabs(step_new_pulse) > 70.0) {
	//  cprintf("snp = %lf   pin = %lf\n",step_new_pulse, position_increment_new);
	// }

	// if (fabs(delta_eint) > 50.0) {
	//  cprintf("%4.0lf ",delta_eint);
	// }

	// Sprawdzenie czy numer algorytmu lub zestawu parametrow sie zmienil?
	// Jezeli tak, to nalezy dokonac uaktualnienia numerow (ewentualnie wykryc niewlasciwosc numerow)
	if ((current_algorithm_no != algorithm_no) || (current_algorithm_parameters_no != algorithm_parameters_no)) {
		switch (algorithm_no)
		{
			case 0: // algorytm nr 0
				switch (algorithm_parameters_no)
				{
					case 0: // zestaw parametrow nr 0
						current_algorithm_parameters_no = algorithm_parameters_no;
						current_algorithm_no = algorithm_no;
						a = 0.4152;
						b0 = 0.9017 * 1.5;
						b1 = 0.7701 * 1.5;
						k_feedforward = 0.35;
						//////////////

						break;
					case 1: // zestaw parametrow nr 1
						current_algorithm_parameters_no = algorithm_parameters_no;
						current_algorithm_no = algorithm_no;
						a = 0.4152;
						b0 = 0.9017 * 1.0;
						b1 = 0.7701 * 1.0;
						k_feedforward = 0;
						break;
					default: // blad => przywrocic stary algorytm i j stary zestaw parametrow
						algorithm_no = current_algorithm_no;
						algorithm_parameters_no = current_algorithm_parameters_no;
						alg_par_status = common::UNIDENTIFIED_ALGORITHM_PARAMETERS_NO;
						break;
				}
				break;
			case 1: // algorytm nr 1
				switch (algorithm_parameters_no)
				{
					case 0: // zestaw parametrow nr 0
						current_algorithm_parameters_no = algorithm_parameters_no;
						current_algorithm_no = algorithm_no;
						a = 0;
						b0 = 0;
						b1 = 0;
						k_feedforward = 0;


												//////////////

						break;
					case 1: // zestaw parametrow nr 1
						current_algorithm_parameters_no = algorithm_parameters_no;
						current_algorithm_no = algorithm_no;
						a = 0;
						b0 = 0;
						b1 = 0;
						k_feedforward = 0;
						break;
					default: // blad - nie ma takiego zestawu parametrow dla tego algorytmu
						// => przywrocic stary algorytm i j stary zestaw parametrow
						algorithm_no = current_algorithm_no;
						algorithm_parameters_no = current_algorithm_parameters_no;
						alg_par_status = common::UNIDENTIFIED_ALGORITHM_PARAMETERS_NO;
						break;
				} // end: switch (algorithm_parameters_no)
				break;
			case 2:
				switch (algorithm_parameters_no)
				{
					case 0: // zestaw parametrow nr 0
						current_algorithm_parameters_no = algorithm_parameters_no;
						current_algorithm_no = algorithm_no;
						a = 0;
						b0 = 0;
						b1 = 0;
						k_feedforward = 0;
						break;
					case 1: // zestaw parametrow nr 1
						current_algorithm_parameters_no = algorithm_parameters_no;
						current_algorithm_no = algorithm_no;

						break;
					default: // blad - nie ma takiego zestawu parametrow dla tego algorytmu
						// => przywrocic stary algorytm i j stary zestaw parametrow
						algorithm_no = current_algorithm_no;
						algorithm_parameters_no = current_algorithm_parameters_no;
						alg_par_status = common::UNIDENTIFIED_ALGORITHM_PARAMETERS_NO;
						break;
				} // end: switch (algorithm_parameters_no)
				break;
			default: // blad - nie ma takiego algorytmu
				// => przywrocic stary algorytm i j stary zestaw parametrow
				algorithm_no = current_algorithm_no;
				algorithm_parameters_no = current_algorithm_parameters_no;
				alg_par_status = common::UNIDENTIFIED_ALGORITHM_NO;
				break;
		}; // end: switch (algorithm_no)
	}

	double kp = 1;
	double ki = 0.05;

	a = 0;
	b0 = kp * (1 + ki);
	b1 = kp;
	max_output_current = 20000;


	double motor;

	switch (algorithm_no)
	{
		case 0: {

			// algorytm nr 0
			current_reg_kp = 100;
			// obliczenie nowej wartosci wypelnienia PWM algorytm PD + I
			set_value_new = (1 + a) * set_value_old - a * set_value_very_old + b0 * delta_eint - b1 * delta_eint_old;
			//set_value_new = set_value_old + kp * deviation + ki * delta_eint;
		}
			break;

		case 1: // algorytm nr 1
		{
			//przekladnia
			pozycja_joint = master.servo_current_joints[0];//(reg_abs_current_motor_pos - m_kin.synchro_motor_position) / m_kin.gear ;

			current_reg_kp = 1;
			set_value_new = -(cos(pozycja_joint )*(dl*m*q))/(an*m_kin.gear) * 1000;

			std::cout << "\tvalue: " << set_value_new << "\tjoint.: " << pozycja_joint << std::endl;

			// przepisanie zadanej wartosci pradu

			//set_value_new = master.sb->command.sb_instruction_.arm.pf_def.desired_torque_or_current[0];
				//printf("set_value_new case 1: %f\n", set_value_new);
		}
			break;

		case 2: {
			double szym_kp = 40;
			double szym_ki = 5;
			current_reg_kp = 100;
			abs_pos_dev = reg_abs_desired_motor_pos - reg_abs_current_motor_pos;
			abs_pos_dev_int = abs_pos_dev_int_old + abs_pos_dev * lib::EDP_STEP;

			// przyrost calki uchybu, czyli calka w tym kroku minus calka w poprzednim, obie znamy
			delta_eint = abs_pos_dev_int - abs_pos_dev_int_old;
			// przyrost uchybu polozenia wzgledem poprzedniego kroku
			delta_abs_pos_dev = abs_pos_dev_old - abs_pos_dev;

			// 								czlon proporcjonalny		czlon calkujacy
			//set_value_new = set_value_old + (kp * delta_abs_pos_dev) + (ki * delta_eint);
			set_value_new = szym_kp * abs_pos_dev + szym_ki * abs_pos_dev_int;
		}
			break;

		default: // w tym miejscu nie powinien wystapic blad zwiazany z
			// nieistniejacym numerem algorytmu
			set_value_new = 0; // zerowe nowe sterowanie
			break;
	}

	compute_set_value_final_computations();

	return alg_par_status;

}
/*-----------------------------------------------------------------------*/

} // namespace sarkofag
} // namespace edp
} // namespace mrrocpp
