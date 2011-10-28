/// \file
/// \bug Ne reconnaît pas tous les fichiers.

#include <extractor/format.h>
#include <glib.h>
#include <string.h>

typedef struct {
	guint8 marker_msb;
	guint8 marker;
	guint16 length;
} __attribute__((packed)) *segment_t;

static size_t probe(const char *p, size_t max) {
	if(strncmp(p, "\377\330\377", 3)) return 0;
	size_t size = 2;
	for(;;) {
		if(size >= max + 1000) return 0;
		const segment_t segment = (segment_t)(p + size);
		size += 2;
		switch(segment->marker) {
			case 0XD9: // "end of image"
				return size;
			case 0XFF: // remplissage avant un marqueur
				--size;
				continue;
			case 0XD0:
			case 0XD1:
			case 0XD2:
			case 0XD3:
			case 0XD4:
			case 0XD5:
			case 0XD6:
			case 0XD7:
			case 0XD8:
				continue;
			case 0XDD:
				size += 2;
				continue;
			case 0XDA: // "start of scan" : on cherche le prochain marqueur...
				size += GUINT16_FROM_BE(segment->length);
				while(p[size] != -1 || !p[size + 1]) ++size;
				continue;
			default:
				size += GUINT16_FROM_BE(segment->length);
		}
	}
}

void init(struct format *p) {
	p->probe = probe;
	p->extension = "jpg";
}
