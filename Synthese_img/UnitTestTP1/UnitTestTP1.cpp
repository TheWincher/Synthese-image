#include "pch.h"
#include "CppUnitTest.h"
#include "..//Portable_anymap/Vector3.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestTP1
{
	TEST_CLASS(OperatorTests)
	{
	public:
		
		TEST_METHOD(operatorPlus)
		{
			Vector3<int> v(0, 0, 0);
			Vector3<int> v2(1, 1, 1);
			Vector3<int> res = v + v2;

			Assert::AreEqual(v.x + v2.x, res.x);
			Assert::AreEqual(v.y + v2.y, res.y);
			Assert::AreEqual(v.z + v2.z, res.z);
		}

		TEST_METHOD(operatorMoins)
		{
			Vector3<int> v(0, 0, 0);
			Vector3<int> v2(1, 1, 1);
			Vector3<int> res = v - v2;

			Assert::AreEqual(v.x - v2.x, res.x);
			Assert::AreEqual(v.y - v2.y, res.y);
			Assert::AreEqual(v.z - v2.z, res.z);
		}

		TEST_METHOD(operatorMult)
		{
			Vector3<int> v(3, 8, 15);
			Vector3<int> v2(6, 4, 7);
			Vector3<int> res = v * v2;

			Assert::AreEqual(v.x * v2.x, res.x);
			Assert::AreEqual(v.y * v2.y, res.y);
			Assert::AreEqual(v.z * v2.z, res.z);
		}

		TEST_METHOD(operatorMultScarlar)
		{
			Vector3<int> v(3, 8, 10);
			int scalar = 5;
			Vector3<int> res = v * scalar;

			Assert::AreEqual(v.x * scalar, res.x);
			Assert::AreEqual(v.y * scalar, res.y);
			Assert::AreEqual(v.z * scalar, res.z);

			res = scalar * v;

			Assert::AreEqual(v.x * scalar, res.x);
			Assert::AreEqual(v.y * scalar, res.y);
			Assert::AreEqual(v.z * scalar, res.z);
		}

		TEST_METHOD(operatorDivise)
		{
			Vector3<int> v(3, 8, 15);
			Vector3<int> v2(6, 4, 7);
			Vector3<int> res = v / v2;

			Assert::AreEqual(v.x / v2.x, res.x);
			Assert::AreEqual(v.y / v2.y, res.y);
			Assert::AreEqual(v.z / v2.z, res.z);
		}

		TEST_METHOD(operatorDiviseScarlar)
		{
			Vector3<int> v(3, 8, 10);
			int scalar = 5;
			Vector3<int> res = v / scalar;

			Assert::AreEqual(v.x / scalar, res.x);
			Assert::AreEqual(v.y / scalar, res.y);
			Assert::AreEqual(v.z / scalar, res.z);
		}

		TEST_METHOD(operatorEgale)
		{
			Vector3<int> v(3, 8, 15);
			Vector3<int> v2(3, 8, 15);

			Assert::IsTrue(v == v2);
		}

		TEST_METHOD(operatorDifferent)
		{
			Vector3<int> v(3, 8, 10);
			Vector3<int> v2(3, 8, 15);
			
			Assert::IsTrue(v != v2);
		}
	};

	TEST_CLASS(MethodTests) 
	{
	public :
		TEST_METHOD(MethodDot)
		{
			Vector3<float> v(3, 8, 10);
			Vector3<float> v2(3, 8, 15);
			float res = v.dot(v2);

			Assert::AreEqual(res, v.x * v2.x + v.y * v2.y + v.z * v2.z);
			
		}

		TEST_METHOD(MethodNorm)
		{
			Vector3<float> v(3, 8, 10);
			float res = v.norm();

			Assert::AreEqual(res, sqrt(v.x * v.x + v.y * v.y + v.z * v.z));

		}
	};
}
