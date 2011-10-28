#include <extractor/format.h>
#include <glib.h>
#include <string.h>

static size_t probe(const char *p, size_t max) {
	if(strncmp(p, "BM", 2)) return 0;
	const size_t size = GUINT32_FROM_LE(*(guint32 *)(p));
	return size >= max ? 0 : size;
}

void init(struct format *p) {
	p->probe = probe;
	p->extension = "bmp";
}
