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

	template <typename T>
	void	Safe_Delete_Array(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete [] pPointer;
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

	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const wstring& pTag)
			: m_pTargetTag(pTag)
		{
		}
		~CTag_Finder() {	}
	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (m_pTargetTag == pair.first)
			{
				return true;
			}

			return false;
		}

	private:
		const wstring&		m_pTargetTag = nullptr;
	};
}