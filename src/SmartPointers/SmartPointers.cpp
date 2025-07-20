#include "SmartPointers.hpp"
#include <memory>
#include <vector>

SmartPointers::Simple::Simple(int id) : m_id{id}
{
    std::cout << "Simple " << m_id << " created" << std::endl;
}
void SmartPointers::Simple::doSomething()
{
    std::cout << "Simple " << m_id << " did something" << std::endl;
}
SmartPointers::Simple::~Simple()
{
    std::cout << "Simple " << m_id << " destroyed" << std::endl;
}

void processSimpleObject(SmartPointers::Simple* simple) {
    simple->doSomething();
}

int* my_alloc(int a) {
    return new int{a};
}

void my_delete(int* p) {
    std::cout << *p << " is deleted" << std::endl;
    delete p;
}

std::unique_ptr<SmartPointers::Simple> create(int n) {
    return std::make_unique<SmartPointers::Simple>(n);
}

void SmartPointers::main_uniquePointer()
{
    std::unique_ptr<Simple> simple0 { std::make_unique<Simple>(0) };
    simple0->doSomething();
    (*simple0).doSomething();
    processSimpleObject(simple0.get()); //.get() to get access to the underlying raw Pointer

    simple0.reset();    //Frees Resource and sets it to nullPtr
    simple0.reset(new Simple(2));   //Frees Resources and sets it to a new Simple Instance

    Simple* rawSimple {simple0.release()};  //simple0 is nullptr as it transfers ownership to the rawSimple
    rawSimple->doSomething();
    delete rawSimple;

    std::unique_ptr<Simple> simple1 { std::make_unique<Simple>(1) };

    std::unique_ptr<int, decltype(&my_delete)> myIntSmartPointer {my_alloc(42), my_delete};

    std::unique_ptr<Simple> simple55 { create(55) };
    simple55->doSomething();
}

class Foo
{
    public:
    Foo(int value) : m_data { value } { }
    int m_data;
};

void SmartPointers::main_shardPointer()
{
    std::shared_ptr<Simple> simple0 { std::make_shared<Simple>(0) };
    std::shared_ptr<Simple> simple01 { simple0 };   //Make a Copy of the Pointer

    processSimpleObject(simple0.get()); //.get() to get access to the underlying raw Pointer
    //simple0.reset();    //Frees Resource and sets it to nullPtr only when the last sharedPtr is reset
    std::cout << "Number of shared_ptr instances that are sharing the same resource: " << simple0.use_count() << std::endl;
    
    //the foo object is only destroyed when both foo and aliasing objects are destroyed
    std::shared_ptr<Foo> foo { std::make_shared<Foo>(42) };
    std::shared_ptr<int> aliasing { std::shared_ptr<int> { foo, &foo->m_data } };
}

void SmartPointers::exercise7()
{
    std::unique_ptr<std::vector<int>> values { std::make_unique<std::vector<int>>(10) };
    for (size_t i = 0; i < values->size(); i++)
    {
        values->at(i) = i;
    }
    values->at(values->size() - 1) = 99;

    for (size_t i = 0; i < values->size(); i++)
    {
        std::cout << values->at(i) << std::endl;
    }
}
