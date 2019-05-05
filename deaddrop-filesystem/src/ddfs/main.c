#include "config.h"
#include "record.h"
#include "crypto.h"
#include "index.h"
#include "macro.h"
#include "ddfs.h"
#include "log.h"

#ifdef DEBUG
#   include <execinfo.h>
#   include <signal.h>
#endif /* DEBUG */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

static ddfs_data_t gddfs_data = {
    .rootdir=NULL,
    .mountdir=NULL
};


void *ddfs_init(struct fuse_conn_info *conn)
{
    /* initialize index if needed */
    IF_NOK(index_create(gddfs_data.rootdir)) {
        log_error("failed to initialize rootdir index.");
    }
    return &gddfs_data;
}

/*
 *
 */
static struct fuse_operations ddfs_ops={
    .getattr=&ddfs_getattr,
    .readlink=NULL,
    .mknod=&ddfs_mknod,
    .mkdir=&ddfs_mkdir,
    .unlink=&ddfs_unlink,
    .rmdir=&ddfs_rmdir,
    .symlink=NULL,
    .rename=&ddfs_rename,
    .link=NULL,
    .chmod=&ddfs_chmod,
    .chown=&ddfs_chown,
    .truncate=NULL,
    .open=&ddfs_open,
    .read=&ddfs_read,
    .write=&ddfs_write,
    .statfs=&ddfs_statfs,
    .flush=NULL,
    .release=&ddfs_release,
    .fsync=&ddfs_fsync,
#ifdef HAVE_SETXATTR
    .setxattr=&ddfs_setxattr,
    .getxattr=&ddfs_getxattr,
    .listxattr=&ddfs_listxattr,
    .removexattr=&ddfs_removexattr,
#endif /* HAVE_SETXATTR */
    .opendir=&ddfs_opendir,
    .readdir=&ddfs_readdir,
    .releasedir=&ddfs_releasedir,
    .fsyncdir=&ddfs_fsyncdir,
    .init=ddfs_init,
    .destroy=NULL,
    .access=&ddfs_access,
    .create=&ddfs_create,
    .ftruncate=&ddfs_ftruncate,
    .fgetattr=&ddfs_fgetattr,
    .lock=NULL,
    .utime=&ddfs_utime,
#ifdef HAVE_UTIMENSAT
    .utimens=&ddfs_utimens,
#endif /* HAVE_UTIMENSAT */
    .bmap=NULL,
    .flag_nullpath_ok=0,
    .flag_nopath=0,
    .flag_utime_omit_ok=0,
    .ioctl=NULL,
    .poll=NULL,
    .write_buf=NULL,
    .read_buf=NULL,
    .flock=NULL,
    .fallocate=NULL
};
/*
 *
 */
void ddfs_usage(const char *prog)
{
	printf("usage: %s username password rootdir mountdir [fuse options]\n", prog);
	printf("\n");
	printf("OPTIONS:\n");
	printf("    username: DDFS username\n");
	printf("    password: DDFS password\n");
	printf("    rootdir:  root directory to be mounted.\n");
	printf("    mountdir: mount point to use\n");
	printf("    [fuse options]: these options will be forwarded to libfuse.\n");
}
/*
 *
 */
int ddfs_parse_options(int *argc, char **argv, ddfs_data_t *ddfs_data)
{
#define CONSUMED 3
	int err, i;
	/* check if command line syntax conditions are met */
	if(((*argc)<(CONSUMED+2))||
		(argv[1][0]=='-')||
		(argv[2][0]=='-')||
		(argv[3][0]=='-')||
		(argv[4][0]=='-')) {
    	ddfs_usage(argv[0]);
    	NOK_GOTO(end);
	}
	/* consume arguments */
	ddfs_data->user.username=argv[1];
	ddfs_data->user.username_len=strlen(argv[1]);
	ddfs_data->user.password=argv[2];
	ddfs_data->user.password_len=strlen(argv[2]);
	ddfs_data->rootdir=realpath(argv[3], NULL);
	ddfs_data->mountdir=realpath(argv[4], NULL);
	/* shift left all other arguments of CONSUMED elements */
	for(i=4;i<(*argc);i++) {
		argv[i-CONSUMED]=argv[i];
	}
	for(i=CONSUMED;i>0;i--) {
		argv[(*argc)-i]=NULL;
	}
	/* decrement argument count */
	(*argc)-=CONSUMED;
	err=OK;
end:
	/* return new argument count */
	return err;
#undef CONSUMED
}
#ifdef DEBUG
/*
 *
 */
void segfault_handler(int sig)
{
    void *array[10];
    size_t size;
    size=backtrace(array, 10);
    fprintf(stderr, "SEGFAULT (sig=%d):\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}
#endif /* DEBUG */
/*
 *
 */
int main(int argc, char **argv)
{
	log_in("argc=%d,argv=%p", argc, argv);
	int err;
    struct fuse_args args;
#ifdef DEBUG
    /* install segfault handler */
    signal(SIGSEGV, segfault_handler);
#endif /* DEBUG */
	/* parse arguments */
   	IF_NOK(ddfs_parse_options(&argc, argv, &gddfs_data)) {
   		log_error("failed to parse ddfs options.");
   		NOK_GOTO(end);
   	}
   	/* debug a bit */
   	debug_ddfs_data_t(&gddfs_data);
   	/* initialize crypto module */
   	IF_NOK(crypto_init(&(gddfs_data.user))) {
   		log_error("failed to initialize crypto module.");
   		NOK_GOTO(end);
   	}
    args.argc=argc;
    args.argv=argv;
    args.allocated=0;
    fuse_opt_parse(&args, NULL, NULL, NULL);
#ifdef DEBUG
    fuse_opt_add_arg(&args, "-f");
    fuse_opt_add_arg(&args, "-d");
#endif /* DEBUG */
    fuse_opt_add_arg(&args, "-s");
    fuse_opt_add_arg(&args, "-odefault_permissions");
   	/* give the control to FUSE */
    err=fuse_main(args.argc, args.argv, &ddfs_ops, NULL);
end:
    /* clear memory */
    FREE(gddfs_data.mountdir); 	/* allocated by realpath */
    FREE(gddfs_data.rootdir);	/* allocated by realpath */
	log_out_err();
    return err;
}
