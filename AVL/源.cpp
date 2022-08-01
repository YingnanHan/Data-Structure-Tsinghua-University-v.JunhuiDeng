#include"BST.h"


using namespace std;


#define tallerChild(x) ( \
   stature( (x)->lc ) > stature( (x)->rc ) ? (x)->lc : ( /*左高*/ \
   stature( (x)->lc ) < stature( (x)->rc ) ? (x)->rc : ( /*右高*/ \
   IsLChild( * (x) ) ? (x)->lc : (x)->rc /*等高：与父亲x同侧者（zIg-zIg或zAg-zAg）优先*/ \
   ) \
   ) \
)


template <typename T>
class AVL : public BST<T>
{
    //由BST派生AVL树模板类
public:
    BinNodePosi(T) connect34(
        BinNodePosi(T) a, BinNodePosi(T) b, BinNodePosi(T) c,
        BinNodePosi(T) T0, BinNodePosi(T) T1, BinNodePosi(T) T2, BinNodePosi(T) T3
    );

    BinNodePosi(T) rotateAt(BinNodePosi(T)x);//对x及其父亲祖父做统一的旋转调整

    BinNodePosi(T) insert(const T& e); //插入（重写）

    bool remove(const T& e); //删除（重写）
};


template <typename T>
BinNodePosi(T) AVL<T>::rotateAt(BinNodePosi(T) v)
{ //v为非空孙辈节点
    BinNodePosi(T) p = v->parent;
    BinNodePosi(T) g = p->parent; //视v、p和g相对位置分四种情况
    if (IsLChild(*p)) /* zig */
        if (IsLChild(*v))
        { /* zig-zig */
            p->parent = g->parent; //向上联接
            return connect34(v, p, g, v->lc, v->rc, p->rc, g->rc);
        }
        else
        { /* zig-zag */
            v->parent = g->parent; //向上联接
            return connect34(p, v, g, p->lc, v->lc, v->rc, g->rc);
        }
    else  /* zag */
    {
        if (IsRChild(*v))
        { /* zag-zag */
            p->parent = g->parent; //向上联接
            return connect34(g, p, v, g->lc, p->lc, v->lc, v->rc);
        }
        else
        { /* zag-zig */
            v->parent = g->parent; //向上联接
            return connect34(g, v, p, g->lc, v->lc, v->rc, p->rc);
        }
    }
}


template <typename T>
BinNodePosi(T) AVL<T>::insert(const T& e)
{ //将关键码e插入AVL树中
    BinNodePosi(T)& x = BST<T>::search(e);
    if (x)
    {
        return x; //确认目标节点不存在
    }
    BinNodePosi(T) xx = x = new BinNode<T>(e, BST<T>::_hot);
    BinTree<T>::_size++; //创建新节点x
    // 此时，x的父亲_hot若增高，则其祖父有可能失衡
    for (BinNodePosi(T) g = BST<T>::_hot; g; g = g->parent)
    {
        //从x之父出发向上，逐层检查各代祖先g
        if (!AvlBalanced(*g))
        { //一旦发现g失衡，则（采用“3 + 4”算法）使之复衡，并将子树
            FromParentTo(*g) = AVL<T>::rotateAt(tallerChild(tallerChild(g))); //重新接入原树
            break; //g复衡后，局部子树高度必然复原；其祖先亦必如此，故调整随即结束
        }
        else //否则（g依然平衡），只需简单地
        {
            BST<T>::updateHeight(g); //更新其高度（注意：即便g未失衡，高度亦可能增加）
        }
    } //至多只需一次调整；若果真做过调整，则全树高度必然复原
    return xx; //返回新节点位置
} //无论e是否存在于原树中，总有AVL::insert(e)->data == e


template <typename T>
bool AVL<T>::remove(const T& e)
{ //从AVL树中删除关键码e
    BinNodePosi(T)& x = BST<T>::search(e);
    if (!x)
    {
        return false; //确认目标存在（留意_hot的设置）
    }
    removeAt(x, BST<T>::_hot);
    BinTree<T>::_size--; //先按BST规则删除之（此后，原节点之父_hot及其祖先均可能失衡）
    for (BinNodePosi(T) g = BST<T>::_hot; g; g = g->parent)
    { //从_hot出发向上，逐层检查各代祖先g
        if (!AvlBalanced(*g)) //一旦发现g失衡，则（采用“3 + 4”算法）使之复衡，并将该子树联至
        {
            g = FromParentTo(*g) = AVL<T>::rotateAt(tallerChild(tallerChild(g))); //原父亲
        }
        BST<T>::updateHeight(g); //并更新其高度（注意：即便g未失衡，高度亦可能降低）
    } //可能需做Ω(logn)次调整——无论是否做过调整，全树高度均可能降低
    return true; //删除成功
} //若目标节点存在且被删除，返回true；否则返回false


template <typename T>
BinNodePosi(T) AVL<T>::connect34(
    BinNodePosi(T) a, BinNodePosi(T) b, BinNodePosi(T) c,
    BinNodePosi(T) T0, BinNodePosi(T) T1, BinNodePosi(T) T2, BinNodePosi(T) T3
)
{
    a->lc = T0;
    if (T0)
    {
        T0->parent = a;
    }
    a->rc = T1;
    if (T1)
    {
        T1->parent = a;
    }
    BST<T>::updateHeight(a);
    c->lc = T2;
    if (T2)
    {
        T2->parent = c;
    }
    c->rc = T3;
    if (T3)
    {
        T3->parent = c;
    }
    BST<T>::updateHeight(c);
    b->lc = a;
    a->parent = b;
    b->rc = c;
    c->parent = b;
    BST<T>::updateHeight(b);
    return b; //该子树新的根节点
}


int main()
{
    AVL<int>obj;

    for (int i = 0; i < 15; i++)
    {
        obj.insert(i);
    }

    if (!obj.search(5))
    {
        cout << "element 5 was not found!" << endl;
    }
    else
    {
        cout << "element 5 was found!" << endl;
    }

    obj.remove(1);

    if (obj.empty())
    {
        cout << "Tree is empty" << endl;
    }

    if (!obj.search(1))
    {
        cout << "element 1 was not found!" << endl;
    }
    else
    {
        cout << "element 1 was found!" << endl;
    }


    obj.travIn(visit<int>);
    system("pause");
    return 0;
}
