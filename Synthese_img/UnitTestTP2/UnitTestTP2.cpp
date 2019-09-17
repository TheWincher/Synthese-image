#include "pch.h"
#include "CppUnitTest.h"
#include "../TP2/TP2.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestTP2
{
	TEST_CLASS(UnitTestTP2)
	{
	public:
		
		TEST_METHOD(TestIntersection)
		{
			sphere s = { Vector3<float>(5,5,0), 2 };
			rayon r = { Vector3<float>(0,0,0),Vector3<float>(1,1,0) };

			Assert::IsTrue(intersectionRayonSphere(s, r).has_value());
		}

		TEST_METHOD(TestNoIntersection)
		{
			sphere s = { Vector3<float>(5,5,0), 2 };
			rayon r = { Vector3<float>(0,0,0),Vector3<float>(1,0,0) };

			Assert::IsFalse(intersectionRayonSphere(s, r).has_value());
		}
	};
}
