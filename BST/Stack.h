#pragma once

#ifndef STACK_H
#define STACK_H

#pragma once
#include"Vector.h"//以向量类，派生出模板类


template <typename T>
class Stack : public Vector<T>
{	//将向量的首/末端作为栈底/顶
public:
	//size()、empty()以及其它开放接口，均可直接沿用
	void push(T const& e)
	{
		//值得注意的是：由于insert接口业已在标准库中有其自己的实现，所以这里要显示的指明
		//所调用的是那一个重载版本
		Vector<T>::insert(e);
	} //入栈：等效于将新元素作为向量的末元素插入

	T pop()
	{
		return Vector<T>::remove(Vector<T>::size() - 1);
	} //出栈：等效于删除向量的末元素

	T& top()
	{
		return (*this)[Vector<T>::size() - 1];
	} //取顶：直接返回向量的末元素
};

#endif 