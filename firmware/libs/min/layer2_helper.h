//
// Created by mcochrane on 1/04/17.
//

#ifndef WS_OST_LAYER2_MACROS_H
#define WS_OST_LAYER2_MACROS_H

#include "encode_decode.h"

/* Macros for unpacking and packing MIN frames in various functions.
 *
 * Declaring stuff in macros like this is not pretty but it cuts down
 * on obscure errors due to typos.
 */
#define DECLARE_BUF(size)		uint8_t m_buf[(size)]; uint8_t m_control = (size); uint8_t m_cursor = 0
#define PACK8(v)				((m_cursor < m_control) ? m_buf[m_cursor] = (v), m_cursor++ : 0)
#define PACK16(v)				((m_cursor + 2U <= m_control) ? encode_16((v), m_buf + m_cursor), m_cursor += 2U : 0)
#define PACK32(v)				((m_cursor + 4U <= m_control) ? encode_32((v), m_buf + m_cursor), m_cursor += 4U : 0)
#define SEND_FRAME(id)			(min_tx_frame((id), m_buf, m_control))

#define DECLARE_UNPACK()		uint8_t m_cursor = 0
#define UNPACK8(v)				((m_cursor < m_control) ? ((v) = m_buf[m_cursor]), m_cursor++ : ((v) = 0))
#define UNPACK16(v)				((m_cursor + 2U <= m_control) ? ((v) = decode_16(m_buf + m_cursor)), m_cursor += 2U : ((v) = 0))
#define UNPACK32(v)				((m_cursor + 4U <= m_control) ? ((v) = decode_32(m_buf + m_cursor)), m_cursor += 4U : ((v) = 0))
#define UNPACK16(v)				((m_cursor + 2U <= m_control) ? ((v) = decode_16(m_buf + m_cursor)), m_cursor += 2U : ((v) = 0))
#define UNPACK32(v)				((m_cursor + 4U <= m_control) ? ((v) = decode_32(m_buf + m_cursor)), m_cursor += 4U : ((v) = 0))


#endif //WS_OST_LAYER2_MACROS_H
