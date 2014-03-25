#pragma once

#include <Windows.h>

#include <map>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stdint.h>

namespace Demo
{
	struct Graph
	{
		template <typename _TyVe>
		static void ReleaseVector(::std::vector<_TyVe*> &vector)
		{
			for (auto &item : vector) { delete item; item = NULL; }
			vector.clear();
		}

		typedef ::HRESULT HResult;
		typedef ::FILE *HFile;

		typedef ::BOOL Bool;
		typedef ::LPSTR String;
		typedef CHAR Char;

		typedef struct Node
		{
			// node unique id for graph (aka local id)
			typedef uint32_t Luid; 

			// simple node collection
			typedef ::std::vector<Node *> Vector; 

			// node map (there can be only one connection, so i`ll let 'std' to handle duplicates) 
			// + the traversal order is not relevant
			typedef ::std::unordered_map<Luid, Node *> UnorderedMap, USMap; 

			Luid Id; // unique id
			USMap N; // graph neighbour nodes

			Node(Luid id) : Id(id), N() { }

			Graph::Bool HasNeighbourWith(Node::Luid id)
			{
				for (auto &pair : N)
				if (pair.first == id)
					return TRUE;
				return FALSE;
			}

		};

		typedef struct XTree
		{
			typedef struct Node
			{
				// simple node collection
				typedef ::std::unordered_map<Graph::Node::Luid, XTree::Node *> UnorderedMap, USMap;
				typedef ::std::multimap<Graph::Node::Luid, XTree::Node *> OMMap;
				typedef ::std::vector<XTree::Node *> Vector;

				XTree::Node::USMap N; // tree neighbour nodes
				XTree::Node::USMap Lp; // tree neighbour node loops

				Graph::Node *Rf = NULL; // graph node reference
				XTree::Node *Bp = NULL; // backpointer
				int8_t Sg = +1; // sign

				Graph::Bool PathContainsNodeWith(
					Graph::Node::Luid nodeId, 
					XTree::Node **node = NULL
					)
				{
					if (Rf->Id == nodeId)
					{
						if (node) (*node) = this;
						return TRUE;
					}
					else
					{
						if (Bp)
						{
							return Bp->PathContainsNodeWith(
								nodeId, 
								node
								);
						}
						else
						{
							if (node) (*node) = NULL;
							return FALSE;
						}
					}
				}

				void CountOfNodesWith(uint16_t &count, int8_t sg = +1)
				{
					if (Sg == sg) count++;
					if (Bp) return Bp->CountOfNodesWith(count, sg);
					else return;
				}

				Graph::Bool IsSameAs(XTree::Node *other, int8_t sg = +1)
				{
					XTree::Node *thisNext = this;
					XTree::Node *otherNext = other;
					Graph::Bool close = FALSE;

					while (thisNext->Bp || otherNext->Bp)
					{
						if (thisNext->Sg == sg &&
							otherNext->Sg == sg)
						{
							if (thisNext->Rf->Id == otherNext->Rf->Id)
							{
								if (thisNext->Bp) thisNext = thisNext->Bp;
								if (otherNext->Bp) otherNext = otherNext->Bp;
								continue;
							}
							else
							{
								return FALSE;
							}
						}

						if (thisNext->Bp && 
							thisNext->Sg != sg) 
							thisNext = thisNext->Bp;

						if (otherNext->Bp && 
							otherNext->Sg != sg) 
							otherNext = otherNext->Bp;

					}

					return TRUE;
				}

			};

			XTree::Node::Vector Nodes; // tree nodes
			XTree::Node *R = NULL; // root

			XTree::Node *AllocNew()
			{
				XTree::Node *node = new XTree::Node();
				Nodes.push_back(node);
				return node;
			}

			XTree() { }
			~XTree() { ReleaseVector(Nodes); }

		};

		typedef ::std::map<uint16_t, uint16_t> _Index2LuidMap;
		typedef ::std::map<uint16_t, XTree::Node *> _Luid2NodeMap;
		typedef ::std::multimap<uint16_t, _Index2LuidMap *> StableSetMap;


		Graph::Node::Vector Nodes; // graph nodes
		Graph::XTree::Node::OMMap Leafs;
		Graph::XTree Tree1;
		Graph::XTree Tree2;
		StableSetMap Ssm;


		Graph() { }
		~Graph() { ReleaseVector(Nodes); }

		Graph::HResult ReadFromFile(
			Graph::String fileName = "graph.txt",
			Graph::Bool oriented = FALSE
			)
		{
			Graph::HFile fp;

			if (!fileName)
				fileName = "graph.txt";
			if (fopen_s(&fp, fileName, "r"))
				return E_INVALIDARG;

			uint16_t const lineSize = 1023;
			Graph::Char line[lineSize + 1] = { 0 };

			uint16_t luid = 0;
			int32_t edgeCount, nodeCount;

			if (fgets(line, lineSize, fp))
			{
				if (sscanf_s(line, "%d %d\n", &nodeCount, &edgeCount) != 2) 
					return E_INVALIDARG;
			}

			Nodes.clear();
			Nodes.resize(nodeCount);
			{
				for (auto &n : Nodes)
					n = new Node(luid++);
			}

			luid = 0;
			while (fgets(line, lineSize, fp))
			{
				int32_t i, j;

				if (//fgets(line, lineSize, fp) && 
					sscanf_s(line, "%d %d\n", &i, &j) == 2)
				{
					Nodes[j]->N[i] = Nodes[i];
					Nodes[i]->N[j] = Nodes[j];

					sprintf_s(line, "%d to %d\n", i, j);
					OutputDebugStringA(line);

				}

				luid++;
			}

			errno_t error = fclose(fp);
			return S_OK;
		}

		void BuildTrees()
		{
			Graph::HResult result = S_OK;

			result = BuildTree(&Tree1, +1);
			result = BuildTree(&Tree2, -1);
		}

		Graph::HResult BuildTree(XTree *tree, int8_t sg = +1)
		{
			XTree::Node *node = tree->AllocNew();
			tree->R = node;

			node->Rf = Nodes.front();
			node->Bp = nullptr;
			node->Sg = sg;

			return BuildTreeNode(node, tree);
		}

		void PrintStableSets()
		{
			static uint16_t const lineSize = 1023;
			static Graph::Char line[lineSize + 1] = { 0 };

			for (auto &ss : Ssm)
			{
				for (auto &sspair : (*ss.second))
				{
					sprintf_s(line, "< %2d ",
						sspair.second//, n->Sg < 0 ? '-' : '+'
						);
					OutputDebugStringA(
						line
						);

				}

				OutputDebugStringA("\n");
			}
		}

		void GenerateStableSets()
		{
			uint16_t best;
			auto i = Leafs.rbegin();
			auto end = Leafs.rend();

			best = (*i).first;

			for (; i != end; i++)
			//for (; i != end && (*i).first == best; i++)
			{
				GenerateStableSets((*i).second);
			}
		}

	private:

#define __Demo_graph_Verbose 0

		void PadTo(std::string &str, const size_t num, const char paddingChar = ' ')
		{
#if __Demo_graph_Verbose
			str.insert(0, num - str.size(), paddingChar);
#endif
		}

		Graph::HResult BuildTreeNode(XTree::Node *node, XTree *tree, Graph::Bool recursive = TRUE, uint16_t depth = 0)
		{
#if __Demo_graph_Verbose
			static uint16_t const lineSize = 1023;
			static Graph::Char line[lineSize + 1] = { 0 };
			::std::string offset; PadTo(offset, depth);
#endif

			XTree::Node *loop;
			Graph::HResult result = S_OK;

			if (!node->Bp) for (auto &pair : node->Rf->N)
			{
				XTree::Node *neighbour = tree->AllocNew();
				neighbour->Rf = pair.second;
				neighbour->Bp = node;

				node->N[pair.first] = neighbour;
				neighbour->Sg = -node->Sg;

#if __Demo_graph_Verbose
				sprintf_s(line, "%s tree: %d (%c) to %d (%c)\n",
					offset.c_str(),
					node->Rf->Id, node->Sg < 0 ? '-' : '+',
					neighbour->Rf->Id, neighbour->Sg < 0 ? '-' : '+'
					);
				OutputDebugStringA(line);
#endif

				if (recursive)
				{
					result = BuildTreeNode(
						neighbour, tree, recursive, 
						depth + 1
						);
				}
			}
			else
			{
				for(auto &pair : node->Rf->N) 
				{
					if (node->Rf->Id != pair.first)
					{
						if (node->PathContainsNodeWith(pair.first, &loop))
						{
							node->Lp[loop->Rf->Id] = loop;

#if __Demo_graph_Verbose
							sprintf_s(line, "%s tree: loop %d (%c) to %d\n",
								offset.c_str(), 
								node->Rf->Id, node->Sg < 0 ? '-' : '+',
								pair.first
								);
							OutputDebugStringA(
								line
								);
#endif

#if 0

							Bool add = TRUE;

							uint16_t posCount = 0;
							node->CountOfNodesWith(posCount);

							if (Leafs.size())
							{
								auto leafsIt = Leafs.rbegin();

								if (add = posCount >= (*leafsIt).first)
								{
									for (; leafsIt != Leafs.rend(); leafsIt++)
									{
										if ((*leafsIt).first < posCount) break; else
										{
											if (node->IsSameAs(
												(*leafsIt).second
												))
											{
												add = FALSE;
												break;
											}
										}
									}
								}

								/*add = !node->IsSameAs(
									(*leafsIt).second
									);*/
							}

							if (add)
								Leafs.insert(std::make_pair(
									posCount, node
									));

#endif

							// add only those that reached the root
							//if (loop->Rf->Id == tree->R->Rf->Id)
								Leafs.insert(std::make_pair(
									depth, node
									));

							continue;
						}

						XTree::Node *neighbour = tree->AllocNew();
						neighbour->Rf = pair.second;
						neighbour->Bp = node;

						node->N[pair.first] = neighbour;
						neighbour->Sg = -node->Sg;

						for (auto &childPair : pair.second->N)
						{
							if (childPair.first != node->Rf->Id)
							{
								if (node->PathContainsNodeWith(childPair.first, &loop))
								{
									if (loop->Sg > 0) { neighbour->Sg = -1; break; }
									//if (loop->Sg > 0) { neighbour->Sg = -1; }
								}
							}
						}

#if __Demo_graph_Verbose
						sprintf_s(line, "%s tree: %d (%c) to %d (%c)\n",
							offset.c_str(), 
							node->Rf->Id, node->Sg < 0 ? '-' : '+',
							neighbour->Rf->Id, neighbour->Sg < 0 ? '-' : '+'
							);
						OutputDebugStringA(
							line
							);
#endif

						if (recursive)
						{
							result = BuildTreeNode(
								neighbour, tree, recursive,
								depth + 1
								);
						}
					}
				}
			}

			return S_OK;
		}

		void TraverseBraches(
			XTree::Node *cn, XTree::Node *pn, uint16_t &index, 
			_Index2LuidMap &ss, _Luid2NodeMap &ts
			)
		{
			if (!pn) return;
			XTree::Node *b = NULL;

			for (auto &branchPair : cn->N)
			{
				if (branchPair.first != pn->Rf->Id && (!cn->Bp || 
					branchPair.first != cn->Bp->Rf->Id))
				{
					b = branchPair.second;
					if (ts[b->Rf->Id]) break;

					/*
					//if (ts[b->Rf->Id]) continue;

					Bool foundss = FALSE;
					for (auto &childPair : b->N)
					{
						if (childPair.second->Sg == +1)
						{
							foundss = TRUE;
							break;
						}
					}

					if (foundss)
						continue;
						//break;
						*/

					ts[b->Rf->Id] = b;
					if (b->Sg == +1)
					{
						ss[index] = b->Rf->Id;
						index++;
					}

					TraverseBraches(
						b, cn, index, 
						ss, ts
						);
				}
			}
		}

		Bool AreEqual(_Index2LuidMap *ss1, _Index2LuidMap *ss2)
		{
			if (ss1->size() != ss2->size()) 
				return FALSE;

			auto ss1i = ss1->begin();
			for (; ss1i != ss1->end(); ss1i++)
			{
				Bool found = FALSE;
				uint16_t ss1id = (*ss1i).second;

				auto ss2i = ss2->begin();
				for (; ss2i != ss2->end(); ss2i++)
				{
					if (ss1id == (*ss2i).second)
					{
						found = TRUE;
						break;
					}
				}

				if (!found) 
					return FALSE;
			}

			return TRUE;
		}

		void AddSSToCollection(_Index2LuidMap *ss)
		{
			for (auto &ssi : Ssm)
			{
				if (AreEqual(ss, ssi.second)) 
					return;
			}

			Ssm.insert(std::make_pair(
				ss->size(), ss
				));
		}

		void PrintAllStableSets()
		{

		}

		void GenerateStableSets(XTree::Node *node)
		{
			_Index2LuidMap *pss = new _Index2LuidMap();
			_Luid2NodeMap *pts = new _Luid2NodeMap();

			_Index2LuidMap &stableSet = *pss;
			_Luid2NodeMap &traversedSet = *pts;

			XTree::Node *n = node;
			XTree::Node *pn = NULL;

			uint16_t index = 0;

			do
			{
				traversedSet[n->Rf->Id] = n;

				if (n->Sg == +1)
				{
					stableSet[index] = n->Rf->Id;
					index++;
				}

				/*TraverseBraches(
					n, pn, index, 
					stableSet, 
					traversedSet
					);*/

				pn = n;
				n = n->Bp;

			} while (n != NULL);

			AddSSToCollection(pss);
			delete pts;
		}

#if 0
		void PrintStableSets(XTree::Node *node)
		{
			static uint16_t const lineSize = 1023;
			static Graph::Char line[lineSize + 1] = { 0 };

			XTree::Node *n = node;

			uint16_t index = 0;

			do
			{
				if (n->Sg == +1)
				{
					sprintf_s(line, "< %2d ",
						n->Rf->Id//, n->Sg < 0 ? '-' : '+'
						);
					OutputDebugStringA(
						line
						);
				}

				n = n->Bp;
				index++;

			} while (n != NULL);

			OutputDebugStringA("\n");
		}
#endif

	};
}
