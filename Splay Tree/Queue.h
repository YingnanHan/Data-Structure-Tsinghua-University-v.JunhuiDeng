#pragma once
#ifndef QUEUE_H
#define QUEUE_H

#include"List.h"

using namespace std;

template<typename T>
class Queue :public List<T>
{
	//队列模板类（继承List原有接口）
public:
	//size(),empty(),以及其他开放接口均可直接沿用
	void enqueue(T const& e)
	{
		List<T>::insertAsLast(e);//入队，尾部插入
	}

	T dequeue()
	{
		return List<T>::remove(List<T>::first());//入队，首部插入
	}

	T& front()
	{
		return List<T>::first()->data;//队首
	}
};


#endif QUEUE_H