/// \file
/// Décode les fichiers de dialogues de Baldur's Gate.

#include <extractor/format.h>
#include <stdio.h>
#include <string.h>

enum string_flag {
	TEXT_PRESENT = 1,
	SND_PRESENT = 2,
	SNDLENGTH_PRESENT = 4,
};

typedef struct {
	char file_type[4];
	char file_version[4];
	unsigned short language_id;
	unsigned string_count;
	unsigned string_entries_offset;
} __attribute__((packed)) *header_t;

typedef struct {
	char flags[2];
	char sound_res_ref[8];
	char volume_variance[4];
	char pitch_variance[4];
	unsigned offset_to_string;
	unsigned string_size;
} __attribute__((packed)) *string_data_element_t;

static size_t probe(const char *p, size_t max) {
	if(strncmp(p, "TLK V1  ", 8)) return 0;
	const header_t header = (header_t)(p);
	const string_data_element_t entries = (string_data_element_t)(p + header->string_entries_offset);
	for(unsigned i = 0; i < header->string_count; ++i) {
		const char *string = p + entries[i].offset_to_string;
		for(unsigned j = 0; j < entries[i].string_size; ++j) {
			if(string[j] == '\n') putchar('|');
			else putchar(string[j]);
		}
		putchar('\n');
	}
	return max;
}

void init(struct format *p) {
	p->probe = probe;
	p->extension = "tlk";
}
