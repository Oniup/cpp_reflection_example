#include <iostream>

#include "reflection.hpp"

class Person {
    REFLECT()
public:
    Person(const std::string& name, int age) : m_name(name), m_age(age) {};

    const std::string& get_name() const { return m_name; }
    int get_age() const { return m_age; }

private:
    std::string m_name{};
    int m_age{};
};

int main(int argc, char* argv[]) {
    Person* person = new Person("Bob", 29);

    reflect::TypeDescriptor* descriptor = reflect::TypeDescriptor::get<Person>();
    reflect::print_value(*person);

    delete person;
    return 0;
}

REFLECT_INIT_BEGIN(Person)
REFLECT_INIT_MEMBER(m_name)
REFLECT_INIT_MEMBER(m_age)
REFLECT_INIT_END()

