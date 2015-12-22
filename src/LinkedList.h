#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

using namespace std;

template<class T>
struct ListItem {
	ListItem<T>(T *i):item(i),next(nullptr),prev(nullptr) {}
	ListItem<T>(T *i, shared_ptr<ListItem<T> > p, shared_ptr<ListItem<T> > n):item(i),next(n),prev(p) {}

	T *item;

	shared_ptr<ListItem<T> > next;
	shared_ptr<ListItem<T> > prev;

	/* inserts a new ListItem after this one */
	inline void insert(T *i) {
		shared_ptr<ListItem<T> > p (new ListItem<T>(i,this,next));
		if(next != this) {
			next->prev = p;
			next = p;
		}
	}

	inline T* remove() {
		if(next == this) {
			auto ret = item;
			item = nullptr;
			return ret;
		}

		auto bak = this->prev;
		this->prev->next = this->next;
		this->next->prev = bak;
		return item;
	}
};

#endif
