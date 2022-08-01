#include"BST.h"//基于BST实现Splay树


using namespace std;


template<typename T>
class Splay :public BST<T>
{
	//由BST派生的Splay树模板类
protected:
	BinNodePosi(T)splay(BinNodePosi(T) v);//将节点v伸至根
public:
	BinNodePosi(T)& search(const T& e);//查找(重写)
	BinNodePosi(T) insert(const T& e);//插入(重写)
	bool remove(const T& e);//删除(重写)
};



template<typename NodePosi>inline //在节点*p与*lc(可能为空)之间建立父(左)子关系
void attachAsLChild(NodePosi p, NodePosi lc)
{
	p->lc = lc;
	if (lc)
	{
		lc->parent = p;
	}
}


template<typename NodePosi> //在节点*p与*rc(可能为空)之间建立父(右)子关系
void attachAsRChild(NodePosi p, NodePosi rc)
{
	p->rc = rc;
	if (rc)
	{
		rc->parent = p;
	}
}


template<typename T>//Splay树伸展算法:从节点v出发逐层伸展
BinNodePosi(T) Splay<T>::splay(BinNodePosi(T) v)
{
	//v为因最近访问而需要伸展的节点的位置
	if (!v)
	{
		return NULL;
	}
	BinNodePosi(T) p;//*v的父亲
	BinNodePosi(T) g;//*v的祖父
	while ((p = v->parent) && (g = p->parent))
	{
		//自下而上，反复对*v进行双层伸展
		BinNodePosi(T) gg = g->parent;//每轮之后*v都以原曾祖父(great-grand parent)为父
		if (IsLChild(*v))
		{
			if (IsLChild(*p))
			{
				//zig-zig
				attachAsLChild(g, p->rc);
				attachAsLChild(p, v->rc);
				attachAsRChild(p, g);
				attachAsRChild(v, p);
			}
			else
			{
				//zig-zag
				attachAsLChild(p, v->rc);
				attachAsRChild(g, v->lc);
				attachAsLChild(v, g);
				attachAsRChild(v, p);
			}
		}
		else if (IsRChild(*p))
		{
			//zag-zag
			attachAsRChild(g, p->lc);
			attachAsRChild(p, v->lc);
			attachAsLChild(p, g);
			attachAsLChild(v, p);
		}
		else
		{
			//zag-zig
			attachAsRChild(p, v->lc);
			attachAsLChild(g, v->rc);
			attachAsRChild(v, g);
			attachAsLChild(v, p);
		}
		if (!gg)
		{
			v->parent = NULL;//若*v原先的曾祖父*gg不存在，则*v应该为现在的树根
		}
		else//否则，*gg此后应该以*v作为左孩子或者右孩子
		{
			(g == gg->lc) ? attachAsLChild(gg, v) : attachAsRChild(gg, v);
		}
		BinTree<T>::updateHeight(g);
		BinTree<T>::updateHeight(p);
		BinTree<T>::updateHeight(v);
	}//双层伸展结束时，必有g==NULL，但是p可能不为空
	if (p = v->parent)
	{
		//如果p果真非空，则额外做一次单旋
		if (IsLChild(*v))
		{
			attachAsLChild(p, v->rc);
			attachAsRChild(v, p);
		}
		else
		{
			attachAsRChild(p, v->lc);
			attachAsLChild(v, p);
		}
		BinTree<T>::updateHeight(p);
		BinTree<T>::updateHeight(v);
	}
	v->parent = NULL;
	return v;
}//调整之后新树根应为被伸展的节点，故返回节点的位置以便上层函数更新树根


template<typename T>
BinNodePosi(T)& Splay<T>::search(const T& e)
{
	//在伸展树中查找e
	BinNodePosi(T) p = searchIn(BinTree<T>::_root, e, BST<T>::_hot = NULL);
	BinTree<T>::_root = Splay<T>::splay(p ? p : BST<T>::_hot);//将最后一个被访问的节点伸展至树根
	return BinTree<T>::_root;
}//与其他BST不同，无论查找成功与否，_root都指向最终被访问过的节点


template<typename T>
BinNodePosi(T) Splay<T>::insert(const T& e)
{
	//将关键码e插入伸展树中
	if (!BinTree<T>::_root)
	{
		BinTree<T>::_size++;
		return BinTree<T>::_root = new BinNode<T>(e);//处理原树为空的退化情况
	}
	if (e == Splay<T>::search(e)->data)
	{
		return BinTree<T>::_root;//确认目标节点不存在
	}
	BinTree<T>::_size++;
	BinNodePosi(T) t = BinTree<T>::_root;//创建新节点，以下调整<=7个指针以完成布局重构
	if (BinTree<T>::_root->data < e)
	{
		//插入新根，以t和t->rc为左右孩子
		t->parent = BinTree<T>::_root = new BinNode<T>(e, NULL, t, t->rc);//2+3个
		if (HasRChild(*t))
		{
			t->rc->parent = BinTree<T>::_root;
			t->rc = NULL;//<=2个
		}
	}
	else//插入新根，以t->lc和t为左右孩子
	{
		t->parent = BinTree<T>::_root = new BinNode<T>(e, NULL, t->lc, t);//2+3个
		if (HasLChild(*t))
		{
			t->lc->parent = BinTree<T>::_root;
			t->lc = NULL;//<=2个
		}
	}
	BinTree<T>::updateHeightAbove(t);//更新t及其祖先(实际上只有_root一个)的高度
	return BinTree<T>::_root;//新节点必然置于树根，返回之
}//无论e是否存在于原树之中,返回时总有_root->data=e


template<typename T>
bool Splay<T>::remove(const T& e)//从伸展树中删除关键码e
{
	if (!BinTree<T>::_root || (e != Splay<T>::search(e)->data))
	{
		//若数空或者目标不存在，则无法删除
		return false;
	}
	BinNodePosi(T)w = BinTree<T>::_root;//assert：经过search()之后的节点e已经被伸展至树根
	if (!HasLChild(*BinTree<T>::_root))//若无左子树，直接删除
	{
		BinTree<T>::_root = BinTree<T>::_root->rc;
		if (BinTree<T>::_root)
		{
			BinTree<T>::_root->parent = NULL;
		}
	}
	else if (!HasRChild(*BinTree<T>::_root))//若无右子树，直接删除
	{
		BinTree<T>::_root = BinTree<T>::_root->lc;
		if (BinTree<T>::_root)
		{
			BinTree<T>::_root->parent = NULL;
		}
	}
	else
	{
		//若左右子树同时存在
		BinNodePosi(T)lTree = BinTree<T>::_root->lc;
		lTree->parent = NULL;
		BinTree<T>::_root->lc = NULL;//暂时将左子树切除
		BinTree<T>::_root = BinTree<T>::_root->rc;
		BinTree<T>::_root->parent = NULL;//只保留右子树
		Splay<T>::search(w->data);//以原树根为目标做一次（必定失败的）查找
		BinTree<T>::_root->lc = lTree;
		//assert:至此，右子树中最小节点必然延伸至树根，而且因为没有雷同节点，其左子树必空，于是
		lTree->parent = BinTree<T>::_root;//只需将左子树接回原位即可
	}
	release(w->data);
	release(w);
	BinTree<T>::_size--;//只需将左子树接回原位即可
	if (BinTree<T>::_root)
	{
		BinTree<T>::updateHeight(BinTree<T>::_root);//此后若书非空，则树根的高度需要更新
	}
	return true;//返回成功标志
}//若目标节点存在且被删除，返回True否则返回False


int main()
{
	Splay<int>obj;

	for (int i = 0; i < 15; i++)
	{
		obj.insert(i);
	}
	obj.travIn(visit<int>);
	if (obj.search(5)->data != 5)
	{
		cout << "element 5 was not found!" << endl;
	}
	else
	{
		cout << "element 5 was found!" << endl;
	}

	obj.remove(1);
	cout << "After remove element with value 1." << endl;
	obj.travIn(visit<int>);
	if (obj.empty())
	{
		cout << "Tree is empty" << endl;
	}
	cout << endl;
	if (obj.search(1)->data != 1)
	{
		cout << "element 1 was not found!" << endl;
	}
	else
	{
		cout << "element 1 was found!" << endl;
	}

	obj.insert(10000);
	cout << "After insert element with value 10000." << endl;
	obj.travIn(visit<int>);
	system("pause");
	return 0;
}
