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

#include <errno.h>
#include "devfs.h"
#include "log/log.h"

U8 isMounted = 0;
char *root[__VFS_MAX_PATH_LEN];

int __devfs_mount(char *source, char *destination, U32 flags)
{
     if( isMounted != 0 )
     {
	  errno = EINVAL;
	  TRACE_ERROR("DevFS can only be mounted once!");
	  return -1;
     }

     INFO(STR("Mounting devfs at %s", destination));

     isMounted = 1;
     strncpy( root, destination, __VFS_MAX_PATH_LEN );
     char *root[__VFS_MAX_PATH_LEN] = '\0';

     return 0;
}

int __devfs_umount(char *path, U32 flags)
{

     if( isMounted == 0 )
     {
	  errno = EINVAL;
	  TRACE_ERROR("Attempting to unmount DevFS which is currently not mounted!");
	  return -1;
     }

     /* TODO: Check open files etc */
     isMounted = 0;
     root[0] = '\0';

     return 0;
}

int __devfs_open(const char *path, int flags, ...)
{
     errno = EINVAL;
     return -1;
}

int __devfs_close(int fd)
{
     errno = EINVAL;
     return -1;
}

int __devfs_fstat(int fd, struct stat *st)
{
     errno = EINVAL;
     return -1;
}

int __devfs_isatty(int fd)
{
     return 0;
}

int __devfs_lseek(int fd, int offset, int whence)
{
     errno = EINVAL;
     return -1;
}

int __devfs_read(int fd, void *buf, int len)
{
     errno = EINVAL;
     return -1;
}

int __devfs_write(int fd, const void *buf, int len)
{
     errno = EINVAL;
     return -1;
}

struct dirent *__devfs_readdir(dir_ptr_t *dir)
{
     return NULL;
}

dir_ptr_t __devfs_opendir(char *path)
{
     return 0;
}

int __devfs_closedir(dir_ptr_t *dir)
{
     return 0;
}

int __devfs_register_device( char *name, __FileOps *fops )
{
     return 0;
}

void __devfs_init()
{
     __FileOps fops = {
     fs_mount:  __devfs_mount,
     fs_umount: __devfs_umount,
     open:      __devfs_open,
     close:     __devfs_close,
     fstat:     __devfs_fstat,
     isatty:    __devfs_isatty,
     lseek:     __devfs_lseek,
     read:      __devfs_read,
     write:     __devfs_write,
     readdir:   __devfs_readdir,
     opendir:   __devfs_opendir,
     closedir:  __devfs_closedir
     };

     if( __vfs_register_fs( "devfs", &fops ) == EERROR )
     {
	  ERROR("Failed to register devfs");
     }
     else
	  INFO("Devfs init done!");
}
