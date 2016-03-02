# asyncaddrinfo

Asynchronous version of [getaddrinfo()](http://man7.org/linux/man-pages/man3/getaddrinfo.3.html).

## Features

* Uses getaddrinfo() underneath, so is quirk-compatible
* Configurable number of resolver threads
* Signals completion via a single file descriptor
	* Compatible with select, poll, epoll, or blocking results
* Simple API; splits getaddrinfo() into two functions, with a file descriptor in between
* valgrind-friendly: closes all fds, frees all allocated memory

## Use

See [example.c](https://github.com/flamingcowtv/asyncaddrinfo/blob/master/example.c)
