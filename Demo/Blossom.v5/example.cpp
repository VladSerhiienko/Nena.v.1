#include <stdio.h>
#include "PerfectMatching.h"

void LoadFile(int& node_num, int& edge_num, int*& edges, int*& weights, char* filename, bool max_cost)
{
	int e = 0;
	char LINE[1000];

	FILE* fp;
	fopen_s(&fp, filename, "r");

	if (!fp) { printf("Can't open %s\n", filename); exit(1); }

	int format = -1; // 0: DIMACS format. node id's start from 1
	                 // 1: simpler format (without "p" and "e"). node id's start from 0

	edge_num = -1;
	while (fgets(LINE, sizeof(LINE)-1, fp))
	{
		if (LINE[0] == 'c') continue;
		if (format < 0)
		{
			if (LINE[0] == 'p')
			{
				format = 0;
				if (sscanf_s(LINE, "p edge %d %d\n", &node_num, &edge_num) != 2) { printf("%s: wrong format #1\n", filename); exit(1); }
			}
			else
			{
				format = 1;
				if (sscanf_s(LINE, "%d %d\n", &node_num, &edge_num) != 2) { printf("%s: wrong format #1\n", filename); exit(1); }
			}

			//////////////////////////////////////////////////////////////////////////////////
			if (node_num <= 0 || edge_num < 0) { printf("# of nodes and edges should be positive\n"); exit(1); }
			if (node_num & 1) { printf("# of nodes is odd: perfect matching cannot exist\n"); exit(1); }
			edges = new int[2*edge_num];
			weights = new int[edge_num];
			//////////////////////////////////////////////////////////////////////////////////
		}
		else
		{
			int i, j;
			char* ptr = LINE;
			if (format == 0) { if (LINE[0] != 'e') continue; ptr = &LINE[1]; }
			else             ptr = &LINE[0];

			int len;
			if (sscanf_s(ptr, "%d %d %d\n", &i, &j, &len) != 3) continue;
			if (format == 0) { i --; j --; }
			edges[2*e] = i;
			edges[2*e+1] = j;
			weights[e] = max_cost ? -len : len;
			e ++;
		}
	}

	if (e != edge_num) { printf("%s: wrong format #3\n", filename); exit(1); }
	fclose(fp);
}

void SaveMatching(int node_num, PerfectMatching* pm, char* filename, double cost)
{
	FILE* fp;
	fopen_s(&fp, filename, "w");

	if (!fp) { printf("Can't open %s\n", filename); exit(1); }
	fprintf(fp, "c cost %f\n", cost);
	fprintf(fp, "%d %d\n", node_num, node_num / 2);
	int i, j;
	for (i=0; i<node_num; i++)
	{
		j = pm->GetMatch(i);
		if (i < j) fprintf(fp, "%d %d\n", i, j);
	}
	fclose(fp);
}

void ShowUsage()
{
	printf("Usage: see USAGE.TXT\n");
	exit(1);
}

int main(int argc, char* argv[])
{
	struct PerfectMatching::Options options;
	char* filename = NULL;
	char* geom_filename = NULL;
	char* save_filename = NULL;
	bool check_perfect_matching = false;
	bool max_cost = false;
	int i, e, node_num, edge_num;
	int* edges = nullptr;
	int* weights = nullptr;

	for (i=1; i<argc; i++)
	{
		if (argv[i][0] != '-') { printf("Unknown option: %s\n", argv[i]); ShowUsage(); }
		switch (argv[i][1])
		{
			case 'e':
				if (filename || argv[i][2] || ++i == argc) ShowUsage();
				filename = argv[i];
				break;
			case 'w':
				if (save_filename || argv[i][2] || ++i == argc) ShowUsage();
				save_filename = argv[i];
				break;
			case 'c':
				if (argv[i][2]) ShowUsage();
				check_perfect_matching = true;
				break;
			case 'm':
				if (argv[i][2]) ShowUsage();
				max_cost = true;
				break;
			default:
				printf("Unknown option: %s\n", argv[i]);
				ShowUsage();
				break;
		}
		
	}

	if (!filename && !geom_filename) ShowUsage();

	if (filename) LoadFile(node_num, edge_num, edges, weights, filename, max_cost);

	if (!geom_filename)
	{
		PerfectMatching *pm = new PerfectMatching(node_num, edge_num);
		for (e=0; e<edge_num; e++) pm->AddEdge(edges[2*e], edges[2*e+1], weights[e]);
		pm->options = options;
		pm->Solve();
		if (check_perfect_matching)
		{
			int res = CheckPerfectMatchingOptimality(node_num, edge_num, edges, weights, pm);
			printf("check optimality: res=%d (%s)\n", res, (res==0) ? "ok" : ((res==1) ? "error" : "fatal error"));
		}
		double cost = ComputePerfectMatchingCost(node_num, edge_num, edges, weights, pm);
		cost = max_cost ? -cost : cost;
		printf("cost = %.1f\n", cost);
		if (save_filename) SaveMatching(node_num, pm, save_filename, cost);
		delete pm;
	}

	if (filename)
	{
		delete [] edges;
		delete [] weights;
	}

	return 0;
}


