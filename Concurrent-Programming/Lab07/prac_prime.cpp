#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#define NUM_THREAD_IN_POOL 4

bool IsPrime(int n) {
    if (n < 2) {
        return false;
    }

    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

void PrintResult(int seq_num, int start, int end, unsigned cnt) {
    std::cout << '(' << seq_num << ')' << "number of primes in " <<
        start << '~' << end << " is " << cnt << std::endl;
}

void CountPrime(int seq_num, int start, int end,
    boost::asio::io_service *pio)
{
    unsigned cnt = 0;
    for (int i = start; i < end; ++i) {
        if (IsPrime(i)) {
            ++cnt;
        }
    }
    
    pio->post(boost::bind(PrintResult, seq_num, start, end, cnt));
}


int main() {
    boost::asio::io_service io;
    boost::asio::io_service print_io;
    boost::thread_group threadpool;
    boost::thread_group print_threadpool;
    boost::asio::io_service::work *work = \
        new boost::asio::io_service::work(io);
    boost::asio::io_service::work *print_work = \
        new boost::asio::io_service::work(print_io);
    
    int range_start;
    int range_end;
    
    for (int i = 0; i < NUM_THREAD_IN_POOL; ++i) {
        threadpool.create_thread(boost::bind(
            &boost::asio::io_service::run, &io));
    }
    
    print_threadpool.create_thread(boost::bind(
        &boost::asio::io_service::run, &print_io));

    int seq = 0;
    while (true) {
        scanf("%d", &range_start);
        if (range_start == -1) {
            break;
        }
        scanf("%d", &range_end);
        io.post(boost::bind(CountPrime, seq, range_start, range_end, &print_io));
        ++seq;
    }
    
    delete work;
    threadpool.join_all();
    
    delete print_work;
    print_threadpool.join_all();
    
    io.stop();
    print_io.stop();
    
    return 0;
}