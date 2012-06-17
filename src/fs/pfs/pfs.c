/* Copyright (c) 2012, Joakim Östlund
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
  This file implements the "Pseudo File System", which is a 
  read-only directory structure that will be used as a root
  file system for now, mostly to test basic FS concepts.
*/

#include <errno.h>
#include "pfs.h"
#include "log/log.h"

int __pfs_mount(char *source, char *destination, U32 flags)
{
     /* VFS will perform sanity check of parameters */
     INFO(STR("Mouting pseudo file system at %s", destination));

     return 0;
}

int __pfs_umount(char *path, U32 flags)
{
     return 0;
}

int __pfs_open(const char *path, int flags, ...)
{
     errno = EINVAL;
     return -1;
}

int __pfs_close(int fd)
{
     errno = EINVAL;
     return -1;
}

int __pfs_fstat(int fd, struct stat *st)
{
     errno = EINVAL;
     return -1;
}

int __pfs_isatty(int fd)
{
     return 0;
}

int __pfs_lseek(int fd, int offset, int whence)
{
     errno = EINVAL;
     return -1;
}

int __pfs_read(int fd, void *buf, int len)
{
     errno = EINVAL;
     return -1;
}

int __pfs_write(int fd, const void *buf, int len)
{
     errno = EINVAL;
     return -1;
}

struct dirent *__pfs_readdir(dir_ptr_t *dir)
{
     return NULL;
}

dir_ptr_t __pfs_opendir(char *path)
{
     return 0;
}

int __pfs_closedir(dir_ptr_t *dir)
{
     return 0;
}

void __pfs_init()
{
     __FileOps fops = {
     fs_mount:  __pfs_mount,
     fs_umount: __pfs_umount,
     open:      __pfs_open,
     close:     __pfs_close,
     fstat:     __pfs_fstat,
     isatty:    __pfs_isatty,
     lseek:     __pfs_lseek,
     read:      __pfs_read,
     write:     __pfs_write,
     readdir:   __pfs_readdir,
     opendir:   __pfs_opendir,
     closedir:  __pfs_closedir
     };

     if( __vfs_register_fs( "pfs", &fops ) == EERROR )
     {
	  ERROR("Failed to register pfs");
     }
     else
	  INFO("Pseudo file system init done!");
}
