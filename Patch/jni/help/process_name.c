#include <stdlib.h>
#include <string.h>

#include <sys/system_properties.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <fcntl.h>

#include <android/log.h>
#include "log.h"
#include "process_name.h"

#define PROCESS_NAME_DEVICE "/sys/qemu_trace/process_name"

static const char* process_name = "unknown";
static int running_in_emulator = -1;

void set_process_name(const char* new_name) {
	char propBuf[PROP_VALUE_MAX];

	if (new_name == NULL) {
		return;
	}

	// We never free the old name. Someone else could be using it.
	int len = strlen(new_name);
	char* copy = (char*) malloc(len + 1);
	strcpy(copy, new_name);
	process_name = (const char*) copy;

	if (len < 16) {
		prctl(PR_SET_NAME, (unsigned long) new_name, 0, 0, 0);
	} else {
		prctl(PR_SET_NAME, (unsigned long) new_name + len - 15, 0, 0, 0);
	}

	// If we know we are not running in the emulator, then return.
	if (running_in_emulator == 0) {
		return;
	}

	// If the "running_in_emulator" variable has not been initialized,
	// then do it now.
	if (running_in_emulator == -1) {
		if (__system_property_get("ro.kernel.qemu", propBuf)) {
			if (propBuf[0] == '1') {
				running_in_emulator = 1;
			} else {
				running_in_emulator = 0;
				return;
			}
		} else {
			running_in_emulator = 0;
			return;
		}
	}

	// If the emulator was started with the "-trace file" command line option
	// then we want to record the process name in the trace even if we are
	// not currently tracing instructions (so that we will know the process
	// name when we do start tracing instructions).  We do not need to execute
	// this code if we are just running in the emulator without the "-trace"
	// command line option, but we don't know that here and this function
	// isn't called frequently enough to bother optimizing that case.
	int fd = open(PROCESS_NAME_DEVICE, O_RDWR);
	if (fd < 0)
		return;
	write(fd, process_name, strlen(process_name) + 1);
	close(fd);
}

const char* get_process_name(void) {
	return process_name;
}

const char* get_process_name2() {
	char* name = (char*) malloc(PROP_VALUE_MAX);
	memset(name, 0, sizeof(name));
	if (prctl(PR_GET_NAME, (unsigned long) name, 0, 0, 0) == 0) {
		return name;
	} else {
		free(name);
		return NULL;
	}
}


void set_process_name_from_main(const char* procname, int argc, char *argv[]) {
	size_t argv0_len = strlen(argv[0]);
	size_t procname_len = strlen(procname);
	size_t max_procname_len =
			(argv0_len > procname_len) ? procname_len : argv0_len;
	strncpy(argv[0], procname, max_procname_len);
	memset(&argv[0][max_procname_len], '\0', argv0_len - max_procname_len);
	set_process_name(procname);
}
