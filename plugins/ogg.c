#include <extractor/format.h>
#include <string.h>

typedef struct {
	unsigned char
		capture_pattern[4],
		stream_structure_version,
		header_type_flag,
		absolute_granule_position[8],
		stream_serial_number[4],
		page_sequence_no[4],
		page_checksum[4],
		page_segments,
		segment_table[];
} __attribute__((packed)) *page_t;

static size_t probe(const char *p, size_t max) {
	if(strncmp(p, "OggS", 4)) return 0;
	page_t page = (page_t)(p);
	if(!(page->header_type_flag & 2)) return 0;
	size_t size = 0;
	for(;;) {
		if(size >= max) return 0;
		page = (page_t)(p + size);
		for(int i = 0; i < page->page_segments; ++i) {
			size += page->segment_table[i];
		}
		size += 27u + page->page_segments;
		if(page->header_type_flag & 4) return size;
	}
}

void init(struct format *p) {
	p->probe = probe;
	p->extension = "ogg";
}
