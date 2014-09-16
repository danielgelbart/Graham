#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>

ssize_t read_with_timeout(boost::asio::ip::tcp::socket &socket,
                          void *buf, size_t count, int seconds);
ssize_t write_with_timeout(boost::asio::ip::tcp::socket &socket,
                           void const *buf, size_t count, int seconds);


#endif // SERVER_UTILS_H
