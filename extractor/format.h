#pragma once

#include <stddef.h>

struct format {
	struct format *next;
	size_t (*probe)(const char *, size_t);
	char *extension;
};
