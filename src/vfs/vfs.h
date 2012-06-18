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

#ifndef _ppcos_vfs_h_
#define _ppcos_vfs_h_

#include "krntypes.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "dirent.h"

#define __VFS_MAX_FD          128
#define __VFS_MAX_FS_NAME_LEN 32
#define __VFS_MAX_PATH_LEN    256

typedef struct _FileOps {
     int (*fs_mount)(char *, char *, U32);
     int (*fs_umount)(char *, U32);
     int (*open)(const char *, int, ...);
     int (*creat)(const char *, mode_t);
     int (*close)(int);
     int (*fstat)(int, struct stat *);
     int (*isatty)(int);
     int (*lseek)(int, int, int);
     int (*read)(int, void *, int);
     int (*write)(int, const void *, int);
     struct dirent *(*readdir)(dir_ptr_t *);
     dir_ptr_t (*opendir)(char *);
     int (*closedir)(dir_ptr_t *);
} __FileOps;

typedef struct _FileSystem {
     char fsName[__VFS_MAX_FS_NAME_LEN + 1];
     __FileOps fops;

     struct _FileSystem *pNext;
} __FileSystem;

typedef struct _FileDesc {
     int           fsFd;
     __FileSystem *fs;
} __FileDesc;

typedef struct _ProcFileDesc {
     U32 pid;
     U32 numOpen;

     __FileDesc openFdList[__VFS_MAX_FD];

     struct _ProcFileDesc *pNext;
} __ProcFileDesc;

typedef struct _MountEntry {
     char          mountPoint[__VFS_MAX_PATH_LEN];
     __FileSystem *fs;

     struct _MountEntry *pNext;
} __MountEntry;

/* Internal */

void __vfs_init();
void __vfs_init_proc(U32 pid);
int __vfs_mount(char *source, char *destination, char *type, U32 flags);
int __vfs_umount(char *path, U32 flags);
int __vfs_register_fs(char *type, __FileOps *fops);
__ProcFileDesc *__vfs_get_proc_fd(U32 pid);
__FileSystem *__vfs_get_fs_handler(const char *path);
__FileSystem *__vfs_get_fs_by_type(char *type);


/* External */

int open(const char *pathname, int flags, ...);
int creat(const char *pathname, mode_t mode);
int close(int fd);
int fstat(int fd, struct stat *st);
int isatty(int fd);
int lseek(int fd, int offset, int whence);
int read(int fd, void *ptr, int len);
int write(int fd, void *ptr, int len);

#endif  /* _ppcos_vfs_h_ */
