/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <unittest/unittest.hh>

#include <libwrapiter/libwrapiter_forward_iterator.hh>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <list>
#include <string>
#include <utility>

class BaseTest;

class TestList
{
    private:
        static std::list<BaseTest *> _tests;

        TestList()
        {
        }

    public:
        typedef libwrapiter::ForwardIterator<TestList, std::list<BaseTest*>::value_type> Iterator;

        static TestList * instance()
        {
            static TestList result;

            return &result;
        }

        void register_test(BaseTest * const test)
        {
            _tests.push_back(test);
        }

        Iterator begin_tests() const
        {
            return Iterator(_tests.begin());
        }

        Iterator end_tests() const
        {
            return Iterator(_tests.end());
        }
};

std::list<BaseTest *> TestList::_tests;

BaseTest::BaseTest(const std::string & id) :
    _id(id)
{
    TestList::instance()->register_test(this);
}

const std::string BaseTest::id() const
{
    return _id;
}

void
BaseTest::check(const char * const function, const char * const file,
    const long line, bool was_ok, const std::string & message) const
{
    std::cout << "." << std::flush;
    if (! was_ok)
        throw TestFailedException(function, file, line, message);
}

int main(int argc, char** argv) 
{
    int result(EXIT_FAILURE);

    for (TestList::Iterator i(TestList::instance()->begin_tests()),i_end(TestList::instance()->end_tests()) ; i != i_end ; ++i)
    {
        try
        {
            (*i)->run();
            std::cout << (*i)->id() << ": PASSED" << std::endl;
            result = EXIT_SUCCESS;
        }
        catch (TestFailedException & e)
        {
            std::cout << (*i)->id() << ": FAILED" << std::endl;
        }
    }

    return result;
}
