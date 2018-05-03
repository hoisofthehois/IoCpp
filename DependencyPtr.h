#pragma once

#include <memory>
#include <variant>

namespace IoCpp
{

	template <typename TI>
	class RawPtrImpl
	{
	private:
		TI* m_pObj;
	public:
		RawPtrImpl(TI* pObj) : m_pObj(pObj) {}
		TI* getPtr() { return m_pObj; }
		const TI* getPtr() const { return m_pObj; }
	};

	template <typename TI>
	class OwningPtrImpl
	{

	private:

		std::unique_ptr<TI> m_pObj;
		std::function<TI*()> m_fnCopy;

	public:

		OwningPtrImpl(TI* pObj, std::function<TI*()>&& fnCopy) 
			: m_pObj(pObj), m_fnCopy(std::move(fnCopy)) {}
		OwningPtrImpl(const OwningPtrImpl& impl) 
			: m_pObj(impl.m_fnCopy()), m_fnCopy(impl.m_fnCopy) {}
		OwningPtrImpl& operator=(const OwningPtrImpl& impl)
		{
			if (&impl != this)
			{
				m_pObj.reset(impl.m_fnCopy());
				m_fnCopy = impl.m_fnCopy;
			}
			return *this;
		}
		OwningPtrImpl(OwningPtrImpl&& impl) 
			: m_pObj(impl.m_fnCopy()), m_fnCopy(std::move(impl.m_fnCopy)) {}
		OwningPtrImpl& operator=(OwningPtrImpl&& impl)
		{
			if (&impl != this)
			{
				m_pObj = std::move(impl.m_pObj);
				m_fnCopy = std::move(impl.m_fnCopy);
			}
			return *this;
		}

		TI* getPtr() { return m_pObj.get(); }
		const TI* getPtr() const { return m_pObj.get(); }

	};

	template <typename TI>
	class SharedPtrImpl
	{
	private:
		std::shared_ptr<TI> m_pShared;
	public:
		SharedPtrImpl(std::shared_ptr<TI>&& pObj) : m_pShared(std::move(pObj)) {}
		TI* getPtr() { return m_pShared.get(); }
		const TI* getPtr() const { return m_pShared.get(); }
	};

	template <typename TI>
	class DependencyPtr final 
	{

	private:

		std::variant<RawPtrImpl<TI>, OwningPtrImpl<TI>, SharedPtrImpl<TI>> m_impl;

		TI* getPtr() 
		{
			return std::visit([](auto&& ptrImpl) { return ptrImpl.getPtr(); }, m_impl);		
		}

		const TI* getPtr() const
		{
			return const_cast<DependencyPtr<TI>*>(this)->getPtr();
		}

		DependencyPtr(TI* pObj) : m_impl(RawPtrImpl<TI>{pObj}) {}
		DependencyPtr(TI* pObj, std::function<TI*()>&& fnCopy) :
			m_impl(OwningPtrImpl<TI>{pObj, std::move(fnCopy)}) {}
		DependencyPtr(std::shared_ptr<TI>&& pObj) :
			m_impl(SharedPtrImpl<TI>{std::move(pObj)}) {}

	public:

		DependencyPtr() : m_impl(RawPtrImpl<TI>{nullptr}) {}

		static DependencyPtr<TI> fromFunction(std::function<TI*()>&& fnCopy)
		{
			auto pObj = fnCopy();
			return DependencyPtr<TI>(pObj, std::move(fnCopy));
		}

		template<
			typename TC,
			typename TPtr = std::enable_if_t<std::is_base_of_v<TI, TC>, DependencyPtr<TI>>
		>
		static TPtr fromInstance(TC* pObj)
		{
			return DependencyPtr<TI>(pObj);
		}

		static DependencyPtr<TI> fromSharedPtr(std::shared_ptr<TI>&& pPtr)
		{
			return DependencyPtr<TI>(std::move(pPtr));
		}

		~DependencyPtr() = default;

		//DependencyPtr(DependencyPtr&& ptr) :
		//	m_pRaw(ptr.m_pRaw),
		//	m_pShared(std::move(ptr.m_pShared)),
		//	m_fnCopy(std::move(ptr.m_fnCopy))
		//{
		//	ptr.m_pRaw = nullptr;
		//}

		//DependencyPtr& operator=(DependencyPtr&& ptr)
		//{
		//	if (&ptr != this)
		//	{
		//		reset();
		//		m_pRaw = ptr.m_pRaw;
		//		m_pShared = std::move(ptr.m_pShared);
		//		m_fnCopy = std::move(ptr.m_fnCopy);
		//		ptr.m_pRaw = nullptr;
		//	}
		//	return *this;
		//}

		//DependencyPtr(const DependencyPtr& ptr) :
		//	m_pRaw(ptr.m_fnCopy ? ptr.m_fnCopy() : ptr.m_pRaw),
		//	m_pShared(ptr.m_pShared),
		//	m_fnCopy(ptr.m_fnCopy) {}

		//DependencyPtr& operator=(const DependencyPtr& ptr)
		//{
		//	if (&ptr != this)
		//	{
		//		reset();
		//		m_pRaw = ptr.m_fnCopy ? ptr.m_fnCopy() : ptr.m_pRaw;
		//		m_pShared = ptr.m_pShared;
		//		m_fnCopy = ptr.m_fnCopy;
		//	}
		//	return *this;
		//}

		TI* operator->() { return getPtr(); }
		const TI* operator->() const { return getPtr(); }

		operator TI*() { return getPtr(); }
		operator const TI*() const { return getPtr(); }

	};

}
