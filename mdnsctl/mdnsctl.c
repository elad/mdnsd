/*
 * Copyright (c) 2010 Christiano F. Haesbaert <haesbaert@haesbaert.org>
 * Copyright (c) 2006 Michele Marchetto <mydecay@openbeer.it>
 * Copyright (c) 2005 Claudio Jeker <claudio@openbsd.org>
 * Copyright (c) 2004, 2005 Esben Norby <norby@openbsd.org>
 * Copyright (c) 2003 Henning Brauer <henning@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_media.h>
#include <net/if_types.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <imsg.h>

#include "mdns.h"
#include "parser.h"

__dead void		 usage(void);
static void		 bhook(char *, char *, char *, int, void *);

struct parse_result	*res;

__dead void
usage(void)
{
	extern char *__progname;

	fprintf(stderr, "usage: %s command [argument ...]\n", __progname);
	exit(1);
}

int
main(int argc, char *argv[])
{
	int			 brsock, r = 0, done = 0;
	struct mdns_browse	 mb;
	struct in_addr		 addr;
	struct hinfo		 hi;
	char			 hostname[MAXHOSTNAMELEN];
	char			 txt[MAX_CHARSTR];
	/* parse options */
	if ((res = parse(argc - 1, argv + 1)) == NULL)
		exit(1);

	done = 0;
	/* process user request */
	switch (res->action) {
	case NONE:
		usage();
		/* not reached */
		break;
	case LOOKUP:
		if (res->flags & F_A || !res->flags) {
			r = mdns_lkup(res->hostname, &addr);
			if (r == 0)
				printf("Address not found.\n");
			else if(r == 1)
				printf("Address: %s\n", inet_ntoa(addr));
			else
				err(1, "mdns_lkup");
		}

		if (res->flags & F_HINFO) {
			r = mdns_lkup_hinfo(res->hostname, &hi);
			if (r == 0)
				printf("Hinfo not found.\n");
			else if (r == 1) {
				printf("Cpu: %s\n", hi.cpu);
				printf("Os: %s\n",  hi.os);
			}
			else
				err(1, "mdns_lkup_hinfo");
		}

		if (res->flags & F_SRV) {
/*			r = mdns_lkup_srv(res->hostname, &srv); */
/*			if (r == 0) */
/*				printf("SRV not found.\n"); */
/*			else if (r == 1) { */
/*				printf("Name: %s\n", srv.dname); */
/*				printf("Port: %u\n", srv.port); */
/*				printf("Priority: %u\n", srv.priority); */
/*				printf("Weight: %u\n", srv.weight); */
/*			} */
/*			else */
/*				err(1, "mdns_lkup_srv"); */
			errx(1, "fix me");
		}

		if (res->flags & F_TXT) {
			r = mdns_lkup_txt(res->hostname, txt, sizeof(txt));
			if (r == 0)
				printf("TXT not found.\n");
			else if (r == 1) {
				printf("TXT: %s\n", txt);
			}
			else
				err(1, "mdns_lkup_txt");
		}

		break;
	case LOOKUP_ADDR:
		r = mdns_lkup_addr(&res->addr, hostname,
		    sizeof(hostname));
		switch (r) {
		case 0:
			printf("Name not found.\n");
			exit(1);
			break;	/* NOTREACHED */
		case 1:
			printf("Hostname: %s\n", hostname);
			exit(0);
			break;	/* NOTREACHED */
		default:
			err(1, "mdns_lkup_addr");
			break;
		}
		break;
	case BROWSE_PROTO:
		if ((brsock = mdns_browse_open(&mb, bhook, &mb)) == -1)
			err(1, "mdns_browse_open");
		/* res->app and res->proto will be NULL if argument is "all" */
		if (mdns_browse_add(&mb, res->app, res->proto) == -1)
			err(1, "mdns_browse_add");
		if (r == -1)
			err(1, "select");
		for (; ;) {
			r = mdns_browse_read(&mb);
			if (r == -1) {
				mdns_browse_close(&mb);
				errx(1, "mdns_browse_read");
			}
			else if (r == 0) {
				mdns_browse_close(&mb);
				errx(1, "Server closed socket");
			}
		}
		break;		/* NOTREACHED */
	}

	return (0);		/* NOTREACHED */
}

static void
bhook(char *name, char *app, char *proto, int ev, void *v_mb)
{
	struct mdns_browse	*mb;
	char			 c;

	mb = v_mb;
	c = ev == SERVICE_UP ? '+' : '-';
	/* This is a service, hence, there is a name for it */
	if (name != NULL) {
		printf("%c%c%c %-48s %-20s %-3s\n", c, c, c, name, app, proto);
		if (res->flags & F_RESOLV) {
/*			struct mdns_service	ms; */
/*			int			r; */

/*			r = mdns_res_service(name, app, proto, &ms); */
/*			if (r == -1) */
/*				err(1, "mdns_res_service"); */
/*			else if (r == 0) */
/*				warnx("Can't find service %s", name); */
/*			else { */
/*				printf("\tName: %s\n", ms.dname); */
/*				printf("\tPriority: %u\n", ms.priority); */
/*				printf("\tWeight: %u\n", ms.weight); */
/*				printf("\tPort: %u\n", ms.port); */
/*				printf("\tAddress: %s\n", inet_ntoa(ms.addr)); */
/*				printf("\tTxt: %s\n", ms.txt); */
/*			} */
			errx(1, "mdns resolve service not implemented");
		}
	}
	else /* No name, this is an application protocol, add browsing for it */
		if (mdns_browse_add(mb, app, proto) == -1)
			err(1, "mdns_browse_add");
	fflush(stdout);
}
