#ifndef SINGLETON_H
#define SINGLETON_H


class Uncopyable {
protected:
	Uncopyable() {}
	~Uncopyable() {}
private:
	Uncopyable(const Uncopyable&);
	Uncopyable& operator=(const Uncopyable&);
};

template <class T>
class Singleton : private Uncopyable {
public:
	static T& get();
protected:
	Singleton() {}
};

#endif // !SINGLETON_H
