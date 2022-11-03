#pragma once

namespace Engine
{
	template <typename T>
	void	Safe_Delete(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete pPointer;
			pPointer = nullptr;
		}
	}

	template <typename T>			// 클래스 포인터들을 해제
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (dwRefCnt == 0)
				pInstance = nullptr;
		}
		return dwRefCnt;
	}

	template <typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != pInstance)
			dwRefCnt = pInstance->AddRef();

		return dwRefCnt;
	}
}