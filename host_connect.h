/* VNC Reflector
 * Copyright (C) 2017 alejandro_liu@hotmail.com.  All rights reserved.g
 * Copyright (C) 2001-2003 HorizonLive.com, Inc.  All rights reserved.
 *
 * This software is released under the terms specified in the file LICENSE,
 * included.  HorizonLive provides e-Learning and collaborative synchronous
 * presentation solutions in a totally Web-based environment.  For more
 * information about HorizonLive, please see our website at
 * http://www.horizonlive.com.
 *
 * This software was authored by Constantin Kaplinsky <const@ce.cctpu.edu.ru>
 * and sponsored by HorizonLive.com, Inc.
 *
 * $Id: host_connect.h,v 1.11 2003/04/21 17:20:35 const Exp $
 * Connecting to a VNC host
 */

#ifndef _REFLIB_HOSTCONNECT_H
#define _REFLIB_HOSTCONNECT_H

void set_host_encodings(int request_tight, int tight_level);
int connect_to_host(char *host_info_file, int cl_listen_port);
int connect_script(const char *script);

/* FIXME: Move this stuff to another file. */
int alloc_framebuffer(int w, int h);

#endif /* _REFLIB_HOSTCONNECT_H */
