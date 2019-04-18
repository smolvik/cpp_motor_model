#ifndef __DELAYER_H__
#define __DELAYER_H__

/**
 * @brief
 * Линия задержки
 */
template <class t> class Delayer {
	t *wnd;
	int sz;
	int idx;
public:
	Delayer(int n){
		sz = n;
		idx = 0;
		wnd = new t[n];
	}
	~Delayer(){
		delete[] wnd;
	}
	
	/**
	 * @brief обновляет состояние
	 * @param x текущее значение элемента 
	 * @return элемент задержанный на n тактов
	 * 
	*/
	t operator ()(t x){
		t ret = wnd[idx];
		wnd[idx] = x;
		idx = (idx+1)%sz;
		return ret;
	}
};


#endif
