#include <iostream>
using namespace std;

class Base {
public:
    virtual void print() { cout << "Base class" << endl; }
};

class Derived : public Base {
public:
    void print() override { cout << "Derived class" << endl; }
    void specific() { cout << "Derived specific method" << endl; }
};

int main() {
    // 1. static_cast - for general type conversions
    double d = 3.14;
    int i = static_cast<int>(d);  // Convert double to int
    cout << "static_cast: " << i << endl;

    // 2. dynamic_cast - for safe downcasting in class hierarchies
    Base* basePtr = new Derived();
    Derived* derivedPtr = dynamic_cast<Derived*>(basePtr);
    if (derivedPtr) {
        cout << "dynamic_cast: ";
        derivedPtr->specific();  // Safe to call Derived methods
    }

    // 3. const_cast - to add/remove const/volatile qualifiers
    const int value = 10;
    int* modifiable = const_cast<int*>(&value);
    *modifiable = 20;  // Undefined behavior if value was originally const
    cout << "const_cast: " << *modifiable << endl;

    // 4. reinterpret_cast - for low-level implementation-dependent casts
    int num = 0x12345678;
    char* bytes = reinterpret_cast<char*>(&num);
    cout << "reinterpret_cast: ";
    for (size_t i = 0; i < sizeof(num); ++i) {
        cout << hex << (int)bytes[i] << " ";
    }
    cout << endl;

    delete basePtr;
    return 0;
}

/*
static_cast: 用于基本类型的转换和类层次间的上行转换
dynamic_cast: 用于类层次间的下行转换，需要运行时类型检查
const_cast: 用于移除const/volatile属性
reinterpret_cast: 用于低层次的重新解释转换

标准输出：static_cast: 3
dynamic_cast: Derived specific method
const_cast: 20
reinterpret_cast: 78 56 34 12 
*/