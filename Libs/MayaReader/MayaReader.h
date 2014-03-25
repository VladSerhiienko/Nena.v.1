#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <Windows.h>
#include <rapidxml.hpp>

#include <Nena\Math.h>

namespace Nena
{
	namespace Utility
	{
		::BOOL HasEnding(std::string const &fullString, std::string const &ending);

		struct MayaReader
		{
			typedef ::rapidxml::xml_document<> XMLDocument;
			typedef ::rapidxml::xml_node<> XMLNode;

			struct Data
			{
				template <typename _Ptrty>
				static void ReleasePointer(_Ptrty *h)
				{
					if (h) 
					{ 
						delete h; 
						h = nullptr; 
					}
				}

				template <typename _Ptrty>
				static void ReleaseArrayPointer(_Ptrty *h)
				{
					if (h) 
					{ 
						delete[] h; 
						h = nullptr; 
					}
				}

				template <typename _Ptrty>
				static void ReleaseVector(::std::vector <_Ptrty*> &h)
				{
					for (::UINT32 i = 0; i < h.size(); i++)
					{
						ReleasePointer(h.at(i));
					}
				}

				template <typename _Ptrty>
				static void ReleaseVector(::std::vector <_Ptrty*> *&h)
				{
					if (h) for (::UINT32 i = 0; i < h->size(); i++)
					{
						ReleasePointer(h->at(i));
					}
				}

				struct Node;
				typedef struct IItem
				{
					friend Node;
					friend MayaReader;

					enum Type
					{
						kNode,
						kSkin,
						kShape,
						kTransform,
						kSkinCluster,
						kAnimationCurve,
						kInvalid,
					};

					inline ::std::string &GetNameReference() { return m_name; }
					inline ::std::string  GetNameCopy() { return m_name; }
					inline ::std::string *GetName() { return &m_name; }
					inline IItem::Type GetType() { return m_type; }

					IItem() : m_type(kInvalid), m_name("") { }
					virtual ~IItem() { }

				protected:

					std::string m_name;
					Type m_type;

				} *Handle;

				struct Node : public IItem
				{
					friend IItem;
					friend MayaReader;

					inline ::std::vector<Handle> &GetHandlesReference() { return m_handles; }
					inline ::std::vector<Node *> &GetAdultsReference() { return m_adults; }
					inline ::std::vector<Node *> &GetKidsReference() { return m_kids; }
					inline ::std::vector<Handle> *GetHandles() { return &m_handles; }
					inline ::std::vector<Node *> *GetAdults() { return &m_adults; }
					inline ::std::vector<Node *> *GetKids() { return &m_kids; }

					inline Handle GetHandle(::std::string name)
					{
						for (auto &n : m_handles) if (n->GetNameReference() == name) return n;
						else return nullptr;
					}

					inline Handle GetHandle(::std::string name, IItem::Type type)
					{
						for (auto &n : m_handles) if (n->GetNameReference() == name && type == n->m_type) return n;
						else return nullptr;
					}

					inline Handle GetHandle(::UINT32 index)
					{
						if (index < m_handles.size()) return m_handles.at(index);
						else return nullptr;
					}

					inline Node *GetAdultNode(::std::string name, ::BOOL returnNew = FALSE)
					{
						for (auto &n : m_adults) if (n->GetNameReference() == name) return n;
						if (returnNew) { auto n = new Node(); m_adults.push_back(n); n->m_name = name; return n; }
						else return nullptr;
					}

					inline Node *GetKidNode(::std::string name, ::BOOL returnNew = FALSE)
					{ 
						for (auto &n : m_kids) if (n->GetNameReference() == name) return n;
						if (returnNew) { auto n = new Node(); m_kids.push_back(n); n->m_name = name; return n; }
						else return nullptr;
					}

					Node() : IItem() { m_handles.clear(); m_adults.clear(); m_kids.clear(); }
					virtual ~Node() { }

				protected:

					::std::vector<Handle> m_handles;
					::std::vector<Node *> m_adults;
					::std::vector<Node *> m_kids;
				};

				struct Shape : public IItem
				{
					struct Polygon
					{
						struct Vertex
						{
							struct Tex
							{
								::std::string UVSet;
								::UINT32 UVIndex;

								Tex() : UVIndex(0)
								{
								}
							};

							::UINT32 PointIndex;
							::UINT32 NormalIndex;
							::UINT32 TexcoordCount;
							::UINT32 BinormalIndex;
							::UINT32 TangentIndex;
							Tex *Texcoords;

							virtual ~Vertex() 
							{ 
								ReleaseArrayPointer(Texcoords); 
							}
							Vertex() : TexcoordCount(0), Texcoords(0),
								PointIndex(0), NormalIndex(0)
							{
							}
						};

						::UINT32 VertexCount;
						Vertex *Vertices;

						virtual ~Polygon() { ReleaseArrayPointer(Vertices); }
						Polygon() : VertexCount(0), Vertices(0)  { }
					};

					struct UVSet
					{
						typedef std::vector<UVSet *> Vector;

						::std::string Name;
						::std::vector<::Nena::Vector2> UVs;
						::std::vector<::Nena::Vector3> BNs;
						::std::vector<::Nena::Vector3> TNs;

						virtual ~UVSet()
						{ 
							//OutputDebugStringA("~UVSet()\n"); 
							UVs.clear(); BNs.clear(); TNs.clear(); 
						}
						UVSet() 
						{ 
							//OutputDebugStringA("UVSet()\n"); 
							UVs.clear(); BNs.clear(); TNs.clear(); 
						}
					};

					struct VectorsInSpace
					{
						::Nena::Vector3 *Vectors;
						::std::string Space;
						::UINT32 Count;

						virtual ~VectorsInSpace()  { ReleaseArrayPointer(Vectors); }
						VectorsInSpace() : Vectors(0), Count(0) { }
					};

					typedef std::vector<VectorsInSpace *> VectorsInSpaceCollection;

					::std::vector<Polygon *> Polygons;

					VectorsInSpaceCollection *Points;
					VectorsInSpaceCollection *Normals;

					UVSet::Vector Sets;

					virtual ~Shape()
					{
						//OutputDebugStringA("~Shape()\n");

						ReleaseVector(Points);
						Points->clear(); 
						ReleasePointer(Points);

						ReleaseVector(Normals); 
						Normals->clear(); 
						ReleasePointer(Normals);

						ReleaseVector(Polygons);
						ReleaseVector(Sets);
					}

					Shape() : Points(0), Normals(0)
					{
						//OutputDebugStringA("Shape()\n");
					}
				};

				struct Transform : public IItem
				{
					struct Curve : public IItem
					{
						typedef std::vector<Curve *> Vector;

						struct Key
						{
							::FLOAT Time;
							::FLOAT Value;
							::Nena::Vector2 In;
							::Nena::Vector2 Out;
							::UINT32 InTangent;
							::UINT32 OutTangent;

							Key() : Time(0), Value(0), InTangent(-1), OutTangent(-1)
							{
							}
						}; 

						::UINT32 Type = -1;
						::UINT32 KeyCount;
						Key *Keys;

						virtual ~Curve() 
						{ 
							//OutputDebugStringA("~Curve()\n"); 
							ReleaseArrayPointer(Keys); 
						}
						Curve() : Keys(NULL), KeyCount(0) 
						{
							//OutputDebugStringA("Curve()\n");
						}

					};

					::Nena::Vector3 Scale;

					::Nena::Vector3 Translate;
					::Nena::Vector3 TranslateTS;
					::Nena::Vector3 TranslateWS;

					::Nena::Vector3 RotateEA;
					::Nena::Quaternion RotateQ;
					::Nena::Quaternion RotateQTS;
					::Nena::Quaternion RotateQWS;
					::Nena::Quaternion RotateO;
					::Nena::Quaternion RotateOOS;
					::Nena::Quaternion RotateOTS;
					::Nena::Quaternion RotateOWS;

					Curve::Vector *Curves;

					::BOOL IsJoint = FALSE;

					virtual ~Transform()  
					{
						//OutputDebugStringA("~Transform()\n");

						if (Curves)
						{
							ReleaseVector(Curves);
						}
					}

					Transform() : Curves(NULL) 
					{
						//OutputDebugStringA("Transform()\n");
					}
				};

				struct SkinCluster : public IItem
				{
					struct Skin : public IItem
					{
						typedef std::vector<Skin *> Vector;

						struct Component
						{
							struct Influence
							{
								::FLOAT Weight;
								::std::string FullPath;
								::std::string PartialPath;

								Influence() : Weight(0) { }
							};

							::UINT32 InfluenceCount;
							Influence *Influences;

							virtual ~Component() 
							{ 
								//OutputDebugStringA("~Component()\n"); 
								ReleaseArrayPointer(Influences); 
							}
							Component() : Influences(0), InfluenceCount(0) 
							{ 
								//OutputDebugStringA("Component()\n"); 
							}
						};

						::UINT32 ComponentCount;
						Component *Components;

						virtual ~Skin() 
						{ 
							//OutputDebugStringA("~Skin()\n"); 
							ReleaseArrayPointer(Components);
						}
						Skin() : Components(0), ComponentCount(0)
						{ 
							//OutputDebugStringA("Skin()\n"); 
						}
					};

					Skin::Vector Geometries;

					virtual ~SkinCluster()  
					{ 
						/*Data::ReleaseVector(Geometries);*/ 
						Geometries.clear(); 
					}
					SkinCluster() 
					{ 
					}

				};

			};

			MayaReader() : Root(nullptr) 
			{ 
				//OutputDebugStringA("MayaReader()\n"); 
			}
			~MayaReader() 
			
			{ 
				//OutputDebugStringA("~MayaReader()\n"); 
				Clear(); 
			}

			::BOOL Import(::std::string file);
			void Clear();

			Data::Node *Root;

		protected:

			void ImportFamily(
				XMLNode *nodeNode,
				Data::Node *hostData
				);

			void ImportSkinCluster(
				XMLNode *clusterNode
				);

			void ImportCurves(
				XMLNode *curveNode,
				Data::Transform *transformData
				);
			void ImportTranslate(
				XMLNode *transformNode,
				Data::Transform *transformData
				);
			void ImportRotate(
				XMLNode *transformNode,
				Data::Transform *transformData
				);
			void ImportScale(
				XMLNode *transformNode,
				Data::Transform *transformData
				);
			void ImportTransform(
				XMLNode *shapeNode
				);

			void ImportShape(
				XMLNode *shapeNode
				);
			void ImportPolygons(
				XMLNode *shapeNode,
				Data::Shape *shapeData
				);
			void ImportUVSets(
				XMLNode *shapeNode,
				Data::Shape *shapeData
				);
			void ImportSpacedVectors(
				XMLNode *shapeNode, 
				Data::Shape::VectorsInSpaceCollection *&container, 
				::std::string const &collection, 
				::std::string const &item
				);

			static ::BOOL IsResultCheckedAt(::UINT32 const & result, ::UINT32 const &at);
			static void GetUIntAttribute(XMLNode *node, ::std::string const &name, ::UINT32 &attribute, ::UINT32 &result);
			static void GetFloatAttribute(XMLNode *node, ::std::string const &name, ::FLOAT &attribute, ::UINT32 &result);
			static void GetStringAttribute(XMLNode *node, ::std::string const &name, ::std::string &attribute, ::UINT32 &result);

			static void GetXYZWAttributes(XMLNode *node, ::FLOAT &x, ::FLOAT &y, ::FLOAT &z, ::FLOAT *w, ::UINT32 &result);
			static void GetUVAttributes(XMLNode *node, ::FLOAT &u, ::FLOAT &v, ::UINT32 &result);
			static void GetIndexCountAttributes(XMLNode *node, ::UINT32 &index, ::UINT32 &count, ::UINT32 &result);
			static void GetCountAttribute(XMLNode *node, ::UINT32 &count, ::UINT32 &result);
			static void GetIndexAttribute(XMLNode *node, ::UINT32 &index, ::UINT32 &result);
			static void GetSpaceAttribute(XMLNode *node, ::std::string &space, ::UINT32 &result);
			static void GetNameAttribute(XMLNode *node, ::std::string &name, ::UINT32 &result);
			static void GetAdultCountAttribute(XMLNode *node, ::std::string &name, ::UINT32 &count, ::UINT32 &result);
			static void GetKidCountAttribute(XMLNode *node, ::std::string &name, ::UINT32 &count, ::UINT32 &result);
			static void GetAdultsNode(XMLNode *node, XMLNode *&adults);
			static void GetKidsNode(XMLNode *node, XMLNode *&kids);

			void ImportShapes();
			void ImportTransforms();
			void ImportSkinClusters();

			XMLNode *m_dataNode;
			XMLDocument m_document;
		};
	}
}