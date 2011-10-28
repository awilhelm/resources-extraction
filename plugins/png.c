#include <extractor/format.h>
#include <glib.h>
#include <string.h>

typedef struct {
	guint32 length;
	char type[4];
} __attribute__((packed)) *chunk_t;

static size_t probe(const char *p, size_t max) {
	if(strncmp(p, "\211PNG\r\n\032\n", 8)) return 0;
	size_t size = 8;
	for(;;) {
		if(size >= max) return 0;
		const chunk_t chunk = (chunk_t)(p + size);
		size += 12 + GUINT32_FROM_BE(chunk->length);
		if(!strncmp(chunk->type, "IEND", 4)) return size;
	}
}

void init(struct format *p) {
	p->probe = probe;
	p->extension = "png";
}
