#pragma once
#define SAFE_DELETE(p)  { if (p) { delete (p);     (p)=0; } }

template< class T >
class Singleton {
public:
	static T* GetInstance(void) {
		static T singleton;
		return &singleton;
	}
protected:
	Singleton();
	virtual ~Singleton();
private:
	Singleton(const Singleton& obj);
	Singleton& operator=(const Singleton& obj);
	static T* singleton;
};

template< class T >
T* Singleton< T >::singleton = 0;

template< class T >
Singleton< T >::Singleton() {

}

template< class T >
Singleton< T >::~Singleton() {

}