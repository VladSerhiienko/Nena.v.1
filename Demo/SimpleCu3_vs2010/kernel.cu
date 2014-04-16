
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
	typedef ::cudaError error;
	typedef ::cudaMemcpyKind memcpykind;
	typedef ::cudaDeviceProp deviceprop;

	static const error success = ::nena::cuda::error::cudaSuccess;
	static const memcpykind h2d = ::nena::cuda::memcpykind::cudaMemcpyHostToDevice;
	static const memcpykind d2h = ::nena::cuda::memcpykind::cudaMemcpyDeviceToHost;
}};

__global__ void kernel_vector_addition(
	_In_	float1 const *a, // vector a
	_In_	float1 const *b, // vector b
	_Out_	float1 *c, // vector c
	_In_	uint1 n // vector size
	)
{
	uint1 idx; 
	idx.x = blockDim.x * blockIdx.x + threadIdx.x;
	if (idx.x < n.x) c[idx.x].x = a[idx.x].x + b[idx.x].x;
}

int main(int argc, char *argv [])
{
	::uint1 n, ii;
	::size_t bytes;

	float1 
		*host_a = nullptr,		// vector a (cpu)
		*host_b = nullptr,		// vector b (cpu) 
		*host_c = nullptr,		// vector c (cpu)
		*device_a = nullptr,	// vector a (gpu)
		*device_b = nullptr,	// vector b (gpu)
		*device_c = nullptr;	// vector c (gpu)

	n.x = 100u; // vectors size
	bytes = n.x * sizeof(float1); // byte size

	host_a = (float1 *) ::malloc(bytes);
	host_b = (float1 *) ::malloc(bytes);
	host_c = (float1 *) ::malloc(bytes);
	::memset(host_a, 0, bytes);
	::memset(host_b, 0, bytes);
	::memset(host_c, 0, bytes);

	ii.x = 0;
	for (; ii.x < n.x; ii.x++)
		host_a[ii.x].x = (float) ::rand() / (float) RAND_MAX,
		host_b[ii.x].x = (float) ::rand() / (float) RAND_MAX,
		host_c[ii.x].x = (float) ::rand() / (float) RAND_MAX;

	::nena::cuda::error cs = ::nena::cuda::success;
	::nena::cuda::memcpykind h2d = ::nena::cuda::h2d;
	::nena::cuda::memcpykind d2h = ::nena::cuda::d2h;

	int deviceCount = 0;
	cs = ::cudaGetDeviceCount(&deviceCount);
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;

	nena::cuda::deviceprop deviceProp;
	decltype(deviceCount) device = 0;
	for (; device < deviceCount; device++)
	{
		::cudaGetDeviceProperties(
			&deviceProp, 
			device
			);
		::printf_s("cuda device %d: %d.%d\n", 
			device, 
			deviceProp.major, 
			deviceProp.minor
			);
	}

    cs = ::cudaSetDevice(0);
	if (cs != ::nena::cuda::success)
		return 1;
	
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

	uint3 block_size = { 1u, 1u, 1u };
	uint3 block_dims = { 1u, 1u, 1u };

	block_size.x = 16;

	block_dims.x = n.x;
	block_dims.x += block_size.x;
	block_dims.x -= 1;
	block_dims.x /= block_size.x;

	kernel_vector_addition<<<block_dims, block_size>>>(
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

	::cudaFree(device_c);
	::cudaFree(device_b);
	::cudaFree(device_a);

    cs = cudaDeviceReset();
	if (cs != ::nena::cuda::success) 
		return EXIT_FAILURE;

	ii.x = 0;
	::printf_s("<results>\n");
	for (; ii.x < n.x; ii.x++)
		::printf_s("  [%2u] %f + %f -> %f\n", ii.x,
		host_a[ii.x].x, host_b[ii.x].x, host_c[ii.x].x);
	::printf_s("</results>\n");
	
	::free(host_c);
	::free(host_b);
	::free(host_a);

	::printf_s("press any key to exit\n");
	::getchar();
    return EXIT_SUCCESS;
}

// enable warning:
// nonstandard extension used 
#pragma warning(default: 4482)
