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

#include "tos_vfs.h"

__STATIC__ vfs_file_t vfs_file_pool[VFS_FILE_OPEN_MAX] = { { K_NULL, 0 } };

__KNL__ vfs_file_t *vfs_fd2file(int fd)
{
    if (unlikely(fd < 0 || fd >= VFS_FILE_OPEN_MAX)) {
        return K_NULL;
    }
    return &vfs_file_pool[fd];
}

__KNL__ int vfs_file2fd(vfs_file_t *file)
{
    if (unlikely((cpu_addr_t)file < (cpu_addr_t)&vfs_file_pool[0] ||
        (cpu_addr_t)file > (cpu_addr_t)&vfs_file_pool[VFS_FILE_OPEN_MAX - 1])) {
        return -1;
    }

    return (cpu_addr_t)file - (cpu_addr_t)&vfs_file_pool[0];
}

__KNL__ vfs_file_t *vfs_file_alloc(void)
{
    int i = 0;
    vfs_file_t *file = K_NULL;

    for (; i < VFS_FILE_OPEN_MAX; ++i) {
        file = &vfs_file_pool[i];
        if (file->inode == K_NULL) {
            return file;
        }
    }
    return K_NULL;
}

__KNL__ void vfs_file_free(vfs_file_t *file)
{
    if (vfs_file2fd(file) < 0) {
        return;
    }

    file->inode = K_NULL;
    file->pos   = 0;
}

__KNL__ vfs_file_t *vfs_file_dup(vfs_file_t *old_file)
{
    vfs_file_t *new_file = K_NULL;

    new_file = vfs_file_alloc();
    if (!new_file) {
        return K_NULL;
    }

    vfs_inode_refinc(old_file->inode);

    new_file->flags     = old_file->flags;
    new_file->pos       = old_file->pos;
    new_file->inode     = old_file->inode;
    new_file->private   = old_file->private;

    return new_file;
}

__KNL__ vfs_dir_t *vfs_dir_alloc(void)
{
    return (vfs_dir_t *)tos_mmheap_calloc(1, sizeof(vfs_dir_t));
}

__KNL__ void vfs_dir_free(vfs_dir_t *dir)
{
    tos_mmheap_free(dir);
}

