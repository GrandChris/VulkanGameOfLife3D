///////////////////////////////////////////////////////////////////////////////
// File: test.cpp
// Date: 13.10.2019
// Version: 1
// Author: Christian Steinbrecher
// Description: Tests VulkanErruption
///////////////////////////////////////////////////////////////////////////////

#include "GameOfLife3D.h"

#include "ParticleRenderer.h"
#include "DynamicPointRenderObject.h"

#include <iostream>
#include <thread>
#include <limits>

using namespace std;


glm::vec3 colorGradient(TElem const val)
{
	glm::vec3 const colors[] = {
		{ 1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		//{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f }
	};
	constexpr size_t const size = std::size(colors);
	constexpr TElem const valsPerColor = static_cast<TElem>(std::numeric_limits<TElem>::max() / (size-1));
	constexpr float const valsPerColorInv = 1.0f / valsPerColor;
	
	size_t const index = static_cast<size_t>(val * valsPerColorInv);

	auto ratio = valsPerColorInv * (val -  valsPerColor * index);
	auto invRatio = 1.0f - ratio;

	return invRatio * colors[index] + ratio * colors[index + 1];

}


void createVertices(TElem const field[N][N][N], std::vector<DynamicPointRenderObject::Vertex> & fieldVertices)
{
	if (fieldVertices.size() < N * N * N)
	{
		return;
	}

	size_t i = 0;
	for (size_t z = 0; z < N; ++z)
	{
		for (size_t y = 0; y < N; ++y)
		{
			for (size_t x = 0; x < N; ++x)
			{
				if (field[z][y][x])
				{
					auto const val = std::min(sqrt(static_cast<double>(field[z][y][x])*300), 255.0);
					auto const val1 = static_cast<TElem>(val);
					//float const b = std::min(1.0f, 1.0f / 255.0f * val * 10);
					//float const r = 1.0f - std::min(1.0f, 1.0f / 255.0f * val * 10);

					//float const r = 0.5f / N * x + 0.5f;
					//float const g = 0.5f / N * y + 0.5f;
					//float const b = 0.5f / N * z + 0.5f;

					fieldVertices[i++] = { {x, y, z}, colorGradient(val1) };
				}
				else
				{
					fieldVertices[i++] = { {999,999,999}, {1.0f, 1.0f, 1.0f} };
				}
			}
		}
	}
}


size_t maxIndex(TElem field[N][N][N])
{
	size_t max = 0;

	for (size_t z = 0; z < N; ++z)
	{
		for (size_t y = 0; y < N; ++y)
		{
			for (size_t x = 0; x < N; ++x)
			{
				if (field[z][y][x])
				{

					int xi = static_cast<int>(x) - N / 2;
					int yi = static_cast<int>(y) - N / 2;
					int zi = static_cast<int>(z) - N / 2;

					if (std::abs(xi) > max)
					{
						max = std::abs(xi);
					}
					if (std::abs(yi) > max)
					{
						max = std::abs(yi);
					}
					if (std::abs(zi) > max)
					{
						max = std::abs(zi);
					}
				}
			}
		}
	}

	return max;
}

TElem field1[N][N][N] = { 0 };
TElem field2[N][N][N] = { 0 };
std::vector<DynamicPointRenderObject::Vertex> fieldVertices1(N* N* N);
std::vector<DynamicPointRenderObject::Vertex> fieldVertices2(N* N* N);

int main()
{ 
	auto current = field1;
	auto next = field2;

	// init
	field1[N / 2][N / 2][N / 2] = 1;

	field1[N / 2+1][N / 2][N / 2] = 1;
	field1[N / 2][N / 2+1][N / 2] = 1;
	field1[N / 2][N / 2][N / 2+1] = 1;

	field1[N / 2 - 1][N / 2][N / 2] = 1;
	field1[N / 2][N / 2 - 1][N / 2] = 1;
	field1[N / 2][N / 2][N / 2 - 1] = 1;

	bool end = false;
	size_t maxN;
	auto physik = [&]()
		{
			GameOfLife(current, next);
			auto tmp = current;
			current = next;
			next = tmp;
			//maxN = maxIndex(current);
			maxN = N / 2;
			createVertices(current, fieldVertices1);
		};


	auto app = ParticleRenderer::createVulkan();
	auto obj = DynamicPointRenderObject::createVulkan();

	
	auto render = [&]() -> std::vector<DynamicPointRenderObject::Vertex>
		{	
			static float count = 0;
			count += 0.01f;

			float const x = N / 2 +  (3*maxN+10) * sin(count);
			float const y = N / 2 +  (3*maxN + 10) * cos(count);
			float const z = N / 2 +  (3*maxN+10)/2 + ( maxN/2) * sin(count*0.7);;

			app->setView({ x, y, z }, { N / 2, N / 2, N / 2 });

			return fieldVertices1;
		};

	obj->setVertices(render, fieldVertices1.size());
	obj->setPosition({ 0.0f, 0.0f, 0.0f });
	obj->setUseCubes(true);
	app->add(std::move(obj));
	app->setView({ N, N, N }, { N / 2, N / 2, N / 2 });


	thread phyisk = thread([&]()
		{
			while (!end)
			{
				physik();
			}
		});

	app->run();

	end = true;
	phyisk.join();
}

