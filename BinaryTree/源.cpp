#include<memory>

#include"Stack.h"
#include"Queue.h"


using namespace std;


#define BinNodePosi(T) BinNode<T>* //节点位置
#define stature(p) ((p) ? (p)->height : -1) //节点高度（与“空树高度为-1”的约定相统一）
#define HeightUpdated(x)( (x).height == 1 + __max( stature( (x).lc ), stature( (x).rc ) ) ) /*高度更新常规条件*/  
#define Balanced(x) ( stature( (x).lc ) == stature( (x).rc ) ) //理想平衡条件
#define BalFac(x) ( stature( (x).lc ) - stature( (x).rc ) ) //平衡因子
#define AvlBalanced(x) ( ( -2 < BalFac(x) ) && ( BalFac(x) < 2 ) ) //AVL平衡条件
#define IsRoot(x) ( ! ( (x).parent ) )
#define IsLChild(x) ( ! IsRoot(x) && ( & (x) == (x).parent->lc ) )
#define IsRChild(x) ( ! IsRoot(x) && ( & (x) == (x).parent->rc ) )
#define HasParent(x) ( ! IsRoot(x) )
#define HasLChild(x) ( (x).lc )
#define HasRChild(x) ( (x).rc )
#define HasChild(x) ( HasLChild(x) || HasRChild(x) ) //至少拥有一个孩子
#define HasBothChild(x) ( HasLChild(x) && HasRChild(x) ) //同时拥有两个孩子
#define IsLeaf(x) ( ! HasChild(x) )
#define sibling(p) ( IsLChild( * (p) ) ? (p)->parent->rc : (p)->parent->lc )/*兄弟*/ 
#define uncle(x) ( IsLChild( * ( (x)->parent ) ) ? (x)->parent->parent->rc : (x)->parent->parent->lc )/*叔叔*/ 
#define FromParentTo(x)  ( IsRoot(x) ? BinTree<T>::_root : ( IsLChild(x) ? (x).parent->lc : (x).parent->rc ) )/*来自父亲的引用*/
#define IsBlack(p) ( ! (p) || ( RB_BLACK == (p)->color ) ) //外部节点也视作黑节点
#define IsRed(p) ( ! IsBlack(p) ) //非黑即红
#define BlackHeightUpdated(x) ( ( stature( (x).lc ) == stature( (x).rc ) ) && ( (x).height == ( IsRed(& x) ? stature( (x).lc ) : stature( (x).lc ) + 1 ) ) ) /*RedBlack高度更新条件*/


typedef enum
{
    RB_RED,
    RB_BLACK
} RBColor; //节点颜色


template <typename T>
struct Cleaner
{
    static void clean(T x)
    {   //相当于递归基
#ifdef _DEBUG
        static int n = 0;
        if (7 > strlen(typeid (T).name()))
        {
            //复杂类型一概忽略，只输出基本类型
            printf("\t<%s>[%d]=", typeid (T).name(), ++n);
            print(x);
            printf(" purged\n");
        }
#endif
    }
};


static int dice(int range)
{
    return rand() % range;
} //取[0, range)中的随机整数


static int dice(int lo, int hi)
{
    return lo + rand() % (hi - lo);
} //取[lo, hi)中的随机整数


static float dice(float range)
{
    return rand() % (1000 * (int)range) / (float)1000.;
}


static double dice(double range)
{
    return rand() % (1000 * (int)range) / (double)1000.;
}


static char dice()
{
    return (char)(32 + rand() % 96);
}


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
        printf("\t<%s>[%d] released\n", typeid (T*).name(), ++n);
#endif
    }
};


template <typename T>
void release(T x)
{
    Cleaner<T>::clean(x);
}


template <typename T>
struct BinNode
{
    //二叉树节点模板类
    // 成员（为简化描述起见统一开放，读者可根据需要进一步封装）

    T data; //数值
    BinNodePosi(T) parent;
    BinNodePosi(T) lc;
    BinNodePosi(T) rc; //父节点及左、右孩子
    int height; //高度（通用）
    int npl; //Null Path Length（左式堆，也可直接用height代替）
    RBColor color; //颜色（红黑树）

 // 构造函数

    BinNode() :
        parent(NULL), lc(NULL), rc(NULL), height(0), npl(1), color(RB_RED) { }

    BinNode(T e, BinNodePosi(T) p = NULL, BinNodePosi(T) lc = NULL, BinNodePosi(T) rc = NULL,
        int h = 0, int l = 1, RBColor c = RB_RED) :
        data(e), parent(p), lc(lc), rc(rc), height(h), npl(l), color(c) { }

    // 操作接口

    int size(); //统计当前节点后代总数，亦即以其为根的子树的规模

    BinNodePosi(T) insertAsLC(T const&); //作为当前节点的左孩子插入新节点

    BinNodePosi(T) insertAsRC(T const&); //作为当前节点的右孩子插入新节点

    BinNodePosi(T) succ(); //取当前节点的直接后继

    template <typename VST>
    void travLevel(VST&); //子树层次遍历

    template <typename VST>
    void travPre(VST&); //子树先序遍历

    template <typename VST>
    void travIn(VST&); //子树中序遍历

    template <typename VST>
    void travPost(VST&); //子树后序遍历

 // 比较器、判等器（各列其一，其余自行补充）
    bool operator< (BinNode const& bn)
    {
        return data < bn.data;
    } //小于

    bool operator== (BinNode const& bn)
    {
        return data == bn.data;
    } //等于

};


template <typename T>
BinNodePosi(T) BinNode<T>::insertAsLC(T const& e)
{
    return lc = new BinNode(e, this);
} //将e作为当前节点的左孩子插入二叉树


template <typename T>
BinNodePosi(T) BinNode<T>::insertAsRC(T const& e)
{
    return rc = new BinNode(e, this);
} //将e作为当前节点的右孩子插入二叉树


template <typename T>
int BinNode<T>::size()
{ //统计当前节点后代总数，即以其为根的子树规模
    int s = 1; //计入本身
    if (lc)
    {
        s += lc->size(); //递归计入左子树规模
    }
    if (rc)
    {
        s += rc->size(); //递归计入右子树规模
    }
    return s;
}


template <typename T>
BinNodePosi(T) BinNode<T>::succ()
{ //定位节点v的直接后继
    BinNodePosi(T) s = this; //记录后继的临时变量
    if (rc)
    { //若有右孩子，则直接后继必在右子树中，具体地就是
        s = rc; //右子树中
        while (HasLChild(*s))
        {
            s = s->lc; //最靠左（最小）的节点
        }
    }
    else
    { //否则，直接后继应是“将当前节点包含于其左子树中的最低祖先”，具体地就是
        while (IsRChild(*s))
        {
            s = s->parent; //逆向地沿右向分支，不断朝左上方移动
        }
        s = s->parent; //最后再朝右上方移动一步，即抵达直接后继（如果存在）
    }
    return s;
}


template <typename T> template <typename VST> //元素类型、操作器
void BinNode<T>::travIn(VST& visit)
{ //二叉树中序遍历算法统一入口
    switch (rand() % 5)
    { //此处暂随机选择以做测试，共五种选择
    case 1:
    {
        travIn_I1(this, visit);
        break; //迭代版#1
    }
    case 2:
    {
        travIn_I2(this, visit);
        break; //迭代版#2
    }
    case 3:
    {
        travIn_I3(this, visit);
        break; //迭代版#3
    }
    case 4:
    {
        travIn_I4(this, visit);
        break; //迭代版#4
    }
    default:
    {
        travIn_R(this, visit);
        break; //递归版
    }
    }
}


template <typename T> //从当前节点出发，沿左分支不断深入，直至没有左分支的节点
static void goAlongVine(BinNodePosi(T) x, Stack<BinNodePosi(T)>& S)
{
    while (x)
    {
        S.push(x);
        x = x->lc;
    } //当前节点入栈后随即向左侧分支深入，迭代直到无左孩子
}


template <typename T, typename VST> //元素类型、操作器
void travIn_I1(BinNodePosi(T) x, VST& visit)
{ //二叉树中序遍历算法（迭代版#1）
    Stack<BinNodePosi(T)> S; //辅助栈
    while (true)
    {
        goAlongVine(x, S); //从当前节点出发，逐批入栈
        if (S.empty())
        {
            break; //直至所有节点处理完毕
        }
        x = S.pop();
        visit(x->data); //弹出栈顶节点并访问之
        x = x->rc; //转向右子树
    }
}


template <typename T, typename VST> //元素类型、操作器
void travIn_I2(BinNodePosi(T) x, VST& visit)
{ //二叉树中序遍历算法（迭代版#2）
    Stack<BinNodePosi(T)> S; //辅助栈
    while (true)
    {
        if (x)
        {
            S.push(x); //根节点进栈
            x = x->lc; //深入遍历左子树
        }
        else if (!S.empty())
        {
            x = S.pop(); //尚未访问的最低祖先节点退栈
            visit(x->data); //访问该祖先节点
            x = x->rc; //遍历祖先的右子树
        }
        else
        {
            break; //遍历完成
        }
    }
}


template <typename T, typename VST> //元素类型、操作器
void travIn_I3(BinNodePosi(T) x, VST& visit)
{ //二叉树中序遍历算法（迭代版#3，无需辅助栈）
    bool backtrack = false; //前一步是否刚从左子树回溯——省去栈，仅O(1)辅助空间
    while (true)
    {
        if (!backtrack && HasLChild(*x)) //若有左子树且不是刚刚回溯，则
        {
            x = x->lc; //深入遍历左子树
        }
        else
        { //否则——无左子树或刚刚回溯（相当于无左子树）
            visit(x->data); //访问该节点
            if (HasRChild(*x))
            { //若其右子树非空，则
                x = x->rc; //深入右子树继续遍历
                backtrack = false; //并关闭回溯标志
            }
            else
            { //若右子树空，则
                if (!(x = x->succ()))
                {
                    break; //回溯（含抵达末节点时的退出返回）
                }
                backtrack = true; //并设置回溯标志
            }
        }
    }
}


template <typename T, typename VST> //元素类型、操作器
void travIn_I4(BinNodePosi(T) x, VST& visit)
{ //二叉树中序遍历（迭代版#4，无需栈或标志位）
    while (true)
    {
        if (HasLChild(*x)) //若有左子树，则
        {
            x = x->lc; //深入遍历左子树
        }
        else
        { //否则
            visit(x->data); //访问当前节点，并
            while (!HasRChild(*x)) //不断地在无右分支处
            {
                if (!(x = x->succ()))
                {
                    return; //回溯至直接后继（在没有后继的末节点处，直接退出）
                }
                else
                {
                    visit(x->data); //访问新的当前节点
                }
            }
            x = x->rc; //（直至有右分支处）转向非空的右子树
        }
    }
}


template <typename T, typename VST> //元素类型、操作器
void travIn_R(BinNodePosi(T) x, VST& visit)
{ //二叉树中序遍历算法（递归版）
    if (!x)
    {
        return;
    }
    travIn_R(x->lc, visit);
    visit(x->data);
    travIn_R(x->rc, visit);
}


template <typename T> template <typename VST> //元素类型、操作器
void BinNode<T>::travLevel(VST& visit)
{ //二叉树层次遍历算法
    Queue<BinNodePosi(T)> Q; //辅助队列
    Q.enqueue(this); //根节点入队
    while (!Q.empty())
    { //在队列再次变空之前，反复迭代
        BinNodePosi(T) x = Q.dequeue();
        visit(x->data); //取出队首节点并访问之
        if (HasLChild(*x))
        {
            Q.enqueue(x->lc); //左孩子入队
        }
        if (HasRChild(*x))
        {
            Q.enqueue(x->rc); //右孩子入队
        }
    }
}


template <typename T> template <typename VST> //元素类型、操作器
void BinNode<T>::travPost(VST& visit)
{ //二叉树后序遍历算法统一入口
    switch (rand() % 2)
    { //此处暂随机选择以做测试，共两种选择
    case 1:
    {
        travPost_I(this, visit);
        break; //迭代版
    }
    default:
    {
        travPost_R(this, visit);
        break; //递归版
    }
    }
}



template <typename T> //在以S栈顶节点为根的子树中，找到最高左侧可见叶节点
static void gotoLeftmostLeaf(Stack<BinNodePosi(T)>& S)
{ //沿途所遇节点依次入栈
    while (BinNodePosi(T) x = S.top()) //自顶而下，反复检查当前节点（即栈顶）
    {
        if (HasLChild(*x))
        { //尽可能向左
            if (HasRChild(*x))
            {
                S.push(x->rc); //若有右孩子，优先入栈
            }
            S.push(x->lc); //然后才转至左孩子
        }
        else //实不得已
        {
            S.push(x->rc); //才向右
        }
    }
    S.pop(); //返回之前，弹出栈顶的空节点
}

template <typename T, typename VST>
void travPost_I(BinNodePosi(T) x, VST& visit)
{
    //二叉树的后序遍历（迭代版）
    Stack<BinNodePosi(T)> S; //辅助栈
    if (x)
    {
        S.push(x); //根节点入栈
    }
    while (!S.empty())
    { //x始终为当前节点
        if (S.top() != x->parent) //若栈顶非x之父（而为右兄）
        {
            gotoLeftmostLeaf(S); //则在其右兄子树中找到HLVFL（相当于递归深入）
        }
        x = S.pop();
        visit(x->data); //弹出栈顶（即前一节点之后继），并访问之
    }
}


template <typename T, typename VST> //元素类型、操作器
void travPost_R(BinNodePosi(T) x, VST& visit)
{ //二叉树后序遍历算法（递归版）
    if (!x)
    {
        return;
    }
    travPost_R(x->lc, visit);
    travPost_R(x->rc, visit);
    visit(x->data);
}


template <typename T> template <typename VST> //元素类型、操作器
void BinNode<T>::travPre(VST& visit)
{ //二叉树先序遍历算法统一入口
    switch (rand() % 3)
    { //此处暂随机选择以做测试，共三种选择
    case 1:
    {
        travPre_I1(this, visit);
        break; //迭代版#1
    }
    case 2:
    {
        travPre_I2(this, visit);
        break; //迭代版#2
    }
    default:
    {
        travPre_R(this, visit);
        break; //递归版
    }
    }
}


template <typename T, typename VST> //元素类型、操作器
void travPre_I1(BinNodePosi(T) x, VST& visit)
{ //二叉树先序遍历算法（迭代版#1）
    Stack<BinNodePosi(T)> S; //辅助栈
    if (x)
    {
        S.push(x); //根节点入栈
    }
    while (!S.empty())
    { //在栈变空之前反复循环
        x = S.pop();
        visit(x->data); //弹出并访问当前节点，其非空孩子的入栈次序为先右后左
        if (HasRChild(*x))
        {
            S.push(x->rc);
        }
        if (HasLChild(*x))
        {
            S.push(x->lc);
        }
    }
}


//从当前节点出发，沿左分支不断深入，直至没有左分支的节点；沿途节点遇到后立即访问
template <typename T, typename VST> //元素类型、操作器
static void visitAlongVine(BinNodePosi(T) x, VST& visit, Stack<BinNodePosi(T)>& S)
{
    while (x)
    {
        visit(x->data); //访问当前节点
        S.push(x->rc); //右孩子入栈暂存（可优化：通过判断，避免空的右孩子入栈）
        x = x->lc;  //沿左分支深入一层
    }
}


template <typename T, typename VST> //元素类型、操作器
void travPre_I2(BinNodePosi(T) x, VST& visit)
{ //二叉树先序遍历算法（迭代版#2）
    Stack<BinNodePosi(T)> S; //辅助栈
    while (true)
    {
        visitAlongVine(x, visit, S); //从当前节点出发，逐批访问
        if (S.empty())
        {
            break; //直到栈空
        }
        x = S.pop(); //弹出下一批的起点
    }
}


template <typename T, typename VST> //元素类型、操作器
void travPre_R(BinNodePosi(T) x, VST& visit)
{ //二叉树先序遍历算法（递归版）
    if (!x)
    {
        return;
    }
    visit(x->data);
    travPre_R(x->lc, visit);
    travPre_R(x->rc, visit);
}


template <typename T> class BinTree
{
    //二叉树模板类
protected:
    int _size;

    BinNodePosi(T) _root; //规模、根节点

    virtual int updateHeight(BinNodePosi(T) x); //更新节点x的高度

    void updateHeightAbove(BinNodePosi(T) x); //更新节点x及其祖先的高度

public:

    BinTree() : _size(0), _root(NULL)
    {

    } //构造函数

    ~BinTree()
    {
        if (0 < _size)
        {
            remove(_root);
        }
    } //析构函数

    int size() const
    {
        return _size;
    } //规模

    bool empty() const
    {
        return !_root;
    } //判空

    BinNodePosi(T) root() const
    {
        return _root;
    } //树根

    BinNodePosi(T) insertAsRoot(T const& e); //插入根节点

    BinNodePosi(T) insertAsLC(BinNodePosi(T) x, T const& e); //e作为x的左孩子（原无）插入

    BinNodePosi(T) insertAsRC(BinNodePosi(T) x, T const& e); //e作为x的右孩子（原无）插入

    BinNodePosi(T) attachAsLC(BinNodePosi(T) x, BinTree<T>*& T); //T作为x左子树接入

    BinNodePosi(T) attachAsRC(BinNodePosi(T) x, BinTree<T>*& T); //T作为x右子树接入

    int remove(BinNodePosi(T) x); //删除以位置x处节点为根的子树，返回该子树原先的规模

    BinTree<T>* secede(BinNodePosi(T) x); //将子树x从当前树中摘除，并将其转换为一棵独立子树

    template <typename VST> //操作器
    void travLevel(VST& visit)
    {
        if (_root)
        {
            _root->travLevel(visit);
        }
    } //层次遍历

    template <typename VST> //操作器
    void travPre(VST& visit)
    {
        if (_root)
        {
            _root->travPre(visit);
        }
    } //先序遍历

    template <typename VST> //操作器
    void travIn(VST& visit)
    {
        if (_root)
        {
            _root->travIn(visit);
        }
    } //中序遍历

    template <typename VST> //操作器
    void travPost(VST& visit)
    {
        if (_root)
        {
            _root->travPost(visit);
        }
    } //后序遍历

    bool operator< (BinTree<T> const& t) //比较器（其余自行补充）
    {
        return _root && t._root && lt(_root, t._root);
    }

    bool operator== (BinTree<T> const& t) //判等器
    {
        return _root && t._root && (_root == t._root);
    }

}; //BinTree


template <typename T> //二叉树子树接入算法：将S当作节点x的左子树接入，S本身置空
BinNodePosi(T) BinTree<T>::attachAsLC(BinNodePosi(T) x, BinTree<T>*& S)
{ //x->lc == NULL
    if (x->lc = S->_root)
    {
        x->lc->parent = x; //接入
    }
    _size += S->_size;
    updateHeightAbove(x); //更新全树规模与x所有祖先的高度
    S->_root = NULL;
    S->_size = 0;
    release(S);
    S = NULL;
    return x; //释放原树，返回接入位置
}


template <typename T> //二叉树子树接入算法：将S当作节点x的右子树接入，S本身置空
BinNodePosi(T) BinTree<T>::attachAsRC(BinNodePosi(T) x, BinTree<T>*& S)
{
    //x->rc == NULL
    if (x->rc = S->_root)
    {
        x->rc->parent = x; //接入
    }
    _size += S->_size;
    updateHeightAbove(x); //更新全树规模与x所有祖先的高度
    S->_root = NULL;
    S->_size = 0;
    release(S);
    S = NULL;
    return x; //释放原树，返回接入位置
}


template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRoot(T const& e)
{
    _size = 1; return _root = new BinNode<T>(e);
} //将e当作根节点插入空的二叉树


template <typename T>
BinNodePosi(T) BinTree<T>::insertAsLC(BinNodePosi(T) x, T const& e)
{
    _size++;
    x->insertAsLC(e);
    updateHeightAbove(x);
    return x->lc;
} //e插入为x的左孩子


template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRC(BinNodePosi(T) x, T const& e)
{
    _size++;
    x->insertAsRC(e);
    updateHeightAbove(x);
    return x->rc;
} //e插入为x的右孩子


template <typename T> //删除二叉树中位置x处的节点及其后代，返回被删除节点的数值
int BinTree<T>::remove(BinNodePosi(T) x)
{
    //assert: x为二叉树中的合法位置
    FromParentTo(*x) = NULL; //切断来自父节点的指针
    updateHeightAbove(x->parent); //更新祖先高度
    int n = removeAt(x);
    _size -= n;
    return n; //删除子树x，更新规模，返回删除节点总数
}


template <typename T> //删除二叉树中位置x处的节点及其后代，返回被删除节点的数值
static int removeAt(BinNodePosi(T) x)
{ //assert: x为二叉树中的合法位置
    if (!x)
    {
        return 0; //递归基：空树
    }
    int n = 1 + removeAt(x->lc) + removeAt(x->rc); //递归释放左、右子树
    release(x->data);
    release(x);
    return n; //释放被摘除节点，并返回删除节点总数
} //release()负责释放复杂结构，与算法无直接关系


template <typename T> //二叉树子树分离算法：将子树x从当前树中摘除，将其封装为一棵独立子树返回
BinTree<T>* BinTree<T>::secede(BinNodePosi(T) x)
{
    //assert: x为二叉树中的合法位置
    FromParentTo(*x) = NULL; //切断来自父节点的指针
    updateHeightAbove(x->parent); //更新原树中所有祖先的高度
    BinTree<T>* S = new BinTree<T>;
    S->_root = x;
    x->parent = NULL; //新树以x为根
    S->_size = x->size();
    _size -= S->_size;
    return S; //更新规模，返回分离出来的子树
}


template <typename T>
int BinTree<T>::updateHeight(BinNodePosi(T) x) //更新节点x高度
{
    return x->height = 1 + __max(stature(x->lc), stature(x->rc));
} //具体规则，因树而异


template <typename T> void BinTree<T>::updateHeightAbove(BinNodePosi(T) x) //更新高度
{
    while (x)
    {
        updateHeight(x);
        x = x->parent;
    }
} //从x出发，覆盖历代祖先。可优化


template <typename T>
struct Double //函数对象：倍增一个T类对象
{
    virtual void operator() (T& e)
    {
        e *= 2;
    }
}; //假设T可直接倍增


template <typename T>
struct Hailstone
{ //函数对象：按照Hailstone规则转化一个T类对象
    virtual void operator() (T& e)
    { //假设T可直接做算术运算
        int step = 0; //转换所需步数
        while (1 != e)
        { //按奇、偶逐步转换，直至为1
            (e % 2) ? e = 3 * e + 1 : e /= 2;
            step++;
        }
        e = step; //返回转换所经步数
    }
};






template<typename T, typename VST>
static void visitAlongLeftBranch(BinNodePosi(T) x, VST& visit, Stack<BinNodePosi(T)>& S)
{
    while (x)
    {
        visit(x);
        S.push(x->rc);
        x = x->lc;
    }
}


int testID = 0; //测试编号


// 随机生成期望高度为h的二叉树
template <typename T> bool randomBinTree(BinTree<T>& bt, BinNodePosi(T) x, int h)
{
    if (0 >= h)
    {
        return false; //至多h层
    }
    if (0 < dice(h)) //以1/h的概率终止当前分支的生长
    {
        randomBinTree(bt, bt.insertAsLC(x, dice((T)h * h * h)), h - 1);
    }
    if (0 < dice(h)) //以1/h的概率终止当前分支的生长
    {
        randomBinTree(bt, bt.insertAsRC(x, dice((T)h * h * h)), h - 1);
    }
    return true;
}


// 在二叉树中随机确定一个节点位置
template <typename T> BinNodePosi(T) randomPosiInBinTree(BinNodePosi(T) root)
{
    if (!HasChild(*root))
    {
        return root;
    }
    if (!HasLChild(*root))
    {
        return dice(6) ? randomPosiInBinTree(root->rc) : root;
    }
    if (!HasRChild(*root))
    {
        return dice(6) ? randomPosiInBinTree(root->lc) : root;
    }
    return dice(2) ? randomPosiInBinTree(root->lc) : randomPosiInBinTree(root->rc);
}






//一个简单的测试用例
int main()
{
    BinTree<int>bt;
    //将根结点设置为1
    BinNode<int>* root = bt.insertAsRoot(1);
    BinNode<int>* LeOfRoot = bt.insertAsLC(root, 2);
    BinNode<int>* RiOfRoot = bt.insertAsRC(root, 3);

    //设置第二层节点  假设为 4 5 6 7
    BinNode<int>* four, * five, * six, * seven;
    four = bt.insertAsLC(LeOfRoot, 4);
    five = bt.insertAsRC(LeOfRoot, 5);
    six = bt.insertAsLC(RiOfRoot, 6);
    seven = bt.insertAsRC(RiOfRoot, 7);

    //遍历二叉树

    //先序遍历
    bt.travPre(visit<int>);
    cout << endl;

    //中序遍历
    bt.travIn(visit<int>);
    cout << endl;

    //后序遍历
    bt.travPost(visit<int>);
    cout << endl;

    //层序遍历
    bt.travLevel(visit<int>);
    cout << endl;

    //二叉树删除节点
    bt.remove(seven);
    //再次遍历二叉树测试删除效果
    bt.travLevel(visit<int>);
    cout << endl;

    //将节点为3的子树摘除，然后遍历原来的二叉树以及新二叉树
    BinTree<int>* newTree = bt.secede(RiOfRoot);
    bt.travLevel(visit<int>);
    cout << endl;
    newTree->travLevel(visit<int>);
    cout << endl;


    //返回树的规模
    cout << "size:" << bt.size() << endl;


    system("pause");
    return 0;

}
