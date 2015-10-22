// MESSAGE VSKYLINE_PID_SETUP PACKING

#define MAVLINK_MSG_ID_VSKYLINE_PID_SETUP 156

typedef struct __mavlink_vskyline_pid_setup_t
{
 float profile; ///< Profile PID .
 float pid_type; ///< PID type .
 float p_pitch; ///< p pitch .
 float i_pitch; ///< i pitch .
 float d_pitch; ///< d pitch  .
 float rate_pitch; ///< rate pitch .
 float p_roll; ///< p roll .
 float i_roll; ///< i roll .
 float d_roll; ///< d roll .
 float rate_roll; ///< rate roll .
 float p_yaw; ///< p yaw .
 float i_yaw; ///< i yaw .
 float d_yaw; ///< d yaw .
 float rate_yaw; ///< rate yaw .
} mavlink_vskyline_pid_setup_t;

#define MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN 56
#define MAVLINK_MSG_ID_156_LEN 56

#define MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_CRC 91
#define MAVLINK_MSG_ID_156_CRC 91



#define MAVLINK_MESSAGE_INFO_VSKYLINE_PID_SETUP { \
	"VSKYLINE_PID_SETUP", \
	14, \
	{  { "profile", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_vskyline_pid_setup_t, profile) }, \
         { "pid_type", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_vskyline_pid_setup_t, pid_type) }, \
         { "p_pitch", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_vskyline_pid_setup_t, p_pitch) }, \
         { "i_pitch", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_vskyline_pid_setup_t, i_pitch) }, \
         { "d_pitch", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_vskyline_pid_setup_t, d_pitch) }, \
         { "rate_pitch", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_vskyline_pid_setup_t, rate_pitch) }, \
         { "p_roll", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_vskyline_pid_setup_t, p_roll) }, \
         { "i_roll", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_vskyline_pid_setup_t, i_roll) }, \
         { "d_roll", NULL, MAVLINK_TYPE_FLOAT, 0, 32, offsetof(mavlink_vskyline_pid_setup_t, d_roll) }, \
         { "rate_roll", NULL, MAVLINK_TYPE_FLOAT, 0, 36, offsetof(mavlink_vskyline_pid_setup_t, rate_roll) }, \
         { "p_yaw", NULL, MAVLINK_TYPE_FLOAT, 0, 40, offsetof(mavlink_vskyline_pid_setup_t, p_yaw) }, \
         { "i_yaw", NULL, MAVLINK_TYPE_FLOAT, 0, 44, offsetof(mavlink_vskyline_pid_setup_t, i_yaw) }, \
         { "d_yaw", NULL, MAVLINK_TYPE_FLOAT, 0, 48, offsetof(mavlink_vskyline_pid_setup_t, d_yaw) }, \
         { "rate_yaw", NULL, MAVLINK_TYPE_FLOAT, 0, 52, offsetof(mavlink_vskyline_pid_setup_t, rate_yaw) }, \
         } \
}


/**
 *  Pack a vskyline_pid_setup message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param profile Profile PID .
 * @param pid_type PID type .
 * @param p_pitch p pitch .
 * @param i_pitch i pitch .
 * @param d_pitch d pitch  .
 * @param rate_pitch rate pitch .
 * @param p_roll p roll .
 * @param i_roll i roll .
 * @param d_roll d roll .
 * @param rate_roll rate roll .
 * @param p_yaw p yaw .
 * @param i_yaw i yaw .
 * @param d_yaw d yaw .
 * @param rate_yaw rate yaw .
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_vskyline_pid_setup_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       float profile, float pid_type, float p_pitch, float i_pitch, float d_pitch, float rate_pitch, float p_roll, float i_roll, float d_roll, float rate_roll, float p_yaw, float i_yaw, float d_yaw, float rate_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN];
	_mav_put_float(buf, 0, profile);
	_mav_put_float(buf, 4, pid_type);
	_mav_put_float(buf, 8, p_pitch);
	_mav_put_float(buf, 12, i_pitch);
	_mav_put_float(buf, 16, d_pitch);
	_mav_put_float(buf, 20, rate_pitch);
	_mav_put_float(buf, 24, p_roll);
	_mav_put_float(buf, 28, i_roll);
	_mav_put_float(buf, 32, d_roll);
	_mav_put_float(buf, 36, rate_roll);
	_mav_put_float(buf, 40, p_yaw);
	_mav_put_float(buf, 44, i_yaw);
	_mav_put_float(buf, 48, d_yaw);
	_mav_put_float(buf, 52, rate_yaw);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN);
#else
	mavlink_vskyline_pid_setup_t packet;
	packet.profile = profile;
	packet.pid_type = pid_type;
	packet.p_pitch = p_pitch;
	packet.i_pitch = i_pitch;
	packet.d_pitch = d_pitch;
	packet.rate_pitch = rate_pitch;
	packet.p_roll = p_roll;
	packet.i_roll = i_roll;
	packet.d_roll = d_roll;
	packet.rate_roll = rate_roll;
	packet.p_yaw = p_yaw;
	packet.i_yaw = i_yaw;
	packet.d_yaw = d_yaw;
	packet.rate_yaw = rate_yaw;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_VSKYLINE_PID_SETUP;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN);
#endif
}

/**
 *  Pack a vskyline_pid_setup message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param profile Profile PID .
 * @param pid_type PID type .
 * @param p_pitch p pitch .
 * @param i_pitch i pitch .
 * @param d_pitch d pitch  .
 * @param rate_pitch rate pitch .
 * @param p_roll p roll .
 * @param i_roll i roll .
 * @param d_roll d roll .
 * @param rate_roll rate roll .
 * @param p_yaw p yaw .
 * @param i_yaw i yaw .
 * @param d_yaw d yaw .
 * @param rate_yaw rate yaw .
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_vskyline_pid_setup_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           float profile,float pid_type,float p_pitch,float i_pitch,float d_pitch,float rate_pitch,float p_roll,float i_roll,float d_roll,float rate_roll,float p_yaw,float i_yaw,float d_yaw,float rate_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN];
	_mav_put_float(buf, 0, profile);
	_mav_put_float(buf, 4, pid_type);
	_mav_put_float(buf, 8, p_pitch);
	_mav_put_float(buf, 12, i_pitch);
	_mav_put_float(buf, 16, d_pitch);
	_mav_put_float(buf, 20, rate_pitch);
	_mav_put_float(buf, 24, p_roll);
	_mav_put_float(buf, 28, i_roll);
	_mav_put_float(buf, 32, d_roll);
	_mav_put_float(buf, 36, rate_roll);
	_mav_put_float(buf, 40, p_yaw);
	_mav_put_float(buf, 44, i_yaw);
	_mav_put_float(buf, 48, d_yaw);
	_mav_put_float(buf, 52, rate_yaw);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN);
#else
	mavlink_vskyline_pid_setup_t packet;
	packet.profile = profile;
	packet.pid_type = pid_type;
	packet.p_pitch = p_pitch;
	packet.i_pitch = i_pitch;
	packet.d_pitch = d_pitch;
	packet.rate_pitch = rate_pitch;
	packet.p_roll = p_roll;
	packet.i_roll = i_roll;
	packet.d_roll = d_roll;
	packet.rate_roll = rate_roll;
	packet.p_yaw = p_yaw;
	packet.i_yaw = i_yaw;
	packet.d_yaw = d_yaw;
	packet.rate_yaw = rate_yaw;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_VSKYLINE_PID_SETUP;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN);
#endif
}

/**
 *  Encode a vskyline_pid_setup struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param vskyline_pid_setup C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_vskyline_pid_setup_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_vskyline_pid_setup_t* vskyline_pid_setup)
{
	return mavlink_msg_vskyline_pid_setup_pack(system_id, component_id, msg, vskyline_pid_setup->profile, vskyline_pid_setup->pid_type, vskyline_pid_setup->p_pitch, vskyline_pid_setup->i_pitch, vskyline_pid_setup->d_pitch, vskyline_pid_setup->rate_pitch, vskyline_pid_setup->p_roll, vskyline_pid_setup->i_roll, vskyline_pid_setup->d_roll, vskyline_pid_setup->rate_roll, vskyline_pid_setup->p_yaw, vskyline_pid_setup->i_yaw, vskyline_pid_setup->d_yaw, vskyline_pid_setup->rate_yaw);
}

/**
 *  Encode a vskyline_pid_setup struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param vskyline_pid_setup C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_vskyline_pid_setup_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_vskyline_pid_setup_t* vskyline_pid_setup)
{
	return mavlink_msg_vskyline_pid_setup_pack_chan(system_id, component_id, chan, msg, vskyline_pid_setup->profile, vskyline_pid_setup->pid_type, vskyline_pid_setup->p_pitch, vskyline_pid_setup->i_pitch, vskyline_pid_setup->d_pitch, vskyline_pid_setup->rate_pitch, vskyline_pid_setup->p_roll, vskyline_pid_setup->i_roll, vskyline_pid_setup->d_roll, vskyline_pid_setup->rate_roll, vskyline_pid_setup->p_yaw, vskyline_pid_setup->i_yaw, vskyline_pid_setup->d_yaw, vskyline_pid_setup->rate_yaw);
}

/**
 *  Send a vskyline_pid_setup message
 * @param chan MAVLink channel to send the message
 *
 * @param profile Profile PID .
 * @param pid_type PID type .
 * @param p_pitch p pitch .
 * @param i_pitch i pitch .
 * @param d_pitch d pitch  .
 * @param rate_pitch rate pitch .
 * @param p_roll p roll .
 * @param i_roll i roll .
 * @param d_roll d roll .
 * @param rate_roll rate roll .
 * @param p_yaw p yaw .
 * @param i_yaw i yaw .
 * @param d_yaw d yaw .
 * @param rate_yaw rate yaw .
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_vskyline_pid_setup_send(mavlink_channel_t chan, float profile, float pid_type, float p_pitch, float i_pitch, float d_pitch, float rate_pitch, float p_roll, float i_roll, float d_roll, float rate_roll, float p_yaw, float i_yaw, float d_yaw, float rate_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN];
	_mav_put_float(buf, 0, profile);
	_mav_put_float(buf, 4, pid_type);
	_mav_put_float(buf, 8, p_pitch);
	_mav_put_float(buf, 12, i_pitch);
	_mav_put_float(buf, 16, d_pitch);
	_mav_put_float(buf, 20, rate_pitch);
	_mav_put_float(buf, 24, p_roll);
	_mav_put_float(buf, 28, i_roll);
	_mav_put_float(buf, 32, d_roll);
	_mav_put_float(buf, 36, rate_roll);
	_mav_put_float(buf, 40, p_yaw);
	_mav_put_float(buf, 44, i_yaw);
	_mav_put_float(buf, 48, d_yaw);
	_mav_put_float(buf, 52, rate_yaw);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP, buf, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP, buf, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN);
#endif
#else
	mavlink_vskyline_pid_setup_t packet;
	packet.profile = profile;
	packet.pid_type = pid_type;
	packet.p_pitch = p_pitch;
	packet.i_pitch = i_pitch;
	packet.d_pitch = d_pitch;
	packet.rate_pitch = rate_pitch;
	packet.p_roll = p_roll;
	packet.i_roll = i_roll;
	packet.d_roll = d_roll;
	packet.rate_roll = rate_roll;
	packet.p_yaw = p_yaw;
	packet.i_yaw = i_yaw;
	packet.d_yaw = d_yaw;
	packet.rate_yaw = rate_yaw;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP, (const char *)&packet, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP, (const char *)&packet, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_vskyline_pid_setup_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  float profile, float pid_type, float p_pitch, float i_pitch, float d_pitch, float rate_pitch, float p_roll, float i_roll, float d_roll, float rate_roll, float p_yaw, float i_yaw, float d_yaw, float rate_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_float(buf, 0, profile);
	_mav_put_float(buf, 4, pid_type);
	_mav_put_float(buf, 8, p_pitch);
	_mav_put_float(buf, 12, i_pitch);
	_mav_put_float(buf, 16, d_pitch);
	_mav_put_float(buf, 20, rate_pitch);
	_mav_put_float(buf, 24, p_roll);
	_mav_put_float(buf, 28, i_roll);
	_mav_put_float(buf, 32, d_roll);
	_mav_put_float(buf, 36, rate_roll);
	_mav_put_float(buf, 40, p_yaw);
	_mav_put_float(buf, 44, i_yaw);
	_mav_put_float(buf, 48, d_yaw);
	_mav_put_float(buf, 52, rate_yaw);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP, buf, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP, buf, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN);
#endif
#else
	mavlink_vskyline_pid_setup_t *packet = (mavlink_vskyline_pid_setup_t *)msgbuf;
	packet->profile = profile;
	packet->pid_type = pid_type;
	packet->p_pitch = p_pitch;
	packet->i_pitch = i_pitch;
	packet->d_pitch = d_pitch;
	packet->rate_pitch = rate_pitch;
	packet->p_roll = p_roll;
	packet->i_roll = i_roll;
	packet->d_roll = d_roll;
	packet->rate_roll = rate_roll;
	packet->p_yaw = p_yaw;
	packet->i_yaw = i_yaw;
	packet->d_yaw = d_yaw;
	packet->rate_yaw = rate_yaw;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP, (const char *)packet, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP, (const char *)packet, MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE VSKYLINE_PID_SETUP UNPACKING


/**
 *  Get field profile from vskyline_pid_setup message
 *
 * @return Profile PID .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_profile(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  0);
}

/**
 *  Get field pid_type from vskyline_pid_setup message
 *
 * @return PID type .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_pid_type(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  4);
}

/**
 *  Get field p_pitch from vskyline_pid_setup message
 *
 * @return p pitch .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_p_pitch(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  8);
}

/**
 *  Get field i_pitch from vskyline_pid_setup message
 *
 * @return i pitch .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_i_pitch(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  12);
}

/**
 *  Get field d_pitch from vskyline_pid_setup message
 *
 * @return d pitch  .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_d_pitch(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  16);
}

/**
 *  Get field rate_pitch from vskyline_pid_setup message
 *
 * @return rate pitch .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_rate_pitch(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  20);
}

/**
 *  Get field p_roll from vskyline_pid_setup message
 *
 * @return p roll .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_p_roll(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  24);
}

/**
 *  Get field i_roll from vskyline_pid_setup message
 *
 * @return i roll .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_i_roll(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  28);
}

/**
 *  Get field d_roll from vskyline_pid_setup message
 *
 * @return d roll .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_d_roll(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  32);
}

/**
 *  Get field rate_roll from vskyline_pid_setup message
 *
 * @return rate roll .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_rate_roll(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  36);
}

/**
 *  Get field p_yaw from vskyline_pid_setup message
 *
 * @return p yaw .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_p_yaw(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  40);
}

/**
 *  Get field i_yaw from vskyline_pid_setup message
 *
 * @return i yaw .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_i_yaw(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  44);
}

/**
 *  Get field d_yaw from vskyline_pid_setup message
 *
 * @return d yaw .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_d_yaw(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  48);
}

/**
 *  Get field rate_yaw from vskyline_pid_setup message
 *
 * @return rate yaw .
 */
static inline float mavlink_msg_vskyline_pid_setup_get_rate_yaw(const mavlink_message_t* msg)
{
	return _MAV_RETURN_float(msg,  52);
}

/**
 *  Decode a vskyline_pid_setup message into a struct
 *
 * @param msg The message to decode
 * @param vskyline_pid_setup C-struct to decode the message contents into
 */
static inline void mavlink_msg_vskyline_pid_setup_decode(const mavlink_message_t* msg, mavlink_vskyline_pid_setup_t* vskyline_pid_setup)
{
#if MAVLINK_NEED_BYTE_SWAP
	vskyline_pid_setup->profile = mavlink_msg_vskyline_pid_setup_get_profile(msg);
	vskyline_pid_setup->pid_type = mavlink_msg_vskyline_pid_setup_get_pid_type(msg);
	vskyline_pid_setup->p_pitch = mavlink_msg_vskyline_pid_setup_get_p_pitch(msg);
	vskyline_pid_setup->i_pitch = mavlink_msg_vskyline_pid_setup_get_i_pitch(msg);
	vskyline_pid_setup->d_pitch = mavlink_msg_vskyline_pid_setup_get_d_pitch(msg);
	vskyline_pid_setup->rate_pitch = mavlink_msg_vskyline_pid_setup_get_rate_pitch(msg);
	vskyline_pid_setup->p_roll = mavlink_msg_vskyline_pid_setup_get_p_roll(msg);
	vskyline_pid_setup->i_roll = mavlink_msg_vskyline_pid_setup_get_i_roll(msg);
	vskyline_pid_setup->d_roll = mavlink_msg_vskyline_pid_setup_get_d_roll(msg);
	vskyline_pid_setup->rate_roll = mavlink_msg_vskyline_pid_setup_get_rate_roll(msg);
	vskyline_pid_setup->p_yaw = mavlink_msg_vskyline_pid_setup_get_p_yaw(msg);
	vskyline_pid_setup->i_yaw = mavlink_msg_vskyline_pid_setup_get_i_yaw(msg);
	vskyline_pid_setup->d_yaw = mavlink_msg_vskyline_pid_setup_get_d_yaw(msg);
	vskyline_pid_setup->rate_yaw = mavlink_msg_vskyline_pid_setup_get_rate_yaw(msg);
#else
	memcpy(vskyline_pid_setup, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_VSKYLINE_PID_SETUP_LEN);
#endif
}
