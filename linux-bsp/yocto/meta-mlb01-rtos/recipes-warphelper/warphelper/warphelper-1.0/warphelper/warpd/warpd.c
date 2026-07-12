/**
 * Warp helper daemon
 *
 *  Copyright (C) 2014 Lineo Solutions, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>
#include <signal.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#include "warpd.h"
#include "warp_helper_config.h"

#define FLAG_KILL		(0x1 << 0)
#define FLAG_FOREGROUND		(0x1 << 1)

volatile sig_atomic_t eflag = 0;

/*------------------------------------------------------------------*/
/* helper                                                           */
/*------------------------------------------------------------------*/
void handler(int signum)
{
	eflag = signum;
}

void help()
{
	printf("options:\n");
	printf("\t-?: show this help\n");
	printf("\t-k: kill running warpd\n");
	printf("\t-f: launch warpd in the foreground\n");
	printf("\t-c [conf_path]: specify config file full path\n");
}

/*------------------------------------------------------------------*/
/* Main                                                             */
/*------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	int i, j;
	int msq_rid = -1;
	int msq_sid = -1;
	int ret = 0;
	int ret2 = 0;
	FILE *ident_file;
	FILE *proc_file;
 	pid_t pid;
	key_t ident_rkey, ident_skey;
	long cnt, size;
	long num_procs = NUM_OF_PROCS;
	long pnum;
	unsigned long flag = 0;
	unsigned long *ready_proc_flags;
	struct msgbuf msq;
	char *conf_file_path = CONF_FILE;

	int ident_rid = IDENT_RID;
	int ident_sid = IDENT_SID;
	char *ident_file_path = IDENT_FILE;
	char *snapshot_path = SNAPSHOT_WRAPPER;
	long proc_delay = 0;

	/**
	 * open syslog
	 */
	openlog("warpd", LOG_PID, LOG_DAEMON);

	/**
	 * command option table
	 */
	const struct option longopt[] = {
		{"help", no_argument, NULL, '?'},
		{"kill", no_argument, NULL, 'k'},
		{"foreground", no_argument, NULL, 'f'},
		{"conffile", required_argument, NULL, 'c'},
		{NULL, 0, NULL, 0}
	};

	/**
	 * parse command option
	 */
	while ((i = getopt_long(argc, argv, "c:fkh?", longopt, NULL)) != -1) {
		switch (i) {
		case 'c':
			conf_file_path = optarg;
			break;
		case 'f':
			flag |= FLAG_FOREGROUND;
			break;
		case 'k':
			flag |= FLAG_KILL;
			break;
		case 'h':
		case '?':
		default:
			help();
			return 0;
		}
	}

	/**
	 * load config file setting
	 */
	ret = read_conffile( conf_file_path);
	if ( ret == 0){
		if ( strlen( warp_ipc_key)){
			ident_file_path =warp_ipc_key;
		}

		if ( strlen( warp_script)){
			snapshot_path = warp_script;
		}

		if ( warp_daemon_id){
			ident_rid = warp_daemon_id;
		}

		if ( warp_client_id){
			ident_sid = warp_client_id;
		}

		if ( warp_processes){
			num_procs = warp_processes;
		}
		if ( warp_delay){
			proc_delay = warp_delay;
		}
	}else{
		syslog(LOG_ERR, "cannot read configuration file.( %s )\n", conf_file_path);
		return -1;
	}

	if (flag & FLAG_KILL) {
		/**
		 * stop existing warpd procsss
		 */
		ident_file = fopen(ident_file_path, "r");
		if (ident_file != NULL) {
			ret = fscanf(ident_file, "%d\n", &pid);
			fclose(ident_file);
			remove(ident_file_path);
			if ((ret =! 0) && (ret != EOF)) {
				if (kill(pid, SIGTERM) == -1) {
					syslog(LOG_ERR, "failed to kill running warpd.\n");
					return -1;
				}
			}
		} else {
			syslog(LOG_ERR, "no warpd process to kill\n");
			return -1;
		}
		return 0;
	} else {
		/**
		 * check existing warpd procsss
		 */
		ident_file = fopen(ident_file_path, "r");
		if (ident_file != NULL) {
			fclose(ident_file);
			syslog(LOG_ERR, "%s already exists. warpd running?\n",
				ident_file_path);
			return -1;
		}
		
	}

	if (flag & FLAG_FOREGROUND) {
		syslog(LOG_INFO, "launched in the foreground.\n");
	} else {
		/**
		 * daemonize warpd
		 */
		if (daemon(1, 0) == -1) {
			syslog(LOG_ERR, "failed to launch as daemon.\n");
			return -1;
		}
		syslog(LOG_INFO, "launched as daemon.\n");
	}

	/**
	 * alloc memory
	 */
	size = num_procs / 8;
	if (num_procs % 8)
		size++;
	if (size % sizeof(unsigned long))
		size += sizeof(unsigned long) - (size % sizeof(unsigned long));
	ready_proc_flags = malloc(size);
	if (!ready_proc_flags) {
		syslog(LOG_ERR, "malloc failed\n");
		return -1;
	}
	memset(ready_proc_flags, 0, size);

	/**
	 * register signal handler
	 */
	if (signal(SIGHUP, handler) == SIG_ERR) {
		syslog(LOG_ERR, "failed to set signal handler\n");
		free(ready_proc_flags);
		return -1;
	}
	if (signal(SIGINT, handler) == SIG_ERR) {
		syslog(LOG_ERR, "failed to set signal handler\n");
		free(ready_proc_flags);
		return -1;
	}
	if (signal(SIGTERM, handler) == SIG_ERR) {
		syslog(LOG_ERR, "failed to set signal handler\n");
		free(ready_proc_flags);
		return -1;
	}

	/**
	 * create ident file, and save the PID
	 */
	pid = getpid();
	ident_file = fopen(ident_file_path, "w+");
	if (ident_file != NULL) {
		fprintf(ident_file, "%d\n", pid);
		fclose(ident_file);
	} else {
		syslog(LOG_ERR, "failed to record process id to file.\n");
		free(ready_proc_flags);
		return -1;
	}

	/**
	 * get warpd rx message queue
	 */
	ident_rkey = ftok(ident_file_path, ident_rid);
	if (ident_rkey == -1) {
		syslog(LOG_ERR, "failed to create System V IPC key.\n");
		ret = -1;
		goto out;
	}
	msq_rid = msgget(ident_rkey, IPC_CREAT|0666);
	if (msq_rid == -1) {
		syslog(LOG_ERR, "failed to create message queue.\n");
		ret = -1;
		goto out;
	}

	/**
	 * get warpd tx message queue
	 */
	ident_skey = ftok(ident_file_path, ident_sid);
	if (ident_skey == -1) {
		syslog(LOG_ERR, "failed to create System V IPC key.\n");
		ret = -1;
		goto out;
	}
	msq_sid = msgget(ident_skey, IPC_CREAT|0666);
	if (msq_sid == -1) {
		syslog(LOG_ERR, "failed to create message queue.\n");
		ret = -1;
		goto out;
	}


	/**
	 * receive messages from each process
	 */
	cnt = num_procs;
	while (!eflag) {
		ret = msgrcv(msq_rid, &msq, sizeof(msq.mdata), 0, 0);
		if (ret == -1) {
			if (eflag)
				break;
			syslog(LOG_ERR, "msgrcv failed(%d).\n", errno);
			goto out;
		}
		//syslog(LOG_INFO, "got message(%ld %s)\n", msq.mtype, msq.mdata);
		if (strncmp(msq.mdata, PROC_READY_STR, ret))
			continue;

		pnum = msq.mtype;
		i = (pnum -1) / (sizeof(unsigned long) * 8);
		j = (pnum -1) % (sizeof(unsigned long) * 8);
		if (!(ready_proc_flags[i] & (1 << j))) {
			ready_proc_flags[i] |= (1 << j);
			cnt--;
		}else{
			syslog(LOG_ERR, "received same proc_num(%ld) message.\n", pnum);
			ret = -1;
			goto out;
		}
		if ( warp_waittable){
			int lp1;
			syslog(LOG_INFO, "Waiting counter = %ld/%ld\n", cnt, num_procs);
			syslog(LOG_INFO, "Waiting Tabel : \n");
			for ( lp1 = 0; lp1 < (size/sizeof(unsigned long)); lp1 ++){
				syslog( LOG_INFO, "0x%08lx",ready_proc_flags[lp1]);
			}
		}
		if (!cnt)
			break;
	}

	ret = 0;
	if (eflag) {
		syslog(LOG_INFO, "stopped with signal(%d).\n", eflag);
		goto out;
	}

	/**
	 * execute snapshot wrapper
	 */
	if ( warp_emulation == 1){
		/* emulation mode */
		syslog(LOG_INFO, "Running snapshot emulation : %s", snapshot_path);
		ret = system(snapshot_path);
		if (ret == -1 || WEXITSTATUS(ret) == 127) {
			syslog(LOG_ERR, "cannot execute emulation resume \n");
			memcpy(msq.mdata, SNAPSHOT_FAIL2_STR, sizeof(msq.mdata));
		}else{
			syslog(LOG_ERR, "resume success\n");
			memcpy(msq.mdata, SNAPSHOT_DONE_STR, sizeof(msq.mdata));
		}
	} else {
		ret = system(snapshot_path);
		if (ret == -1 || WEXITSTATUS(ret) == 127) {
			syslog(LOG_ERR, "cannot execute %s.\n", snapshot_path);
			memcpy(msq.mdata, SNAPSHOT_FAIL2_STR, sizeof(msq.mdata));
		} else {
			int chk_stat = 0;

			proc_file = fopen(WARP_ERROR_FILE, "r");
			if (proc_file != NULL) {
				ret2 = fscanf(proc_file, "%d\n", &ret);
				fclose(proc_file);
				if ((ret2 != 0) && (ret2 != EOF)) {
					if (!ret) {
						chk_stat = 1;
					} else {
						syslog(LOG_ERR, "snapshot failed(%d).\n", ret);
						memcpy(msq.mdata, SNAPSHOT_FAIL1_STR,
							sizeof(msq.mdata));
					}
				} else {
					syslog(LOG_ERR, "cannot read %s.\n", WARP_ERROR_FILE);
					memcpy(msq.mdata, SNAPSHOT_FAIL3_STR, sizeof(msq.mdata));
					ret = -1;
				}
			} else {
				syslog(LOG_ERR, "cannot open %s.\n", WARP_ERROR_FILE);
				memcpy(msq.mdata, SNAPSHOT_FAIL3_STR, sizeof(msq.mdata));
				ret = -1;
			}

			if (chk_stat) {
				proc_file = fopen(WARP_STAT_FILE, "r");
				if (proc_file != NULL) {
					ret2 = fscanf(proc_file, "%d\n", &ret);
					fclose(proc_file);
					if ((ret2 != 0) && (ret2 != EOF)) {
						if (!ret) {
							syslog(LOG_INFO, "snapshot success.\n");
							memcpy(msq.mdata,
								SNAPSHOT_DONE_STR,
								sizeof(msq.mdata));
						} else {
							syslog(LOG_ERR, "warp boot success.\n");
							memcpy(msq.mdata,
								WARP_BOOT_DONE_STR,
								sizeof(msq.mdata));
						}
					} else {
						syslog(LOG_ERR, "cannot read %s.\n", WARP_STAT_FILE);
						memcpy(msq.mdata, SNAPSHOT_FAIL3_STR, sizeof(msq.mdata));
						ret = -1;
					}
				} else {
					syslog(LOG_ERR, "cannot open %s.\n", WARP_STAT_FILE);
					memcpy(msq.mdata, SNAPSHOT_FAIL3_STR, sizeof(msq.mdata));
					ret = -1;
				}
			}
		}
	}

	/**
	 * send messages to each process
	 */
	cnt = 1;
	while (!eflag) {
		usleep(	proc_delay);
		msq.mtype = cnt;
		ret2 = msgsnd(msq_sid, &msq, sizeof(msq.mdata), 0);
		if (ret2 == -1) {
			if (eflag)
				break;
			syslog(LOG_ERR, "msgsnd failed(%d).\n", errno);
			if (!ret)
				ret = -1;
			goto out;
		}
		cnt++;
		if (cnt > num_procs)
			break;
	}

	if (eflag)
		syslog(LOG_INFO, "stopped with signal(%d).\n", eflag);

out:
	if (msq_rid != -1)
		msgctl(msq_rid, IPC_RMID, NULL);
	if (msq_sid != -1)
		msgctl(msq_sid, IPC_RMID, NULL);
	remove(ident_file_path);
	free(ready_proc_flags);
	return ret;
}
