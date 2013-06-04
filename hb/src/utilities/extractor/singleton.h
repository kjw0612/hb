#ifndef SINGLETON_H
#define SINGLETON_H

template <class T>
class Singleton : private Uncopyable {
public:
	static T& get();
protected:
	Singleton() {}
};

#endif // !SINGLETON_H
