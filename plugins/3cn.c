/// \file
/// Décode les archives de « Microsoft 3D Movie Maker ».

#include <extractor/format.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	char id1[4];
	gint16 unknown1[8];
	char id2[4];
	gint16 unknown2[5];
	guint8 length;
	char data[];
} __attribute__((packed)) *entry_t;

static size_t probe(const char *p, size_t max) {
	const entry_t pp = (entry_t) p;
	if(max < sizeof(*pp) || strncmp(pp->id1, "DNSM", 4)) return 0;
	printf("%4.4s %s\n", pp->id2, pp->data);
	return 0;
}

void init(struct format *p) {
	p->probe = probe;
	p->extension = "3cn";
}
