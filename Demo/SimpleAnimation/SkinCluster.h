#pragma once

#include <vector>

#include "Skeleton.h"

namespace Demo
{
	struct SkinCluster
	{
		struct Influence
		{
			typedef ::std::vector<Influence*> Vector;
			typedef Vector::iterator Iterator;
			typedef float SINGLE32, FLOAT32, FLOAT;

			Demo::SkeletonJoint *Joint = nullptr;
			Influence::FLOAT32 Weight = 0.00f;
		};

		struct Component
		{
			typedef ::std::vector<Component *> Vector;
			typedef Vector::iterator Iterator;

			Influence::Vector Influences;

			~Component()
			{
				if (Influences.size())
				{
					for (auto p : Influences) { delete p; p = nullptr; }
					Influences.clear();
				}
			}

			Component() { }
		};

		Component::Vector Components;

		~SkinCluster() 
		{
			if (Components.size())
			{
				for (auto p : Components) { delete p; p = nullptr; }
				Components.clear();
			}
		}

		SkinCluster() { }
	};
}