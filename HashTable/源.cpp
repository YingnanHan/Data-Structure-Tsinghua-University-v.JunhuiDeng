#pragma warning(disable:4996)

#include<iostream>
#include<memory>


using namespace std;


static size_t hashCode(char c)
{
	return (size_t)c;//字符
}

static size_t hashCode(int k)
{
	return (size_t)k;//整数以及长长整数
}


static size_t hashCode(long long i)
{
	return (size_t)((i >> 32) + (int)i);
}


static size_t hashCode(char s[])
{
	int h = 0;
	for (size_t n = strlen(s), i = 0; i < n; i++)
	{
		h = (h << 5) | (h >> 27);
		h += (int)s[i];
	}
	return (size_t)h;
}


template <typename K, typename V>
struct Entry
{
	//词条模板类
	K key;

	V value; //关键码、数值

	Entry(K k = K(), V v = V()) : key(k), value(v)
	{

	}; //默认构造函数
	Entry(Entry<K, V> const& e) : key(e.key), value(e.value)
	{

	}; //基于克隆的构造函数

	bool operator< (Entry<K, V> const& e)
	{
		return key < e.key;
	}  //比较器：小于

	bool operator> (Entry<K, V> const& e)
	{
		return key > e.key;
	}  //比较器：大于

	bool operator== (Entry<K, V> const& e)
	{
		return key == e.key;
	} //判等器：等于

	bool operator!= (Entry<K, V> const& e)
	{
		return key != e.key;

	} //判等器：不等于
}; //得益于比较器和判等器，从此往后，不必严格区分词条及其对应的关键码


template <typename K, typename V>
struct Dictionary
{
	//词典Dictionary模板类
	virtual int size() const = 0; //当前词条总数
	virtual bool put(K, V) = 0; //插入词条（禁止雷同词条时可能失败）
	virtual V* get(K k) = 0; //读取词条
	virtual bool remove(K k) = 0; //删除词条
};


//位图：Bitmap类
class Bitmap
{
private:
	char* M;//比特图所存放的空间M[]，容量为N*sizeof(char)*8比特
	int N;
protected:
	void init(int n)
	{
		N = (n + 7) / 8;
		M = new char[N];
		memset(M, 0, N);
	}
public:
	Bitmap(int n = 8)
	{
		init(n);//按指定或默认规模创建比特图
	}

	Bitmap(char* file, int n = 8)
	{
		//按指定规模或者默认规模，从文件之中读取比特图
		init(n);
		FILE* fp = fopen(file, "r");
		fread(M, sizeof(char), N, fp);
		fclose(fp);
	}

	~Bitmap()
	{
		delete[]M;
		M = NULL;//析构时释放比特图空间
	}

	void set(int k)
	{
		expand(k);
		M[k >> 3] |= (0x80 >> (k & 0x07));
	}

	void clear(int k)
	{
		expand(k);
		M[k >> 3] &= ~(0x80 >> (k & 0x07));
	}

	bool test(int k)
	{
		expand(k);
		return M[k >> 3] & ((0x80) >> (k & 0x07));
	}

	void dump(char* file)
	{
		//将位图整体导出至指定文件，以便此后对新的位图进行整体初始化
		FILE* fp = fopen(file, "w");
		fwrite(M, sizeof(char), N, fp);
		fclose(fp);
	}

	char* bits2string(int n)
	{
		//将前N位转换为字符串
		expand(n - 1);//此时可能被访问的最高位为bitmap[n-1]
		char* s = new char[n + 1];
		s[n] = '\0';
		for (int i = 0; i < n; i++)
		{
			s[i] = test(i) ? '1' : '0';
		}
		return s;//返回字符串
	}

	void expand(int k)//若被访问的Bitmap[k]已出界，则需扩容
	{
		if (k < 8 * N)
		{
			return;//若仍在界内，无需扩容
		}
		int oldN = N;
		char* oldM = M;
		init(2 * k);//与向量类似，加倍策略
		memcpy_s(M, N, oldM, oldN);
		//将元数据转移至新的的空间
		delete[] oldM;
	}
};


template<typename K, typename V>//key,value
class HashTable :public Dictionary<K, V>
{
	//符合Dictionary接口的HashTable集合
private:
	Entry<K, V>** ht;//桶数组，存放词条指针
	int M;//桶数组容量
	int N;//词条容量
	Bitmap* lazyRemoval;//懒惰删除标记

#define lazilyRemoved(x) (lazyRemoval->test(x))
#define markAsRemoved(x) (lazyRemoval->set(x))

protected:
	int probe4Hit(const K& k);//沿关键码K对应的值查找链，找到词条匹配的桶
	int probe4Free(const K& k);//沿关键码K对应的值查找链，找到首个可用空桶
	void rehash();//重散列算法，扩充桶数组，保证装填因子在警戒线以下
public:
	HashTable(int c = 5);//创建一个容量不小于c的散列表（为测试暂时选用较小的默认值）
	~HashTable();//释放桶数组及其中的各个(非空)元素所指向的词条
	int size()const//返回当前的词条数目
	{
		return N;
	}
	bool put(K, V);//插入，禁止雷同词条
	V* get(K k);//读取
	bool remove(K k);//删除
};



template <typename T> struct Cleaner {
	static void clean(T x) { //相当于递归基
#ifdef _DEBUG
		static int n = 0;
		if (7 > strlen(typeid (T).name())) { //复杂类型一概忽略，只输出基本类型
			printf("\t<%s>[%d]=", typeid (T).name(), ++n);
			print(x);
			printf(" purged\n");
		}
#endif
	}
};

template <typename T>
struct Cleaner<T*>
{
	static void clean(T* x)
	{
		if (x)
		{
			delete x;
		} //如果其中包含指针，递归释放
#ifdef _DEBUG
		static int n = 0;
#endif
	}
};


template <typename T>
void release(T x)
{
	Cleaner<T>::clean(x);
}



bool isPrime(int x)//素数是指一个大于一的自然数除了1和它自身外，不能被其他自然数整除的数
{
	if (x == 2)
	{
		return true;
	}
	for (int i = 2; i < x; i++)
	{
		if (x % i == 0)
		{
			return false;
		}
	}
	return true;
}

int primeNLT(int c, int n)
{
	int d = c;
	//在[c,n)之中取最小的素数
	while (d < n)
	{
		if (!isPrime(d))
		{
			d++;
		}
		else
		{
			return d;
		}
	}
}


template<typename K, typename V>
HashTable<K, V>::HashTable(int c)
{
	//创建散列表容量为不小于c的素数N
	M = primeNLT(c, 10000);
	N = 0;
	ht = new Entry<K, V> * [M];//开辟桶数组，初始装填因子N/M=0%
	memset(ht, 0, sizeof(Entry<K, V>*) * M);//初始化各个桶
	lazyRemoval = new Bitmap(M);//懒惰删除标记比特图
}


template<typename K, typename V>
HashTable<K, V>::~HashTable()
{
	//析构之前释放桶数组以及非空词条
	for (int i = 0; i < M; i++)
	{
		//逐一检查各个桶
		if (ht[i])
		{
			release(ht[i]);//释放非空的桶
		}
	}
	release(ht);//释放桶数组
	release(lazyRemoval);//释放懒惰删除标记
}


template<typename K, typename V>
V* HashTable<K, V>::get(K k)
{
	//散列词条查找算法
	int r = probe4Hit(k);
	return ht[r] ? &(ht[r]->value) : NULL;//禁止词条key值雷同
}


template<typename K, typename V>
int HashTable<K, V>::probe4Hit(const K& k)
{
	int r = hashCode(k) % M;
	while ((ht[r] && (k != ht[r]->key)) || (!ht[r] && lazilyRemoved(r)))
	{
		r = (r + 1) % M;//沿着查找链线性试探，跳过所有具有冲突的桶，以及带有懒惰删除标记的桶
	}
	return r;
}



template<typename K, typename V>
bool HashTable<K, V>::remove(K k)
{
	//散列表词条删除算法
	int r = probe4Hit(k);
	if (!ht[r])
	{
		return false;//对应词条不存在的时候，无法删除
	}
	release(ht[r]);
	ht[r] = NULL;
	markAsRemoved(r);
	N--;
	return true;//否则释放桶中的词条，设置懒惰删除标记，并更新词条总数
}


template<typename K, typename V>
bool HashTable<K, V>::put(K k, V v)
{
	//散列表词条插入
	if (ht[probe4Hit(k)])
	{
		return false;//雷同元素不必重复插入
	}
	int r = probe4Free(k);//为词条找到一个空桶
	ht[r] = new Entry<K, V>(k, v);
	N++;//插入，注意懒惰删除标记无需复位
	if (N * 2 > M)
	{
		rehash();//装填因子高于50%之后重散列
	}
	return true;
}



template<typename K, typename V>
int HashTable<K, V>::probe4Free(const K& k)
{
	int r = hashCode(k) % M;//从起始桶出发
	while (ht[r])//沿查找链猪桶试探，直到首个空桶(无论是否带有懒惰删除标记)
	{
		r = (r + 1) % M;//为保证空桶总能找到，装填因子以及散列表长度需要合理设置
	}
	return r;
}


template<typename K, typename V>
void HashTable<K, V>::rehash()
{
	int old_capacity = M;
	Entry<K, V>** old_ht = ht;
	M = primeNLT(2 * M, 1048576);
	N = 0;
	ht = new Entry<K, V> * [M];
	memset(ht, 0, sizeof(Entry<K, V>*) * M);
	release(lazyRemoval);
	lazyRemoval = new Bitmap(M);
	for (int i = 0; i < old_capacity; i++)
	{
		if (old_ht[i])
		{
			put(old_ht[i]->key, old_ht[i]->value);
		}
	}
	release(old_ht);
}



int main()
{
	HashTable<int, int >obj(100);
	obj.put(12, 12);
	if (obj.get(13))
	{
		cout << "founded！" << endl;
	}
	else
	{
		cout << "not founded！" << endl;
	}

	if (obj.get(12))
	{
		cout << "founded！" << endl;
	}
	else
	{
		cout << "not founded！" << endl;
	}
	obj.remove(12);


	if (obj.get(12))
	{
		cout << "founded！" << endl;
	}
	else
	{
		cout << "not founded！" << endl;
	}

	obj.put(100, 100);
	cout << obj.size() << endl;

	system("pause");
	return 0;
}
