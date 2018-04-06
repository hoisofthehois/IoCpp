#pragma once

#include <memory>
#include <functional>
#include <type_traits>
#include <memory>

namespace IoCpp
{

	template <typename TDep>
	class Depends
	{

	private:

		std::shared_ptr<TDep> m_pObj;

	protected:

		Depends() = default;

		TDep* use() { return m_pObj.get(); }
		const TDep* use() const { return m_pObj.get(); }

		void inject(std::shared_ptr<TDep>&& pDep) { m_pObj = std::move(pDep); }

	};



	template <typename... TDep>
	class DependsOn : public Depends<TDep>...
	{

	protected:

		DependsOn() = default;

	public:

		template <typename TD>
		void inject(std::shared_ptr<TD>&& pDep) { Depends<TD>::inject(std::move(pDep)); }

		template <typename TD>
		TD* use() { return Depends<TD>::use(); }

		template <typename TD>
		const TD* use() const { return Depends<TD>::use(); }

	};


	template <typename TI, typename TC>
	struct Map
	{
		typedef TI interface_type;
		typedef TC concrete_type;
	};

	template <typename TI, typename TC>
	struct OwnerMap : public Map<TI, TC>
	{

	protected:

		template <
			typename TReq,
			typename TPtr = std::enable_if_t<std::is_same_v<TReq, TI>, std::shared_ptr<TI>>
		>
		TPtr make_concrete()
		{ 
			return std::shared_ptr<TI>(new TC{});
		}

	};

	template <typename TI, typename TC>
	struct SharedMap : public Map<TI, TC>
	{

	protected:

		template <
			typename TReq,
			typename TPtr = std::enable_if_t<std::is_same_v<TReq, TI>, std::shared_ptr<TI>>
		>
		TPtr make_concrete()
		{
			static TC tcObj;
			return std::shared_ptr<TI>(&tcObj, [](TI* pObj) {});
		}

	};

	template <typename TI>
	using factory_func = std::function<std::shared_ptr<TI>()>;

	template <typename TI>
	struct FactoryMap : public Map<TI, TI>
	{

	protected:

		factory_func<TI> m_fnMakeShared;

		template <
			typename TReq,
			typename TPtr = std::enable_if_t<std::is_same_v<TReq, TI>, std::shared_ptr<TI>>
		>
		TPtr make_concrete()
		{
			return m_fnMakeShared ? m_fnMakeShared() : TPtr{};
		}

	};


	template <typename... TM>
	class Container : public TM...
	{

	private:

		template < typename TMap, typename TObj >
		void inject_concrete(TObj* pObj)
		{
			if constexpr (std::is_base_of_v<Depends<typename TMap::interface_type>, TObj>)
				pObj->inject<typename TMap::interface_type>(TMap::make_concrete<typename TMap::interface_type>());
		}

		template < typename TMap, typename TA >
		void set_concrete(std::shared_ptr<TA>& pObj)
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
			auto fold = {0, (inject_concrete<TM>(&tObj), 0)...};
			return tObj;
		}

		template <
			typename TA,
			typename TRes = std::enable_if_t<std::is_abstract_v<TA>, std::shared_ptr<TA>>
		>
		TRes make()
		{
			std::shared_ptr<TA> pObj;
			auto fold = {0, (set_concrete<TM, TA>(pObj),0)...};
			return pObj;
		}

		template< typename TI >
		void setFactory(factory_func<TI> fnMakeShared)
		{
			auto fold = {0, (set_factory<TM, TI>(fnMakeShared),0)...};
		}

	};

}
