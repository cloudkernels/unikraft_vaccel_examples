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
#define LIB_PATH "/data/BS/libkrnl_BS.so"

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

int vaccel_blackscholes_genop(char *close, size_t close_size, char *dates, size_t dates_size)
{
	int ret = 0;
	struct vaccel_session sess;
	struct vaccel_arg args[6];
	uint32_t output;

	enum vaccel_op_type op_type = VACCEL_EXEC;
        ret = vaccel_sess_init(&sess, 0);
        if (ret != VACCEL_OK) {
                fprintf(stderr, "Could not initialize session\n");
                return 1;
        }

        printf("Initialized session with id: %u\n", sess.session_id);

	char operation[256] = "blackscholes_unpack";
	char library[512] = LIB_PATH;
#if 0
	ret = get_path(library, __FILE__, "libkrnl_BS.so", 512);
	if (ret)
		return VACCEL_ENOENT;
#endif

	memset(args, 0, sizeof(args));
	/* VACCEL_EXEC */
	args[0].size = sizeof(enum vaccel_op_type);
	args[0].buf = &op_type;

	/* shared object on host */
	args[1].size = strlen(library);
	args[1].buf = library;
	/* symbol to call in object*/
	args[2].size = strlen(operation);
	args[2].buf = operation;

	/* operation arguments*/
	args[3].size = close_size;
	args[3].buf = close;
	args[4].size = dates_size;
	args[4].buf = dates;

	/* output*/
	args[5].size = sizeof(uint32_t);
	args[5].buf = &output;

	printf("library:%s\n", library);
        ret = vaccel_genop(&sess, &args[0], 5, &args[5], 1);
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

int vaccel_blackscholes(char *close, size_t close_size, char *dates, size_t dates_size)
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

	char operation[256] = "blackscholes_unpack";
	char library[512] = LIB_PATH;
#if 0
	ret = get_path(library, __FILE__, "libkrnl_BS.so", 512);
	if (ret)
		return VACCEL_ENOENT;
#endif

	memset(args, 0, sizeof(args));
	args[0].size = close_size;
	args[0].buf = close;
	args[1].size = dates_size;
	args[1].buf = dates;

	printf("%ld\n", close_size);
	printf("%ld\n", dates_size);
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
struct vaccel_prof_region blackscholes_op_stats =
	VACCEL_PROF_REGION_INIT("vaccel_blackscholes");
#endif

int blackscholes(int argc, char **argv)
{
	size_t close_size, dates_size;
	char *close, *dates;

	if (argc < 2 )
		return -1;

	if (read_file(argv[1], &close, &close_size))
		return 1;

	if (read_file(argv[2], &dates, &dates_size))
		return 1;

#if 0
	vaccel_prof_region_start(&blackscholes_op_stats);
#endif

	int ret = vaccel_blackscholes(close, close_size, dates, dates_size);

#if 0
	vaccel_prof_region_stop(&blackscholes_op_stats);
#endif

	return ret;
}

#if 0
__attribute__((constructor))
static void vaccel_blackscholes_init(void)
{
}

__attribute__((destructor))
static void vaccel_blackscholes_fini(void)
{
	vaccel_prof_region_print(&blackscholes_op_stats);
}
#endif

int main(int argc, char **argv)
{
	int iterations = 1;
	printf("argc:%d\n", argc);
	if (argc > 2 && strcmp(argv[argc-2], "-it") == 0) {
		iterations = atoi(argv[argc-1]);
		argc=argc-2;
	}

	for (int i = 0; i < iterations; i++)
		blackscholes(argc, argv);

	return 0;
}
