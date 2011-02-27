/**
 * @file ecp_g_neuron_generator.h
 * @brief Header file for neuron_generator class
 * @author Tomasz Bem (mebmot@wp.pl)
 * @author Rafal Tulwin (rtulwin@stud.elka.pw.edu.pl)
 * @ingroup neuron
 * @date 02.07.2010
 */

#ifndef ECP_G_NEURON_GENERATOR_H_
#define ECP_G_NEURON_GENERATOR_H_

#include "base/lib/impconst.h"
#include "base/lib/com_buf.h"
#include "neuron_sensor.h"
#include "base/lib/mrmath/mrmath.h"

#include "generator/ecp/ecp_g_teach_in.h"

#include "../../generator/ecp/velocity_profile_calculator/bang_bang_profile.h"
#include "../../base/lib/trajectory_pose/bang_bang_trajectory_pose.h"
#include <vector>

namespace mrrocpp{
namespace ecp{
namespace common{
namespace generator{

/**
 * @brief Generator working with VSP and neural networks.
 * @details Trajectory generator is intended to work with external VSP module.
 * Generator generates trajectory basing on one coordinates given from VSP for
 * next 5 macro steps, therefor it has to interpolate each macro step between
 * current and given after 5 macro steps. At the beginning it takes first
 * coordinates of a trajectory in first step and later on at the end of 5th
 * macro step and before next 1st macro step current position of a robot is
 * sent to VSP along with other movement assesment data, and VSP returns
 * position for next 5 macro steps, which are processed and interpolated in
 * next step method of the generator.
 */
class neuron_generator: public common::generator::generator{
	private:

		/**
		 * @brief Communication manager between VSP and MRROC++.
		 * @details Neuron sensor from the point of view of this class is used
		 * to send current position and assesment data and receive next postion
		 */
		ecp_mp::sensor::neuron_sensor *neuron_sensor;

		/**
		 * @brief Matrix to which the current position of the robot is written.
		 */
		lib::Homog_matrix actual_position_matrix;

		/**
		 * @brief Matrix to which the new position for the robot is written.
		 */
		lib::Homog_matrix position_matrix;

		/**
		 * @brief Temporary angle vector used while reading the current robot position.
		 */
		lib::Xyz_Angle_Axis_vector angle_axis_vector;

		/**
		 * @brief Current position of the robot.
		 */
		double actual_position[6];

                std::vector<double> actual_position_vect;

		/**
		 * @brief Desired position received from vsp.
		 */
		double desired_position[6];

                std::vector<double> desired_position_vect;

		/**
		 * @brief Array filled with coordinates send to the robot.
		 */
		double position[6];

		/**
		 * @brief Provides information whether start breaking or not.
		 * @details If true, generator tries to break in each axis until robot
		 * stops.
		 */
		bool breaking;

		/**
		 * @brief Current velocity in all axes.
		 */
		double v[6];

		/**
		 * @brief Maximal allowed acceleration in all axes.
		 */
		double a_max[6];

                std::vector<double> a_max_vect;

		/**
		 * @brief Maximal allowed velocity in all axes.
		 */
		double v_max[6];

                std::vector<double> v_max_vect;

		/**
		 * @brief Node counter used for breaking.
		 * @details When generator knows that it should start breaking, it
		 * calculates how many macro steps its going to need to stop. The
		 * member describes in which breaking macro step according to
		 * calculated one it is.
		 */
		int breaking_node;

		/**
		 * @brief Informs whether desired position is reach or not.
		 * @details Set to true if robot reached desired position for each axes
		 * for currently processed trajectory.
		 */
		bool reached[6];

		/**
		 * @brief Motion direction.
		 */
		int k[6];

		/**
		 * @brief Acceleration while breaking
		 */
		double a;

		/**
		 * @brief Distance covered in the set of five macrosteps.
		 */
		double s[6];

                std::vector<double> s_vect;

		/**
		 * @brief Set to true if change of the direction is needed.
		 */
		bool change[6];

		/**
		 * @brief Current position error.
		 */
		double u[6];

		/**
		 * @brief Time of a macrostep.
		 */
		double t;

		/**
		 * @brief Flag set to true if final breaking begins.
		 */
		bool almost_reached[6];

		/**
		 * @brief Flag set to true if breaking without overshoot is possible.
		 */
		bool breaking_possible[6];

		/**
		 * @brief Difference between last and last but one position.
		 * @details Calculated on the VSP side.
		 */
		double normalized_vector[3];

		/**
		 * @brief Value of an overshoot.
		 * @details Overshoot is a maximal distance from the perpendicular hyperplane to
		 * normalized vector.
		 */
		double overshoot;

		/**
		 * @brief Number of macro steps between consequtive data.
		 */
		uint8_t macroSteps;

                bool first_breaking_node;

                void calculate();

                bool check_if_able_to_break(double s, double v, double a_max);

                void clear_vectors();

                std::vector<std::vector <ecp_mp::common::trajectory_pose::bang_bang_trajectory_pose> >pose_vector_list;
                /**
                 * Position vector iterator.
                 */
                std::vector <ecp_mp::common::trajectory_pose::bang_bang_trajectory_pose>::iterator pose_vector_iterator;

            ecp::common::generator::velocity_profile_calculator::bang_bang_profile vpc;

                bool load_trajectory_pose(std::vector<ecp_mp::common::trajectory_pose::bang_bang_trajectory_pose> & pose_vector,
                    const std::vector<double> & coordinates, lib::MOTION_TYPE motion_type, lib::ECP_POSE_SPECIFICATION pose_spec,
                    const std::vector<double> & v, const std::vector<double> & a, const std::vector<double> & start_pos,
                    const std::vector<double> & s);

        public:
		neuron_generator(common::task::task& _ecp_task);
		virtual ~neuron_generator();
		virtual bool first_step();
		virtual bool next_step();

		double get_breaking_time();
		double * get_position();
		double get_overshoot();
		void reset();
};

}//generator
}//common
}//ecp
}//mrrocpp

#endif /* ECP_G_NEURON_GENERATOR_H_ */
