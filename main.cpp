#include <vector>
#include <iostream>
#include <iomanip>

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <pthread.h>

#define srand48(s) srand(s)
#define drand48() (((double)rand())/((double)RAND_MAX))
#define lrand48() rand()

//namespace my
//{
//// N is the size (in elements) of 90% of the usages
//// and has been measured from real-world cases.

//// TODO: instead of straight delegating to std::vector, provide
//// your own, more performant implementation.
//// Only the constructor, push_back() and size() are required.

//template <class T,int N>
//class vector
//{
//public:
//    inline explicit vector( size_t s ): container(s)
//    {
//        container.reserve(N);
//    }

//    inline void push_back( const T& t )
//    {
//        container.push_back( t );
//    }

//    inline size_t size() const
//    {
//        return container.size();
//    }

//private:
//    std::vector<T> container;
//};
//}

namespace my
{
template <class T,int N>
class vector
{
public:
    vector(size_t s );
    void push_back( const T& t );
    size_t size();
    ~vector();

private:
    T* container_;
    size_t size_;
    size_t capacity_;
};

template <class T, int N>
vector<T, N>::vector(size_t s) {
    if(s > N) {
        capacity_ = s;
    } else {
        capacity_ = N;
    }
    size_ = 0;
    container_ = new T[capacity_];
}

template <class T, int N>
vector<T, N>::~vector() {
    delete[] container_;
}

template <class T, int N>
size_t vector<T, N>::size() {
    return size_;
}

template <class T, int N>
void vector<T, N>::push_back(const T &t) {
        // relocate memory
        if(size_ == capacity_) {
            size_t newCapacity = capacity_ + (size_t)ceil(N * 0.2);
            T* newContainer = new T[newCapacity];
            memcpy(newContainer, container_, capacity_ * sizeof(T));
            capacity_ = newCapacity;
            delete[] container_;
            container_ = newContainer;
        }
        container_[size_] = t;
        size_++;
}
}



/******************* TEST HARNESS *******************/
// Do not modify unless for solving compiling issues


template<class V,int N>
void run()
{
    V vector( 0 ); // Size is unknown at construction time

    // Generate a random size following an exponential distribution, with
    // size<N 90% of the time.
    const float lambda = -logf(0.1f)/N;
    const size_t maxSize = 1000000; // Clamp size to 1,000,000
    const size_t size = std::min( maxSize, size_t(-logf( drand48() )/lambda) );

    for( size_t j=0 ; j<size ; j++ )
    {
        vector.push_back( j );
    }
    assert( vector.size() == size );
}

timespec timediff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0)
    {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    }
    else
    {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

template<class V,int N>
void* threadmain( void* )
{
    const int64_t runs = 1000000;
    for( int64_t i=0 ; i<runs ; i++ )
    {
        run<V,N>();
    }
    return NULL;
}

template<class V,int N>
timespec test()
{
    timespec start;
    clock_gettime( CLOCK_REALTIME, &start );

    const int32_t nThreads = 4;
    pthread_t threads[nThreads];
    for( int32_t t=0 ; t<nThreads ; ++t )
    {
        pthread_create( threads+t, NULL, &threadmain<V,N>, NULL );
    }
    for( int32_t t=0 ; t<nThreads ; ++t )
    {
        void* result = NULL;
        pthread_join( threads[t], &result );
    }

    timespec end;
    clock_gettime( CLOCK_REALTIME, &end );

    return timediff( start, end );
}

std::ostream& operator<<( std::ostream& s, const timespec& t )
{
    return s << t.tv_sec << "." << std::setfill ('0') << std::setw (9) << t.tv_nsec;
}

template<int N>
void harness()
{
    std::cout << "Testing with 90% of the vectors under " << N << " elements: " << std::endl;

    const timespec t1 = test<my::vector<int,N>,N>();
    std::cout << " * " << t1 << " seconds with custom vector" << std::endl;

    const timespec t2 = test<std::vector<int>,N>();
    std::cout << " * "  << t2 << " seconds with std::vector" << std::endl;

    const double speedup = (t2.tv_sec + 1e-9*t2.tv_nsec)/(t1.tv_sec + 1e-9*t1.tv_nsec);
    std::cout << " * Speedup factor: "  << std::setprecision(2) << speedup << "x" << std::endl;
}

int main(int /*argc*/, char * /*argv*/ [])
{
    srand48( time( NULL ) );
    harness<8>();
    harness<64>();
    harness<100>();
    harness<200>();
    harness<500>();
    return 0;
}

