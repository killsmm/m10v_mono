#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/select.h>
#include <sys/reboot.h>
#include <sys/wait.h>
#include <signal.h>

/* set to NULL to disable shell */
static const char *shell_path	= "/bin/sh";
static const char *console_dev	= "/dev/ttyUSI0";
static const char *server_log	= "/tmp/log";
/* optional list of modules to make sure are inserted very early */
static const char *modpath	= "/etc/early-modules";
static const char *init_fifo	= "/dev/initctl";

struct mp {
	const char *source;	/* source if any, like /dev/mmcblk0p1 */
	const char *target;	/* mount point */
	const char *type;	/* filesystem type */
	unsigned long flags;	/* flags or 0 for defaults */
	const char *mkdir;	/* NULL or directory to create after mount */
	unsigned int perms;	/* permissions on mkdir if non-NULL */
};

static const struct mp mountpoints[] = {
	{
		.source		= "devtmpfs",
		.target		= "/dev",
		.type		= "devtmpfs",
		.mkdir		= "/dev/pts",
		.perms		= 0755
	}, {
		.source		= "proc",
		.target		= "/proc",
		.type		= "proc"
	}, {
		.source		= "sysfs",
		.target		= "/sys",
		.type		= "sysfs"
	}, {
		.source		= "devpts",
		.target		= "/dev/pts",
		.type		= "devpts",
	}, {
		.source		= "tmpfs",
		.target		= "/tmp",
		.type		= "tmpfs"
	}, {
		.source		= "tmpfs",
		.target		= "/var/run",
		.type		= "tmpfs",
		.mkdir		= "/var/run/dbus",
		.perms		= 0777
	}, {
		.source		= "tmpfs",
		.target		= "/run",
		.type		= "tmpfs",
		.perms		= 0777
	}, {
		.source		= "tmpfs",
		.target		= "/mnt",
		.type		= "tmpfs",
		.perms		= 0777
	}, {
		.source		= "tmpfs",
		.target		= "/var/volatile",
		.type		= "tmpfs",
		.mkdir		= "/var/volatile/log",
		.perms		= 0777
	},
};

struct spawn_args {
	const char *wait_path;		/* delay start until this file exists */
	const char *path;		/* run this */
	char * const args[6];	/* last must be NULL */
	const char *env_name;
	const char *env_val;
};

static struct spawn_args spawns[] = {
	{
		.path		= "/usr/bin/dbus-daemon",
		.args		= { "", "--system", "--fork" },
	}, {
		.path		= "/usr/sbin/dropbear",
	}, {
		.path		= "/sbin/udevd",
		.env_name	= "SYSTEMD_LOG_TARGET",
		.env_val	= "console",
	}, {
		.path		= "/etc/init.d/rc.local",
	}, { /* cold plug */
		.wait_path	= "/run/udev/control",
		.path		= "/bin/udevadm",
		.args		= { "", "trigger", "-c", "add", "-y", "mmcblk0" },
	}, { /* cold plug */
		.wait_path	= "/run/udev/control",
		.path		= "/bin/udevadm",
		.args		= { "", "trigger", "-c", "add", "-y", "mmcblk0p1" },
	}, { /* cold plug */
		.wait_path	= "/run/udev/control",
		.path		= "/bin/udevadm",
		.args		= { "", "trigger", "-c", "add", "-y", "mmcblk0p2" },
	}, {
		.path		= "/sbin/ifup",
		.args		= { "", "-af" },
	}, {
		.path		= "/usr/bin/camera_Ev_rec",
	},
};

struct init_request {
	int magic;
	int cmd;
	int runlevel;
	int sleeptime;
	char data[368];
};

#define ARRAY_SIZE(m) (sizeof(m) / sizeof(m[0]))

static int finit_module(int fd, const char *param_values, int flags)
{
	return syscall(SYS_finit_module, fd, param_values, flags);
}

static int insmod(const char *path)
{
	int fd;
	int n;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return -1;

	n = finit_module(fd, "", 0);

	close(fd);

	return n;
}

static int seal(int std, const char *log, int perms)
{
	int tty;

	close(std);
	tty = open(log, perms, 0777);
	if (tty < 0)
		return tty;
	dup2(tty, std);

	return 0;
}

int main(void)
{
	struct init_request request;
	struct timeval tv;
	const char *mpoint;
	char pm[4096];
	fd_set fds;
	int n, fd, len;

//	puts("\nfinit g"BUILD_HASH", "
//		BUILD_USER"@"BUILD_MACHINE", "BUILD_DATE"\n");

	for (n = 0; n < ARRAY_SIZE(mountpoints); n++) {
		mount(mountpoints[n].source,
			mountpoints[n].target,
			mountpoints[n].type,
			mountpoints[n].flags, NULL);
		mkdir(mountpoints[n].mkdir, mountpoints[n].perms);
	}

	n = fork();
	if (n < 0)
		return n;
	if (!n) {
		/* forked child */

		if (shell_path && console_dev) {
			/* sort out a decent tty */
			seal(0, console_dev, O_RDONLY);
			seal(1, console_dev, O_WRONLY);
			seal(2, console_dev, O_WRONLY);

			 /* get session leader */
			setsid();

			/* set controlling terminal */
			ioctl (0, TIOCSCTTY, 1);

			execl(shell_path, shell_path, NULL);

			return 0;
		}
	}

	/* original PID 1 */

	/* insert any modules that are urgent */

	fd = open(modpath, O_RDONLY);
	if (fd >= 0) {
		char buf[4096];
		char m[256];
		int n;
		int c = 0;
		int e = read(fd, buf, sizeof(buf));

		while (c < e) {
			n = 0;
			while (buf[c] != '\n' && c < e)
				m[n++] = buf[c++];

			m[n] = '\0';
			c++;

			insmod(m);
		}
		close(fd);
	}

	fd = open(server_log, O_CREAT | O_TRUNC | O_RDWR, 0777);
	close(fd);

	/* spawn all the apps at once */

	for (n = 0; n < ARRAY_SIZE(spawns); n++) {
		if (fork())
			continue;

		/* if he depends on some file, wait for it */

		if (spawns[n].wait_path) {
			struct stat s;

			/* don't wait if app doesn't exist itself */

			fd = open(spawns[n].path, O_RDONLY);
			if (fd < 0)
				return 0;
			close(fd);

			fd = -1;
			while (fd < 0) {
				fd = stat(spawns[n].wait_path, &s);
				if (fd < 0)
					sleep(1);
			}
		}

		/* add any special environment private to this process */

		if (spawns[n].env_name)
			setenv(spawns[n].env_name, spawns[n].env_val, 1);

		/* redirect his stderr/stdout to the log file and spawn him */

		close(0);
		//seal(1, server_log, O_WRONLY | O_APPEND);
		//seal(2, server_log, O_WRONLY | O_APPEND);

		/* chop up the args */

		*((const char **)&spawns[n].args[0]) = spawns[n].path;
                execv(spawns[n].path, &spawns[n].args[0]);

                /* in case the execl fails */
		return 0;
	}

	/* original PID 1 listens on initctl fifo */

	mkfifo(init_fifo, 0600);
	fd = open(init_fifo, O_RDWR|O_NONBLOCK);
	if (fd < 0) {
		puts("unable to open init fifo\n");
		return 1;
	}

	while (1) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		n = select(fd + 1, &fds, NULL, NULL, &tv);

		waitpid((pid_t)-1, NULL, WNOHANG);

		if (n < 0)
			continue;

		n = read(fd, &request, sizeof(request));
		if (n <= 0)
			continue;

		if (request.magic != 0x03091969 || n != sizeof(request))
			continue;

		if (request.cmd != 1 /* runlevel */)
			continue;

		close(fd);

		/* try to go down nicely */
		kill(-1, SIGTERM);
		sleep(1);
		kill(-1, SIGKILL);
		sleep(1);

		/* unmount everything in /proc/mounts, in reverse */

		fd = open("/proc/mounts", O_RDONLY);
		if (fd < 0)
			goto complete;
		len = read(fd, pm, sizeof pm);
		close(fd);
		if (len < 10)
			goto complete;

		while (len) {
			/* ignore the first line */
			while (len && pm[len] != '\n')
				len--;

			if (!len)
				continue;

			n = len;
			len --; /* finished with that /n */
			while (pm[n] != ' ')
				n++;
			n++;
			mpoint = &pm[n];
			while (pm[n] != ' ')
				n++;
			pm[n] = '\0';

			/* we will deal with / later */
			if ((mpoint[0] == '/' && mpoint[1] == '\0') ||
			     mpoint[0] == '\0')
				continue;

			n = umount2(mpoint, MNT_FORCE);
			printf("  umount for %s: %d\n", mpoint, n);
		}

		/* everything killed, unmounted: attempt to remount / ro */

		if (mount("/", "/", "", MS_RDONLY | MS_REMOUNT, NULL))
			puts("  remount / ro failed\n");
		else
			puts("  remounted / ro\n");

	complete:
		if (request.runlevel == 48)
			reboot(RB_HALT_SYSTEM);

		reboot(RB_AUTOBOOT);
	}

	return 0;
}

