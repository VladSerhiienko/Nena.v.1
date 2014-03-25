#include <iostream>
using namespace std;

#define MAX_N 36

#define in std::cin 
#define out std::cout 


int C[MAX_N][MAX_N];    // Матрица "пропускных способностей"
int F[MAX_N][MAX_N];    // Матрица "текущего потока в графе"
int P[MAX_N][MAX_N];    // Матрица "стоимости (расстояний)"
int push[MAX_N];        // Поток в вершину [v] из начальной точки
int mark[MAX_N];        // Отметки на вершинах, в которых побывали
int pred[MAX_N];        // Откуда пришли в вершину [v] (предок)
int dist[MAX_N];        // Расстояние до вершины [v] из начальной точки
int N, M, s, t;         // Кол-во вершин, ребер, начальная и конечные точки
int max_flow;
int min_cost;

void file_read()
{
	int u, v, c, p;
	in >> N >> M >> s >> t; N++;

	for (int i = 0; i < M; i++)
	{
		in >> u >> v >> c >> p;
		C[u][v] = c;
		P[u][v] = p;
		P[v][u] = -p;
	}
}

int edge_cost(int u, int v)
{
	if (C[u][v] - F[u][v] > 0) return P[u][v];
	else return MAX_VAL;
}

int check_cycles()
{
	for (int u = 1; u < N; u++)
	for (int v = 1; v < N; v++)
	if (dist[v] > dist[u] + edge_cost(u, v))
		return u;

	return MAX_VAL;
}

void init()
{
	for (int i = 1; i < N; i++)
	{
		mark[i] = 0;
		push[i] = 0;
		pred[i] = 0;
		dist[i] = MAX_VAL;
	}
}

// Алгоритм Поиска в ширину

int bf(int s)
{
	init();
	queue<int> Q;
	pred[s] = s;
	dist[s] = 0;

	Q.push(s);
	Q.push(MAX_N);

	int u, series = 0;
	while (!Q.empty())
	{
		while (Q.front() == MAX_N)
		{
			Q.pop();
			if (++series > N) return check_cycles();
			else Q.push(MAX_N);
		}

		u = Q.front(); Q.pop();
		for (int v = 1; v < N; v++)
		if (dist[v] > dist[u] + edge_cost(u, v))
		{
			dist[v] = dist[u] + edge_cost(u, v);
			pred[v] = u;
			Q.push(v);
		}
	}
}

// Алгоритм Беллмана-Форда

int bfs(int s, int t)
{
	init();
	queue<int> Q;
	mark[s] = 1;
	pred[s] = s;
	push[s] = MAX_VAL;

	Q.push(s);
	while (!mark[t] && !Q.empty())
	{
		int u = Q.front(); Q.pop();
		for (int v = 1; v < N; v++)
		if (!mark[v] && (C[u][v] - F[u][v] > 0))
		{
			push[v] = min(push[u], C[u][v] - F[u][v]);
			mark[v] = 1;
			pred[v] = u;
			Q.push(v);
		}
	}

	return mark[t];
}

// Алгоритм Форда-Фалкерсона

void max_flow_ff()
{
	int u, v, flow = 0;

	while (bfs(s, t))
	{
		int add = push[t];

		v = t; u = pred[v];
		while (v != s)
		{
			F[u][v] += add;
			F[v][u] -= add;
			v = u; u = pred[v];
		}
		flow += add;
	}

	max_flow = flow;
}

// Алгоритм вычисления Максимального поток минимальной стоимости

void min_cost_flow()
{
	max_flow_ff();

	int u, v, flow = 0;
	int add = MAX_VAL;
	int neg_cycle;

	neg_cycle = bf(t);
	while (neg_cycle != MAX_VAL)
	{
		v = neg_cycle; u = pred[v];
		do
		{
			add = min(add, C[u][v] - F[u][v]);
			v = u; u = pred[v];
		} while (v != neg_cycle);

		v = neg_cycle; u = pred[v];
		do
		{
			F[u][v] += add;
			F[v][u] -= add;
			v = u; u = pred[v];
		} while (v != neg_cycle);

		neg_cycle = bf(t);
	}

	for (int u = 1; u < N; u++)
	for (int v = 1; v < N; v++)
	if (F[u][v] > 0)
		min_cost += F[u][v] * P[u][v];
}

void file_write()
{
	out << max_flow << endl;
	out << min_cost << endl;
}

void main()
{
	file_read();
	min_cost_flow();
	file_write();
}