#pragma once

#include "plg/any.hpp"
#include "plg/format.hpp"

// format support
#ifdef FMT_HEADER_ONLY
namespace fmt {
#else
namespace std {
#endif
    template<>
    struct formatter<plg::invalid> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::invalid&, FormatContext& ctx) const {
            return std::format_to(ctx.out(), "<invalid>");
        }
    };

    template<>
    struct formatter<plg::none> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::none&, FormatContext& ctx) const {
            return std::format_to(ctx.out(), "<void>");
        }
    };

    template<>
    struct formatter<plg::variant<plg::none>> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::variant<plg::none>&, FormatContext& ctx) const {
            return std::format_to(ctx.out(), "variant<void>");
        }
    };

    template<>
    struct formatter<plg::any> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::any& var, FormatContext& ctx) const {
            plg::string out;
            plg::visit([&out](auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<char16_t, T>) {
                    std::format_to(std::back_inserter(out), "{}", static_cast<uint16_t>(v));
                } else {
                    std::format_to(std::back_inserter(out), "{}", v);
                }
            }, var);
            return std::format_to(ctx.out(), "{}", std::move(out));
        }
    };

    template<>
    struct formatter<plg::function> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::function& t, FormatContext& ctx) const {
            return std::format_to(ctx.out(), "{}", t.ptr);
        }
    };

    template<>
    struct formatter<plg::vec2> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::vec2& t, FormatContext& ctx) const {
            return std::format_to(ctx.out(), "{{{}, {}}}", t.x, t.y);
        }
    };

    template<>
    struct formatter<plg::vec3> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::vec3& t, FormatContext& ctx) const {
            return std::format_to(ctx.out(), "{{{}, {}, {}}}", t.x, t.y, t.z);
        }
    };

    template<>
    struct formatter<plg::vec4> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::vec4& t, FormatContext& ctx) const {
            return std::format_to(ctx.out(), "{{{}, {}, {}, {}}}", t.x, t.y, t.z, t.w);
        }
    };

    template<>
    struct formatter<plg::mat4x4> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::mat4x4& t, FormatContext& ctx) const {
            return std::format_to(ctx.out(), "{{{{{}, {}, {}, {}}}, {{{}, {}, {}, {}}}, {{{}, {}, {}, {}}}, {{{}, {}, {}, {}}}}}",
                                  t.m[0][0], t.m[0][1], t.m[0][2], t.m[0][3],
                                  t.m[1][0], t.m[1][1], t.m[1][2], t.m[1][3],
                                  t.m[2][0], t.m[2][1], t.m[2][2], t.m[2][3],
                                  t.m[3][0], t.m[3][1], t.m[3][2], t.m[3][3]);
        }
    };

    template<>
    struct formatter<char16_t> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const char16_t& t, FormatContext& ctx) const {
            return std::format_to(ctx.out(), "{}", static_cast<uint16_t>(t));
        }
    };

    template<typename T, typename Allocator>
    struct formatter<plg::vector<T, Allocator>> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::vector<T, Allocator>& vec, FormatContext& ctx) const {
            std::format_to(ctx.out(), "{}", "{");
            if (!vec.empty()) {
                std::format_to(ctx.out(), "{}", vec[0]);
                for (auto it = std::next(vec.begin()); it != vec.end(); ++it) {
                    std::format_to(ctx.out(), ", {}", *it);
                }
            }
            return std::format_to(ctx.out(), "{}", "}");
        }
    };

    template<typename Allocator>
    struct formatter<plg::vector<char16_t, Allocator>> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::vector<char16_t, Allocator>& vec, FormatContext& ctx) const {
            std::format_to(ctx.out(), "{}", "{");
            if (!vec.empty()) {
                std::format_to(ctx.out(), "{}", static_cast<uint16_t>(vec[0]));
                for (auto it = std::next(vec.begin()); it != vec.end(); ++it) {
                    std::format_to(ctx.out(), ", {}", static_cast<uint16_t>(*it));
                }
            }
            return std::format_to(ctx.out(), "{}", "}");
        }
    };

    template<typename Allocator>
    struct formatter<plg::vector<plg::string, Allocator>> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }

        template<class FormatContext>
        auto format(const plg::vector<plg::string, Allocator>& vec, FormatContext& ctx) const {
            std::format_to(ctx.out(), "{}", "{");
            if (!vec.empty()) {
                std::format_to(ctx.out(), "'{}'", vec[0]);
                for (auto it = std::next(vec.begin()); it != vec.end(); ++it) {
                    std::format_to(ctx.out(), ", '{}'", *it);
                }
            }
            return std::format_to(ctx.out(), "{}", "}");
        }
    };

}// namespace std