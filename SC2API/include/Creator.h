#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <iostream>
#include <type_traits>

namespace SC2API
{
	template<class T>
	class Creator
	{
	public:
		~Creator() = default;

		//Constructs from parameter pack and forwards them to object's constructor
		template<class ... ArgsT>
		explicit Creator(ArgsT&& ... args)
			: functor([args ...]  /*copy*/
		{
			return std::make_unique<T>(args ...);
		})
		{
			static_assert(!std::is_abstract<T>::value, "Type must not be abstract");
		}

		//move ctor
		template<
			class OtherT,
			class = typename std::enable_if<std::is_base_of<T, OtherT>::value>::type>
			Creator(Creator<OtherT>&& other)
			: functor(std::move(other.functor))
		{}

		//copy ctor
		template<
			class OtherT,
			class = typename std::enable_if<std::is_base_of<T, OtherT>::value>::type>
			Creator(const Creator<OtherT>& other)
			: functor(other.functor)
		{}

		//move assign
		template<
			class OtherT,
			class = typename std::enable_if<std::is_base_of<T, OtherT>::value>::type>
			Creator<T>& operator=(Creator<OtherT>&& other)
		{
			functor = std::move(other.functor);
			return *this;
		}

		//copy assign
		template<
			class OtherT,
			class = typename std::enable_if<std::is_base_of<T, OtherT>::value>::type>
			Creator<T>& operator=(const Creator<OtherT>& other)
		{
			functor = other.functor;
			return *this;
		}

		inline auto operator()()
		{
			return functor();
		}
	private:
		std::function<std::unique_ptr<T>()> functor;

		template<class OtherT> friend class Creator;
	};
}
