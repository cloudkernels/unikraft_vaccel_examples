#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <vaccel.h>

/*
 * TODO: THere should be a better way to declare or better search the
 * location of the library
 */
//#include "get_path.h"
#define LIB_PATH "/data/LeNet/libkrnl_LeNet.so"

#define perror(...)			\
do {					\
	fprintf( stderr,"Errno = %d: ", errno);	\
	fprintf( stderr, __VA_ARGS__ );	\
} while(0)

int read_file(const char *filename, char **img, size_t *img_size)
{
	int fd;
	long bytes = 0;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror("open: ");
		return 1;
	}

	struct stat info;
	fstat(fd, &info);
	fprintf(stdout, "File size: %luB\n", info.st_size);

	char *buf = (char*)malloc(info.st_size + 1);
	if (!buf) {
		fprintf(stderr, "Could not allocate memory for image\n");
		goto close_file;
	}

	do {
		int ret = read(fd, buf, info.st_size);
		if (ret < 0) {
			perror("Error while reading image: ");
			goto free_buff;
		}
		bytes += ret;
	} while (bytes < info.st_size);

	if (bytes < info.st_size) {
		fprintf(stderr, "Could not read image\n");
		goto free_buff;
	}

	buf[info.st_size] = '\0';
	*img = buf;
	*img_size = info.st_size + 1;
	close(fd);

	return 0;

free_buff:
	free(buf);
close_file:
	close(fd);
	return 1;
}

int vaccel_lenet(char *file, size_t file_size, char *command, size_t command_size)
{
	int ret = 0;
	struct vaccel_session sess;
	struct vaccel_arg args[3];
	uint32_t output;

        ret = vaccel_sess_init(&sess, 0);
        if (ret != VACCEL_OK) {
                fprintf(stderr, "Could not initialize session\n");
                return 1;
        }

        printf("Initialized session with id: %u\n", sess.session_id);

	char operation[256] = "lenet_unpack";
	//char mode[10] = "fpga";
	char library[512] = LIB_PATH;
#if 0
	ret = get_path(library, __FILE__, "libkrnl_LeNet.so", 512);
	if (ret)
		return VACCEL_ENOENT;
#endif

	memset(args, 0, sizeof(args));
	args[0].size = file_size;
	args[0].buf = file;
	args[1].size = command_size;
	args[1].buf = command;
	//args[2].size = strlen("fpga");
	//args[2].buf = mode;

	args[2].size = sizeof(uint32_t);
	args[2].buf = &output;

	printf("librar:%s\n", library);
        ret = vaccel_exec(&sess, library, operation, &args[0], 2, &args[2], 1);
	if (ret) {
		fprintf(stderr, "Could not run op: %d\n", ret);
		goto close_session;
	}

	printf("output:%u\n", output);
close_session:
        if (vaccel_sess_free(&sess) != VACCEL_OK) {
                fprintf(stderr, "Could not clear session\n");
                return 1;
        }

	return ret;
}

#if 0
	/*
	 * Profiling is not supported in Unikraft yet.
	 */
struct vaccel_prof_region lenet_op_stats =
	VACCEL_PROF_REGION_INIT("vaccel_lenet");
#endif

int lenet(int argc, char **argv)
{
	size_t file_size, command_size;
	char *file, *command;

	if (argc < 2 )
		return -1;

	if (read_file(argv[1], &file, &file_size))
		return 1;

	if (read_file(argv[2], &command, &command_size))
		return 1;

#if 0
	/*
	 * Profiling is not supported in Unikraft yet.
	 */
	vaccel_prof_region_start(&lenet_op_stats);
#endif

	int ret = vaccel_lenet(file, file_size, command, command_size);

#if 0
	/*
	 * Profiling is not supported in Unikraft yet.
	 */
	vaccel_prof_region_stop(&lenet_op_stats);
#endif

	return ret;
}

#if 0
__attribute__((constructor))
static void vaccel_lenet_init(void)
{
}

__attribute__((destructor))
static void vaccel_lenet_fini(void)
{
	vaccel_prof_region_print(&lenet_op_stats);
}
#endif

int main(int argc, char **argv)
{
	int iterations = 1;
	if (argc > 2 && strcmp(argv[argc-2], "-it") == 0) {
		iterations = atoi(argv[argc-1]);
		argc=argc-2;
	}

	for (int i = 0; i < iterations; i++)
		lenet(argc, argv);

	return 0;
}
