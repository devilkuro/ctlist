#include "array.h"

int Array::sumA = 0;
int Array::t = 0;
Array::Array(int n)
{
	int i;

	p = new int[n];
	N = n;
	for(i = 0; i < N; i++)
		p[i] = 0;
}

void Array::Insert(Request r)
{
	int i, end;

	end = t + r.ts + r.td;
	//进行接纳控制
	i = t + r.ts ;
	if(r.td > N)
		return;
	while(i < end && i < N && p[i] + r.bw <= MAX)
	{
		i++;
	}
	if(p[i] + r.bw > MAX)
	{
		return;
	}
	else if(i >= N)
	{
		while(i < end && p[i-N] + r.bw <= MAX)
	    {
			i++;
		}
		if(p[i-N] + r.bw > MAX)
		{
			return;
		}
	}

	sumA++;
	i = t + r.ts ;
	while(i < end && i < N)
	{
		p[i] += r.bw;
		i++;
	}
	if(i >= N)
	{
		while(i < end)
	    {
			p[i-N] += r.bw;
			i++;
		}
	}
}

void Array::Clear()
{
	p[t] = 0;
	t = (t+1) % N;
}

void Array::Display()
{
	int i;

	for(i = t; i < N; i++)
		cout<<p[i]<<' ';
	for(i = 0; i < t; i++)
		cout<<p[i]<<' ';
	cout<<endl;
}
