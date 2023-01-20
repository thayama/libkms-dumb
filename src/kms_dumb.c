// SPDX-License-Identifier: MIT
//
// Derived from libkms by VMware, Inc.
//
// Copyright (C) 2023 Renesas Electronics Corporation
// Copyright (C) 2009 VMware, Inc., Palo Alto, CA., USA
//
// Author: Takanari Hayama <taki@igel.co.jp>
//
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>

#include <xf86drm.h>

#include "kms_dumb.h"

struct kms_driver {
	int fd;
};

struct kms_bo {
	struct kms_driver *kms;
	unsigned handle;
	unsigned pitch;
	size_t size;
	void *ptr;
};

int kms_bo_create(struct kms_driver *kms, const unsigned *attr, struct kms_bo **out)
{
	struct drm_mode_create_dumb arg = {
		.bpp = 32,
	};

	while (*attr != KMS_TERMINATE_PROP_LIST) {
		unsigned key = *attr++;
		unsigned value = *attr++;

		switch (key) {
		case KMS_WIDTH:
			arg.width = value;
			break;
		case KMS_HEIGHT:
			arg.height = value;
			break;
		case KMS_BO_TYPE:
			// ignore
			break;
		default:
			return -EINVAL;
                }
        }

        if (arg.width == 0 || arg.height == 0)
                return -EINVAL;

	struct kms_bo *bo = calloc(1, sizeof(*bo));
	if (!bo)
		return -ENOMEM;

	int rc = drmIoctl(kms->fd, DRM_IOCTL_MODE_CREATE_DUMB, &arg);
	if (rc) {
		free(bo);
		return rc;
	}

	bo->kms = kms;
	bo->handle = arg.handle;
	bo->pitch = arg.pitch;
	bo->size = arg.size;
	*out = bo;

	return 0;
}

int kms_bo_get_prop(struct kms_bo *bo, unsigned key, unsigned *out)
{
	switch (key) {
	case KMS_PITCH:
		*out = bo->pitch;
		break;
	case KMS_HANDLE:
		*out = bo->handle;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

int kms_bo_map(struct kms_bo *bo, void **out)
{
	if (!bo->ptr) {
		struct drm_mode_map_dumb arg = {
			.handle = bo->handle,
		};

		int rc = drmIoctl(bo->kms->fd, DRM_IOCTL_MODE_MAP_DUMB, &arg);
		if (rc)
			return rc;

		void *map = mmap(NULL, bo->size, PROT_READ | PROT_WRITE, MAP_SHARED, bo->kms->fd, arg.offset);
		if (map == MAP_FAILED)
			return -errno;
	}

	*out = bo->ptr;
	return 0;
}

int kms_bo_unmap(struct kms_bo *bo)
{
	// NOP
	return 0;
}

int kms_bo_destroy(struct kms_bo **bo)
{
	struct drm_mode_destroy_dumb arg = {
		.handle = (*bo)->handle,
	};
	int rc = drmIoctl((*bo)->kms->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &arg);
	if (rc)
		return -errno;
	free(*bo);
	*bo = NULL;
	return 0;
}

int kms_create(int fd, struct kms_driver **out)
{
	uint64_t cap;
	int rc = drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &cap);

	if (rc != 0 || cap == 0)
		return -EINVAL;

	*out = calloc(1, sizeof(struct kms_driver));	
	(*out)->fd = fd;

	return 0;
}

int kms_destroy(struct kms_driver **kms)
{
	if (*kms) {
		free(*kms);
		*kms = NULL;
	}
	return 0;
}
