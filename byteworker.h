/*
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Martin Wenger <martin.wenger@arcormail.de> and Stefan Rupp <struppi@erlangen.ccc.de>
 * wrote this file. As long as you retain this notice you can do whatever you want
 * with this stuff. If we meet some day, and you think this stuff is worth it,
 * you can buy me/us a beer in return.
 * (c) 2005-2007 Martin Wenger, Stefan Rupp
 */
#ifndef __BYTEWORKER_H__
#define __BYTEWORKER_H__

void bw_ledSet(uint8_t ledId, uint8_t state);
void bw_ledToggle(uint8_t ledId);
void bw_outputSet(uint8_t channel, uint8_t state);
int8_t bw_canInit( uint16_t bitrate /* kbits */ );

#endif //__BYTEWORKER_H__
