#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include "main.h"

static lcu_sync_t exit_sync;

static void install_handlers();
static void exit_handler(int, siginfo_t *, void *);

int main(int argc, const char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    lcu_sync_create(&exit_sync);
    
    // let's enable our multi-threaded logger to stdout
    if (lcu_logger_create(LCU_STDOUT) == -1)
    {
        printf("error: lcu_logger_create() failed\n");
        exit(EXIT_FAILURE);
    }

    install_handlers();

    example_linked_list();
    
    example_tpool();

    lcu_logger_print("Ctrl + C to exit\n\n");
    lcu_sync_wait(&exit_sync);

    lcu_logger_destroy();

    return EXIT_SUCCESS;
}

#define INSTALL_SIGNAL_HANDLER(sig, act) \
    do { \
        if (sigaction(sig, &act, NULL) == -1) { \
            lcu_logger_print("%d: %s", errno, strerror(errno)); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

void install_handlers()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = &exit_handler;
    sigemptyset(&act.sa_mask);

    INSTALL_SIGNAL_HANDLER(SIGTERM, act);
    INSTALL_SIGNAL_HANDLER(SIGINT, act);
    INSTALL_SIGNAL_HANDLER(SIGABRT, act);
}

void exit_handler(int sig, siginfo_t *sig_info, void *context)
{
    UNUSED(sig);
    UNUSED(sig_info);
    UNUSED(context);

    lcu_sync_signal(&exit_sync);
}
