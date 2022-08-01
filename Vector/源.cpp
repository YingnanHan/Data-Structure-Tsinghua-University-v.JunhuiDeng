#include<iostream>
#include<algorithm>
#include<ctime>

using namespace std;

typedef int Rank; //秩

#define DEFAULT_CAPACITY  3 //默认的初始容量（实际应用中可设置为更大）

template <typename T>
class Vector
{
    //向量模板类
public:
    //规模、容量、数据区
    Rank _size;

    int _capacity;

    T* _elem;

    void copyFrom(T const* A, Rank lo, Rank hi); //复制数组区间A[lo, hi)

    void expand(); //空间不足时扩容

    void shrink(); //装填因子过小时压缩

    bool bubble(Rank lo, Rank hi); //扫描交换

    void bubbleSort(Rank lo, Rank hi); //起泡排序算法

    Rank max(Rank lo, Rank hi); //选取最大元素

    void selectionSort(Rank lo, Rank hi); //选择排序算法

    void merge(Rank lo, Rank mi, Rank hi); //归并算法

    void mergeSort(Rank lo, Rank hi); //归并排序算法

    void heapSort(Rank lo, Rank hi); //堆排序（稍后结合完全堆讲解）

    Rank partition(Rank lo, Rank hi); //轴点构造算法

    void quickSort(Rank lo, Rank hi); //快速排序算法

    void shellSort(Rank lo, Rank hi); //希尔排序算法

public:
    // 构造函数
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0) //容量为c、规模为s、所有元素初始为v
    {
        _elem = new T[_capacity = c];
        for (_size = 0; _size < s; _elem[_size++] = v);
    } //s<=c

    Vector(T const* A, Rank n)
    {
        copyFrom(A, 0, n);
    } //数组整体复制

    Vector(T const* A, Rank lo, Rank hi)
    {
        copyFrom(A, lo, hi);
    } //区间

    Vector(Vector<T> const& V)
    {
        copyFrom(V._elem, 0, V._size);
    } //向量整体复制

    Vector(Vector<T> const& V, Rank lo, Rank hi)
    {
        copyFrom(V._elem, lo, hi);
    } //区间

    // 析构函数
    ~Vector()
    {
        delete[] _elem;
    } //释放内部空间

    // 只读访问接口
    Rank size() const
    {
        return _size;
    } //规模

    bool empty() const
    {
        return !_size;
    } //判空

    Rank find(T const& e) const
    {
        return find(e, 0, _size);
    } //无序向量整体查找

    Rank find(T const& e, Rank lo, Rank hi) const; //无序向量区间查找

    Rank search(T const& e) const //有序向量整体查找
    {
        return (0 >= _size) ? -1 : search(e, 0, _size);
    }

    Rank search(T const& e, Rank lo, Rank hi) const; //有序向量区间查找

    // 可写访问接口
    T& operator[] (Rank r); //重载下标操作符，可以类似于数组形式引用各元素

    const T& operator[] (Rank r) const; //仅限于做右值的重载版本

    Vector<T>& operator= (Vector<T> const&); //重载赋值操作符，以便直接克隆向量

    T remove(Rank r); //删除秩为r的元素

    int remove(Rank lo, Rank hi); //删除秩在区间[lo, hi)之内的元素

    Rank insert(Rank r, T const& e); //插入元素

    Rank insert(T const& e)
    {
        return insert(_size, e);
    } //默认作为末元素插入

    void sort(Rank lo, Rank hi); //对[lo, hi)排序

    void sort()
    {
        sort(0, _size);
    } //整体排序

    void unsort(Rank lo, Rank hi); //对[lo, hi)置乱

    void unsort()
    {
        unsort(0, _size);
    } //整体置乱

    int deduplicate(); //无序去重

    int uniquify(); //有序去重

    // 遍历
    void traverse(void (*) (T&)); //遍历（使用函数指针，只读或局部性修改）

    template <typename VST>
    void traverse(VST&); //遍历（使用函数对象，可全局性修改）

}; //Vector


class Fib
{ //Fibonacci数列类
private:
    int f, g; //f = fib(k - 1), g = fib(k)。均为int型，很快就会数值溢出
public:
    Fib(int n) //初始化为不小于n的最小Fibonacci项
    {
        f = 1;
        g = 0;
        while (g < n)
        {
            next();
        }
    } //fib(-1), fib(0)，O(log_phi(n))时间

    int get()
    {
        return g;
    } //获取当前Fibonacci项，O(1)时间

    int next()
    {
        g += f;
        f = g - f;
        return g;
    } //转至下一Fibonacci项，O(1)时间

    int prev()
    {
        f = g - f;
        g -= f;
        return g;
    } //转至上一Fibonacci项，O(1)时间
};


template <typename T>
struct Increase //函数对象：递增一个T类对象
{
    virtual void operator() (T& e)
    {
        e++;
    }
}; //假设T可直接递增或已重载++


template <typename T>
void increase(Vector<T>& V) //统一递增向量中的各元素
{
    V.traverse(Increase<T>());
} //以Increase<T>()为基本操作进行遍历


template <typename T>
void permute(Vector<T>& V)
{ //随机置乱向量，使各元素等概率出现于各位置
    for (int i = V.size(); i > 0; i--) //自后向前
    {
        swap(V[i - 1], V[rand() % i]); //V[i - 1]与V[0, i)中某一随机元素交换
    }
}


template <typename T>
static bool lt(T* a, T* b)
{
    return lt(*a, *b);
} //less than


template <typename T>
static bool lt(T& a, T& b)
{
    return a < b;
} //less than


template <typename T>
static bool eq(T* a, T* b)
{
    return eq(*a, *b);
} //equal


template <typename T>
static bool eq(T& a, T& b)
{
    return a == b;
} //equal


template <typename T> //元素类型
void Vector<T>::copyFrom(T const* A, Rank lo, Rank hi)
{ //以数组区间A[lo, hi)为蓝本复制向量
    _elem = new T[_capacity = 2 * (hi - lo)];
    _size = 0; //分配空间，规模清零
    while (lo < hi) //A[lo, hi)内的元素逐一
    {
        _elem[_size++] = A[lo++]; //复制至_elem[0, hi - lo)
    }
}


template <typename T>
void Vector<T>::expand()
{ //向量空间不足时扩容
    if (_size < _capacity)
    {
        return; //尚未满员时，不必扩容
    }
    if (_capacity < DEFAULT_CAPACITY)
    {
        _capacity = DEFAULT_CAPACITY; //不低于最小容量
    }
    T* oldElem = _elem;
    _elem = new T[_capacity <<= 1]; //容量加倍
    for (int i = 0; i < _size; i++)
    {
        _elem[i] = oldElem[i]; //复制原向量内容（T为基本类型，或已重载赋值操作符'='）
    }
    /*DSA*/
    //printf("\n_ELEM [%x]*%d/%d expanded and shift to [%x]*%d/%d\n", oldElem, _size, _capacity/2, _elem, _size, _capacity);
    delete[] oldElem; //释放原空间
}


template <typename T>
void Vector<T>::shrink()
{ //装填因子过小时压缩向量所占空间
    if (_capacity < DEFAULT_CAPACITY << 1)
    {
        return; //不致收缩到DEFAULT_CAPACITY以下
    }
    if (_size << 2 > _capacity)
    {
        return; //以25%为界
    }
    T* oldElem = _elem;
    _elem = new T[_capacity >>= 1]; //容量减半
    for (int i = 0; i < _size; i++)
    {
        _elem[i] = oldElem[i]; //复制原向量内容
    }
    delete[] oldElem; //释放原空间
}


template <typename T> //无序向量的顺序查找：返回最后一个元素e的位置；失败时，返回lo - 1
Rank Vector<T>::find(T const& e, Rank lo, Rank hi) const
{ //assert: 0 <= lo < hi <= _size
    while ((lo < hi--) && (e != _elem[hi])); //从后向前，顺序查找
    return hi; //若hi < lo，则意味着失败；否则hi即命中元素的秩
}


template <typename T> //向量的起泡排序（跳跃版）
void Vector<T>::bubbleSort(Rank lo, Rank hi)
{ //assert: 0 <= lo < hi <= size
    for (Rank last = --hi; lo < hi; hi = last)
    {
        for (Rank i = last = lo; i < hi; i++)
        {
            if (_elem[i] > _elem[i + 1]) //若逆序，则
            {
                swap(_elem[last = i], _elem[i + 1]); //经交换使局部有序
            }
        }
    }
}


template <typename T>
Rank Vector<T>::max(Rank lo, Rank hi)
{ //在[lo, hi]内找出最大者
    Rank mx = hi;
    while (lo < hi--) //逆向扫描
    {
        if (_elem[hi] > _elem[mx]) //且严格比较
        {
            mx = hi; //故能在max有多个时保证后者优先，进而保证selectionSort稳定
        }
    }
    return mx;
}


template<typename T>
bool Vector<T>::bubble(Rank lo, Rank hi) //一趟扫描交换
{
    bool sorted = true;//整体有序标志
    while (++lo < hi)
    {
        //自左向右，逐一检查个对相邻元素
        if (_elem[lo - 1] > _elem[lo])
        {
            //若逆序，则意味着尚未整体有序，并需要通过交换使得局部有序
            sorted = false;
            swap(_elem[lo - 1], _elem[lo]);
        }
    }
    return sorted;//返回有序标志
}


template <typename T> //向量选择排序
void Vector<T>::selectionSort(Rank lo, Rank hi)
{ //0 <= lo < hi <= size
    while (lo < --hi)
    {
        swap(_elem[max(lo, hi)], _elem[hi]); //将[hi]与[lo, hi]中的最大者交换
    }
}


template <typename T> //有序向量（区间）的归并
void Vector<T>::merge(Rank lo, Rank mi, Rank hi)
{ //各自有序的子向量[lo, mi)和[mi, hi)
    T* A = _elem + lo; //合并后的向量A[0, hi - lo) = _elem[lo, hi)
    int lb = mi - lo;
    T* B = new T[lb]; //前子向量B[0, lb) = _elem[lo, mi)
    for (Rank i = 0; i < lb; i++)
    {
        B[i] = A[i]; //复制前子向量
    }
    int lc = hi - mi;
    T* C = _elem + mi; //后子向量C[0, lc) = _elem[mi, hi)
    for (Rank i = 0, j = 0, k = 0; j < lb; ) //归并：反复从B和C首元素中取出更小者
    {
        A[i++] = (lc <= k || B[j] <= C[k]) ? B[j++] : C[k++]; //将其归入A中
    }
    delete[] B; //释放临时空间B
}


template <typename T> //向量归并排序
void Vector<T>::mergeSort(Rank lo, Rank hi)
{ //0 <= lo < hi <= size
    /*DSA*/ //printf ( "\tMERGEsort [%3d, %3d)\n", lo , hi );
    if (hi - lo < 2)
    {
        return; //单元素区间自然有序，否则...
    }
    int mi = (lo + hi) / 2; //以中点为界
    mergeSort(lo, mi);
    mergeSort(mi, hi); //分别排序
    merge(lo, mi, hi); //归并
}



template <typename T> //轴点构造算法：通过调整元素位置构造区间[lo, hi)的轴点，并返回其秩
Rank Vector<T>::partition(Rank lo, Rank hi)
{ //版本A：基本形式
    swap(_elem[lo], _elem[lo + rand() % (hi - lo)]); //任选一个元素与首元素交换
    hi--;
    T pivot = _elem[lo]; //以首元素为候选轴点——经以上交换，等效于随机选取
    while (lo < hi)
    { //从向量的两端交替地向中间扫描
        while ((lo < hi) && (pivot <= _elem[hi])) //在不小于pivot的前提下
        {
            hi--; //向左拓展右端子向量
        }
        _elem[lo] = _elem[hi]; //小于pivot者归入左侧子序列
        while ((lo < hi) && (_elem[lo] <= pivot)) //在不大于pivot的前提下
        {
            lo++; //向右拓展左端子向量
        }
        _elem[hi] = _elem[lo]; //大于pivot者归入右侧子序列
    } //assert: lo == hi
    _elem[lo] = pivot; //将备份的轴点记录置于前、后子向量之间
    return lo; //返回轴点的秩
}


template <typename T> //向量快速排序
void Vector<T>::quickSort(Rank lo, Rank hi)
{ //0 <= lo < hi <= size
    /*DSA*/ //printf ( "\tQUICKsort [%3d, %3d)\n", lo, hi );
    if (hi - lo < 2)
    {
        return; //单元素区间自然有序，否则...
    }
    Rank mi = partition(lo, hi); //在[lo, hi)内构造轴点
    quickSort(lo, mi); //对前缀递归排序
    quickSort(mi + 1, hi); //对后缀递归排序
}


template <typename T> //向量希尔排序
void Vector<T>::shellSort(Rank lo, Rank hi)
{ //0 <= lo < hi <= size <= 2^30
    /*DSA*/ printf("\tSHELLsort [%3d, %3d)\n", lo, hi);
    for (int d = 0x3FFFFFFF; 0 < d; d >>= 1) //PS Sequence: { 1, 3, 7, 15, ..., 1073741823 }
    {
        for (int j = lo + d; j < hi; j++)
        { //for each j in [lo+d, hi)
            T x = _elem[j];
            int i = j - d;
            while (lo <= i && _elem[i] > x)
            {
                _elem[i + d] = _elem[i];
                i -= d;
            }
            _elem[i + d] = x; //insert [j] into its subsequence
        }
    }
}

// 二分查找算法（版本A）：在有序向量的区间[lo, hi)内查找元素e，0 <= lo <= hi <= _size
template <typename T>
static Rank binSearchA(T* S, T const& e, Rank lo, Rank hi)
{
    /*DSA*/
    //printf("BIN search (A)\n");
    while (lo < hi)
    { //每步迭代可能要做两次比较判断，有三个分支
        /*DSA*/
        for (int i = 0; i < lo; i++)
        {
            printf("");
        }
        if (lo >= 0)
        {
            for (int i = lo; i < hi; i++)
            {
                printf("");
            }
        }
        //printf("\n");
        Rank mi = (lo + hi) >> 1; //以中点为轴点（区间宽度的折半，等效于宽度之数值表示的右移）
        if (e < S[mi])
        {
            hi = mi; //深入前半段[lo, mi)继续查找
        }
        else
        {
            if (S[mi] < e)
            {
                lo = mi + 1; //深入后半段(mi, hi)继续查找
            }
            else
            {
                return mi; //在mi处命中
            }
        }
        if (lo >= hi)
        {
            for (int i = 0; i < mi; i++)
            {
                printf("");
            }
            if (mi >= 0)
            {
                printf("");
            }
            else
            {
                printf("");
            }
        }
    } //成功查找可以提前终止
    return -1; //查找失败

} //有多个命中元素时，不能保证返回秩最大者；查找失败时，简单地返回-1，而不能指示失败的位置


// 二分查找算法（版本B）：在有序向量的区间[lo, hi)内查找元素e，0 <= lo < hi <= _size
template <typename T>
static Rank binSearchB(T* S, T const& e, Rank lo, Rank hi)
{

    while (1 < hi - lo)
    { //每步迭代仅需做一次比较判断，有两个分支；成功查找不能提前终止
        Rank mi = (lo + hi) >> 1; //以中点为轴点（区间宽度的折半，等效于宽度之数值表示的右移）
        (e < S[mi]) ? hi = mi : lo = mi; //经比较后确定深入[lo, mi)或[mi, hi)
    } //出口时hi = lo + 1，查找区间仅含一个元素A[lo]
    return e < S[lo] ? lo - 1 : lo; //返回位置，总是不超过e的最大者
} //有多个命中元素时，不能保证返回秩最大者；查找失败时，简单地返回-1，而不能指示失败的位置



// 二分查找算法（版本C）：在有序向量的区间[lo, hi)内查找元素e，0 <= lo <= hi <= _size
template <typename T>
static Rank binSearchC(T* S, T const& e, Rank lo, Rank hi)
{
    while (lo < hi)
    { //每步迭代仅需做一次比较判断，有两个分支
        Rank mi = (lo + hi) >> 1; //以中点为轴点（区间宽度的折半，等效于宽度之数值表示的右移）
        (e < S[mi]) ? hi = mi : lo = mi + 1; //经比较后确定深入[lo, mi)或(mi, hi)
    } //成功查找不能提前终止
    return lo - 1; //循环结束时，lo为大于e的元素的最小秩，故lo - 1即不大于e的元素的最大秩
} //有多个命中元素时，总能保证返回秩最大者；查找失败时，能够返回失败的位置



template <typename T> //在有序向量的区间[lo, hi)内，确定不大于e的最后一个节点的秩
Rank Vector<T>::search(T const& e, Rank lo, Rank hi) const
{ //assert: 0 <= lo < hi <= _size
    return (rand() % 2) ? //按各50%的概率随机使用二分查找或Fibonacci查找
        binSearch(_elem, e, lo, hi) : fibSearch(_elem, e, lo, hi);
}


template <typename T>
T& Vector<T>::operator[] (Rank r) //重载下标操作符
{
    return _elem[r];
} // assert: 0 <= r < _size


template <typename T>
const T& Vector<T>::operator[] (Rank r) const //仅限于做右值
{
    return _elem[r];
} // assert: 0 <= r < _size


template <typename T>
T Vector<T>::remove(Rank r)
{ //删除向量中秩为r的元素，0 <= r < size
    T e = _elem[r]; //备份被删除元素
    remove(r, r + 1); //调用区间删除算法，等效于对区间[r, r + 1)的删除
    return e; //返回被删除元素
}


template <typename T>
int Vector<T>::remove(Rank lo, Rank hi)
{ //删除区间[lo, hi)
    if (lo == hi)
    {
        return 0; //出于效率考虑，单独处理退化情况，比如remove(0, 0)
    }
    while (hi < _size)
    {
        _elem[lo++] = _elem[hi++]; //[hi, _size)顺次前移hi - lo个单元
    }
    _size = lo; //更新规模，直接丢弃尾部[lo, _size = hi)区间
    shrink(); //若有必要，则缩容
    return hi - lo; //返回被删除元素的数目
}


template <typename T>
Vector<T>& Vector<T>::operator= (Vector<T> const& V)
{ //深复制
    if (_elem)
    {
        delete[] _elem; //释放原有内容
    }
    copyFrom(V._elem, 0, V.size()); //整体复制
    return *this; //返回当前对象的引用，以便链式赋值
}


template <typename T> //将e作为秩为r元素插入
Rank Vector<T>::insert(Rank r, T const& e)
{ //assert: 0 <= r <= size
    expand(); //若有必要，扩容
    for (int i = _size; i > r; i--)
    {
        _elem[i] = _elem[i - 1]; //自后向前，后继元素顺次后移一个单元
    }
    _elem[r] = e; _size++; //置入新元素并更新容量
    return r; //返回秩
}


template <typename T>
void Vector<T>::unsort(Rank lo, Rank hi)
{ //等概率随机置乱区间[lo, hi)
    T* V = _elem + lo; //将子向量_elem[lo, hi)视作另一向量V[0, hi - lo)
    for (Rank i = hi - lo; i > 0; i--) //自后向前
    {
        swap(V[i - 1], V[rand() % i]); //将V[i - 1]与V[0, i)中某一元素随机交换
    }
}


template <typename T>
void Vector<T>::sort(Rank lo, Rank hi)
{ //向量区间[lo, hi)排序
    switch (rand() % 6)
    {
    case 1:
    {
        bubbleSort(lo, hi);
        break; //起泡排序
    }
    case 2:
    {
        selectionSort(lo, hi);
        break; //选择排序（习题）
    }
    case 3:
    {
        mergeSort(lo, hi);
        break; //归并排序
    }
    case 4:
    {
        quickSort(lo, hi);
        break; //快速排序（第14章）
    }
    default:
    {
        shellSort(lo, hi);
        break; //希尔排序（第14章）
    }
    } //随机选择算法以充分测试。实用时可视具体问题的特点灵活确定或扩充
}


template <typename T>
int Vector<T>::deduplicate()
{ //删除无序向量中重复元素（高效版）
    int oldSize = _size; //记录原规模
    Rank i = 1; //从_elem[1]开始
    while (i < _size) //自前向后逐一考查各元素_elem[i]
    {
        if (find(_elem[i], 0, i) < 0) //在前缀[0,i)中寻找与之雷同者（至多一个）
            i++; //若无雷同则继续考查其后继
        else
            remove(i); //否则删除当前元素
    }
    return oldSize - _size; //被删除元素总数
}


template <typename T>
int Vector<T>::uniquify()
{ //有序向量重复元素剔除算法（高效版）
    Rank i = 0, j = 0; //各对互异“相邻”元素的秩
    while (++j < _size) //逐一扫描，直至末元素
    {
        if (_elem[i] != _elem[j]) //跳过雷同者
            _elem[++i] = _elem[j]; //发现不同元素时，向前移至紧邻于前者右侧
    }
    _size = ++i; shrink(); //直接截除尾部多余元素
    return j - i; //向量规模变化量，即被删除元素总数
}


template <typename T>
void Vector<T>::traverse(void (*visit) (T&)) //借助函数指针机制
{
    for (int i = 0; i < _size; i++)
    {
        visit(_elem[i]);
    }
    cout << endl;
} //遍历向量



//这里使用了函数指针机制
template<typename T>
void visit(T& elem)
{
    cout << elem << '\t';
}

template <typename T> template <typename VST> //元素类型、操作器
void Vector<T>::traverse(VST& visit) //借助函数对象机制
{
    for (int i = 1; i <= _size; i++)
    {
        visit(_elem[i - 1]);
        //控制每一行输出20个元素
        if ((i) % 10 == 0)
        {
            printf("\n");
        }
    }
    cout << endl;
} //遍历向量


template <typename T>
static Rank fibSearchA(T* S, T const& e, Rank lo, Rank hi)
{
    /*DSA*/
    //printf("FIB search (A)\n");
    //用O(log_phi(n = hi - lo)时间创建Fib数列
    for (Fib fib(hi - lo); lo < hi; )
    {  //Fib数列制表备查；此后每步迭代仅一次比较、两个分支
        /*DSA*/
        for (int i = 0; i < lo; i++)
        {
            // printf("     ");
        }
        if (lo >= 0)
        {
            for (int i = lo; i < hi; i++)
            {
                // printf("....^");
            }
        }
        else
        {
            // printf("<<<<|");
        }
        //printf("\n");
        while (hi - lo < fib.get())
        {
            fib.prev(); //自后向前顺序查找（分摊O(1)）
        }
        Rank mi = lo + fib.get() - 1; //确定形如Fib(k) - 1的轴点
        if (e < S[mi])
        {
            hi = mi; //深入前半段[lo, mi)继续查找
        }
        else
        {
            if (S[mi] < e)
            {
                lo = mi + 1; //深入后半段(mi, hi)继续查找
            }
            else
            {
                return mi; //在mi处命中
            }
        }
        /*DSA*/
        if (lo >= hi)
        {
            for (int i = 0; i < mi; i++)
            {
                // printf("     ");
            }
            if (mi >= 0)
            {
                //  printf("....|\n");
            }
            else
            {
                // printf("<<<<|\n");
            }
        }
    } //成功查找可以提前终止
    return -1; //查找失败
} //有多个命中元素时，不能保证返回秩最大者；失败时，简单地返回-1，而不能指示失败的位置



// Fibonacci查找算法（版本B）：在有序向量的区间[lo, hi)内查找元素e，0 <= lo <= hi <= _size
template <typename T>
static Rank fibSearchB(T* S, T const& e, Rank lo, Rank hi)
{
    //printf("FIB search (B)\n");
    for (Fib fib(hi - lo); lo < hi; )
    { //Fib数列制表备查 
        while (hi - lo < fib.get())
        {
            fib.prev(); //自后向前顺序查找（分摊O(1)）
        }
        Rank mi = lo + fib.get() - 1; //确定形如Fib(k) - 1的轴点
        (e < S[mi]) ? hi = mi : lo = mi + 1; //比较后确定深入前半段[lo, mi)或后半段(mi, hi)
    } //成功查找不能提前终止
    return --lo; //循环结束时，lo为大于e的元素的最小秩，故lo - 1即不大于e的元素的最大秩
} //有多个命中元素时，总能保证返回最秩最大者；查找失败时，能够返回失败的位置



int main()
{
    Vector<int>obj;
    //生成随机数*100，插入向量中
    srand((unsigned)time(NULL));
    for (int i = 0; i < 20; i++)
    {
        obj.insert(rand() % 1000);
    }

    cout << "------------------01.Initial vector:-----------------" << endl;
    obj.traverse(visit);
    cout << endl;

    cout << "------------------02.Insert a value:-----------------" << endl;
    obj.insert(1);
    obj.traverse(visit);
    cout << endl;

    cout << "------------------03.After sorted:-------------------" << endl;
    obj.sort();
    obj.traverse(visit);
    cout << endl;

    cout << "------------------04.After deduplicated:---------------" << endl;
    obj.deduplicate();
    obj.traverse(visit);
    cout << endl;

    cout << "------------------05.After shuffled:----------------" << endl;
    obj.unsort();
    obj.traverse(visit);
    cout << endl;

    cout << "------------------06.After uniquify:----------------" << endl;
    obj.insert(1, 2);
    obj.uniquify();
    obj.traverse(visit);
    cout << endl;

    cout << "------------------07.Search a element 100 one by one:----------------" << endl;
    int res = obj.find(100);
    cout << (res == -1 ? "not found!" : "founded") << endl;
    cout << endl;

    cout << "------------------08.Search a element 19 by fib search01:----------------" << endl;
    //define an array with 20 elements
    int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    res = fibSearchA(a, 19, 0, sizeof(a) / sizeof(int));
    cout << (res == -1 ? "not found!" : "founded") << endl;
    cout << endl;


    cout << "------------------09.Search a element 19 by fib search02:----------------" << endl;
    //define an array with 20 elements
    int b[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    res = fibSearchB(b, 19, 0, sizeof(b) / sizeof(int));
    cout << (res == -1 ? "not found!" : "founded") << endl;
    cout << endl;

    cout << "------------------10.Search a element 19 by binary search01:----------------" << endl;
    //define an array with 20 elements
    int c[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    res = binSearchA(b, 19, 0, sizeof(c) / sizeof(int));
    cout << (res == -1 ? "not found!" : "founded") << endl;
    cout << endl;


    cout << "------------------11.Search a element 19 by binary search02:----------------" << endl;
    //define an array with 20 elements
    int d[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    res = binSearchB(d, 19, 0, sizeof(d) / sizeof(int));
    cout << (res == -1 ? "not found!" : "founded") << endl;
    cout << endl;

    cout << "------------------12.Search a element 19 by binary search03:----------------" << endl;
    //define an array with 20 elements
    int e[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    res = binSearchC(e, 19, 0, sizeof(e) / sizeof(int));
    cout << (res == -1 ? "not found!" : "founded") << endl;
    cout << endl;


    system("pause");
    return 0;
}
