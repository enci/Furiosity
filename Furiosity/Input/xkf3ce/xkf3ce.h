#ifndef XKF3CE_H
#define XKF3CE_H

#ifdef _WIN32
#include <pstdint.h>
#pragma warning (disable:4099) // C/C++ boundary
#define XKF3CE_DLL_IMPORT __declspec(dllimport)
#define XKF3CE_DLL_EXPORT __declspec(dllexport)
#else
#include <stdint.h>
#define XKF3CE_DLL_IMPORT
#define XKF3CE_DLL_EXPORT __attribute__((visibility("default")))
#endif

#ifdef BUILD_XKF3CE
#define XKF3CE_EXPORT XKF3CE_DLL_EXPORT
#else
#define XKF3CE_EXPORT XKF3CE_DLL_IMPORT
#endif

/* Keep this in sync with android's hardware/sensors.h */
#define XKF3CE_DATA_TYPE_ACCELEROMETER			1
#define XKF3CE_DATA_TYPE_MAGNETOMETER			2
#define XKF3CE_DATA_TYPE_GYROSCOPE			4
#define XKF3CE_DATA_TYPE_MAGNETOMETER_UNCALIBRATED	14
#define XKF3CE_DATA_TYPE_GYROSCOPE_UNCALIBRATED		16

/* Xsens specific */
#define XKF3CE_DATA_TYPE_DQDV				2000

#define XKF3CE_PROFILE_NAVIGATION	0x01
#define XKF3CE_PROFILE_GAME		0x02

struct xkf3ce;
struct xkf3ce_results;
struct xkf3ce_input_data;

#ifdef __cplusplus
extern "C" {
#endif

XKF3CE_EXPORT struct xkf3ce *xkf3ce_create_filter(float lat, float lon, float alt, int year, int month, int day);
XKF3CE_EXPORT void xkf3ce_destroy_filter(struct xkf3ce *xkf);

XKF3CE_EXPORT void xkf3ce_enable_filter_profile(struct xkf3ce *xkf, int profile, int enable);

XKF3CE_EXPORT void xkf3ce_set_location(struct xkf3ce *xkf, float lat, float lon, float alt, int year, int month, int day);

XKF3CE_EXPORT int xkf3ce_one_filter_step(struct xkf3ce *xkf, const struct xkf3ce_input_data *input);

XKF3CE_EXPORT void xkf3ce_get_results(const struct xkf3ce *xkf, struct xkf3ce_results *results);

XKF3CE_EXPORT int xkf3ce_save_state(const struct xkf3ce *xkf, float *state);
XKF3CE_EXPORT void xkf3ce_restore_state(struct xkf3ce *xkf, const float *state);

XKF3CE_EXPORT void xkf3ce_get_version(int *major, int *minor, int *revision);

XKF3CE_EXPORT void xkf3ce_set_hardware_configuration(struct xkf3ce *xkf, const char *hardware_file);

#ifdef __cplusplus
}
#endif

#define XKF3CE_FLAG_ACC_CLIP_X		(1 << 0)
#define XKF3CE_FLAG_ACC_CLIP_Y		(1 << 1)
#define XKF3CE_FLAG_ACC_CLIP_Z		(1 << 2)
#define XKF3CE_FLAG_GYR_CLIP_X		(1 << 3)
#define XKF3CE_FLAG_GYR_CLIP_Y		(1 << 4)
#define XKF3CE_FLAG_GYR_CLIP_Z		(1 << 5)

/*! \brief A structure containing aiding flags and pointers to data as input for xkf3ce */
struct xkf3ce_input_data {
	/*! \brief The type of the data */
	int type;
	/*! \brief The time at which the sample was taken */
	int64_t timestamp;
	union {
		/*! \brief A pointer to the data
		 *
		 * Use this field to point to data of types
		 *
		 * - XKF3CE_DATA_TYPE_ACCELEROMETER
		 * - XKF3CE_DATA_TYPE_MAGNETOMETER
		 * - XKF3CE_DATA_TYPE_GYROSCOPE
		 * - XKF3CE_DATA_TYPE_MAGNETOMETER_UNCALIBRATED
		 * - XKF3CE_DATA_TYPE_GYROSCOPE_UNCALIBRATED
		 *
		 * The vector should contain three values according to [X,Y,Z]
		 */
		float data[3];
		struct {
			/*! \brief A pointer to dQ or orientation increment data
			 *
			 * This field should point to a quaternion that follows the
			 * convention of [1,i,j,k]: element 0 should be the scalar,
			 * elements 1..3 should be the imaginary vector.
			 */
			float dq[4];
			/*! \brief A pointer to dV or velocity increment data
			 *
			 * The vector should contain three values according to [X,Y,Z]
			 */
			float dv[3];
			/*! \brief The clip flag field */
			uint16_t clip_flags;
			/*! \brief Indicate that a new gyro bias is being applied */
			uint8_t new_bias_applied;
		} sdi_data;
	};
};

/* sensor accuracy. Keep this in sync with android's hardware/sensor.h */
#define XKF3CE_UNRELIABLE	0
#define XKF3CE_ACCURACY_LOW	1
#define XKF3CE_ACCURACY_MEDIUM	2
#define XKF3CE_ACCURACY_HIGH	3

/*! \brief A structure containing filter profile specific results */
struct xkf3ce_profile_results {
	/*! \brief Quaternion representing the orientation of the device
	 *
	 * The orientation is expressed with respect to the Local coordinates.
	 * First element (index 0) is the real part, second to fourth
	 * elements the imaginary parts. The quaternion is normalized.
	 *
	 * The quaternion is a unit quaternion according to [W X Y Z].
	 */
	float q_ls[4];

	/*! \brief The user acceleration
	 *
	 * The user acceleration is the acceleration
	 * (second derivative of position, gravity subtracted) of the user expressed in
	 * [m/s2] in the Local frame.
	 *
	 * The user acceleration is expressed in the Local frame in [m/s2] according to [aX aY aZ].
	 */
	float acc_l[3];

	/*! \brief the accuracy of the output
	 *
	 * The accuracy of the output expressed in the Android qualifications.
	 *
	 * \see XKF3CE_UNRELIABLE
	 * \see XKF3CE_ACCURACY_LOW
	 * \see XKF3CE_ACCURACY_MEDIUM
	 * \see XKF3CE_ACCURACY_HIGH
	 */
	uint8_t accuracy;

	/*! \brief Set to non-zero if output for the filter profile is available
	 */
	uint8_t available;
};

/*! \brief A structure containing all filter results */
struct xkf3ce_results {
	struct xkf3ce_profile_results navigation;
	struct xkf3ce_profile_results gaming;

	/*! \brief The corrected accelerometer data
	 *
	 * The corrected accelerometer data is the input accelerometer data in Sensor coordinate frame.
	 * Any calibration applied by the filter is reflected in this vector.
	 *
	 * The corrected accelerometer data is expressed in the Sensor frame in [m/s2] according to [aX aY aZ].
	 */

	float acc_s[3];
	/*! \brief The corrected magnetic field
	 *
	 * The corrected magnetic field data is the input magnetic field data in Sensor coordinate frame.
	 * Any calibration applied by the filter is reflected in this vector.
	 *
	 * The corrected magnetic field data is expressed in the Sensor frame in [uT] according to [mX mY mZ].
	 */

	float mag_s[3];
	/*! \brief The corrected gyroscope data
	 *
	 * The corrected gyroscope data is the input gyroscope data in Sensor coordinate frame.
	 * Any calibration applied by the filter is reflected in this vector.
	 *
	 * The corrected gyroscope data is expressed in the Sensor frame in [rad/s] according to [gX gY gZ].
	 */
	float omega_s[3];

	/*! \brief The up-direction vector in Sensor coordinate frame
	 *
	 * Unit vector giving the
	 * direction of the vertical, expressed in Sensor coordinates. The Sensor (device)
	 * coordinate system with respect to the device, with Y pointing \em upwards along
	 * the long edge of the device and Z pointing towards the user, from the screen.
	 *
	 * The up-vector is a unit-vector in the Sensor frame according to [X Y Z].
	 */
	float up[3];
};

#endif // XKF3CE_H
