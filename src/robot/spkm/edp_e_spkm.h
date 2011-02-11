/*!
 * \file edp_e_spkm.h
 * \brief File containing the declaration of edp::spkm::effector class.
 *
 * \author yoyek
 * \date 2009
 *
 */

#ifndef __EDP_E_SPKM_H
#define __EDP_E_SPKM_H

#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "base/edp/edp_e_manip.h"
#include "robot/spkm/const_spkm.h"
#include "robot/epos/epos.h"

namespace mrrocpp {
namespace edp {
namespace spkm {

// Klasa reprezentujaca robota IRp-6 na postumencie.
/*!
 * \brief class of EDP SwarmItFix parallel kinematic manipulator
 *
 * It is the base of the head mounted on the mobile base.
 */
class effector : public common::manip_effector
{
private:
	//! Access to the CAN gateway unit
	boost::shared_ptr<epos::epos_access> gateway;

	//! PKM axes
	boost::shared_ptr<epos::epos> axisA, axisB, axisC, axis1, axis2, axis3;

	//! Axes container
	boost::array<epos::epos *, 3> axes;

	static const double Vdefault[6];// = { 5000, 5000, 5000, 5000, 5000, 5000 };
	static const double Adefault[6]; // = { 2000, 2000, 2000, 2000, 2000, 2000 };
	static const double Ddefault[6]; // = { 2000, 2000, 2000, 2000, 2000, 2000 };

protected:
	lib::spkm::cbuffer ecp_edp_cbuffer;
	lib::spkm::rbuffer edp_ecp_rbuffer;

	/*!
	 * \brief method,  creates a list of available kinematic models for spkm effector.
	 *
	 * Here it is parallel manipulator direct and inverse kinematic transform
	 * and motor to joint transform
	 */
	virtual void create_kinematic_models_for_given_robot(void);

public:

	/*!
	 * \brief class constructor
	 *
	 * The attributes are initialized here.
	 */
	effector(lib::configurator &_config);

	/*!
	 * \brief motors synchronisation
	 *
	 * This method synchronises motors of the robots.
	 */
	void synchronise();

	/*!
	 * \brief method to create threads other then EDP master thread.
	 *
	 * Here there is only one extra thread - reader_thread.
	 */
	void create_threads();

	/*!
	 * \brief method to move robot arm
	 *
	 * it chooses the single thread variant from the manip_effector
	 */
	void move_arm(const lib::c_buffer &instruction); // przemieszczenie ramienia

	void get_controller_state(lib::c_buffer &instruction);

	/*!
	 * \brief method to get position of the arm
	 *
	 * Here it calls common::manip_effector::get_arm_position_get_arm_type_switch
	 */
	void get_arm_position(bool read_hardware, lib::c_buffer &instruction); // odczytanie pozycji ramienia

	/*!
	 * \brief method to choose master_order variant
	 *
	 * IHere the single thread variant is chosen
	 */
	void master_order(common::MT_ORDER nm_task, int nm_tryb);

	/*!
	 * \brief method to deserialize part of the reply
	 *
	 * Currently simple memcpy implementation
	 */
	void instruction_deserialization();

	/*!
	 * \brief method to serialize part of the reply
	 *
	 * Currently simple memcpy implementation
	 */
	void reply_serialization();
};

} // namespace spkm
} // namespace edp
} // namespace mrrocpp


#endif
