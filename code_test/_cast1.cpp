
/////////////////////////////////////////////////////////////////////////////
// cast_operator_comparison.cpp                                                      
// Language:   C++                   
// Complier:    Visual Studio 2010, Xcode3.2.6 
// Platform:    MacBook Pro 2010
// Application:  none  
// Author:      Ider, Syracuse University  ider.cs@gmail.com
///////////////////////////////////////////////////////////////////////////
#include <string>
#include <iostream>
using namespace std;

class Parents
{
public:
	Parents(string n="Parent"){ name = n;}
	virtual ~Parents(){}
    
	virtual void Speak()
	{
	    cout << "1." << endl;
		cout << "\tI am " << name << ", I love my children." << endl;
	}
	void Work()
	{
	    cout << "2." << endl;
		cout << "\tI am " << name <<", I need to work for my family." << endl;;
	}
protected:
	string name;
};

class Children : public Parents
{
public:
	Children(string n="Child"):Parents(n){ }

	virtual ~Children(){}

	virtual void Speak()
	{
	    cout << "3." << endl;
		cout << "\tI am " << name << ", I love my parents." << endl;
	}
	/*
	 **Children inherit Work() method from parents,
	 **it could be treated like part-time job.
	 */
	void Study()
	{
	    cout << "4." << endl;
		cout << "\tI am " << name << ", I need to study for future." << endl;;
	}
	
private:
	//string name; //Inherit "name" member from Parents
};

class Stranger 
{
public:
	Stranger(string n="stranger"){name = n;}
	virtual ~Stranger(){}

	void Self_Introduce()
	{
	    cout << "5." << endl;
		cout << "\tI am a stranger" << endl;
	}
	void Speak()
	{
	    cout << "6." << endl;
		//cout << "I am a stranger" << endl;
		cout << "\tDo not talk to "<< name << ", who is a stranger." << endl;
	}
private:
	string name;
};

/**
 * @brief 演示C++中各种类型转换的使用场景和安全性
 *
 * 该代码段展示了dynamic_cast、static_cast和reinterpret_cast在不同继承关系中的使用：
 * 1. 子类向基类转换（安全）
 * 2. 基类向子类转换（不安全）
 * 3. 无关类之间转换（危险）
 * 4. 转换回原始类型的尝试
 *
 * 通过条件判断和空指针检查体现了dynamic_cast的安全特性，
 * 同时展示了static_cast和reinterpret_cast的潜在风险。
 *
 * @note 包含内存释放操作，演示了完整的对象生命周期管理
 */
int main()
{

    /******* cast from child class to base class *******/
    cout << "dynamic_cast from child class to base class:" << endl;
	Children * daughter_d = new Children("Daughter who pretend to be my mother");
	Parents * mother_d = dynamic_cast<Parents*> (daughter_d); //right, cast with polymorphism
	mother_d->Speak();
	mother_d->Work();
	//mother_d->Study(); //Error, no such method
	
	cout << "static_cast from child class to base class:" << endl;
	Children * son_s = new Children("Son who pretend to be my father");
	Parents * father_s = static_cast<Parents*> (son_s); //right, cast with polymorphism
	father_s->Speak();	
	father_s->Work();
	//father_s->Study(); //Error, no such method
	
	cout << endl;
	
	/******* cast from base class to child class *******/	
	cout << "dynamic_cast from base class to child class:" << endl;
	Parents * father_d = new Parents("Father who pretend to be a my son");
	Children * son_d = dynamic_cast<Children*> (father_d); //no error, but not safe
	if (son_d)
	{
		son_d->Speak();
		son_d->Study();
	}
	else cout << "\t[null]" << endl;
	
	cout << "static_cast from base class to child class:" << endl;
	Parents * mother_s = new Parents("Mother who pretend to be a my daugher");
	Children * daughter_s = static_cast<Children*> (mother_s);  //no error, but not safe
	if (daughter_s)
	{
		daughter_s->Speak();
		daughter_s->Study();
	}
	else cout << "\t[null]" << endl;
	
	cout << endl;
	
	/******* cast between non-related class *******/	
	cout << "dynamic_cast to non-related class:" << endl;
	Stranger* stranger_d = dynamic_cast<Stranger*> (daughter_d);
	if (stranger_d)
	{
		stranger_d->Self_Introduce();
		stranger_d->Speak();	
	}
	else cout <<"\t[null]"<<endl;
	
	//Stranger* stranger_s = static_cast<Stranger*> (son_s);    //Error, invalid cast
	
	cout << "reinterpret_cast to non-related class:" << endl;
	Stranger* stranger_r = reinterpret_cast<Stranger*> (son_s);
	if (stranger_r)
	{
		stranger_d->Self_Introduce();
		//stranger_d->Speak();	//This line would cause program crush,
		//as "name" could not be found corretly.
	}
	else cout << "\t[null]" << endl;

	cout << endl;
	
	/******* cast back*******/
	cout << "use dynamic_cast to cast back from static_cast:" << endl;
	Children* child_s = dynamic_cast<Children*> (father_s);
	if (child_s)
	{
		child_s->Speak();
		child_s->Work();
	}
	else cout << "\t[null]" << endl;
	
    //cout<<typeid(stranger_r).name()<<endl;
    
	cout << "use dynamic_cast to cast back from reinterpret_cast:" << endl;
	Children* child_r = dynamic_cast<Children*> (stranger_r);
	if (child_r)
	{
		child_r->Speak();
		child_r->Work();
	}
	else cout << "\t[null]" << endl;
	
	delete daughter_d;
	delete son_s;
	delete father_d;
	delete mother_s;
	
	return 0;
}

/********************* Result *********************/

/*
标准输出：dynamic_cast from child class to base class:
3.
	I am Daughter who pretend to be my mother, I love my parents.
2.
	I am Daughter who pretend to be my mother, I need to work for my family.
static_cast from child class to base class:
3.
	I am Son who pretend to be my father, I love my parents.
2.
	I am Son who pretend to be my father, I need to work for my family.

dynamic_cast from base class to child class:
	[null]
static_cast from base class to child class:
1.
	I am Mother who pretend to be a my daugher, I love my children.
4.
	I am Mother who pretend to be a my daugher, I need to study for future.

dynamic_cast to non-related class:
	[null]
reinterpret_cast to non-related class:
5.
	I am a stranger

use dynamic_cast to cast back from static_cast:
3.
	I am Son who pretend to be my father, I love my parents.
2.
	I am Son who pretend to be my father, I need to work for my family.
use dynamic_cast to cast back from reinterpret_cast:
	[null]

*/
  