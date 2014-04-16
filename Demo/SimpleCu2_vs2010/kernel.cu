
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <sal.h>

#include <random>

// suppress warning:
// nonstandard extension used 
#pragma warning(disable: 4482)

namespace nena { namespace cuda
{
	typedef cudaError error;
	typedef cudaMemcpyKind memcpykind;

	static const error success = 
		::nena::cuda::error::cudaSuccess;
}};

__global__ void kernel_vector_addition(
	_In_	float1 const *a, 
	_In_	float1 const *b, 
	_Out_	float1 *c, 
	_In_	uint1 n
	)
{
	uint1 idx; idx.x = threadIdx.x;
	c[idx.x].x = a[idx.x].x + b[idx.x].x;
}


int main(int argc, char *argv [])
{
	uint1 n, ii;

	n.x = 100u;
	float1 *host_a = new float1[n.x];
	float1 *host_b = new float1[n.x];
	float1 *host_c = new float1[n.x];

	ii.x = 0;
	for (; ii.x < n.x; ii.x++)
		host_a[ii.x].x = (float) ::rand() / (float) RAND_MAX,
		host_b[ii.x].x = (float) ::rand() / (float) RAND_MAX,
		host_c[ii.x].x = (float) ::rand() / (float) RAND_MAX;

	::nena::cuda::error cs = ::nena::cuda::success;

	int deviceCount = 0;
	cs = ::cudaGetDeviceCount(&deviceCount);
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;

	cudaDeviceProp deviceProps;
	decltype(deviceCount) device = 0;
	for (; device < deviceCount; device++)
	{
		::cudaGetDeviceProperties(
			&deviceProps, 
			device
			);
		::printf_s("cuda device %d: %d.%d\n", 
			device, 
			deviceProps.major, 
			deviceProps.minor
			);
	}

    cs = ::cudaSetDevice(0);
	if (cs != ::nena::cuda::success)
		return 1;
	
	float1 *device_a = nullptr;
	float1 *device_b = nullptr;
	float1 *device_c = nullptr;

	::size_t bytes = n.x * sizeof(float1);
	auto h2d =  ::cudaMemcpyKind::cudaMemcpyHostToDevice;
	auto d2h =  ::cudaMemcpyKind::cudaMemcpyDeviceToHost;
	
	cs = ::cudaMalloc((void**)&device_a, bytes);
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;
	cs = ::cudaMalloc((void**)&device_b, bytes);
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;
	cs = ::cudaMalloc((void**)&device_c, bytes);
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;

	cs = ::cudaMemcpy(device_a, host_a, bytes, h2d);
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;
	cs = ::cudaMemcpy(device_b, host_b, bytes, h2d);
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;


	kernel_vector_addition<<<1, n.x>>>(
		device_a, 
		device_b, 
		device_c, 
		n
		);

	cs = ::cudaGetLastError();
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;
	cs = ::cudaDeviceSynchronize();
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;
	cs = ::cudaMemcpy(host_c, device_c, bytes, d2h);
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;

	cudaFree(device_c);
	cudaFree(device_b);
	cudaFree(device_a);

    cs = cudaDeviceReset();
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;

	ii.x = 0;
	::printf_s("<results>\n");
	for (; ii.x < n.x; ii.x++)
		::printf_s("\t [%u] \t %f + %f -> %f\n", ii.x,
		host_a[ii.x].x, host_b[ii.x].x, host_c[ii.x].x);
	::printf_s("</results>\n");
	
	delete [] host_c;
	delete [] host_b;
	delete [] host_a;

	::printf_s("press any key to exit\n");
	::getchar();
    return EXIT_SUCCESS;
}

// enable warning:
// nonstandard extension used 
#pragma warning(default: 4482)
