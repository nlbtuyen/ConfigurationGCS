// MESSAGE VSKYLINE_OSD_LOAD_SETTING PACKING

#define MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING 160

typedef struct __mavlink_vskyline_osd_load_setting_t
{
 uint16_t tilt_rate; ///< Tilt rate parameter.
 uint16_t yaw_rate; ///< Yaw rate parameter.
 uint16_t limit_tilt; ///< Limit tilt.
 uint16_t p_pitch; ///< P pitch parameter.
 uint16_t i_pitch; ///< I*1000 pitch parameter.
 uint16_t d_pitch; ///< D pitch parameter.
 uint16_t drate_pitch; ///< DRate pitch parameter.
 uint16_t p_roll; ///< P roll parameter.
 uint16_t i_roll; ///< I*1000 roll parameter.
 uint16_t d_roll; ///< D roll parameter.
 uint16_t drate_roll; ///< Drate roll parameter.
 uint16_t p_yaw; ///< P yaw parameter.
 uint16_t i_yaw; ///< I*1000 yaw parameter.
 uint16_t d_yaw; ///< D yaw parameter.
 uint16_t g_yaw; ///< Gain*1000 yaw parameter.
 uint8_t profile_num; ///< Profile num OSD.
 uint8_t link_pitch_roll; ///< Link pitch and roll.
} mavlink_vskyline_osd_load_setting_t;

#define MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN 32
#define MAVLINK_MSG_ID_160_LEN 32

#define MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_CRC 108
#define MAVLINK_MSG_ID_160_CRC 108



#define MAVLINK_MESSAGE_INFO_VSKYLINE_OSD_LOAD_SETTING { \
	"VSKYLINE_OSD_LOAD_SETTING", \
	17, \
	{  { "tilt_rate", NULL, MAVLINK_TYPE_UINT16_T, 0, 0, offsetof(mavlink_vskyline_osd_load_setting_t, tilt_rate) }, \
         { "yaw_rate", NULL, MAVLINK_TYPE_UINT16_T, 0, 2, offsetof(mavlink_vskyline_osd_load_setting_t, yaw_rate) }, \
         { "limit_tilt", NULL, MAVLINK_TYPE_UINT16_T, 0, 4, offsetof(mavlink_vskyline_osd_load_setting_t, limit_tilt) }, \
         { "p_pitch", NULL, MAVLINK_TYPE_UINT16_T, 0, 6, offsetof(mavlink_vskyline_osd_load_setting_t, p_pitch) }, \
         { "i_pitch", NULL, MAVLINK_TYPE_UINT16_T, 0, 8, offsetof(mavlink_vskyline_osd_load_setting_t, i_pitch) }, \
         { "d_pitch", NULL, MAVLINK_TYPE_UINT16_T, 0, 10, offsetof(mavlink_vskyline_osd_load_setting_t, d_pitch) }, \
         { "drate_pitch", NULL, MAVLINK_TYPE_UINT16_T, 0, 12, offsetof(mavlink_vskyline_osd_load_setting_t, drate_pitch) }, \
         { "p_roll", NULL, MAVLINK_TYPE_UINT16_T, 0, 14, offsetof(mavlink_vskyline_osd_load_setting_t, p_roll) }, \
         { "i_roll", NULL, MAVLINK_TYPE_UINT16_T, 0, 16, offsetof(mavlink_vskyline_osd_load_setting_t, i_roll) }, \
         { "d_roll", NULL, MAVLINK_TYPE_UINT16_T, 0, 18, offsetof(mavlink_vskyline_osd_load_setting_t, d_roll) }, \
         { "drate_roll", NULL, MAVLINK_TYPE_UINT16_T, 0, 20, offsetof(mavlink_vskyline_osd_load_setting_t, drate_roll) }, \
         { "p_yaw", NULL, MAVLINK_TYPE_UINT16_T, 0, 22, offsetof(mavlink_vskyline_osd_load_setting_t, p_yaw) }, \
         { "i_yaw", NULL, MAVLINK_TYPE_UINT16_T, 0, 24, offsetof(mavlink_vskyline_osd_load_setting_t, i_yaw) }, \
         { "d_yaw", NULL, MAVLINK_TYPE_UINT16_T, 0, 26, offsetof(mavlink_vskyline_osd_load_setting_t, d_yaw) }, \
         { "g_yaw", NULL, MAVLINK_TYPE_UINT16_T, 0, 28, offsetof(mavlink_vskyline_osd_load_setting_t, g_yaw) }, \
         { "profile_num", NULL, MAVLINK_TYPE_UINT8_T, 0, 30, offsetof(mavlink_vskyline_osd_load_setting_t, profile_num) }, \
         { "link_pitch_roll", NULL, MAVLINK_TYPE_UINT8_T, 0, 31, offsetof(mavlink_vskyline_osd_load_setting_t, link_pitch_roll) }, \
         } \
}


/**
 *  Pack a vskyline_osd_load_setting message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param profile_num Profile num OSD.
 * @param tilt_rate Tilt rate parameter.
 * @param yaw_rate Yaw rate parameter.
 * @param limit_tilt Limit tilt.
 * @param link_pitch_roll Link pitch and roll.
 * @param p_pitch P pitch parameter.
 * @param i_pitch I*1000 pitch parameter.
 * @param d_pitch D pitch parameter.
 * @param drate_pitch DRate pitch parameter.
 * @param p_roll P roll parameter.
 * @param i_roll I*1000 roll parameter.
 * @param d_roll D roll parameter.
 * @param drate_roll Drate roll parameter.
 * @param p_yaw P yaw parameter.
 * @param i_yaw I*1000 yaw parameter.
 * @param d_yaw D yaw parameter.
 * @param g_yaw Gain*1000 yaw parameter.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint8_t profile_num, uint16_t tilt_rate, uint16_t yaw_rate, uint16_t limit_tilt, uint8_t link_pitch_roll, uint16_t p_pitch, uint16_t i_pitch, uint16_t d_pitch, uint16_t drate_pitch, uint16_t p_roll, uint16_t i_roll, uint16_t d_roll, uint16_t drate_roll, uint16_t p_yaw, uint16_t i_yaw, uint16_t d_yaw, uint16_t g_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN];
	_mav_put_uint16_t(buf, 0, tilt_rate);
	_mav_put_uint16_t(buf, 2, yaw_rate);
	_mav_put_uint16_t(buf, 4, limit_tilt);
	_mav_put_uint16_t(buf, 6, p_pitch);
	_mav_put_uint16_t(buf, 8, i_pitch);
	_mav_put_uint16_t(buf, 10, d_pitch);
	_mav_put_uint16_t(buf, 12, drate_pitch);
	_mav_put_uint16_t(buf, 14, p_roll);
	_mav_put_uint16_t(buf, 16, i_roll);
	_mav_put_uint16_t(buf, 18, d_roll);
	_mav_put_uint16_t(buf, 20, drate_roll);
	_mav_put_uint16_t(buf, 22, p_yaw);
	_mav_put_uint16_t(buf, 24, i_yaw);
	_mav_put_uint16_t(buf, 26, d_yaw);
	_mav_put_uint16_t(buf, 28, g_yaw);
	_mav_put_uint8_t(buf, 30, profile_num);
	_mav_put_uint8_t(buf, 31, link_pitch_roll);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN);
#else
	mavlink_vskyline_osd_load_setting_t packet;
	packet.tilt_rate = tilt_rate;
	packet.yaw_rate = yaw_rate;
	packet.limit_tilt = limit_tilt;
	packet.p_pitch = p_pitch;
	packet.i_pitch = i_pitch;
	packet.d_pitch = d_pitch;
	packet.drate_pitch = drate_pitch;
	packet.p_roll = p_roll;
	packet.i_roll = i_roll;
	packet.d_roll = d_roll;
	packet.drate_roll = drate_roll;
	packet.p_yaw = p_yaw;
	packet.i_yaw = i_yaw;
	packet.d_yaw = d_yaw;
	packet.g_yaw = g_yaw;
	packet.profile_num = profile_num;
	packet.link_pitch_roll = link_pitch_roll;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN);
#endif
}

/**
 *  Pack a vskyline_osd_load_setting message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param profile_num Profile num OSD.
 * @param tilt_rate Tilt rate parameter.
 * @param yaw_rate Yaw rate parameter.
 * @param limit_tilt Limit tilt.
 * @param link_pitch_roll Link pitch and roll.
 * @param p_pitch P pitch parameter.
 * @param i_pitch I*1000 pitch parameter.
 * @param d_pitch D pitch parameter.
 * @param drate_pitch DRate pitch parameter.
 * @param p_roll P roll parameter.
 * @param i_roll I*1000 roll parameter.
 * @param d_roll D roll parameter.
 * @param drate_roll Drate roll parameter.
 * @param p_yaw P yaw parameter.
 * @param i_yaw I*1000 yaw parameter.
 * @param d_yaw D yaw parameter.
 * @param g_yaw Gain*1000 yaw parameter.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint8_t profile_num,uint16_t tilt_rate,uint16_t yaw_rate,uint16_t limit_tilt,uint8_t link_pitch_roll,uint16_t p_pitch,uint16_t i_pitch,uint16_t d_pitch,uint16_t drate_pitch,uint16_t p_roll,uint16_t i_roll,uint16_t d_roll,uint16_t drate_roll,uint16_t p_yaw,uint16_t i_yaw,uint16_t d_yaw,uint16_t g_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN];
	_mav_put_uint16_t(buf, 0, tilt_rate);
	_mav_put_uint16_t(buf, 2, yaw_rate);
	_mav_put_uint16_t(buf, 4, limit_tilt);
	_mav_put_uint16_t(buf, 6, p_pitch);
	_mav_put_uint16_t(buf, 8, i_pitch);
	_mav_put_uint16_t(buf, 10, d_pitch);
	_mav_put_uint16_t(buf, 12, drate_pitch);
	_mav_put_uint16_t(buf, 14, p_roll);
	_mav_put_uint16_t(buf, 16, i_roll);
	_mav_put_uint16_t(buf, 18, d_roll);
	_mav_put_uint16_t(buf, 20, drate_roll);
	_mav_put_uint16_t(buf, 22, p_yaw);
	_mav_put_uint16_t(buf, 24, i_yaw);
	_mav_put_uint16_t(buf, 26, d_yaw);
	_mav_put_uint16_t(buf, 28, g_yaw);
	_mav_put_uint8_t(buf, 30, profile_num);
	_mav_put_uint8_t(buf, 31, link_pitch_roll);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN);
#else
	mavlink_vskyline_osd_load_setting_t packet;
	packet.tilt_rate = tilt_rate;
	packet.yaw_rate = yaw_rate;
	packet.limit_tilt = limit_tilt;
	packet.p_pitch = p_pitch;
	packet.i_pitch = i_pitch;
	packet.d_pitch = d_pitch;
	packet.drate_pitch = drate_pitch;
	packet.p_roll = p_roll;
	packet.i_roll = i_roll;
	packet.d_roll = d_roll;
	packet.drate_roll = drate_roll;
	packet.p_yaw = p_yaw;
	packet.i_yaw = i_yaw;
	packet.d_yaw = d_yaw;
	packet.g_yaw = g_yaw;
	packet.profile_num = profile_num;
	packet.link_pitch_roll = link_pitch_roll;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN);
#endif
}

/**
 *  Encode a vskyline_osd_load_setting struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param vskyline_osd_load_setting C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_vskyline_osd_load_setting_t* vskyline_osd_load_setting)
{
	return mavlink_msg_vskyline_osd_load_setting_pack(system_id, component_id, msg, vskyline_osd_load_setting->profile_num, vskyline_osd_load_setting->tilt_rate, vskyline_osd_load_setting->yaw_rate, vskyline_osd_load_setting->limit_tilt, vskyline_osd_load_setting->link_pitch_roll, vskyline_osd_load_setting->p_pitch, vskyline_osd_load_setting->i_pitch, vskyline_osd_load_setting->d_pitch, vskyline_osd_load_setting->drate_pitch, vskyline_osd_load_setting->p_roll, vskyline_osd_load_setting->i_roll, vskyline_osd_load_setting->d_roll, vskyline_osd_load_setting->drate_roll, vskyline_osd_load_setting->p_yaw, vskyline_osd_load_setting->i_yaw, vskyline_osd_load_setting->d_yaw, vskyline_osd_load_setting->g_yaw);
}

/**
 *  Encode a vskyline_osd_load_setting struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param vskyline_osd_load_setting C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_vskyline_osd_load_setting_t* vskyline_osd_load_setting)
{
	return mavlink_msg_vskyline_osd_load_setting_pack_chan(system_id, component_id, chan, msg, vskyline_osd_load_setting->profile_num, vskyline_osd_load_setting->tilt_rate, vskyline_osd_load_setting->yaw_rate, vskyline_osd_load_setting->limit_tilt, vskyline_osd_load_setting->link_pitch_roll, vskyline_osd_load_setting->p_pitch, vskyline_osd_load_setting->i_pitch, vskyline_osd_load_setting->d_pitch, vskyline_osd_load_setting->drate_pitch, vskyline_osd_load_setting->p_roll, vskyline_osd_load_setting->i_roll, vskyline_osd_load_setting->d_roll, vskyline_osd_load_setting->drate_roll, vskyline_osd_load_setting->p_yaw, vskyline_osd_load_setting->i_yaw, vskyline_osd_load_setting->d_yaw, vskyline_osd_load_setting->g_yaw);
}

/**
 *  Send a vskyline_osd_load_setting message
 * @param chan MAVLink channel to send the message
 *
 * @param profile_num Profile num OSD.
 * @param tilt_rate Tilt rate parameter.
 * @param yaw_rate Yaw rate parameter.
 * @param limit_tilt Limit tilt.
 * @param link_pitch_roll Link pitch and roll.
 * @param p_pitch P pitch parameter.
 * @param i_pitch I*1000 pitch parameter.
 * @param d_pitch D pitch parameter.
 * @param drate_pitch DRate pitch parameter.
 * @param p_roll P roll parameter.
 * @param i_roll I*1000 roll parameter.
 * @param d_roll D roll parameter.
 * @param drate_roll Drate roll parameter.
 * @param p_yaw P yaw parameter.
 * @param i_yaw I*1000 yaw parameter.
 * @param d_yaw D yaw parameter.
 * @param g_yaw Gain*1000 yaw parameter.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_vskyline_osd_load_setting_send(mavlink_channel_t chan, uint8_t profile_num, uint16_t tilt_rate, uint16_t yaw_rate, uint16_t limit_tilt, uint8_t link_pitch_roll, uint16_t p_pitch, uint16_t i_pitch, uint16_t d_pitch, uint16_t drate_pitch, uint16_t p_roll, uint16_t i_roll, uint16_t d_roll, uint16_t drate_roll, uint16_t p_yaw, uint16_t i_yaw, uint16_t d_yaw, uint16_t g_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN];
	_mav_put_uint16_t(buf, 0, tilt_rate);
	_mav_put_uint16_t(buf, 2, yaw_rate);
	_mav_put_uint16_t(buf, 4, limit_tilt);
	_mav_put_uint16_t(buf, 6, p_pitch);
	_mav_put_uint16_t(buf, 8, i_pitch);
	_mav_put_uint16_t(buf, 10, d_pitch);
	_mav_put_uint16_t(buf, 12, drate_pitch);
	_mav_put_uint16_t(buf, 14, p_roll);
	_mav_put_uint16_t(buf, 16, i_roll);
	_mav_put_uint16_t(buf, 18, d_roll);
	_mav_put_uint16_t(buf, 20, drate_roll);
	_mav_put_uint16_t(buf, 22, p_yaw);
	_mav_put_uint16_t(buf, 24, i_yaw);
	_mav_put_uint16_t(buf, 26, d_yaw);
	_mav_put_uint16_t(buf, 28, g_yaw);
	_mav_put_uint8_t(buf, 30, profile_num);
	_mav_put_uint8_t(buf, 31, link_pitch_roll);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN);
#endif
#else
	mavlink_vskyline_osd_load_setting_t packet;
	packet.tilt_rate = tilt_rate;
	packet.yaw_rate = yaw_rate;
	packet.limit_tilt = limit_tilt;
	packet.p_pitch = p_pitch;
	packet.i_pitch = i_pitch;
	packet.d_pitch = d_pitch;
	packet.drate_pitch = drate_pitch;
	packet.p_roll = p_roll;
	packet.i_roll = i_roll;
	packet.d_roll = d_roll;
	packet.drate_roll = drate_roll;
	packet.p_yaw = p_yaw;
	packet.i_yaw = i_yaw;
	packet.d_yaw = d_yaw;
	packet.g_yaw = g_yaw;
	packet.profile_num = profile_num;
	packet.link_pitch_roll = link_pitch_roll;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING, (const char *)&packet, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING, (const char *)&packet, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_vskyline_osd_load_setting_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t profile_num, uint16_t tilt_rate, uint16_t yaw_rate, uint16_t limit_tilt, uint8_t link_pitch_roll, uint16_t p_pitch, uint16_t i_pitch, uint16_t d_pitch, uint16_t drate_pitch, uint16_t p_roll, uint16_t i_roll, uint16_t d_roll, uint16_t drate_roll, uint16_t p_yaw, uint16_t i_yaw, uint16_t d_yaw, uint16_t g_yaw)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint16_t(buf, 0, tilt_rate);
	_mav_put_uint16_t(buf, 2, yaw_rate);
	_mav_put_uint16_t(buf, 4, limit_tilt);
	_mav_put_uint16_t(buf, 6, p_pitch);
	_mav_put_uint16_t(buf, 8, i_pitch);
	_mav_put_uint16_t(buf, 10, d_pitch);
	_mav_put_uint16_t(buf, 12, drate_pitch);
	_mav_put_uint16_t(buf, 14, p_roll);
	_mav_put_uint16_t(buf, 16, i_roll);
	_mav_put_uint16_t(buf, 18, d_roll);
	_mav_put_uint16_t(buf, 20, drate_roll);
	_mav_put_uint16_t(buf, 22, p_yaw);
	_mav_put_uint16_t(buf, 24, i_yaw);
	_mav_put_uint16_t(buf, 26, d_yaw);
	_mav_put_uint16_t(buf, 28, g_yaw);
	_mav_put_uint8_t(buf, 30, profile_num);
	_mav_put_uint8_t(buf, 31, link_pitch_roll);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING, buf, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN);
#endif
#else
	mavlink_vskyline_osd_load_setting_t *packet = (mavlink_vskyline_osd_load_setting_t *)msgbuf;
	packet->tilt_rate = tilt_rate;
	packet->yaw_rate = yaw_rate;
	packet->limit_tilt = limit_tilt;
	packet->p_pitch = p_pitch;
	packet->i_pitch = i_pitch;
	packet->d_pitch = d_pitch;
	packet->drate_pitch = drate_pitch;
	packet->p_roll = p_roll;
	packet->i_roll = i_roll;
	packet->d_roll = d_roll;
	packet->drate_roll = drate_roll;
	packet->p_yaw = p_yaw;
	packet->i_yaw = i_yaw;
	packet->d_yaw = d_yaw;
	packet->g_yaw = g_yaw;
	packet->profile_num = profile_num;
	packet->link_pitch_roll = link_pitch_roll;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING, (const char *)packet, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING, (const char *)packet, MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE VSKYLINE_OSD_LOAD_SETTING UNPACKING


/**
 *  Get field profile_num from vskyline_osd_load_setting message
 *
 * @return Profile num OSD.
 */
static inline uint8_t mavlink_msg_vskyline_osd_load_setting_get_profile_num(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  30);
}

/**
 *  Get field tilt_rate from vskyline_osd_load_setting message
 *
 * @return Tilt rate parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_tilt_rate(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  0);
}

/**
 *  Get field yaw_rate from vskyline_osd_load_setting message
 *
 * @return Yaw rate parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_yaw_rate(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  2);
}

/**
 *  Get field limit_tilt from vskyline_osd_load_setting message
 *
 * @return Limit tilt.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_limit_tilt(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  4);
}

/**
 *  Get field link_pitch_roll from vskyline_osd_load_setting message
 *
 * @return Link pitch and roll.
 */
static inline uint8_t mavlink_msg_vskyline_osd_load_setting_get_link_pitch_roll(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  31);
}

/**
 *  Get field p_pitch from vskyline_osd_load_setting message
 *
 * @return P pitch parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_p_pitch(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  6);
}

/**
 *  Get field i_pitch from vskyline_osd_load_setting message
 *
 * @return I*1000 pitch parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_i_pitch(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  8);
}

/**
 *  Get field d_pitch from vskyline_osd_load_setting message
 *
 * @return D pitch parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_d_pitch(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  10);
}

/**
 *  Get field drate_pitch from vskyline_osd_load_setting message
 *
 * @return DRate pitch parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_drate_pitch(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  12);
}

/**
 *  Get field p_roll from vskyline_osd_load_setting message
 *
 * @return P roll parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_p_roll(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  14);
}

/**
 *  Get field i_roll from vskyline_osd_load_setting message
 *
 * @return I*1000 roll parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_i_roll(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  16);
}

/**
 *  Get field d_roll from vskyline_osd_load_setting message
 *
 * @return D roll parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_d_roll(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  18);
}

/**
 *  Get field drate_roll from vskyline_osd_load_setting message
 *
 * @return Drate roll parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_drate_roll(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  20);
}

/**
 *  Get field p_yaw from vskyline_osd_load_setting message
 *
 * @return P yaw parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_p_yaw(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  22);
}

/**
 *  Get field i_yaw from vskyline_osd_load_setting message
 *
 * @return I*1000 yaw parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_i_yaw(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  24);
}

/**
 *  Get field d_yaw from vskyline_osd_load_setting message
 *
 * @return D yaw parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_d_yaw(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  26);
}

/**
 *  Get field g_yaw from vskyline_osd_load_setting message
 *
 * @return Gain*1000 yaw parameter.
 */
static inline uint16_t mavlink_msg_vskyline_osd_load_setting_get_g_yaw(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  28);
}

/**
 *  Decode a vskyline_osd_load_setting message into a struct
 *
 * @param msg The message to decode
 * @param vskyline_osd_load_setting C-struct to decode the message contents into
 */
static inline void mavlink_msg_vskyline_osd_load_setting_decode(const mavlink_message_t* msg, mavlink_vskyline_osd_load_setting_t* vskyline_osd_load_setting)
{
#if MAVLINK_NEED_BYTE_SWAP
	vskyline_osd_load_setting->tilt_rate = mavlink_msg_vskyline_osd_load_setting_get_tilt_rate(msg);
	vskyline_osd_load_setting->yaw_rate = mavlink_msg_vskyline_osd_load_setting_get_yaw_rate(msg);
	vskyline_osd_load_setting->limit_tilt = mavlink_msg_vskyline_osd_load_setting_get_limit_tilt(msg);
	vskyline_osd_load_setting->p_pitch = mavlink_msg_vskyline_osd_load_setting_get_p_pitch(msg);
	vskyline_osd_load_setting->i_pitch = mavlink_msg_vskyline_osd_load_setting_get_i_pitch(msg);
	vskyline_osd_load_setting->d_pitch = mavlink_msg_vskyline_osd_load_setting_get_d_pitch(msg);
	vskyline_osd_load_setting->drate_pitch = mavlink_msg_vskyline_osd_load_setting_get_drate_pitch(msg);
	vskyline_osd_load_setting->p_roll = mavlink_msg_vskyline_osd_load_setting_get_p_roll(msg);
	vskyline_osd_load_setting->i_roll = mavlink_msg_vskyline_osd_load_setting_get_i_roll(msg);
	vskyline_osd_load_setting->d_roll = mavlink_msg_vskyline_osd_load_setting_get_d_roll(msg);
	vskyline_osd_load_setting->drate_roll = mavlink_msg_vskyline_osd_load_setting_get_drate_roll(msg);
	vskyline_osd_load_setting->p_yaw = mavlink_msg_vskyline_osd_load_setting_get_p_yaw(msg);
	vskyline_osd_load_setting->i_yaw = mavlink_msg_vskyline_osd_load_setting_get_i_yaw(msg);
	vskyline_osd_load_setting->d_yaw = mavlink_msg_vskyline_osd_load_setting_get_d_yaw(msg);
	vskyline_osd_load_setting->g_yaw = mavlink_msg_vskyline_osd_load_setting_get_g_yaw(msg);
	vskyline_osd_load_setting->profile_num = mavlink_msg_vskyline_osd_load_setting_get_profile_num(msg);
	vskyline_osd_load_setting->link_pitch_roll = mavlink_msg_vskyline_osd_load_setting_get_link_pitch_roll(msg);
#else
	memcpy(vskyline_osd_load_setting, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_VSKYLINE_OSD_LOAD_SETTING_LEN);
#endif
}
