// MESSAGE VSKYLINE_OSD_STATUS PACKING

#define MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS 154

typedef struct __mavlink_vskyline_osd_status_t
{
 uint32_t pilot_name; ///< Pilot name OSD.
 uint16_t osd_status; ///< Status OSD.
 uint8_t video_pal; ///< Video pal or ntfc OSD.
} mavlink_vskyline_osd_status_t;

#define MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN 7
#define MAVLINK_MSG_ID_154_LEN 7

#define MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_CRC 198
#define MAVLINK_MSG_ID_154_CRC 198



#define MAVLINK_MESSAGE_INFO_VSKYLINE_OSD_STATUS { \
	"VSKYLINE_OSD_STATUS", \
	3, \
	{  { "pilot_name", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_vskyline_osd_status_t, pilot_name) }, \
         { "osd_status", NULL, MAVLINK_TYPE_UINT16_T, 0, 4, offsetof(mavlink_vskyline_osd_status_t, osd_status) }, \
         { "video_pal", NULL, MAVLINK_TYPE_UINT8_T, 0, 6, offsetof(mavlink_vskyline_osd_status_t, video_pal) }, \
         } \
}


/**
 *  Pack a vskyline_osd_status message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param osd_status Status OSD.
 * @param pilot_name Pilot name OSD.
 * @param video_pal Video pal or ntfc OSD.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_vskyline_osd_status_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint16_t osd_status, uint32_t pilot_name, uint8_t video_pal)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN];
	_mav_put_uint32_t(buf, 0, pilot_name);
	_mav_put_uint16_t(buf, 4, osd_status);
	_mav_put_uint8_t(buf, 6, video_pal);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN);
#else
	mavlink_vskyline_osd_status_t packet;
	packet.pilot_name = pilot_name;
	packet.osd_status = osd_status;
	packet.video_pal = video_pal;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN);
#endif
}

/**
 *  Pack a vskyline_osd_status message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param osd_status Status OSD.
 * @param pilot_name Pilot name OSD.
 * @param video_pal Video pal or ntfc OSD.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_vskyline_osd_status_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint16_t osd_status,uint32_t pilot_name,uint8_t video_pal)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN];
	_mav_put_uint32_t(buf, 0, pilot_name);
	_mav_put_uint16_t(buf, 4, osd_status);
	_mav_put_uint8_t(buf, 6, video_pal);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN);
#else
	mavlink_vskyline_osd_status_t packet;
	packet.pilot_name = pilot_name;
	packet.osd_status = osd_status;
	packet.video_pal = video_pal;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN);
#endif
}

/**
 *  Encode a vskyline_osd_status struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param vskyline_osd_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_vskyline_osd_status_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_vskyline_osd_status_t* vskyline_osd_status)
{
	return mavlink_msg_vskyline_osd_status_pack(system_id, component_id, msg, vskyline_osd_status->osd_status, vskyline_osd_status->pilot_name, vskyline_osd_status->video_pal);
}

/**
 *  Encode a vskyline_osd_status struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param vskyline_osd_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_vskyline_osd_status_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_vskyline_osd_status_t* vskyline_osd_status)
{
	return mavlink_msg_vskyline_osd_status_pack_chan(system_id, component_id, chan, msg, vskyline_osd_status->osd_status, vskyline_osd_status->pilot_name, vskyline_osd_status->video_pal);
}

/**
 *  Send a vskyline_osd_status message
 * @param chan MAVLink channel to send the message
 *
 * @param osd_status Status OSD.
 * @param pilot_name Pilot name OSD.
 * @param video_pal Video pal or ntfc OSD.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_vskyline_osd_status_send(mavlink_channel_t chan, uint16_t osd_status, uint32_t pilot_name, uint8_t video_pal)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN];
	_mav_put_uint32_t(buf, 0, pilot_name);
	_mav_put_uint16_t(buf, 4, osd_status);
	_mav_put_uint8_t(buf, 6, video_pal);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN);
#endif
#else
	mavlink_vskyline_osd_status_t packet;
	packet.pilot_name = pilot_name;
	packet.osd_status = osd_status;
	packet.video_pal = video_pal;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS, (const char *)&packet, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS, (const char *)&packet, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_vskyline_osd_status_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint16_t osd_status, uint32_t pilot_name, uint8_t video_pal)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, pilot_name);
	_mav_put_uint16_t(buf, 4, osd_status);
	_mav_put_uint8_t(buf, 6, video_pal);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN);
#endif
#else
	mavlink_vskyline_osd_status_t *packet = (mavlink_vskyline_osd_status_t *)msgbuf;
	packet->pilot_name = pilot_name;
	packet->osd_status = osd_status;
	packet->video_pal = video_pal;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS, (const char *)packet, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS, (const char *)packet, MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE VSKYLINE_OSD_STATUS UNPACKING


/**
 *  Get field osd_status from vskyline_osd_status message
 *
 * @return Status OSD.
 */
static inline uint16_t mavlink_msg_vskyline_osd_status_get_osd_status(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  4);
}

/**
 *  Get field pilot_name from vskyline_osd_status message
 *
 * @return Pilot name OSD.
 */
static inline uint32_t mavlink_msg_vskyline_osd_status_get_pilot_name(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 *  Get field video_pal from vskyline_osd_status message
 *
 * @return Video pal or ntfc OSD.
 */
static inline uint8_t mavlink_msg_vskyline_osd_status_get_video_pal(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  6);
}

/**
 *  Decode a vskyline_osd_status message into a struct
 *
 * @param msg The message to decode
 * @param vskyline_osd_status C-struct to decode the message contents into
 */
static inline void mavlink_msg_vskyline_osd_status_decode(const mavlink_message_t* msg, mavlink_vskyline_osd_status_t* vskyline_osd_status)
{
#if MAVLINK_NEED_BYTE_SWAP
	vskyline_osd_status->pilot_name = mavlink_msg_vskyline_osd_status_get_pilot_name(msg);
	vskyline_osd_status->osd_status = mavlink_msg_vskyline_osd_status_get_osd_status(msg);
	vskyline_osd_status->video_pal = mavlink_msg_vskyline_osd_status_get_video_pal(msg);
#else
	memcpy(vskyline_osd_status, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_VSKYLINE_OSD_STATUS_LEN);
#endif
}
