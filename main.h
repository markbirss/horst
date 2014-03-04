/* horst - Highly Optimized Radio Scanning Tool
 *
 * Copyright (C) 2005-2011 Bruno Randolf (br1@einfach.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdlib.h>
#include <sys/time.h>

#include "list.h"
#include "average.h"

#define VERSION "3.0"

#ifndef DO_DEBUG
#define DO_DEBUG 0
#endif

#ifndef DO_UPLOAD
#define DO_UPLOAD 0
#endif

#define MAC_LEN			6

#define MAX_NODES		255
#define MAX_ESSIDS		255
#define MAX_BSSIDS		255
#define MAX_HISTORY		255
#define MAX_CHANNELS		64
#define MAX_ESSID_LEN		32
#define MAX_RATES		44	/* 12 legacy rates and 32 MCS */
#define MAX_FSTYPE		0xff
#define MAX_FILTERMAC		9

/* packet types we actually care about, e.g filter */
#define PKT_TYPE_CTRL		0x000001
#define PKT_TYPE_MGMT		0x000002
#define PKT_TYPE_DATA		0x000004

#define PKT_TYPE_BADFCS		0x000008

#define PKT_TYPE_BEACON		0x000010
#define PKT_TYPE_PROBE		0x000020
#define PKT_TYPE_ASSOC		0x000040
#define PKT_TYPE_AUTH		0x000080
#define PKT_TYPE_RTS		0x000100
#define PKT_TYPE_CTS		0x000200
#define PKT_TYPE_ACK		0x000400
#define PKT_TYPE_NULL		0x000800

#define PKT_TYPE_ARP		0x001000
#define PKT_TYPE_IP		0x002000
#define PKT_TYPE_ICMP		0x004000
#define PKT_TYPE_UDP		0x008000
#define PKT_TYPE_TCP		0x010000
#define PKT_TYPE_OLSR		0x020000
#define PKT_TYPE_OLSR_LQ	0x040000
#define PKT_TYPE_OLSR_GW	0x080000
#define PKT_TYPE_BATMAN		0x100000
#define PKT_TYPE_MESHZ		0x200000
#define PKT_TYPE_QDATA		0x400000

#define PKT_TYPE_ALL_MGMT	(PKT_TYPE_BEACON | PKT_TYPE_PROBE | PKT_TYPE_ASSOC | PKT_TYPE_AUTH)
#define PKT_TYPE_ALL_CTRL	(PKT_TYPE_RTS | PKT_TYPE_CTS | PKT_TYPE_ACK)
#define PKT_TYPE_ALL_DATA	(PKT_TYPE_NULL | PKT_TYPE_ARP | PKT_TYPE_ICMP | PKT_TYPE_IP | \
				 PKT_TYPE_UDP | PKT_TYPE_TCP | PKT_TYPE_OLSR | PKT_TYPE_OLSR_LQ | \
				 PKT_TYPE_OLSR_GW | PKT_TYPE_BATMAN | PKT_TYPE_MESHZ | PKT_TYPE_QDATA)

#define WLAN_MODE_AP		0x01
#define WLAN_MODE_IBSS		0x02
#define WLAN_MODE_STA		0x04
#define WLAN_MODE_PROBE		0x08

#define PHY_FLAG_SHORTPRE	0x0001
#define PHY_FLAG_BADFCS		0x0002
#define PHY_FLAG_A		0x0010
#define PHY_FLAG_B		0x0020
#define PHY_FLAG_G		0x0040
#define PHY_FLAG_MODE_MASK	0x00f0

/* default config values */
#define INTERFACE_NAME		"wlan0"
#define NODE_TIMEOUT		60	/* seconds */
#define CHANNEL_TIME		250000	/* 250 msec */
/* update display every 100ms - "10 frames per sec should be enough for everyone" ;) */
#define DISPLAY_UPDATE_INTERVAL 100000	/* usec */
#define SERVER_UPLOAD_INTERVAL	2	/* sec */
#define RECV_BUFFER_SIZE	0	/* not used by default */
#define DEFAULT_PORT		"4444"	/* string because of getaddrinfo() */
#define DEFAULT_CONTROL_PIPE	"/tmp/horst"

#ifndef ARPHRD_IEEE80211_RADIOTAP
#define ARPHRD_IEEE80211_RADIOTAP 803    /* IEEE 802.11 + radiotap header */
#endif

#ifndef ARPHRD_IEEE80211_PRISM
#define ARPHRD_IEEE80211_PRISM 802      /* IEEE 802.11 + Prism2 header  */
#endif


struct packet_info {
	/* general */
	unsigned int		pkt_types;	/* bitmask of packet types */

	/* wlan phy (from radiotap) */
	int			phy_signal;	/* signal strength (usually dBm) */
	int			phy_noise;	/* noise level (usually dBm) */
	unsigned int		phy_snr;	/* signal to noise ratio */
	unsigned int		phy_rate;	/* physical rate * 10 (=in 100kbps) */
	unsigned char		phy_rate_idx;	/* MCS index */
	unsigned char		phy_rate_flags;	/* MCS flags */
	unsigned int		phy_freq;	/* frequency from driver */
	unsigned char		phy_chan;	/* channel from driver */
	unsigned int		phy_flags;	/* A, B, G, shortpre */

	/* wlan mac */
	unsigned int		wlan_len;	/* packet length */
	unsigned int		wlan_type;	/* frame control field */
	unsigned char		wlan_src[MAC_LEN];
	unsigned char		wlan_dst[MAC_LEN];
	unsigned char		wlan_bssid[MAC_LEN];
	char			wlan_essid[MAX_ESSID_LEN];
	u_int64_t		wlan_tsf;	/* timestamp from beacon */
	unsigned int		wlan_bintval;	/* beacon interval */
	unsigned int		wlan_mode;	/* AP, STA or IBSS */
	unsigned char		wlan_channel;	/* channel from beacon, probe */
	unsigned char		wlan_qos_class;	/* for QDATA frames */
	unsigned int		wlan_nav;	/* frame NAV duration */
	unsigned int		wlan_seqno;	/* sequence number */

	/* flags */
	unsigned int		wlan_wep:1,	/* WEP on/off */
				wlan_retry:1,
				wlan_wpa:1,
				wlan_rsn:1;

	/* IP */
	unsigned int		ip_src;
	unsigned int		ip_dst;
	unsigned int		tcpudp_port;
	unsigned int		olsr_type;
	unsigned int		olsr_neigh;
	unsigned int		olsr_tc;

	/* calculated from other values */
	unsigned int		pkt_duration;	/* packet "airtime" */
	int			pkt_chan_idx;	/* received while on channel */
	int			wlan_retries;	/* retry count for this frame */
};

struct essid_info;

struct node_info {
	/* housekeeping */
	struct list_head	list;
	struct list_head	essid_nodes;
	struct list_head	on_channels;	/* channels this node was seen on */
	unsigned int		num_on_channels;
	time_t			last_seen;	/* timestamp */

	/* general packet info */
	unsigned int		pkt_types;	/* bitmask of packet types we've seen */
	unsigned int		pkt_count;	/* nr of packets seen */

	/* wlan phy (from radiotap) */
	int			phy_snr_min;
	int			phy_snr_max;
	struct ewma		phy_snr_avg;
	int			phy_sig_max;

	/* wlan mac */
	unsigned char		wlan_bssid[MAC_LEN];
	unsigned int		wlan_channel;	/* channel from beacon, probe frames */
	unsigned int		wlan_mode;	/* AP, STA or IBSS */
	u_int64_t		wlan_tsf;
	unsigned int		wlan_bintval;
	unsigned int		wlan_retries_all;
	unsigned int		wlan_retries_last;
	unsigned int		wlan_seqno;
	struct essid_info*	essid;
	struct node_info*	wlan_ap_node;

	unsigned int		wlan_wep:1,	/* WEP active? */
				wlan_wpa:1,
				wlan_rsn:1;
	/* IP */
	unsigned int		ip_src;		/* IP address (if known) */
	unsigned int		olsr_count;	/* number of OLSR packets */
	unsigned int		olsr_neigh;	/* number if OLSR neighbours */
	unsigned int		olsr_tc;	/* unused */

	struct packet_info	last_pkt;
};

extern struct list_head nodes;

struct essid_info {
	struct list_head	list;
	char			essid[MAX_ESSID_LEN];
	struct list_head	nodes;
	unsigned int		num_nodes;
	int			split;
};

struct essid_meta_info {
	struct list_head	list;
	struct essid_info*	split_essid;
	int			split_active;
};

extern struct essid_meta_info essids;

struct history {
	int			signal[MAX_HISTORY];
	int			noise[MAX_HISTORY];
	int			rate[MAX_HISTORY];
	unsigned int		type[MAX_HISTORY];
	unsigned int		retry[MAX_HISTORY];
	unsigned int		index;
};

extern struct history hist;

struct statistics {
	unsigned long		packets;
	unsigned long		retries;
	unsigned long		bytes;
	unsigned long		duration;

	unsigned long		packets_per_rate[MAX_RATES];
	unsigned long		bytes_per_rate[MAX_RATES];
	unsigned long		duration_per_rate[MAX_RATES];

	unsigned long		packets_per_type[MAX_FSTYPE];
	unsigned long		bytes_per_type[MAX_FSTYPE];
	unsigned long		duration_per_type[MAX_FSTYPE];

	unsigned long		filtered_packets;

	struct timeval		stats_time;
};

extern struct statistics stats;

/* channel to frequency mapping */
struct chan_freq {
	int			chan;
	int			freq;
};

extern struct chan_freq channels[MAX_CHANNELS];

struct channel_info {
	int			signal;
	struct ewma		signal_avg;
	int			noise;
	unsigned long		packets;
	unsigned long		bytes;
	unsigned long		durations;
	unsigned long		durations_last;
	struct ewma		durations_avg;
	struct list_head	nodes;
	unsigned int		num_nodes;
};

extern struct channel_info spectrum[MAX_CHANNELS];

/* helper for keeping lists of nodes for each channel
 * (a node can be on more than one channel) */
struct chan_node {
	struct node_info*	node;
	struct channel_info*	chan;
	struct list_head	chan_list;	/* list for nodes per channel */
	struct list_head	node_list;	/* list for channels per node */
	int			sig;
	struct ewma		sig_avg;
	unsigned long		packets;
};

struct config {
	char*			ifname;
	char*			port;
	int			quiet;
	int			node_timeout;
	int			channel_time;
	int			channel_max;
	int			current_channel;	/* index into channels array */
	int			display_interval;
	int			upload_interval;
	char*			upload_server;
	char*			dumpfile;
	int			recv_buffer_size;
	char*			serveraddr;
	char*			control_pipe;

	unsigned char		filtermac[MAX_FILTERMAC][MAC_LEN];
	char			filtermac_enabled[MAX_FILTERMAC];
	unsigned char		filterbssid[MAC_LEN];
	int			filter_pkt;
	unsigned int		filter_off:1,
				do_change_channel:1,
				allow_client:1,
				allow_control:1,
	/* this isn't exactly config, but wtf... */
				do_macfilter:1,
				have_noise:1,
				display_initialized:1;
	int			arphrd; // the device ARP type
	unsigned char		my_mac_addr[MAC_LEN];
	int			paused;
	int			num_channels;
};

extern struct config conf;

extern struct timeval the_time;


#define CONF_CURRENT_CHANNEL (conf.current_channel >= 0 && conf.current_channel < MAX_CHANNELS ? \
	channels[conf.current_channel].chan : 0)

void
free_lists(void);

int
change_channel(int idx);

int
find_channel_index(int c);

void
init_channels(void);

void
update_spectrum_durations(void);

void
handle_packet(struct packet_info* p);

void __attribute__ ((format (printf, 1, 2)))
printlog(const char *fmt, ...);

void
horst_pause(int pause);

void
horst_dumpfile_open(char* name);

#endif
