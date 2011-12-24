#include <extractor/format.h>
#include <glib.h>
#include <string.h>

typedef struct {
	char chunk_id[4];
	guint32 chunk_size;
} __attribute__((packed)) *chunk_t;

static size_t probe(const char *p, size_t max) {
	const chunk_t pp = (chunk_t) p;
	if(strncmp(pp->chunk_id, "RIFF", 4)) return 0;
	size_t size = 8 + GUINT32_FROM_LE(pp->chunk_size);
	return size < max ? size : 0;
}

void init(struct format *p) {
	p->probe = probe;
	p->extension = "wav";
}
