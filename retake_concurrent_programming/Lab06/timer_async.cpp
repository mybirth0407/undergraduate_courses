#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

void Print(const boost::system::error_code &e) {
    std::cout << "Hello, World!" << std::endl;
}

int main() {
    boost::asio::io_service io;
    boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
    t.async_wait(&Print);
    puts("after async_wait");
    
    io.run();
    puts("after io.run()");
    
    return 0;
}
