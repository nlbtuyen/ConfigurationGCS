// MESSAGE VSKYLINE_OSD_FLIGHT_CONTROL_SETUP PACKING

#define MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP 158

typedef struct __mavlink_vskyline_osd_flight_control_setup_t
{
 uint32_t camera_name; ///< Camera name.
 uint32_t stype_name; ///< Stype name.
 uint16_t motor_kv; ///< Motor KV.
 uint16_t battery_mah; ///< Battery mAh.
 uint8_t profile_num; ///< Profile num OSD.
 uint8_t battery_cell; ///< Battery cell.
} mavlink_vskyline_osd_flight_control_setup_t;

#define MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN 14
#define MAVLINK_MSG_ID_158_LEN 14

#define MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_CRC 143
#define MAVLINK_MSG_ID_158_CRC 143



#define MAVLINK_MESSAGE_INFO_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP { \
	"VSKYLINE_OSD_FLIGHT_CONTROL_SETUP", \
	6, \
	{  { "camera_name", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_vskyline_osd_flight_control_setup_t, camera_name) }, \
         { "stype_name", NULL, MAVLINK_TYPE_UINT32_T, 0, 4, offsetof(mavlink_vskyline_osd_flight_control_setup_t, stype_name) }, \
         { "motor_kv", NULL, MAVLINK_TYPE_UINT16_T, 0, 8, offsetof(mavlink_vskyline_osd_flight_control_setup_t, motor_kv) }, \
         { "battery_mah", NULL, MAVLINK_TYPE_UINT16_T, 0, 10, offsetof(mavlink_vskyline_osd_flight_control_setup_t, battery_mah) }, \
         { "profile_num", NULL, MAVLINK_TYPE_UINT8_T, 0, 12, offsetof(mavlink_vskyline_osd_flight_control_setup_t, profile_num) }, \
         { "battery_cell", NULL, MAVLINK_TYPE_UINT8_T, 0, 13, offsetof(mavlink_vskyline_osd_flight_control_setup_t, battery_cell) }, \
         } \
}


/**
 *  Pack a vskyline_osd_flight_control_setup message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param profile_num Profile num OSD.
 * @param motor_kv Motor KV.
 * @param battery_cell Battery cell.
 * @param battery_mah Battery mAh.
 * @param camera_name Camera name.
 * @param stype_name Stype name.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_vskyline_osd_flight_control_setup_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint8_t profile_num, uint16_t motor_kv, uint8_t battery_cell, uint16_t battery_mah, uint32_t camera_name, uint32_t stype_name)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN];
	_mav_put_uint32_t(buf, 0, camera_name);
	_mav_put_uint32_t(buf, 4, stype_name);
	_mav_put_uint16_t(buf, 8, motor_kv);
	_mav_put_uint16_t(buf, 10, battery_mah);
	_mav_put_uint8_t(buf, 12, profile_num);
	_mav_put_uint8_t(buf, 13, battery_cell);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN);
#else
	mavlink_vskyline_osd_flight_control_setup_t packet;
	packet.camera_name = camera_name;
	packet.stype_name = stype_name;
	packet.motor_kv = motor_kv;
	packet.battery_mah = battery_mah;
	packet.profile_num = profile_num;
	packet.battery_cell = battery_cell;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN);
#endif
}

/**
 *  Pack a vskyline_osd_flight_control_setup message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param profile_num Profile num OSD.
 * @param motor_kv Motor KV.
 * @param battery_cell Battery cell.
 * @param battery_mah Battery mAh.
 * @param camera_name Camera name.
 * @param stype_name Stype name.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_vskyline_osd_flight_control_setup_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint8_t profile_num,uint16_t motor_kv,uint8_t battery_cell,uint16_t battery_mah,uint32_t camera_name,uint32_t stype_name)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN];
	_mav_put_uint32_t(buf, 0, camera_name);
	_mav_put_uint32_t(buf, 4, stype_name);
	_mav_put_uint16_t(buf, 8, motor_kv);
	_mav_put_uint16_t(buf, 10, battery_mah);
	_mav_put_uint8_t(buf, 12, profile_num);
	_mav_put_uint8_t(buf, 13, battery_cell);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN);
#else
	mavlink_vskyline_osd_flight_control_setup_t packet;
	packet.camera_name = camera_name;
	packet.stype_name = stype_name;
	packet.motor_kv = motor_kv;
	packet.battery_mah = battery_mah;
	packet.profile_num = profile_num;
	packet.battery_cell = battery_cell;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN);
#endif
}

/**
 *  Encode a vskyline_osd_flight_control_setup struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param vskyline_osd_flight_control_setup C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_vskyline_osd_flight_control_setup_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_vskyline_osd_flight_control_setup_t* vskyline_osd_flight_control_setup)
{
	return mavlink_msg_vskyline_osd_flight_control_setup_pack(system_id, component_id, msg, vskyline_osd_flight_control_setup->profile_num, vskyline_osd_flight_control_setup->motor_kv, vskyline_osd_flight_control_setup->battery_cell, vskyline_osd_flight_control_setup->battery_mah, vskyline_osd_flight_control_setup->camera_name, vskyline_osd_flight_control_setup->stype_name);
}

/**
 *  Encode a vskyline_osd_flight_control_setup struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param vskyline_osd_flight_control_setup C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_vskyline_osd_flight_control_setup_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_vskyline_osd_flight_control_setup_t* vskyline_osd_flight_control_setup)
{
	return mavlink_msg_vskyline_osd_flight_control_setup_pack_chan(system_id, component_id, chan, msg, vskyline_osd_flight_control_setup->profile_num, vskyline_osd_flight_control_setup->motor_kv, vskyline_osd_flight_control_setup->battery_cell, vskyline_osd_flight_control_setup->battery_mah, vskyline_osd_flight_control_setup->camera_name, vskyline_osd_flight_control_setup->stype_name);
}

/**
 *  Send a vskyline_osd_flight_control_setup message
 * @param chan MAVLink channel to send the message
 *
 * @param profile_num Profile num OSD.
 * @param motor_kv Motor KV.
 * @param battery_cell Battery cell.
 * @param battery_mah Battery mAh.
 * @param camera_name Camera name.
 * @param stype_name Stype name.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_vskyline_osd_flight_control_setup_send(mavlink_channel_t chan, uint8_t profile_num, uint16_t motor_kv, uint8_t battery_cell, uint16_t battery_mah, uint32_t camera_name, uint32_t stype_name)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN];
	_mav_put_uint32_t(buf, 0, camera_name);
	_mav_put_uint32_t(buf, 4, stype_name);
	_mav_put_uint16_t(buf, 8, motor_kv);
	_mav_put_uint16_t(buf, 10, battery_mah);
	_mav_put_uint8_t(buf, 12, profile_num);
	_mav_put_uint8_t(buf, 13, battery_cell);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN);
#endif
#else
	mavlink_vskyline_osd_flight_control_setup_t packet;
	packet.camera_name = camera_name;
	packet.stype_name = stype_name;
	packet.motor_kv = motor_kv;
	packet.battery_mah = battery_mah;
	packet.profile_num = profile_num;
	packet.battery_cell = battery_cell;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP, (const char *)&packet, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP, (const char *)&packet, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_vskyline_osd_flight_control_setup_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t profile_num, uint16_t motor_kv, uint8_t battery_cell, uint16_t battery_mah, uint32_t camera_name, uint32_t stype_name)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, camera_name);
	_mav_put_uint32_t(buf, 4, stype_name);
	_mav_put_uint16_t(buf, 8, motor_kv);
	_mav_put_uint16_t(buf, 10, battery_mah);
	_mav_put_uint8_t(buf, 12, profile_num);
	_mav_put_uint8_t(buf, 13, battery_cell);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN);
#endif
#else
	mavlink_vskyline_osd_flight_control_setup_t *packet = (mavlink_vskyline_osd_flight_control_setup_t *)msgbuf;
	packet->camera_name = camera_name;
	packet->stype_name = stype_name;
	packet->motor_kv = motor_kv;
	packet->battery_mah = battery_mah;
	packet->profile_num = profile_num;
	packet->battery_cell = battery_cell;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP, (const char *)packet, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP, (const char *)packet, MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE VSKYLINE_OSD_FLIGHT_CONTROL_SETUP UNPACKING


/**
 *  Get field profile_num from vskyline_osd_flight_control_setup message
 *
 * @return Profile num OSD.
 */
static inline uint8_t mavlink_msg_vskyline_osd_flight_control_setup_get_profile_num(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  12);
}

/**
 *  Get field motor_kv from vskyline_osd_flight_control_setup message
 *
 * @return Motor KV.
 */
static inline uint16_t mavlink_msg_vskyline_osd_flight_control_setup_get_motor_kv(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  8);
}

/**
 *  Get field battery_cell from vskyline_osd_flight_control_setup message
 *
 * @return Battery cell.
 */
static inline uint8_t mavlink_msg_vskyline_osd_flight_control_setup_get_battery_cell(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  13);
}

/**
 *  Get field battery_mah from vskyline_osd_flight_control_setup message
 *
 * @return Battery mAh.
 */
static inline uint16_t mavlink_msg_vskyline_osd_flight_control_setup_get_battery_mah(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  10);
}

/**
 *  Get field camera_name from vskyline_osd_flight_control_setup message
 *
 * @return Camera name.
 */
static inline uint32_t mavlink_msg_vskyline_osd_flight_control_setup_get_camera_name(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 *  Get field stype_name from vskyline_osd_flight_control_setup message
 *
 * @return Stype name.
 */
static inline uint32_t mavlink_msg_vskyline_osd_flight_control_setup_get_stype_name(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  4);
}

/**
 *  Decode a vskyline_osd_flight_control_setup message into a struct
 *
 * @param msg The message to decode
 * @param vskyline_osd_flight_control_setup C-struct to decode the message contents into
 */
static inline void mavlink_msg_vskyline_osd_flight_control_setup_decode(const mavlink_message_t* msg, mavlink_vskyline_osd_flight_control_setup_t* vskyline_osd_flight_control_setup)
{
#if MAVLINK_NEED_BYTE_SWAP
	vskyline_osd_flight_control_setup->camera_name = mavlink_msg_vskyline_osd_flight_control_setup_get_camera_name(msg);
	vskyline_osd_flight_control_setup->stype_name = mavlink_msg_vskyline_osd_flight_control_setup_get_stype_name(msg);
	vskyline_osd_flight_control_setup->motor_kv = mavlink_msg_vskyline_osd_flight_control_setup_get_motor_kv(msg);
	vskyline_osd_flight_control_setup->battery_mah = mavlink_msg_vskyline_osd_flight_control_setup_get_battery_mah(msg);
	vskyline_osd_flight_control_setup->profile_num = mavlink_msg_vskyline_osd_flight_control_setup_get_profile_num(msg);
	vskyline_osd_flight_control_setup->battery_cell = mavlink_msg_vskyline_osd_flight_control_setup_get_battery_cell(msg);
#else
	memcpy(vskyline_osd_flight_control_setup, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_VSKYLINE_OSD_FLIGHT_CONTROL_SETUP_LEN);
#endif
}
