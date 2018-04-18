#pragma once

#include <memory>
#include <functional>
#include <type_traits>
#include <memory>

#include "DependencyPtr.h"
#include "TmpHelpers.h"

namespace IoCpp
{

	template <typename TDep>
	class Depends
	{

	private:

		DependencyPtr<TDep> m_pObj;

	protected:

		Depends() = default;

		TDep* use() { return m_pObj; }
		const TDep* use() const { return m_pObj; }

		void inject(DependencyPtr<TDep>&& pDep) { m_pObj = std::move(pDep); }

	};



	template <typename... TDep>
	class DependsOn : public Depends<TDep>...
	{

	protected:

		DependsOn() = default;

	public:

		template <typename TD>
		void inject(DependencyPtr<TD>&& pDep) { Depends<TD>::inject(std::move(pDep)); }

		template <typename TD>
		TD* use() { return Depends<TD>::use(); }

		template <typename TD>
		const TD* use() const { return Depends<TD>::use(); }

	};


	template <typename TI>
	struct Map
	{
		using interface_type = TI;
	};

	template <typename TI, typename TC>
	struct OwnerMap : public Map<TI>
	{

		template <
			typename TReq,
			typename TPtr = std::enable_if_t<std::is_same_v<TReq, TI>, DependencyPtr<TI>>
		>
		TPtr make_concrete()
		{ 
			return DependencyPtr<TI>::fromFunction([]() -> TI* {return new TC{}; });
		}

	};

	template <typename TI, typename TC>
	struct SharedMap : public Map<TI>
	{

		template <
			typename TReq,
			typename TPtr = std::enable_if_t<std::is_same_v<TReq, TI>, DependencyPtr<TI>>
		>
		TPtr make_concrete()
		{
			static TC tcObj;
			return DependencyPtr<TI>::fromInstance(&tcObj);
		}

	};

	template <typename TI>
	using factory_func = std::function<std::shared_ptr<TI>()>;

	template <typename TI>
	struct FactoryMap : public Map<TI>
	{

		factory_func<TI> m_fnMakeShared;

		template <
			typename TReq,
			typename TPtr = std::enable_if_t<std::is_same_v<TReq, TI>, DependencyPtr<TI>>
		>
		TPtr make_concrete()
		{
			auto pTI = m_fnMakeShared ? m_fnMakeShared() : std::shared_ptr<TI>{};
			return DependencyPtr<TI>::fromSharedPtr(std::move(pTI));
		}

	};


	template <typename... TM>
	class Container : private TM...
	{

		static_assert(tmp::check_no_duplicate<typename TM::interface_type...>(), "Duplicate type mapping detected!");

	private:

		template < typename TMap, typename TObj >
		void inject_concrete(TObj* pObj)
		{
			if constexpr (std::is_base_of_v<Depends<typename TMap::interface_type>, TObj>)
				pObj->inject<typename TMap::interface_type>(TMap::make_concrete<typename TMap::interface_type>());
		}

		template < typename TMap, typename TA >
		void set_concrete(DependencyPtr<TA>& pObj)
		{
			if constexpr (std::is_base_of_v<typename TMap::interface_type, TA>)
				pObj = TMap::make_concrete<typename TMap::interface_type>();
		}

		template < typename TMap, typename TA >
		void set_factory(factory_func<TA> fnFactory)
		{
			if constexpr (std::is_same_v<TMap, FactoryMap<TA>>)
				TMap::m_fnMakeShared = fnFactory;
		}

	public:

		template <
			typename TC, 
			typename... TArg,
			typename TRes = std::enable_if_t<std::is_constructible_v<TC, TArg...>, TC>
		>
		TRes make(TArg&&... args)
		{
			TC tObj{std::forward<TArg>(args)...};
			(void)std::initializer_list<int>{((void)inject_concrete<TM>(&tObj), 0)...};
			return tObj;
		}

		template <
			typename TA,
			typename TRes = std::enable_if_t<std::is_abstract_v<TA>, DependencyPtr<TA>>
		>
		TRes make()
		{
			DependencyPtr<TA> pObj;
			(void)std::initializer_list<int>{((void)set_concrete<TM, TA>(pObj),0)...};
			return pObj;
		}

		template< typename TI >
		void setFactory(factory_func<TI> fnMakeShared)
		{
			(void)std::initializer_list<int>{((void)set_factory<TM, TI>(fnMakeShared),0)...};
		}

	};

}
