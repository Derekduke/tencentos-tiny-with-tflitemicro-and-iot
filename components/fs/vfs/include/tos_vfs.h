/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _TOS_VFS_H_
#define  _TOS_VFS_H_

#include "tos_k.h"

#include "tos_vfs_err.h"
#include "tos_vfs_types.h"
#include "tos_vfs_file.h"
#include "tos_vfs_device.h"
#include "tos_vfs_fs.h"
#include "tos_vfs_inode.h"

#define TOS_CFG_VFS_PATH_MAX        26

__API__ int tos_vfs_open(const char *pathname, vfs_oflag_t flags);

__API__ int tos_vfs_close(int fd);

__API__ ssize_t tos_vfs_read(int fd, void *buf, size_t count);

__API__ ssize_t tos_vfs_write(int fd, const void *buf, size_t count);

__API__ vfs_off_t tos_vfs_lseek(int fd, vfs_off_t offset, vfs_whence_t whence);

__API__ int tos_vfs_ioctl(int fd, unsigned long request, ...);

__API__ int tos_vfs_sync(int fd);

__API__ int tos_vfs_dup(int oldfd);

__API__ int tos_vfs_fstat(int fd, vfs_fstat_t *buf);

__API__ int tos_vfs_ftruncate(int fd, vfs_off_t length);

__API__ VFS_DIR *tos_vfs_opendir(const char *name);

__API__ int tos_vfs_closedir(VFS_DIR *dirp);

__API__ vfs_dirent_t *tos_vfs_readdir(VFS_DIR *dirp);

__API__ int tos_vfs_rewinddir(VFS_DIR *dirp);

__API__ int tos_vfs_statfs(const char *path, vfs_fsstat_t *buf);

__API__ int tos_vfs_unlink(const char *pathname);

__API__ int tos_vfs_mkdir(const char *pathname);

__API__ int tos_vfs_rmdir(const char *pathname);

__API__ int tos_vfs_rename(const char *oldpath, const char *newpath);

__API__ int tos_vfs_stat(const char *pathname, vfs_fstat_t *buf);

#endif /* _TOS_VFS_H_ */

