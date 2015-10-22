// MESSAGE VSKYLINE_OSD_PILOT_NAME PACKING

#define MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME 156

typedef struct __mavlink_vskyline_osd_pilot_name_t
{
 uint32_t pilot_name; ///< Pilot name OSD.
} mavlink_vskyline_osd_pilot_name_t;

#define MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN 4
#define MAVLINK_MSG_ID_156_LEN 4

#define MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_CRC 32
#define MAVLINK_MSG_ID_156_CRC 32



#define MAVLINK_MESSAGE_INFO_VSKYLINE_OSD_PILOT_NAME { \
	"VSKYLINE_OSD_PILOT_NAME", \
	1, \
	{  { "pilot_name", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_vskyline_osd_pilot_name_t, pilot_name) }, \
         } \
}


/**
 *  Pack a vskyline_osd_pilot_name message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param pilot_name Pilot name OSD.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_vskyline_osd_pilot_name_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint32_t pilot_name)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN];
	_mav_put_uint32_t(buf, 0, pilot_name);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN);
#else
	mavlink_vskyline_osd_pilot_name_t packet;
	packet.pilot_name = pilot_name;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN);
#endif
}

/**
 *  Pack a vskyline_osd_pilot_name message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param pilot_name Pilot name OSD.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_vskyline_osd_pilot_name_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint32_t pilot_name)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN];
	_mav_put_uint32_t(buf, 0, pilot_name);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN);
#else
	mavlink_vskyline_osd_pilot_name_t packet;
	packet.pilot_name = pilot_name;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN);
#endif
}

/**
 *  Encode a vskyline_osd_pilot_name struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param vskyline_osd_pilot_name C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_vskyline_osd_pilot_name_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_vskyline_osd_pilot_name_t* vskyline_osd_pilot_name)
{
	return mavlink_msg_vskyline_osd_pilot_name_pack(system_id, component_id, msg, vskyline_osd_pilot_name->pilot_name);
}

/**
 *  Encode a vskyline_osd_pilot_name struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param vskyline_osd_pilot_name C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_vskyline_osd_pilot_name_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_vskyline_osd_pilot_name_t* vskyline_osd_pilot_name)
{
	return mavlink_msg_vskyline_osd_pilot_name_pack_chan(system_id, component_id, chan, msg, vskyline_osd_pilot_name->pilot_name);
}

/**
 *  Send a vskyline_osd_pilot_name message
 * @param chan MAVLink channel to send the message
 *
 * @param pilot_name Pilot name OSD.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_vskyline_osd_pilot_name_send(mavlink_channel_t chan, uint32_t pilot_name)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN];
	_mav_put_uint32_t(buf, 0, pilot_name);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN);
#endif
#else
	mavlink_vskyline_osd_pilot_name_t packet;
	packet.pilot_name = pilot_name;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME, (const char *)&packet, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME, (const char *)&packet, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_vskyline_osd_pilot_name_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t pilot_name)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, pilot_name);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN);
#endif
#else
	mavlink_vskyline_osd_pilot_name_t *packet = (mavlink_vskyline_osd_pilot_name_t *)msgbuf;
	packet->pilot_name = pilot_name;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME, (const char *)packet, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME, (const char *)packet, MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE VSKYLINE_OSD_PILOT_NAME UNPACKING


/**
 *  Get field pilot_name from vskyline_osd_pilot_name message
 *
 * @return Pilot name OSD.
 */
static inline uint32_t mavlink_msg_vskyline_osd_pilot_name_get_pilot_name(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 *  Decode a vskyline_osd_pilot_name message into a struct
 *
 * @param msg The message to decode
 * @param vskyline_osd_pilot_name C-struct to decode the message contents into
 */
static inline void mavlink_msg_vskyline_osd_pilot_name_decode(const mavlink_message_t* msg, mavlink_vskyline_osd_pilot_name_t* vskyline_osd_pilot_name)
{
#if MAVLINK_NEED_BYTE_SWAP
	vskyline_osd_pilot_name->pilot_name = mavlink_msg_vskyline_osd_pilot_name_get_pilot_name(msg);
#else
	memcpy(vskyline_osd_pilot_name, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_VSKYLINE_OSD_PILOT_NAME_LEN);
#endif
}
