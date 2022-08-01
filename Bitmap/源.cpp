#pragma warning(disable:4996)
#include<iostream>
#include<algorithm>
#include<memory>

using namespace std;

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


int main()
{
	int n = 1000;
	Bitmap bt(n);
	for (int i = 0; i < n; i++)
	{
		bt.set(i);
	}
	if (bt.test(20000))
	{
		cout << "founded!" << endl;
	}
	else
	{
		cout << "not founded!" << endl;
	}
	system("pause");
	return 0;
}
