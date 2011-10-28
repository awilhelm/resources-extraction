#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <glib.h>
#include <glob.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "format.h"

static struct format
	*formats = 0;
static bool
	opt_dry_run = false,
	opt_keep_going = false;
static size_t
	opt_pulse = 0x1000;

static void extract(const char *file) {
	const int fd = open(file, O_RDONLY); assert(fd >= 0);
	struct stat st;
	assert(!stat(file, &st));
	const char *p = mmap(0, (size_t)(st.st_size), PROT_READ, MAP_PRIVATE, fd, 0); assert(p != MAP_FAILED);
	fprintf(stderr, "%s\n", file);
	for(size_t offset = 0; offset < (size_t)(st.st_size); ++offset, ++p) {
		if(!(offset % opt_pulse)) fprintf(stderr, "\r%d", offset);
		for(struct format *format = formats; format; format = format->next) {

			const size_t size = format->probe(p, (size_t)(st.st_size) - offset);
			if(!size) continue;

			if(opt_dry_run) {
				fprintf(stderr, "\n%d.%s (%d)\n", offset, format->extension, size);
				continue;
			}

			char out_file[100];
			assert(sprintf(out_file, "%d.%s", offset, format->extension) > 0);
			const int out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC); assert(out_fd >= 0);
			assert(write(out_fd, p, size) >= 0);
			assert(!close(out_fd));

			if(!opt_keep_going) continue;
		}
	}
	fprintf(stderr, "\n");
	assert(!close(fd));
}

static void load_format(const char *file) {
	void *module = dlopen(file, RTLD_LAZY | RTLD_LOCAL); if(!module) goto except;
	void (*init)(struct format *) = dlsym(module, "init"); if(!init) goto except;
	struct format *format = malloc(sizeof(struct format)); assert(format);
	init(format);
	format->next = formats;
	formats = format;
except:
	fprintf(stderr, "%s\n", dlerror());
	abort();
}

static void for_each(const char *pattern, void(*function)(const char *)) {
	glob_t files = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	assert(!glob(pattern, 0, 0, &files));
	for(size_t i = 0; i < files.gl_pathc; ++i) {
		function(files.gl_pathv[i]);
	}
	globfree(&files);
}

static int on_arg_import(void *_1, const char *pattern, void *_2, void *_3) {
	(void) _1;
	(void) _2;
	(void) _3;
	for_each(pattern, load_format);
	return TRUE;
}

static int on_arg_other(void *_1, const char *pattern, void *_2, void *_3) {
	(void) _1;
	(void) _2;
	(void) _3;
	for_each(pattern, extract);
	return TRUE;
}

int main(int argc, char **argv) {
	const GOptionEntry entries[] = {

		{"dry-run", 'n', 0, G_OPTION_ARG_NONE, &opt_dry_run, "Print offsets, do not extract", 0},
		{"no-dry-run", 'N', G_OPTION_FLAG_REVERSE | G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_NONE, &opt_dry_run, 0, 0},

		{"keep-going", 'k', 0, G_OPTION_ARG_NONE, &opt_keep_going, "Try all formats, even if one of them was successful", 0},
		{"no-keep-going", 'K', G_OPTION_FLAG_REVERSE | G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_NONE, &opt_keep_going, 0, 0},

		{"import", 'f', G_OPTION_FLAG_FILENAME, G_OPTION_ARG_CALLBACK, on_arg_import, "Import the given modules", "PATTERN"},

		{"pulse", 0, 0, G_OPTION_ARG_INT, &opt_pulse, "Set the progress indicator refresh rate", "N"},

		{G_OPTION_REMAINING, 0, G_OPTION_FLAG_FILENAME, G_OPTION_ARG_CALLBACK, on_arg_other, 0, "FILES"},

		{0, 0, 0, 0, 0, 0, 0}};
	GOptionContext *context = g_option_context_new("- Extract resources from games"); assert(context);
	g_option_context_add_main_entries(context, entries, 0);
	assert(g_option_context_parse(context, &argc, &argv, 0));
}
