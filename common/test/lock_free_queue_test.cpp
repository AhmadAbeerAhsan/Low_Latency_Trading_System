#include "lock_free_queue.hpp"
#include "thread_utils.hpp"

struct MyStruct
{
    int d_[3];
};

using namespace Common;

auto consumeFunction(LFQueue<MyStruct> *lfq)
{
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(5s);    // so that "lfq->size()" returns true in the while loop

    while (lfq->size())
    {
        const auto d = lfq->getNextToRead();
        lfq->updateReadIndex();
        std::cout << "consumeFunction read elem:" << d->d_[0]
                  << "," << d->d_[1] << "," << d->d_[2] << " lfq-size:"
                  << lfq->size() << std::endl;
        std::this_thread::sleep_for(1s);
    }
    std::cout << "consumeFunction exiting." << std::endl;
}

int main()
{
    LFQueue<MyStruct> lfq(20);
    auto ct = createAndStartThread("", consumeFunction, &lfq);

    for (auto i = 0; i < 50; ++i)
    {
        const MyStruct d{i, i * 10, i * 100};
        *(lfq.getNextToWriteTo()) = d;
        lfq.updateWriteIndex();
        std::cout << "main constructed elem:" << d.d_[0] << ","
                  << d.d_[1] << "," << d.d_[2] << " lfq-size:" << lfq.size() << std::endl;
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

    ct->join();
    std::cout << "main exiting." << std::endl;
    return 0;
}