#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;

int main()
{
    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::duration<long long, std::ratio<1l,100000l> > fsec;
    auto t0 = Time::now();
    for(int i=0;i<100;i++)
    {
    	auto t1 = Time::now();
    	std::cout << std::chrono::duration_cast<fsec>(t1 - t0).count() << "\n";
    }
}
