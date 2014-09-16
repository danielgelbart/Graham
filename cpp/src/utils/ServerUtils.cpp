#include "ServerUtils.h"

/**
 * helper function
 */
void 
set_result(boost::optional<boost::system::error_code> *destination,
           boost::system::error_code source) 
{
    destination->reset( source );
}

/**
 * helper function
 */
void 
set_bytes_result(boost::optional<boost::system::error_code> *error_destination,
                 size_t *transferred_destination,
                 boost::system::error_code error_source,
                 size_t transferred_source) 
{
    error_destination->reset( error_source );
    *transferred_destination = transferred_source;
}

/**
 * emulate synchronous read with a timeout on socket
 *
 * returns -1 on error or socket close, 0 on timeout, or bytes received
 */
ssize_t 
read_with_timeout(boost::asio::ip::tcp::socket &socket,
                  void *buf, size_t count, int seconds) 
{
    boost::optional<boost::system::error_code> timer_result;
    boost::optional<boost::system::error_code> read_result;
    size_t bytes_transferred;

    // set up a timer on the io_service for this socket
    boost::asio::deadline_timer timer( socket.get_io_service() );
    timer.expires_from_now( boost::posix_time::seconds( seconds ) );
    timer.async_wait(
        boost::bind(
            set_result,
            &timer_result,
            boost::asio::placeholders::error
        )
    );

    // set up asynchronous read (respond when ANY data is received)
    boost::asio::async_read(
        socket,
        boost::asio::buffer( (char *)buf, count ),
        boost::asio::transfer_at_least( 1 ),
        boost::bind(
            set_bytes_result,
            &read_result,
            &bytes_transferred,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );

    socket.get_io_service().reset();

    // set default result to zero (timeout) because another thread
    // may call io_service().stop() deliberately to interrupt the
    // the read (say, if it wanted to signal the thread that there
    // was data to write)
    ssize_t result = 0;
    bool resultset = false;
    while ( socket.get_io_service().run_one() ) {
        if ( read_result ) {
            //boost::system::error_code e = (*read_result);
            //std::cerr << "read_result was " << e.message() << std::endl;
            timer.cancel();
            if ( resultset == false ) {
                result = ( bytes_transferred < 0 ) ? -1 : bytes_transferred;
                resultset = true;
            }
            read_result.reset();
        } else if ( timer_result ) {
            socket.cancel();
            if ( resultset == false ) {
                result = 0;
                resultset = 0;
            }
            timer_result.reset();
        }
    }

    return( result );
}               

ssize_t
write_with_timeout(boost::asio::ip::tcp::socket &socket,
                   void const *buf, size_t count, int seconds) 
{               
    boost::optional<boost::system::error_code> timer_result;
    boost::optional<boost::system::error_code> write_result;
    size_t bytes_transferred;
    
    boost::asio::deadline_timer timer( socket.get_io_service() );
    timer.expires_from_now( boost::posix_time::seconds( seconds ) );
    timer.async_wait(boost::bind(set_result, &timer_result,
                                 boost::asio::placeholders::error));
    
    boost::asio::async_write(
        socket,
        boost::asio::buffer( (char *)buf, count ),
        boost::asio::transfer_at_least( count ), // want to transfer ALL of it
        boost::bind(
            set_bytes_result,
            &write_result,
            &bytes_transferred,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );

    socket.get_io_service().reset();

    size_t result = -1;
    bool resultset = false;
    while ( socket.get_io_service().run_one() ) {
        if ( write_result ) {
            timer.cancel();
            if ( resultset == false ) {
                result = ( bytes_transferred <= 0 ) ? -1 : bytes_transferred;
                resultset = true;
            }
            write_result.reset();
        } else if ( timer_result ) {
            socket.cancel();
            if ( resultset == false ) {
                result = 0;
                resultset = true;
            }
            timer_result.reset();
        }
    }
    return( result );
}
