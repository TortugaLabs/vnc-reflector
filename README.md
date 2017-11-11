# VNC Reflector

## About VNC Reflector

VNC Reflector is a specialized VNC server which acts as a proxy
sitting between real VNC server (host) and a number of VNC clients. It
was designed to work efficiently with large number of clients, but
current version is optimized to work primarily with clients that
request 8-bit color format known as BGR233. Other color formats are
supported too but performance may be worse if there are many such
clients.

VNC Reflector can switch between different host connections on the
fly, supports reverse host connections and hosts with variable desktop
geometries. It supports full-control and read-only client connections
depending on which of two passwords was used to initiate a client
session.

Raw, Hextile, Tight and CopyRect encodings are suported both at the
host side and at the client side. JPEG sub-encoding in Tight is
supported on client connections.

Please note that the documentation is incomplete.


## Licensing terms

* Copyright (C) 2017 alejandro_liu@hotmail.com
* Copyright (C) 2001-2003 HorizonLive.com, Inc.  
* Copyright (c) 1988,1989,1990,1991,1992 by Richard Outerbridge.  
  (GEnie : OUTER; CIS : [71755,204]) Graven Imagery, 1992.
* Copyright 1987, 1988, 1989, 1998  The Open Group

All rights reserved.

This software is released under the terms specified in the file LICENSE,
included.

This software was originally authored by Constantin Kaplinsky <const@ce.cctpu.edu.ru>
and sponsored by HorizonLive.com, Inc.

## Command-line usage

* * *
````
./vncreflector [OPTIONS...] HOST_INFO_FILE
````
* * *



Options:

*  -i PID_FILE     - write pid file, appending listening port to the filename
*  -p PASSWD_FILE  - read a plaintext client password file [default: passwd]
*  -a ACTIVE_FILE  - create file during times when a host is connected
*  -c ACTIONS_FILE - on events, execute commands specified in a file
*  -l LISTEN_PORT  - port to listen for client connections [default: 5999]
*  -b IP_ADDRESS   - bind listening sockets to a specific IP [default: any]
*  -s FBS_PREFIX   - save host sessions in rfbproxy-compatible files
                    (optionally appending 3-digit session IDs to the
                    filename prefix, only if used without the -j option)
*  -j              - join saved sessions (see -s option) in one session file
*  -t              - use Tight encoding for host communications if possible
*  -T COMPR_LEVEL  - like -t, but use the specified compression level (1..9)
*  -g LOG_FILE     - write logs to the specified file [default: reflector.log]
*  -v LOG_LEVEL    - set verbosity level for the log file (0..6) [default: 4]
*  -f LOG_LEVEL    - run in foreground, show logs on stderr at the specified
                    verbosity level (0..6) [note: use 3 for normal output]
*  -q              - suppress printing copyright banner at startup
*  -h              - print this help message

## VNC Multiplexer mode

If the `LISTEN_PORT` specified is `0` (zero), then VNC multiplexer mode
is activated.  What this does is that instead of listening on a port
it will simply use `STDIN/STDOUT` as a pre-opened socket.  As if it was
invoked from [inetd][inetd].  Then the `HOST_INFO_FILE` is treated
as a script that will launch a `vnc` session (again in [inetd][inetd]
mode.  The script will have an argument which is the file descriptor
that will receive a new host to connect to in the format of the
`HOST_INFO_FILE` explained later.

## Format of the HOST_INFO_FILE

HOST_INFO_FILE provides information about the host to connect to. It's
first line should begin with hostname (or with an asterisk if we would wait
for reversed host->reflector connection). Next thing is VNC display number
after a colon (or a TCP port number, for reversed connections). Then
follows plaintext host password after a space. For non-reversed
connections, negative display numbers are allowed (e.g. display number
-5820 corresponds to port number 80 at the host side). For reversed
connections, default listening port number is 5500.

A sample to connect to some.hostname.com's display :0 (port 5900):

* * *
````
some.hostname.com:0 PaS$w0rD
````
* * *

A sample to listen for host connections on TCP port 5598:

* * *
````
*:5598 PaS$w0rD
````
* * *

## Format of the PASSWD_FILE

PASSWD_FILE keeps passwords for client connections.

It may contain two lines with one password on each line. First line is for
full-control client password, second line is for read-only client password.
Empty lines denote no authentication. If the file contains one line, then
there is no read-only password, and all clients would have been granted
full-control access.


## Format of the ACTIONS_FILE

ACTIONS_FILE used with the -c option specifies commands that should be
executed upon certain events. Each line starts with a keyword denoting a
particular event type, then follows colon character (":"), and after an
optional sequence of spaces follows the command to execute. An example:

* * *
````
host_activate: echo "new host" >> /tmp/connections.txt
````
* * *

Currently, the only supported event type is `host_activate', which is raised
on each new host connection.


## Control signals

  * SIGHUP signal causes VNC Reflector to disconnect all connected clients.

  * SIGUSR1 signal tells VNC Reflector that it should re-read
    HOST_INFO_FILE and to reconnect to host specified there (or to start
    listening for reversed host connections). Host connection would be
    closed when new host connection is established successfully (auth is ok
    etc.). Client connections would be preserved.

  * SIGUSR2 signal tells VNC Reflector that it should close current host
    connection, then re-read HOST_INFO_FILE and to reconnect to host
    specified there (or to start listening for reversed host connections). 
    Client connections would be preserved.

## TODO

1. Enable Client SetDesktopSize message
   - host_io: Implement ExtendedDesktopSize pseudo encoding support
   - client_io: Implement SetDesktopSize message handling (sending
     error)
   - client_io: Implement ExtendedDesktopSize semantics
   - client_io: Implement SetDesktopSize message handling
2. display reader code:
   - upon receiving reconnect message
   - try connect to new host
   - if success, we switch to new host and set the host
     slot to quit program on close.
   - if fail, we continue with the old host.  Old host should
     have the loginscript running and after a while if it stays
     running it assumes that something went wrong.  Show a message
     and allow to re-login.
3. In inetd mode, when entering inetd mode, we set the client slot
   to quit program on close.  The host slot should be setup on close
   to quit program.  (We could re-run loginscript but most likely
   that would lead to an infinite loop)

    [inetd]: https://en.wikipedia.org/wiki/Inetd "About Inetd"
