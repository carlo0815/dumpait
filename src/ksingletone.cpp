#include <stdio.h>
using namespace std;

template <typename T> class CSingleton {
private:
	static T* p_handle;
public:
	static T* instance() {
		return (p_handle) ? p_handle : (p_handle = new T);
	}
	static void release() {
		if(!p_handle) delete p_handle;
	}
};

#if 0
class CTest {
private:
	int m_v;
public:
	CTest():m_v(0){}
	~CTest(){}
	void dump() { 
		printf("this is the singletone test [%d]!!\n", m_v); 
	}
	void set(int v) {
		m_v = v;
	}
};
template<> CTest* CSingleton<CTest>::p_handle = 0;

int main(void) {
	CSingleton<CTest>::instance()->dump();
	CSingleton<CTest>::instance()->set(1);
	CSingleton<CTest>::instance()->dump();
	CSingleton<CTest>::instance()->set(2);
	CSingleton<CTest>::instance()->dump();
	return 0;
}
#endif

