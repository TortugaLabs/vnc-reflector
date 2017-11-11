/**
 * vncmuxer
 * Copyright (C) 2017 alejandro_liu@hotmail.com.  All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Misc utility functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <stdarg.h>

#include "logging.h"
#include "utils.h"

/*
 * Make sure that the stdin fd is a socket...
 */
void init_stdio(void) {
  struct stat b;
  int sp[2];
  pid_t child;
  int i,n;

  n = sysconf(_SC_OPEN_MAX);
  for(i = 3; i < n; i++) close(i);


  if (fstat(0,&b) == -1) die(__LINE__,"stat: %s",strerror(errno));
  if (S_ISSOCK(b.st_mode)) return; // Trivial case, do nothing...
  
  log_write(LL_MSG,"Converting STDIN to Socket I/O");
  if (socketpair(AF_LOCAL,SOCK_STREAM,0,sp) == -1) die(__LINE__,"socketpair: %s",strerror(errno));

  child = fork();

  if (child == -1) die(__LINE__,"fork: %s",strerror(errno));
  if (child != 0) {
    // Parent... Copy STDIO to the socket...
    fd_set rfds;
    int i, j, nfds;
    char buf[16384];

    close(sp[1]);
    for (;;) {
      FD_ZERO(&rfds); FD_SET(0,&rfds); FD_SET(sp[0],&rfds);
      nfds = sp[0]+1;
      i = select(nfds,&rfds,NULL,NULL,NULL);
      if (!i) continue;
      if (FD_ISSET(0,&rfds)) {
	j = read(0,buf,sizeof(buf));
	if (j <= 0) break;
	write(sp[0],buf,j);
      }
      if (FD_ISSET(sp[0],&rfds)) {
	j = read(sp[0],buf,sizeof(buf));
	if (j <= 0) break;
	write(1,buf,j);
      }
    }
    close(sp[0]); close(0); close(1);
    wait(&i);
    exit(WEXITSTATUS(i));
  }
  // STDIO is now a network socket.
  close(sp[0]);
  close(0);close(1);
  dup2(sp[1],0);
  dup2(sp[1],1);
  close(sp[1]);
  return;
}

void die(int retcode,const char *msgfmt, ...) {
  va_list ap;
  
  va_start(ap,msgfmt);
  vfprintf(stderr,msgfmt,ap);
  va_end(ap);
  exit(retcode);
}

