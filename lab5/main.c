#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <time.h>
#include <unistd.h>


#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

sigset_t mask;


int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    return fcntl(fd, F_SETLK, &fl);
}

void reread(void)
{

}

void *thr_fn(void *str)
{
    int err, signo;

    for (;;) {
        syslog(LOG_INFO, "message: %s, thread: tid: %d, pid: %d\n",(char*)str, gettid(), getpid());
        err = sigwait(&mask, &signo);
        if (err != 0) {
            syslog(LOG_ERR, "ошибка вызова функции sigwait");
            exit(-1);
        }
        switch (signo) {
        case SIGHUP:
            syslog(LOG_INFO, "Чтение конфигурационного файла");
            reread();
            break;
        case SIGTERM:
            syslog(LOG_INFO, "получен сигнал SIGTERM; выход tid=%d",gettid());
            pthread_exit(NULL);
        case SIGKILL:
            syslog(LOG_INFO, "получен сигнал SIGKILL, завершение tid: %d",gettid());
            pthread_exit(NULL);
        case SIGINT:
            syslog(LOG_INFO, "получен сигнал SIGINT, завершение tid: %d",gettid());
            pthread_exit(NULL); ;
        default:
            syslog(LOG_INFO, "получен непредвиденный сигнал %d\n", signo);
        }
    }
    return 0;
}

int already_running(void)
{
    int fd;
    char buf[16];
    struct flock fl;
    fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
    if (fd < 0) {
        syslog(LOG_ERR, "Невозможно открыть %s: %s", LOCKFILE, strerror(errno));
        exit(-1);
    }
    if (lockfile(fd) < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
            return 1;
        }
        syslog(LOG_ERR, "Невозможно установить блокировку на %s: %s", LOCKFILE, strerror(errno));
        exit(-1);
    }
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf)+1);
    return 0;
}

void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    if (umask(0) < 0)
    {
        printf("%s: невозможно выполнить unmask", cmd);
        exit(-1);
    }
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        printf("%s: невозможно получить максимальный номер дескриптора ", cmd);
        exit(-1);
    }
    if ((pid = fork()) < 0) {
        printf("%s: ошибка вызова функции fork", cmd);
        exit(-1);
    }
    else if (pid != 0)
        exit(0);
    if (setsid() < 0)
    {
        printf("невозможно выполнение setsid");
        exit(-1);
    }
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        printf("%s: невозможно игнорировать сигнал SIGHUP", cmd);
        exit(-1);
    }
    if (chdir("/") < 0) {
        printf("%s: невозможно сделать текущим рабочим каталогом /", cmd);
        exit(-1);
    }
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "ошибочные файловые дескрипторы %d %d %d",
                fd0, fd1, fd2);
        exit(-1);
    }
}

int main(int argc, char *argv[])
{


    time_t mytime;
    struct tm *now;
    int err, s;
    pthread_t tid1, tid2;
    pthread_attr_t attr;
    char *cmd;
    struct sigaction sa;
    if ((cmd = strrchr(argv[0], '/')) == NULL)
        cmd = argv[0];
    else
        cmd++;

    daemonize(cmd);
    if (already_running()) {
        syslog(LOG_ERR, "демон уже запущен");
        exit(-1);
    }

    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        printf("Невозможно восстановить действие SIG_DFL для SIGHUP");
        exit(-1);
    }

    sigfillset(&mask);
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0) {
        printf("Ошибка выполнения операции SIG_BLOCK");
        exit(-1);
    }
    s = pthread_attr_init(&attr);
    if (s != 0) {
        printf("Ошибка инициализации атрибутов потока");
        exit(-1);
    }

    char *st1 = "bbb";
    char *st2 = "aaa";

    err = pthread_create(&tid1, NULL, thr_fn, (void *) st1);
    if (err != 0) {
        printf("Невозможно создать поток");
        exit(-1);
    }

    err = pthread_create(&tid2, NULL, thr_fn, (void *) st2);
    if (err != 0) {
        printf("Невозможно создать поток");
        exit(-1);
    }

    if (pthread_join(tid1, NULL)) {
        printf("Ошибка pthread_join");
        exit(-1);
    }

    if (pthread_join(tid2, NULL)) {
        printf("Ошибка pthread_join");
        exit(-1);
    }

    s = pthread_attr_destroy(&attr);
    if (s != 0) {
        printf("Ошибка атрибутов");
        exit(-1);
    }

    for (;;) {
        sleep(3);
        time(&mytime);
        now = localtime(&mytime);
        syslog(LOG_INFO, "Current time is: %s, pid= %d", asctime(now),getpid());
    }
}
