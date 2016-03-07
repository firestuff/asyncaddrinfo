#include <assert.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "asyncaddrinfo.h"

int main(int argc, char *argv[]) {
	asyncaddrinfo_init(2);

	// These will run in parallel. You can still queue more lookups; they'll run 
	// on the first available thread.
	int fd1 = asyncaddrinfo_resolve("github.com", "https", NULL);
	assert(fd1 >= 0);
	int fd2 = asyncaddrinfo_resolve("google.com", "https", NULL);
	assert(fd2 >= 0);

	// This thread continues while lookups run in parallel.

	// We use epoll here, but you could use poll, select, or just call
	// asyncaddrinfo_result() and block. You could also use an event loop library
	// like libevent.
	int epoll_fd = epoll_create1(0);
	assert(epoll_fd >= 0);

	struct epoll_event ev = {
		.events = EPOLLIN,
	};
	
	// See epoll_ctl(2) for details on how to attach pointers to application data.
	ev.data.fd = fd1;
	assert(!epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd1, &ev));

	ev.data.fd = fd2;
	assert(!epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd2, &ev));


	int max_events = 2;
	while (max_events) {
		struct epoll_event events[max_events];
		int num_events = epoll_wait(epoll_fd, events, max_events, -1);
		assert(num_events > 0);
		for (int i = 0; i < num_events; i++) {
			struct addrinfo *addrs;
			int err = asyncaddrinfo_result(events[i].data.fd, &addrs);
			if (err) {
				// Error; see gai_strerror(err)
			} else {
				// Success; see addrs
				freeaddrinfo(addrs);
			}
			max_events--;
		}
	}

	assert(!close(epoll_fd));
	// asyncaddrinfo_result() closes the fds opened by asyncaddrinfo_resolve()

	asyncaddrinfo_cleanup();
}
