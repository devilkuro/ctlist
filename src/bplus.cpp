#include "bplus.h"

int Bplus::sumB = 0;
Bplus::Bplus() {
	root = NULL;
	first = NULL;
}

Result* Bplus::Search(int x) {
	struct BNode *p, *q;
	struct Result *r;
	int i;

	r = new Result;

	r->ptr = NULL;
	r->i = 0;
	if (root == NULL) {
		cout << "空树!" << endl;
		r->tag = 0;
	} else if (x > root->key[root->keynum - 1]) //查找的关键字比原树中的所有关键字都大
			{
		r->tag = 1;
	} else {
		p = root;
		q = NULL;
		while (p) {
			q = p;
			i = 0;
			while (i < p->keynum && p->key[i] < x)
				i++;
			p = p->ptr[i];
		}
		r->ptr = q;
		r->i = i;
		if (q->key[i] == x) //查找的关键字已存在
			r->tag = 2;
		else
			//查找的关键字不存在，但不大于原树中最大的关键字
			r->tag = 3;
	}

	return r;
}

void Bplus::Split(BNode *tmp) {
	struct BNode *t, *l;
	int i;

	while (tmp->keynum > m) //需要分裂
	{
		if (tmp == root) //根节点分裂，新建根节点
				{
			t = new BNode;
			t->keynum = s2;
			for (i = 0; i < s2; i++) {
				t->key[i] = tmp->key[s1 + i];
				t->record[i] = tmp->record[s1 + i];
				t->ptr[i] = tmp->ptr[s1 + i];
				t->ptr[i]->parent = t;
				t->ptr[i]->seq = i;
			}
			tmp->keynum = s1;
			t->next = tmp->next;
			tmp->next = t;

			root = new BNode;
			root->keynum = 2;
			root->key[0] = tmp->key[s1 - 1];
			root->ptr[0] = tmp;
			root->key[1] = t->key[s2 - 1];
			root->ptr[1] = t;
			root->parent = NULL;
			root->next = NULL;
			root->seq = -1;

			tmp->parent = root;
			t->parent = root;
			tmp->seq = 0;
			t->seq = 1;
		} else {
			t = new BNode;
			t->keynum = s2;
			for (i = 0; i < s2; i++) {
				t->key[i] = tmp->key[s1 + i];
				t->record[i] = tmp->record[s1 + i];
				t->ptr[i] = tmp->ptr[s1 + i];
			}
			if (tmp->ptr[0]) {
				for (i = 0; i < s2; i++) {
					t->ptr[i]->parent = t;
					t->ptr[i]->seq = i;
				}
			}

			tmp->keynum = s1;
			t->next = tmp->next;
			tmp->next = t;
			l = tmp->parent;
			t->parent = l;
			t->seq = tmp->seq + 1;

			for (i = l->keynum - 1; i > tmp->seq; i--) {
				l->key[i + 1] = l->key[i];
				l->record[i + 1] = l->record[i];
				l->ptr[i + 1] = l->ptr[i];
				l->ptr[i + 1]->seq = i + 1;
			}
			l->key[tmp->seq] = tmp->key[s1 - 1];
			l->ptr[tmp->seq] = tmp;
			l->key[t->seq] = t->key[s2 - 1];
			l->ptr[t->seq] = t;
			l->keynum++;

			tmp = l;
		}
	}
}

bool Bplus::Insert(Request a) {
	struct Result *r;
	struct BNode *p, *q, *t, *l, *tmp;
	int nowtd, i, j;

	r = Search(a.ts);
	//接纳控制
	nowtd = a.td;
	p = r->ptr;
	i = r->i;
	int flag = 0;//If flag == 1, request a is ending up before node r->ptr. If else, request a will continue in node r->ptr.
	if (r->tag == 3) {
		if (i != 0) {
			if (p->record[i-1] + a.bw > MAX)
				return false;
			else if (true) {
				if (nowtd > (p->key[i] - a.ts))
					nowtd -= (p->key[i] - a.ts);
				else
					flag = 1;
			}
		} else {
			q = first;
			while (q->next != p)
				q = q->next;
			if (q->record[q->keynum - 1] + a.bw > MAX)
				return false;
			else {
				if (nowtd > (p->key[i] - a.ts))
					nowtd -= (p->key[i] - a.ts);
				else
					flag = 1;
			}
		}
	}
	if ((r->tag == 3 && flag == 0) || r->tag == 2) {
		while (nowtd) {
			while (nowtd && i < p->keynum - 1 && p->record[i] + a.bw <= MAX)
			{
				if (nowtd > p->key[i + 1] - p->key[i])
					nowtd -= p->key[i + 1] - p->key[i];
				else
					nowtd = 0;
				i++;
			}
			if ( nowtd && p->record[i] + a.bw > MAX)
				return false;
			if (nowtd && i == p->keynum - 1) {
				if (p->next) {
					q = p->next;
					if (p->record[p->keynum - 1] + a.bw <= MAX) {
						if (nowtd > q->key[0] - p->key[p->keynum - 1]) {
							nowtd -= q->key[0] - p->key[p->keynum - 1];
							p = q;
							i = 0;
						} else
							nowtd = 0;
					}
				} else
					nowtd = 0;
			}
		}
	}

	sumB++;
	//插入请求
	if (r->tag == 0) //空树
			{
		p = new BNode;
		p->keynum = 3;
		p->key[0] = 0;
		p->key[1] = a.ts;
		p->key[2] = a.ts + a.td;
		p->record[0] = 0;
		p->record[1] = a.bw;
		p->record[2] = 0;
		p->ptr[0] = p->ptr[1] = p->ptr[2] = NULL;
		p->next = NULL;

		q = new BNode;
		q->keynum = 1;
		q->key[0] = p->key[2];
		q->record[0] = -1;
		q->ptr[0] = p;
		q->next = NULL;
		q->parent = NULL;
		q->seq = -1;

		p->parent = q;
		p->seq = 0;

		root = q;
		first = p;
	} else {
		//##########################对起始位置的处理###############################

		if (r->tag == 2) //此关键字已存在
				{
			nowtd = a.td;
			p = r->ptr; //当前查找到的结点
			i = r->i; //记录起始位置
		} else //此关键字不存在，于查询返回的叶子结点中插入
		{
			nowtd = a.td;
			p = r->ptr;
			i = r->i;
			if (r->tag == 1) //当前的查找值比原树中的所有值都要大
					{
				tmp = root;
				while (tmp->ptr[0]) //tmp为内部结点
				{
					tmp->key[tmp->keynum - 1] = a.ts;
					tmp = tmp->ptr[tmp->keynum - 1];
				}
				p = tmp;
				p->key[p->keynum] = a.ts;
				p->record[p->keynum] = p->record[p->keynum - 1];
				p->ptr[p->keynum] = NULL;
				i = p->keynum;
				p->keynum++;
			} else {
				for (j = p->keynum - 1; j >= i; j--) {
					p->key[j + 1] = p->key[j];
					p->record[j + 1] = p->record[j];
					p->ptr[j + 1] = p->ptr[j];
				}
				p->key[i] = a.ts;
				if (i != 0)
					p->record[i] = p->record[i - 1];
				else {
					q = first;
					while (q->next != p)
						q = q->next;
					p->record[i] = q->record[q->keynum - 1];
				}
				p->ptr[i] = NULL;
				p->keynum++;
			}

			tmp = p;
			if (tmp->keynum > m) //叶子结点需要分裂
					{
				t = new BNode;
				t->keynum = s2;
				for (j = 0; j < s2; j++) {
					t->key[j] = tmp->key[s1 + j];
					t->record[j] = tmp->record[s1 + j];
					t->ptr[j] = tmp->ptr[s1 + j];
				}
				tmp->keynum = s1;
				t->next = tmp->next;
				tmp->next = t;
				l = tmp->parent;
				t->parent = l;
				t->seq = tmp->seq + 1;

				if (i < s1)
					p = tmp;
				else {
					p = t;
					i -= s1;
				}

				for (j = l->keynum - 1; j > tmp->seq; j--) {
					l->key[j + 1] = l->key[j];
					l->record[j + 1] = l->record[j];
					l->ptr[j + 1] = l->ptr[j];
					l->ptr[j + 1]->seq = j + 1;
				}
				l->key[tmp->seq] = tmp->key[s1 - 1];
				l->ptr[tmp->seq] = tmp;
				l->key[t->seq] = t->key[s2 - 1];
				l->ptr[t->seq] = t;
				l->keynum++;

				tmp = l;
			}

			if (tmp->keynum > m)
				Split(tmp); //内部结点需要继续分裂
		}
//#########################对终止位置的处理##############################
		q = NULL;
		while (nowtd) {
			if (q) {
				if (nowtd >= p->key[i] - q->key[q->keynum - 1]) {
					q->record[q->keynum - 1] += a.bw;
					nowtd -= (p->key[i] - q->key[q->keynum - 1]);
				} else //此请求已可插入完成，下一个结点的第一个
				{
					nowtd = 0;
					for (j = p->keynum - 1; j >= 0; j--) {
						p->key[j + 1] = p->key[j];
						p->record[j + 1] = p->record[j];
						p->ptr[j + 1] = p->ptr[j];
					}
					p->key[0] = a.ts + a.td;
					p->record[0] = q->record[q->keynum - 1];
					p->ptr[0] = NULL;
					q->record[q->keynum - 1] += a.bw;
					p->keynum++;

					if (p->keynum > m)
						Split(p);
				}
			} //q

			while (nowtd > 0 && i < p->keynum - 1
					&& nowtd >= p->key[i + 1] - p->key[i]) {
				p->record[i] += a.bw;
				nowtd -= (p->key[i + 1] - p->key[i]);
				i++;
			}
			if (nowtd == 0) //终止位置的关键字恰好存在
				break;
			else if (i == p->keynum - 1) //已预留到当前结点的最后一个关键字
					{
				if (p->next) {
					q = p;
					p = p->next;
				} else //当前结点为最后一个结点，此请求已可插入完成
				{
					nowtd = 0;
					p->key[p->keynum] = a.ts + a.td;
					p->record[p->keynum] = p->record[p->keynum - 1];
					p->ptr[p->keynum] = NULL;
					p->record[p->keynum - 1] += a.bw;
					p->keynum++;

					tmp = p->parent;
					while (tmp) {
						tmp->key[tmp->keynum - 1] = a.ts + a.td;
						tmp = tmp->parent;
					}

					if (p->keynum > m)
						Split(p);
				}
			} else //此请求已可插入完成，当前结点插入
			{
				nowtd = 0;
				for (j = p->keynum - 1; j >= i + 1; j--) {
					p->key[j + 1] = p->key[j];
					p->record[j + 1] = p->record[j];
					p->ptr[j + 1] = p->ptr[j];
				}
				p->key[i + 1] = a.ts + a.td;
				p->record[i + 1] = p->record[i];
				p->ptr[i + 1] = NULL;
				p->record[i] += a.bw;
				p->keynum++;

				if (p->keynum > m)
					Split(p);
			}
			i = 0;
		}
	}
	return true;
}

void Bplus::DSplit(BNode *tmp) {
	struct BNode *t, *l;
	int i, j;

	while (tmp->keynum > m) //需要分裂
	{
		t = new BNode;
		t->keynum = tmp->keynum / 2;
		int n = tmp->keynum - t->keynum;
		for (i = 0; i < t->keynum; i++) {
			t->key[i] = tmp->key[n + i];
			t->record[i] = tmp->record[n + i];
			t->ptr[i] = tmp->ptr[n + i];
		}
		if (tmp->ptr[0]) {
			for (i = 0; i < t->keynum; i++) {
				t->ptr[i]->parent = t;
				t->ptr[i]->seq = i;
			}
		}
		tmp->keynum = n;
		t->next = tmp->next;
		tmp->next = t;
		l = tmp->parent;
		t->parent = l;

		i = 0;
		while (l->ptr[i] == NULL)
			i++;
		for (j = 0; j < l->keynum; j++) {
			l->key[j] = l->key[i + j];
			l->ptr[j] = l->ptr[i + j];
			l->ptr[j]->seq = j;
		}

		for (i = l->keynum - 1; i > tmp->seq; i--) {
			l->key[i + 1] = l->key[i];
			l->record[i + 1] = l->record[i];
			l->ptr[i + 1] = l->ptr[i];
			l->ptr[i + 1]->seq = i + 1;
		}
		t->seq = tmp->seq + 1;
		l->key[tmp->seq] = tmp->key[tmp->keynum - 1];
		l->ptr[tmp->seq] = tmp;
		l->key[t->seq] = t->key[t->keynum - 1];
		l->ptr[t->seq] = t;
		l->keynum++;

		tmp = l;
	}
}

bool Bplus::Delete(int x) //每隔x时间段，删除叶子结点（自底向上逐层删除）
		{
	struct BNode *tmp, *p, *q, *t, *l;
	int i, j, a;

	tmp = first;

	while (tmp && tmp->key[tmp->keynum - 1] <= x) {
		p = tmp->parent;
		p->ptr[tmp->seq] = NULL;
		p->keynum--;

		while (p->keynum == 0) {
			if (p == root) //整棵树删没了,退出
					{
				root = NULL;
				first = NULL;
				return true;
			} else {
				q = p->parent;
				q->ptr[p->seq] = NULL;
				q->keynum--;

				t = p;
				p = q;
				delete t; //释放内部空结点
			}
		}

		a = tmp->record[tmp->keynum - 1];
		t = tmp;
		tmp = tmp->next;
		delete t; //释放叶子空结点
	}
	first = tmp;

	//将当前删除的截止时间插入first节点中
	if (x < first->key[0]) {
		for (i = first->keynum - 1; i >= 0; i--) {
			first->key[i + 1] = first->key[i];
			first->record[i + 1] = first->record[i];
			first->ptr[i + 1] = first->ptr[i];
		}
		first->key[0] = x;
		first->record[0] = a;
		first->ptr[0] = NULL;
		first->keynum++;
		if (first->keynum > m)
			DSplit(first);
	}

	p = first->parent;
	while (p) {
		if (p->keynum < s2 && p->next && p->parent) //需要合并,且存在后继结点可用于合并
				{
			if (p->keynum + p->next->keynum <= m) {
				q = p->parent;
				q->ptr[p->seq] = NULL;
				q->keynum--;

				while (q->keynum == 0) {
					l = q->parent;
					l->ptr[q->seq] = NULL;
					l->keynum--;

					t = q;
					q = l;
					delete t;
				}

				q = p->next;
				x = p->keynum;

				for (i = q->keynum - 1; i >= 0; i--) {
					q->key[x + i] = q->key[i];
					q->ptr[x + i] = q->ptr[i];
					q->ptr[x + i]->seq = x + i;
				}
				for (i = 0; i < x; i++) {
					q->key[i] = tmp->key[tmp->keynum - 1];
					q->ptr[i] = tmp;
					q->ptr[i]->parent = q;
					q->ptr[i]->seq = i;

					tmp = tmp->next;
				}
				q->keynum += x;
				tmp = q;
				t = p;
				p = q->parent;
				delete t;
			} else {
				i = 0;
				while (p->ptr[i] == 0)
					i++;
				for (j = 0; j < p->keynum; j++) {
					p->key[j] = p->key[i + j];
					p->ptr[j] = p->ptr[i + j];
					p->ptr[j]->seq = j;
				}
				q = p->next;
				int n = (p->keynum + q->keynum) / 2;
				for (i = p->keynum; i < n; i++) {
					p->key[i] = q->key[i - p->keynum];
					p->ptr[i] = q->ptr[i - p->keynum];
					p->ptr[i]->parent = p;
					p->ptr[i]->seq = i;
				}
				int nm = n - p->keynum;
				q->keynum -= nm;
				p->keynum = n;
				for (i = 0; i < q->keynum; i++) {
					q->key[i] = q->key[i + nm];
					q->ptr[i] = q->ptr[i + nm];
					q->ptr[i]->seq = i;
				}

			}
		} else //不合并
		{
			if (tmp->seq != 0) //结点内的关键字需要移动
					{
				i = 0;
				while (p->ptr[i] == NULL)
					i++;
				for (j = 0; j < p->keynum; j++) {
					p->key[j] = p->key[i + j];
					p->ptr[j] = p->ptr[i + j];
					p->ptr[j]->seq = j;
				}
			}
			tmp = p;
			p = p->parent;
		}
	}

	p = root;
	if (p->keynum == 1) {
		while (p->keynum == 1)
			p = p->ptr[0];
		root = p->parent;
	}
	return true;
}

void Bplus::Display() //自顶向下逐层输出
{
	BNode *p, *q;
	int i;

	p = root;
	while (p) {
		q = p; //q记录该层的第一个结点
		while (p) {
			for (i = 0; i < p->keynum; i++)
				cout << p->key[i] << ' ';
			cout << '*';
			p = p->next;
		}

		cout << endl;
		p = q->ptr[0]; //往下一层
	}

	p = first;
	while (p) {
		for (i = 0; i < p->keynum; i++)
			cout << p->record[i] << ' ';
		cout << '*';
		p = p->next;
	}
	cout << endl;
}

void Bplus::FDisplay(int t2) //自顶向下逐层输出
		{
	struct BNode *p, *q;
	int i;

	ofstream file("display.txt", ios::app);
	file << "删除时间:" << t2 << endl;
	p = root;
	while (p) {
		q = p; //q记录该层的第一个结点
		while (p) {
			for (i = 0; i < p->keynum; i++)
				file << p->key[i] << ' ';
			file << '*';
			p = p->next;
		}

		file << endl;
		p = q->ptr[0]; //往下一层
	}

	p = first;
	while (p) {
		for (i = 0; i < p->keynum; i++)
			file << p->record[i] << ' ';
		file << '*';
		p = p->next;
	}
	file << endl;

	file.close();
}

int Bplus::Count() {
	int c = 0;
	BNode *p, *q;

	p = root;
	while (p) {
		q = p; //q记录该层的第一个结点
		while (p) {
			c++;
			p = p->next;
		}
		p = q->ptr[0]; //往下一层
	}

	return c;
}

int Bplus::End() {
	if (root) {
		return root->key[root->keynum - 1];
	}

	return 0;
}

bool Bplus::Output() {
	struct BNode *p;
	cout<<"BPLUS:DISPLAY."<<endl;
	p = first;
		while (p) {
			for (int i = 0; i < p->keynum; i++){
				cout<< "rs: " << p->key[i] << ", " << p->record[i] << endl;
			}
			p = p->next;
		}
	return true;
}
