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

#include "vfs.h"
#include "proc/process.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static __ProcFileDesc *__fd_list = NULL;
static __FileSystem *__fs_list = NULL;
static __MountEntry *__mount_list = NULL;

int open(const char *pathname, int flags, ...)
{
     __ProcFileDesc *pfd = __vfs_get_proc_fd( current_process() );
     __FileSystem *fs;
     va_list args;
     int ret;
     int newFd;

     if( pfd == NULL || pfd->numOpen >= __VFS_MAX_FD )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     fs = __vfs_get_fs_handler( pathname );

     if( fs == NULL || fs->fops.open == NULL )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     va_start( args, flags );

     ret = fs->fops.open( pathname, flags, args );

     va_end( args );

     if( ret < 0 )
	  return ret;

     /* Add fd in correct place */
     for( newFd = 1; newFd < __VFS_MAX_FD; newFd++ )
     {
	  if( pfd->openFdList[newFd].fsFd == 0 )
	       break;
     }

     pfd->openFdList[newFd].fsFd = ret;
     pfd->openFdList[newFd].fs   = fs;

     return ret;
}

int creat(const char *pathname, mode_t mode)
{
     /* TODO: Handle creat from fops struct */
     return open(pathname, O_CREAT|O_WRONLY|O_TRUNC, mode);
}

int close(int fd)
{
     __ProcFileDesc *pfd = __vfs_get_proc_fd( current_process() );
     int ret;
     __FileSystem *fs;

     if( pfd == NULL || pfd->openFdList[fd].fsFd == 0 )
     {
	  errno = EBADF;
	  return EERROR;
     }

     fs = pfd->openFdList[fd].fs;

     if( fs->fops.close == NULL )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     ret = fs->fops.close( pfd->openFdList[fd].fsFd );

     if( ret >= 0 )
	  pfd->openFdList[fd].fsFd = 0;

     return ret;
}

int fstat(int fd, struct stat *st)
{
     __ProcFileDesc *pfd = __vfs_get_proc_fd( current_process() );
     __FileSystem *fs;

     if( pfd == NULL || pfd->openFdList[fd].fsFd == 0 )
     {
	  errno = EBADF;
	  return EERROR;
     }

     fs = pfd->openFdList[fd].fs;

     if( fs->fops.fstat == NULL )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     return fs->fops.fstat( pfd->openFdList[fd].fsFd, st );
}

int isatty(int fd)
{
     __ProcFileDesc *pfd = __vfs_get_proc_fd( current_process() );
     __FileSystem *fs;

     if( pfd == NULL || pfd->openFdList[fd].fsFd == 0 )
     {
	  errno = EBADF;
	  return EERROR;
     }

     fs = pfd->openFdList[fd].fs;

     if( fs->fops.isatty == NULL )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     return fs->fops.isatty( pfd->openFdList[fd].fsFd );
}

int lseek(int fd, int offset, int whence)
{
     __ProcFileDesc *pfd = __vfs_get_proc_fd( current_process() );
     __FileSystem *fs;

     if( pfd == NULL || pfd->openFdList[fd].fsFd == 0 )
     {
	  errno = EBADF;
	  return EERROR;
     }

     fs = pfd->openFdList[fd].fs;

     if( fs->fops.lseek == NULL )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     return fs->fops.lseek( pfd->openFdList[fd].fsFd, offset, whence );
}

int read(int fd, void *ptr, int len)
{
     __ProcFileDesc *pfd = __vfs_get_proc_fd( current_process() );
     __FileSystem *fs;

     if( pfd == NULL || pfd->openFdList[fd].fsFd == 0 )
     {
	  errno = EBADF;
	  return EERROR;
     }

     fs = pfd->openFdList[fd].fs;

     if( fs->fops.read == NULL )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     return fs->fops.read( pfd->openFdList[fd].fsFd, ptr, len );
}

void __vfs_init()
{
     /* Currently empty */
}

void __vfs_init_proc(U32 pid)
{
     __ProcFileDesc *fdl;
     fdl = malloc( sizeof(__ProcFileDesc) );

     fdl->pid     = pid;
     fdl->numOpen = 0;
     fdl->pNext   = NULL;

     memset( &(fdl->openFdList), 0, sizeof(__FileDesc) * __VFS_MAX_FD );

     if( !__fd_list )
	  __fd_list = fdl;
     else
     {
	  __ProcFileDesc *fdl_p = __fd_list;
	  while( fdl_p->pNext )
	       fdl_p = fdl_p->pNext;

	  fdl_p->pNext = fdl;
     }
}

/* Return the list of file descriptors for the process,
   or NULL if process not found */
__ProcFileDesc *__vfs_get_proc_fd(U32 pid)
{
     __ProcFileDesc *fdl = __fd_list;

     if( fdl == NULL )
	  return NULL;

     while( fdl != NULL && fdl->pid != pid )
	  fdl = fdl->pNext;

     return fdl;
}

int __vfs_mount(char *source, char *destination, char *type, U32 flags)
{
     /* For now, we do not validate that the mount point exists.
	This must be added in the future */

     __MountEntry *me = NULL;
     __FileSystem *fs = NULL;
     int ret;

     if( destination == NULL || strlen(destination) == 0 || 
	 type == NULL || strlen(type) == 0 )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     fs = __vfs_get_fs_by_type( type );

     if( fs == NULL )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     /* Call FS specific mount handler */
     ret = fs->fops.fs_mount( source, destination, flags );

     if( ret < 0 )
	  return ret;

     me = malloc( sizeof(__MountEntry) );
     strncpy( me->mountPoint, destination, __VFS_MAX_PATH_LEN );

     me->mountPoint[__VFS_MAX_PATH_LEN] = '\0';
     me->fs = fs;
     me->pNext = NULL;

     if( __mount_list == NULL )
	  __mount_list = me;
     else
     {
	  __MountEntry *mel = __mount_list;

	  while( mel->pNext != NULL )
	       mel = mel->pNext;

	  mel->pNext = me;
     }

     return 0;
}

int __vfs_umount(char *path, U32 flags)
{
     return 0;
}

int __vfs_register_fs(char *type, __FileOps *fops)
{
     __FileSystem *fs = malloc( sizeof(__FileSystem) );

     if( fops == NULL || type == NULL || strlen( type ) == 0 ||
	  fops->fs_mount == NULL || fops->fs_umount == NULL )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     strncpy( fs->fsName, type, __VFS_MAX_FS_NAME_LEN );

     /* This is correct, as the field length is (max len) + 1
	Add NULL terminator as strncpy does not guarantee this */
     fs->fsName[__VFS_MAX_FS_NAME_LEN] = '\0';

     memcpy( &fs->fops, fops, sizeof(__FileSystem) );

     if( __fs_list == NULL )
	  __fs_list = fs;
     else
     {
	  __FileSystem *fs_p = __fs_list;

	  while( fs_p->pNext != NULL )
	       fs_p = fs_p->pNext;

	  fs_p->pNext = fs;
     }

     return 0;
}

__FileSystem *__vfs_get_fs_handler(const char *path)
{
     __MountEntry *pList = __mount_list;
     __FileSystem *fs    = NULL;

     if( pList == NULL )
	  return NULL;

     while( pList != NULL )
     {
	  if( strcasestr( path, pList->mountPoint ) == path )
	  {
	       /* Currently best match for target FS */
	       fs = pList->fs;
	  }

	  pList = pList->pNext;
     }

     return fs;
}

__FileSystem *__vfs_get_fs_by_type(char *type)
{
     __FileSystem *pFs = __fs_list;

     while( pFs != NULL )
     {
	  if( strcmp( pFs->fsName, type ) == 0 )
	       return pFs;

	  pFs = pFs->pNext;
     }

     return NULL;
}
