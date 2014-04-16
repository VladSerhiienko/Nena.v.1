#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <iostream>

#include <Nena\Math.h>

#define MAX_THREADS 8
#define BUF_SIZE 255

DWORD WINAPI MyThreadFunction(LPVOID lpParam);
void ErrorHandler(LPTSTR lpszFunction);

// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
typedef struct MyData
{
	int val1;
	int val2;
} MYDATA, *PMYDATA;

class D
{
public:
	D()
	{
		std::cout << "D::D()" << std::endl;
	}
	~D()
	{
		std::cout << "D::~D()" << std::endl;
	}
};

class E
{
public:
	E()
	{
		std::cout << "E::E()" << std::endl;
	}
	~E()
	{
		std::cout << "E::~E()" << std::endl;
	}
};

class C
{
public:
	C()
	{
		std::cout << "C::C()" << std::endl;
	}
	~C()
	{
		std::cout << "C::~C()" << std::endl;
	}
};

class B
{
	C c1;
	D c2;
	D c3;
	C c4;

public:
	B()
	{
		std::cout << "B::B()" << std::endl;
		MethodA();
	}
	~B()
	{
		std::cout << "B::~B()" << std::endl;
		MethodA();
		MethodB();
	}

	B(int) : c1(), c4(), c2(), c3()
	{
		std::cout << "B::B(int)" << std::endl;
		MethodB();
	}
	virtual void MethodA()
	{
		std::cout << "B::MethodA" << std::endl;
	}
	virtual void MethodB()
	{
		std::cout << "B::MethodB" << std::endl;
	}
};

class A : public B, public C
{
public:
	A()
	{
		std::cout << "A::A()" << std::endl;
	}
	A(int) : C(), B(0)
	{
		std::cout << "A::A(int)" << std::endl;
	}
	~A()
	{
		std::cout << "A::~A()" << std::endl;
		MethodA();
	}
	virtual void MethodA()
	{
		std::cout << "A::MethodA" << std::endl;
		B::MethodA();
	}
};

struct MathData
{
	DirectX::XMMATRIX m1;
	DirectX::XMMATRIX m2;
	DirectX::XMVECTOR v1;
	double d1;
	double d2;
	float f1;
	short s1;
	short s2;
	char c1;
	char c2;
	char c3;
	char c4;
	char c5;
	int i1;
};

DECLSPEC_ALIGN(128) struct MathDataAligned
{
	DirectX::XMMATRIX m1;
	DirectX::XMMATRIX m2;
	DirectX::XMVECTOR v1;
	double d1;
	double d2;
	float f1;
	short s1;
	short s2;
	char c1;
	char c2;
	char c3;
	char c4;
	char c5;
	int i1;
};

template <typename T>
union HorribleCast
{
	T data;
	unsigned char bytes[sizeof T];
};

template <typename T>
void DumpMemory(T const &mem)
{
	char str[16];

	sprintf_s(str, "%3d:", sizeof mem),
		OutputDebugStringA(str);

	UCHAR *bytes = (UCHAR *) (void*) &mem;
	for (int i = 0; i < sizeof mem; i++) if (bytes[i] != 0)
		sprintf_s(str, "%02X|", bytes[i]), OutputDebugStringA(str);
	else OutputDebugStringA("**|");

	OutputDebugStringA("\n");
}

int _tmain()
{
	{
		A a(0);
		A aa;
		a.MethodA();
		std::cout << "--------------" << std::endl;
	}

	int i = 5;
	std::cout << std::endl << "1> " << i << std::endl;
	{
		i++;
		std::cout << "2> " << i << std::endl;
		int i = 15;
		std::cout << "3> " << i << std::endl;
		i++;
		std::cout << "4> " << i << std::endl;
	}

	std::cout << "5> " << i << std::endl;
	i++;
	std::cout << "6> " << i << std::endl;

	MathData md;
	MathDataAligned mda;

	ZeroMemory(&md, sizeof md);
	ZeroMemory(&mda, sizeof md);
	//ZeroMemory(&mda, sizeof mda);

	md.m1.r[0].m128_f32[0] = mda.m1.r[0].m128_f32[0] = 1;
	md.m1.r[0].m128_f32[3] = mda.m1.r[0].m128_f32[3] = 1;
	md.m1.r[3].m128_f32[0] = mda.m1.r[3].m128_f32[0] = 1;
	md.m1.r[3].m128_f32[3] = mda.m1.r[3].m128_f32[3] = 1;

	md.d1 = mda.d1 = 1;
	md.d2 = mda.d2 = 1;

	md.s1 = mda.s1 = USHRT_MAX;
	md.s2 = mda.s2 = USHRT_MAX;

	md.c1 = mda.c1 = 12;
	md.c2 = mda.c2 = 13;
	md.c3 = mda.c3 = 14;
	md.c4 = mda.c4 = 15;

	md.c5 = mda.c5 = 32;
	md.i1 = mda.i1 = 33;

	//*((&md.c4) + 1) = *((&mda.c4) + 1) = 16;
	//*((&md.c4) + 2) = *((&mda.c4) + 2) = 17;
	//*((&md.c4) + 3) = *((&mda.c4) + 3) = 18;

	DumpMemory(md);
	DumpMemory(mda);

	//PMYDATA pDataArray[MAX_THREADS];
	//DWORD dwThreadIdArray[MAX_THREADS];
	//HANDLE hThreadArray[MAX_THREADS];

	// Create MAX_THREADS worker threads.

	//for (int i = 0; i<MAX_THREADS; i++)
	//{
	//	// Allocate memory for thread data.

	//	pDataArray[i] = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
	//		sizeof(MYDATA));

	//	if (pDataArray[i] == NULL)
	//	{
	//		// If the array allocation fails, the system is out of memory
	//		// so there is no point in trying to print an error message.
	//		// Just terminate execution.
	//		ExitProcess(2);
	//	}

	//	// Generate unique data for each thread to work with.

	//	pDataArray[i]->val1 = i;
	//	pDataArray[i]->val2 = i + 100;

	//	// Create the thread to begin execution on its own.

	//	hThreadArray[i] = CreateThread(
	//		NULL,                   // default security attributes
	//		0,                      // use default stack size  
	//		MyThreadFunction,       // thread function name
	//		pDataArray[i],          // argument to thread function 
	//		0,                      // use default creation flags 
	//		&dwThreadIdArray[i]);   // returns the thread identifier 


	//	// Check the return value for success.
	//	// If CreateThread fails, terminate execution. 
	//	// This will automatically clean up threads and memory. 

	//	if (hThreadArray[i] == NULL)
	//	{
	//		ErrorHandler(TEXT("CreateThread"));
	//		ExitProcess(3);
	//	}
	//} // End of main thread creation loop.

	//// Wait until all threads have terminated.

	//WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

	//// Close all thread handles and free memory allocations.

	//for (int i = 0; i<MAX_THREADS; i++)
	//{
	//	CloseHandle(hThreadArray[i]);
	//	if (pDataArray[i] != NULL)
	//	{
	//		HeapFree(GetProcessHeap(), 0, pDataArray[i]);
	//		pDataArray[i] = NULL;    // Ensure address is not reused.
	//	}
	//}

	getchar();
	return 0;
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	HANDLE hStdout;
	PMYDATA pDataArray;

	TCHAR msgBuf[BUF_SIZE];
	size_t cchStringSize;
	DWORD dwChars;

	// Make sure there is a console to receive output results. 

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == INVALID_HANDLE_VALUE)
		return 1;

	// Cast the parameter to the correct data type.
	// The pointer is known to be valid because 
	// it was checked for NULL before the thread was created.

	pDataArray = (PMYDATA) lpParam;

	// Print the parameter values using thread-safe functions.

	StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Parameters = %d, %d\n"),
		pDataArray->val1, pDataArray->val2);
	StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
	WriteConsole(hStdout, msgBuf, (DWORD) cchStringSize, &dwChars, NULL);

	return 0;
}

void ErrorHandler(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code.

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL);

	// Display the error message.

	lpDisplayBuf = (LPVOID) LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR) lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK);

	// Free error-handling buffer allocations.

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}