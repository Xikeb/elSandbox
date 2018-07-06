#ifndef ELECS_LEGACY_SIGNATURE_HPP
	#define ELECS_LEGACY_SIGNATURE_HPP

	namespace ecs {
		template<typename TSettings, typename ...TTypes>
		class Signature {
		public:
			using Settings = TSettings;
			using ComponentList = typename Settings::ComponentList;
			using TagList = typename Settings::TagList;
			using Types = el::type_list<TTypes...>;
			using Components = el::type_of<decltype(ComponentList().filter(ComponentList::has))>;
			using Tags = el::type_of<decltype(TagList().filter(TagList::has))>;
			using Required = Types;

			Signature()/*: Signature(
				std::make_index_sequence<ComponentList::size>(),
				std::make_index_sequence<TagList::size>()
			)*/
			{
				ComponentList().for_each([&](auto &e, auto &) {
					this->_sto.template enableComponent<typename decltype(+e)::type>();
				});
				TagList().for_each([&](auto &e, auto &) {
					this->_sto.template enableTag<typename decltype(+e)::type>();
				});
			}

			constexpr static std::size_t componentCount = Components::size;
			constexpr static std::size_t tagCount = Tags::size;

			bool compare(SignatureBitset<Settings> const &othPrint) const noexcept
			{
				return this->_sto.matches(othPrint);
			}
		private:
			SignatureBitset<Settings> _sto;

			/*template<std::size_t ...CIdxs, std::size_t ...TIdxs>
			Signature(std::index_sequence<CIdxs...>, std::index_sequence<TIdxs...>):
			_sto(
				(char[]){
					el::conditional<
						Components::template Contains<
							typename ComponentList::template At<CIdxs>
						>::value,
						el::char_c<'0'>, el::char_c<'1'>
					>::type::value...,
					el::conditional<
						Tags::template Contains<
							typename ComponentList::template At<TIdxs>
						>::value,
						el::char_c<'0'>, el::char_c<'1'>
					>::type::value...
				}
			)
			{
			}*/
		};
	} // ecs
#endif // ELECS_LEGACY_SIGNATURE_HPP