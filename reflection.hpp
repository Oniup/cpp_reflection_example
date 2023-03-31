#ifndef __REFLECTION_HPP__
#define __REFLECTION_HPP__

#include <iostream>
#include <vector>

#include <string>
#include <cstddef>

namespace reflect {

    struct TypeDescriptor {
        const char* name{ nullptr };
        size_t size{};
        bool primitive{ true };

        TypeDescriptor(const char* name = nullptr, size_t size = 0, bool primitive = true) : name(name), size(size), primitive(primitive) {}
        virtual ~TypeDescriptor() = default;

        virtual void print_value(const std::byte* data, size_t offset) const {};

        template <typename _T> static char func(decltype(&_T::m_Reflection));
        template <typename _T> static int func(...);
        template <typename _T>
        struct IsReflected {
            enum { value = (sizeof(func<_T>(nullptr)) == sizeof(char)) };
        };

        template <typename _T>
        static TypeDescriptor* get_primitive();

        template <typename _T, typename std::enable_if<IsReflected<_T>::value, int>::type = 0>
        static TypeDescriptor* get() {
            return &_T::m_Reflection;
        }

        template <typename _T, typename std::enable_if<!IsReflected<_T>::value, int>::type = 0>
        static TypeDescriptor* get() {
            return get_primitive<_T>();
        }
    };

    struct TypeDescriptor_Object : public TypeDescriptor {
        TypeDescriptor_Object(void (*init_object_reflection_ptrfn)(TypeDescriptor_Object*)) {
            init_object_reflection_ptrfn(this);
        }

        struct Member {
            const char* name;
            const TypeDescriptor* type{ nullptr };
            size_t offset;

            Member& operator=(const Member& other) {
                name = other.name;
                offset = other.offset;
                type = other.type;

                return *this;
            }
        };

        std::vector<Member> members{};
    };

    template <typename _ReflectedObject>
    void print_value(const _ReflectedObject& object) {
        TypeDescriptor* descriptor = TypeDescriptor::get<_ReflectedObject>();
        if (descriptor->primitive) {
            std::cout << "isn't object\n";
            return;
        }

        TypeDescriptor_Object* object_descriptor = static_cast<TypeDescriptor_Object*>(descriptor);
        std::cout << "object " << object_descriptor->name << " {\n";

        for (TypeDescriptor_Object::Member& member : object_descriptor->members) {
            std::cout << "\t" << member.name << " = ";
            member.type->print_value(reinterpret_cast<const std::byte*>(&object), member.offset);
            std::cout << "\n";
        }

        std::cout << "}\n";
    }

    struct TypeDescriptor_Int : public TypeDescriptor { 
        TypeDescriptor_Int() : TypeDescriptor("int", sizeof(int)) {} 

        virtual void print_value(const std::byte* data, size_t offset) const override {
            std::cout << *(int*)(data + offset);
        }
    };
    struct TypeDescriptor_StdString : public TypeDescriptor { 
        TypeDescriptor_StdString() : TypeDescriptor("std::string", sizeof(std::string)) {} 

        virtual void print_value(const std::byte* data, size_t offset) const override {
            std::cout << *(std::string*)(data + offset);
        }
    };

    template <> TypeDescriptor* TypeDescriptor::get_primitive<int>() { static TypeDescriptor_Int descriptor{}; return &descriptor; }
    template <> TypeDescriptor* TypeDescriptor::get_primitive<std::string>() { static TypeDescriptor_StdString descriptor{}; return &descriptor; }

}

#define REFLECT()\
    private:\
        friend struct reflect::TypeDescriptor;\
        static reflect::TypeDescriptor_Object m_Reflection;\
        static void _init_object_reflection(reflect::TypeDescriptor_Object* object);\

#define REFLECT_INIT_BEGIN(TYPE)\
    reflect::TypeDescriptor_Object TYPE::m_Reflection(TYPE::_init_object_reflection);\
    void TYPE::_init_object_reflection(reflect::TypeDescriptor_Object* object) {\
        using _T = TYPE;\
        object->name = #TYPE;\
        object->size = sizeof(_T);\
        object->primitive = false;\
        object->members = {\

#define REFLECT_INIT_MEMBER(NAME)\
            { #NAME, reflect::TypeDescriptor::get<decltype(_T::NAME)>(), offsetof(_T, NAME) },\

#define REFLECT_INIT_END()\
        };\
    }\

#endif