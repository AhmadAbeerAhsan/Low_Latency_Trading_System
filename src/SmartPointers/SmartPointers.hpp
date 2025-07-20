#include <iostream>

namespace SmartPointers
{
    void main_uniquePointer();
    void main_shardPointer();

    class Simple
    {
    private:
        int m_id;
    public:
        Simple(int id);
        void doSomething();
        ~Simple();
    };

    void exercise7();
} // namespace SmartPointers
