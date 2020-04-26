/*******************************************************************************
 * Copyright (c) 2010 Wind River Systems, Inc. and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 * The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 * You may elect to redistribute this code under either of these licenses.
 *
 * Contributors:
 *     Wind River Systems - initial API and implementation
 ******************************************************************************/

#include <vxWorks.h>
#include <logLib.h>

#include <tcf_config.h>
#include <framework/asyncreq.h>
#include <framework/events.h>
#include <framework/timed_event_queue.h>
#include <framework/trace.h>
#include <services/discovery.h>
#include <main/cmdline.h>
#include <main/services.h>
#include <main/server.h>

#include "tcfLib.h"

#define VX_TCF_TASK_OPTIONS     (VX_FP_TASK)

#define EVENT_STACK_SIZE  (40*1024)
#define TIMED_STACK_SIZE  (4*1024)

VX_TASK(tTCF_events, EVENT_STACK_SIZE);
VX_TASK(tTCF_timed_events, TIMED_STACK_SIZE);

#if (_WRS_VXWORKS_MAJOR < 6)                                            \
        || (_WRS_VXWORKS_MAJOR == 6) && (_WRS_VXWORKS_MINOR < 9)
typedef int _Vx_usr_arg_t;
#endif

static Protocol * proto;

void tcf_init_service (void (*init_func) (Protocol *)) {
    init_func(proto);
}

static void vx_run_event_loop (Protocol * proto, TCFBroadcastGroup * bcg) {
    size_t url_size = 512;
    char * url;
    extern pthread_t event_thread;

    open_log_file("-");
    log_mode = 0;

    url = malloc(url_size);
    if (url == NULL)
        goto error;
    while (snprintf(url, url_size,"TCP:;Name=VxWorks System Agent;BSP=%s", sysModel()) >= url_size) {
        url_size *= 2;
        url = realloc(url, url_size);
        if (url == NULL)
            goto error;
    }
    /* Process events - must run on the initial thread since ptrace()
     * returns ECHILD otherwise, thinking we are not the owner. */
    event_thread = pthread_self();

    /* The static services must be initialised before the plugins */
    ini_services(proto, bcg);
    ini_server(url, proto, bcg);
    free(url);
    discovery_start();

    run_event_loop();
    return;

error:
    logMsg("Unable to start the system TCF agent event loop\n", 1,2,3,4,5,6);
}


static void vx_run_timed_event_loop (void) {
    timed_event_queue_init();
    timed_event_loop_run();
}


/**
 * Initialize the TCF agent and associated services on this target.
 * This is intended to be called from VxWorks' startup routines
 * (usrToolsInit(), etc.) so defers most of its work to a spawned task which
 * eventually drives TCF's event thread.  However, since other routines may
 * want to register their own TCF services, we need to set up the protocol
 * handle here.
 *
 * Much of this code is shared with main/main.c:main(), and should probably be
 * better abstracted between the two.
 */
void tcfLibInit (unsigned int task_priority) {
    TCFBroadcastGroup * bcg;

    ini_mdep();
    ini_trace();
    ini_events_queue();
    ini_asyncreq();

    bcg = broadcast_group_alloc();
    proto = protocol_alloc();

    VX_TASK_INSTANTIATE(tTCF_events, task_priority, VX_TCF_TASK_OPTIONS, \
                        EVENT_STACK_SIZE, vx_run_event_loop,		\
                        (_Vx_usr_arg_t) proto, (_Vx_usr_arg_t) bcg,	\
                        2,3,4,5,6,7,8,9);
#if 0
    /* the timed event loop is currently not used by clients, so don't bother
       starting it.  At some point this could be parameterized via CDF/VSB
       options. */
    VX_TASK_INSTANTIATE(tTCF_timed_events, task_priority, VX_TCF_TASK_OPTIONS, \
                        TIMED_STACK_SIZE, vx_run_timed_event_loop,	\
                        0,1,2,3,4,5,6,7,8,9);
#endif
}
