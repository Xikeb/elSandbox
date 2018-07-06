#ifndef ELECS_SIGNATURE_BITSET_HPP
	#define ELECS_SIGNATURE_BITSET_HPP
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

			SignatureBitset(SignatureBitset const &oth):
			_bitset(oth._bitset)
			{
			}

			SignatureBitset &operator=(SignatureBitset const &rhs) noexcept
			{
				this->_bitset = rhs._bitset;
				return *this;
			}

			template<typename T>
			constexpr static bool isComponent = ComponentList::template Contains<T>::value;

			template<typename T>
			constexpr static std::size_t componentId = ComponentList::template IndexOf<T>::value;

			template<typename T>
			constexpr static std::size_t componentBit = ComponentList::template IndexOf<T>::value;

			template<typename T>
			constexpr static bool isTag = TagList::template Contains<T>::value;

			template<typename T>
			constexpr static std::size_t tagId = TagList::template IndexOf<T>::value;

			template<typename T>
			constexpr static std::size_t tagBit = componentCount + TagList::template IndexOf<T>::value;

			This &reset() noexcept
			{
				this->_bitset.reset();
				return *this;
			}

			template<typename T>
			bool hasComponent() const noexcept
			{
				static_assert(isComponent<T>, "T must be a Component according to Settings");
				return this->_bitset.test(componentBit<T>);
			}

			template<typename T>
			This &enableComponent() noexcept
			{
				static_assert(isComponent<T>, "T must be a Component according to Settings");
				this->_bitset.set(componentBit<T>);
				return *this;
			}

			template<typename T>
			This &disableComponent() noexcept
			{
				static_assert(isComponent<T>, "T must be a Component according to Settings");
				this->_bitset.reset(componentBit<T>);
				return *this;
			}

			template<typename T>
			This &toggleComponent() noexcept
			{
				static_assert(isComponent<T>, "T must be a Component according to Settings");
				this->_bitset.flip(componentBit<T>);
				return *this;
			}

			template<typename T>
			bool hasTag() const noexcept
			{
				static_assert(isTag<T>, "T must be a Tag according to Settings");
				return this->_bitset.test(tagBit<T>);
			}

			template<typename T>
			This &enableTag() noexcept
			{
				static_assert(isTag<T>, "T must be a Tag according to Settings");
				this->_bitset.set(tagBit<T>);
				return *this;
			}

			template<typename T>
			This &disableTag() noexcept
			{
				static_assert(isTag<T>, "T must be a Tag according to Settings");
				this->_bitset.reset(tagBit<T>);
				return *this;
			}

			template<typename T>
			This &toggleTag() noexcept
			{
				static_assert(isTag<T>, "T must be a Tag according to Settings");
				this->_bitset.flip(tagBit<T>);
				return *this;
			}

			bool matches(This const &oth) const noexcept
			{
				return (this->_bitset & oth._bitset) == this->_bitset;
			}

			bool matches(Storage const &oth) const noexcept
			{
				return oth == this->_bitset;
			}

			Storage mask(This const &oth) const noexcept
			{
				return this->_bitset & oth._bitset;
			}

			Storage mask(Storage const &oth) const noexcept
			{
				return this->_bitset & oth;
			}

		private:
			Storage _bitset;
		};
	
	} // ecs
#endif // ELECS_SIGNATURE_BITSET_HPP