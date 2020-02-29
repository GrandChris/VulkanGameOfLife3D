///////////////////////////////////////////////////////////////////////////////
// File: main.cpp
// Date: 29.02.2020
// Version: 1
// Author: Christian Steinbrecher
// Description: Runs Game of Life
///////////////////////////////////////////////////////////////////////////////

#include "GameOfLife3D.h"

#include "ParticleRenderer.h"
#include "DynamicPointRenderObject.h"
#include "Array3DShader.h"

#include <iostream>
#include <thread>
#include <limits>
#include <chrono>

using namespace std;


using ShaderType = Array3DShader<eShader::Points>;
using RenderObj = DynamicPointRenderObject<ShaderType>;
using Vertices = std::vector<RenderObj::Vertex>;


TElem field1[N][N][N] = { 0 };
TElem field2[N][N][N] = { 0 };
Vertices fieldVertices1(N* N* N);

void createVertices(TElem const field[N][N][N], Vertices& fieldVertices)
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
				fieldVertices[i++].color = field[z][y][x];
			}
		}
	}
}

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

	// physik
	bool end = false;
	size_t maxN = N/2;
	auto physik = [&]()
		{
			GameOfLife(current, next);
			auto tmp = current;
			current = next;
			next = tmp;
			createVertices(current, fieldVertices1);
		};


	// render
	createVertices(current, fieldVertices1);
	auto app = ParticleRenderer::createVulkan();
	auto obj = RenderObj::createVulkan();

	auto render = [&]() -> Vertices
		{	
			typedef std::chrono::high_resolution_clock Time;
			typedef std::chrono::duration<float> fsec;
			static auto t0 = Time::now();
			auto t1 = Time::now();
			fsec fs = t1 - t0;
			float count = fs.count() * 0.2f;

			//physik();

			float const x = N / 2 +  (3*maxN+10) * sin(count);
			float const y = N / 2 +  (3*maxN + 10) * cos(count);
			float const z = N / 2.0f +  (3.0f*maxN+10.0f)/2.0f + ( maxN/2.0f) * sin(count*0.7f);;

			app->setView({ x, y, z }, { N / 2, N / 2, N / 2 });

			return fieldVertices1;
		};

	obj->setVertices(render, fieldVertices1.size());
	obj->setPosition({ 0.0f, 0.0f, 0.0f });
	ShaderType::UniformBufferObject ubo;
	ubo.maxIndex = glm::uvec3(N, N, N);
	obj->setUbo(ubo);

	app->add(std::move(obj));

	// start
	float const x = N / 2.0f + (3.0f * maxN + 10.0f) * sin(0.0f);
	float const y = N / 2.0f + (3.0f * maxN + 10.0f) * cos(0.0f);
	float const z = N / 2.0f + (3.0f * maxN + 10.0f) / 2.0f + (maxN / 2.0f) * sin(0 * 0.7f);;
	app->setView({ x, y, z }, { N / 2, N / 2, N / 2 });
	app->setWindowSize(3440, 1440, false);

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
