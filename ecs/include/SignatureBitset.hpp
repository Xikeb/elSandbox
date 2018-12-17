#pragma once
#include <bitset>
#include <cstdlib>

namespace ecs {
	template<typename TSettings>
	class SignatureBitset {
	public:
		using This = SignatureBitset<TSettings>;
		using Settings = TSettings;
		using ComponentList = typename Settings::ComponentList;
		using TagList = typename Settings::TagList;

		constexpr static std::size_t componentCount = ComponentList::size;
		constexpr static std::size_t tagCount = TagList::size;
		using Storage = std::bitset<componentCount + tagCount>;

		constexpr SignatureBitset(): _bitset(0)
		{
		}

		SignatureBitset(char const bits[componentCount + tagCount]): _bitset(bits)
		{
		}

		SignatureBitset(Storage bits): _bitset(bits)
		{
		}

		template<typename T>
		constexpr static bool isComponent = ComponentList::template Contains<T>::value;

		template<typename T>
		constexpr static std::size_t componentId = ComponentList::template IndexOf<T>::value;

		template<typename T>
		constexpr static std::size_t componentBit = componentId<T>;

		template<typename T>
		constexpr static bool isTag = TagList::template Contains<T>::value;

		template<typename T>
		constexpr static std::size_t tagId = TagList::template IndexOf<T>::value;

		template<typename T>
		constexpr static std::size_t tagBit = componentCount + tagId<T>;

		This &reset() noexcept
		{
			this->_bitset.reset();
			return *this;
		}

		template<typename T>
		bool hasComponent() const noexcept
		{
			static_assert(isComponent<T>, "This isn't a Component according to Settings");
			return this->_bitset.test(componentBit<T>);
		}

		template<typename T>
		This &enableComponent() noexcept
		{
			static_assert(isComponent<T>, "This isn't a Component according to Settings");
			this->_bitset.set(componentBit<T>);
			return *this;
		}

		template<typename T>
		This &disableComponent() noexcept
		{
			static_assert(isComponent<T>, "This isn't a Component according to Settings");
			this->_bitset.reset(componentBit<T>);
			return *this;
		}

		template<typename T>
		This &toggleComponent() noexcept
		{
			static_assert(isComponent<T>, "This isn't a Component according to Settings");
			this->_bitset.flip(componentBit<T>);
			return *this;
		}

		template<typename T>
		bool hasTag() const noexcept
		{
			static_assert(isTag<T>, "This isn't a Tag according to Settings");
			return this->_bitset.test(tagBit<T>);
		}

		template<typename T>
		This &enableTag() noexcept
		{
			static_assert(isTag<T>, "This isn't a Tag according to Settings");
			this->_bitset.set(tagBit<T>);
			return *this;
		}

		template<typename T>
		This &disableTag() noexcept
		{
			static_assert(isTag<T>, "This isn't a Tag according to Settings");
			this->_bitset.reset(tagBit<T>);
			return *this;
		}

		template<typename T>
		This &toggleTag() noexcept
		{
			static_assert(isTag<T>, "This isn't a Tag according to Settings");
			this->_bitset.flip(tagBit<T>);
			return *this;
		}

		bool matches(This const &oth) const noexcept
		{
			return (this->_bitset & oth._bitset) == this->_bitset;
		}

		bool matches(Storage const &oth) const noexcept
		{
			return (this->_bitset & oth) == this->_bitset;
		}

		template<typename OthSettings>
		bool matches(SignatureBitset<OthSettings> const &oth) const noexcept
		{
			static_assert(el::is_same_v<OthSettings, Settings>, "Trying to compare SignatureBitsets of different Settings");
			return (this->_bitset & oth._bitset) == this->_bitset;
		}

		// template<std::size_t N>
		// bool matches(std::bitset<N> const &oth) const noexcept
		// {
		// 	static_assert(el::is_same_v<Storage, std::bitset<N>>, "Trying to compare std::bitset's of different lengths");
		// 	return (this->_bitset & oth) == this->_bitset;
		// }

		This operator~() const noexcept { return This{~this->_bitset}; }
		This operator&(This const &oth) const noexcept { return This{this->_bitset & oth._bitset}; }
		This operator|(This const &oth) const noexcept { return This{this->_bitset | oth._bitset}; }
		This operator^(This const &oth) const noexcept { return This{this->_bitset ^ oth._bitset}; }

	private:
		Storage _bitset;
	};

	namespace detail {
		template<typename T>
		struct is_signature_bitset;

		template<typename TSettings>
		struct is_signature_bitset<ecs::SignatureBitset<TSettings>>: el::true_c {};
		template<typename T>
		struct is_signature_bitset: el::false_c {};
	} // detail
} // ecs